#include "main.h"
#include "stm32f1xx_hal.h"
#include "stepmotor.h"


/* 速度定义 */
#define	MAX_SPEED			40
#define TIM_CNT_TOP		2520

/* 电机加速控制 */
#define MOVE_ACCEL_TIME   ((curSpeed[i]<<2)*curSpeed[i])  // 每个速度下的脉冲数

uint16_t speedTable[MAX_SPEED + 1] = {0};

GPIO_TypeDef* menPort[MOTOR_NUM] = {NULL};		// 电机使能驱动端口
uint32_t menPin[MOTOR_NUM] = {0};							// 电机使能驱动管脚
uint8_t menRev[MOTOR_NUM] = {0};
GPIO_TypeDef* mdirPort[MOTOR_NUM] = {NULL};		// 电机方向驱动端口
uint32_t mdirPin[MOTOR_NUM] = {0};							// 电机方向驱动管脚
uint8_t mdirRev[MOTOR_NUM] = {0};
GPIO_TypeDef* mclkPort[MOTOR_NUM] = {NULL};		// 电机脉冲驱动端口
uint32_t mclkPin[MOTOR_NUM] = {0};							// 电机脉冲驱动管脚
uint8_t mclkRev[MOTOR_NUM] = {0};
GPIO_TypeDef* endsPort[MOTOR_NUM] = {NULL};		// 限位开关端口
uint32_t endsPin[MOTOR_NUM] = {0};							// 限位开关管脚
uint32_t endsPhase[MOTOR_NUM] = {0};							// 限位开关管脚

uint8_t bStop[MOTOR_NUM] = {0};					// 是否停止
uint8_t bAccel[MOTOR_NUM] = {0};				// 是否加减速
int8_t iDir[MOTOR_NUM] = {0};						// 方向
uint8_t curSpeed[MOTOR_NUM] = {0};			// 当前速度
uint16_t pulseCount[MOTOR_NUM] = {0};		// 脉冲宽度控制
uint32_t stepCount[MOTOR_NUM] = {0};		// 步数
uint32_t stepDecel[MOTOR_NUM] = {0};		// 到达步数时开始减速
uint32_t stepAccel[MOTOR_NUM] = {0};		// 到达步数时开始调速
uint32_t stopCount[MOTOR_NUM] = {0};		// 中途停止时剩余步数

extern uint8_t bLaser;

uint8_t IsEndStop(uint8_t i)
{
	if(endsPort[i] != NULL && IS_ENDS(i) == endsPhase[i])
	{
		return 1;
	}
	return 0;
}

void TickMotor()
{
	uint8_t i;
	for(i=0;i<MOTOR_NUM;i++)
	{	// 轮流处理每个电机
		
		if(mclkPort[i] != NULL && stepCount[i])
		{	// 有步数时运行
			if(endsPort[i] != NULL && IS_ENDS(i) == endsPhase[i] && iDir[i] == -1)
			{	// 触发限位,stopCount记录停止时剩余脉冲数，然后stepCount清零，停止转动
				stopCount[i] = stepCount[i];
				stepCount[i] = 0;
				MCLK_HIGH(i);
				continue;
			}
			if(pulseCount[i] == 0)
			{	// 脉宽计数到，给脉冲
				pulseCount[i] = speedTable[curSpeed[i]];	// 指定脉宽
				if(!bAccel[i])
				{ // 匀速运动
					if(bStop[i])
					{   // 停止，记录未执行位置
						stopCount[i] = stepCount[i] - 1;
						stepCount[i] = 0;
						MCLK_HIGH(i);
						continue;
					}
				}
				else if(stepCount[i] > stepDecel[i])
				{ // 加速阶段
					if(bStop[i])
					{   // 停止，记录未执行位置，直接进入减速
						stopCount[i] = stepCount[i] - stepDecel[i] - 1;
						stepCount[i] = stepDecel[i] + 1;
					}
					else if(stepCount[i] == stepAccel[i])
					{ // 到达调速点
						if(curSpeed[i] < MAX_SPEED)
						{ // 加速
							stepDecel[i] += MOVE_ACCEL_TIME;
							if(stepDecel[i] < stepCount[i])
							{ // 提速
								curSpeed[i]++;
								stepAccel[i] -= MOVE_ACCEL_TIME;
							}
							else
							{ // 取消提速
								stepDecel[i] -= MOVE_ACCEL_TIME;
								stepAccel[i] -= MOVE_ACCEL_TIME;
							}
						}
						else
						{ // 设置调速点
							stepAccel[i] -= MOVE_ACCEL_TIME;
						}
					}
				}
				else if(stepCount[i] == stepDecel[i])
				{ // 开始减速
					if(curSpeed[i] > 1)
					{
						curSpeed[i]--;
						stepAccel[i] = stepDecel[i] - MOVE_ACCEL_TIME;
					}
				}
				else if(stepCount[i] == stepAccel[i])
				{ // 到达减速点
					if(curSpeed[i] > 1)
					{
						curSpeed[i]--;
						stepAccel[i] -= MOVE_ACCEL_TIME;
					}
				}
				MCLK_HIGH(i);
			}
			else
			{	// 脉宽计数
				if(pulseCount[i] == 50)
				{	// 脉宽一半时换极性
					MCLK_LOW(i);
					stepCount[i]--;		// 计数
				}
				pulseCount[i]--;
			}
		}
	}
}

void InitMotor(uint8_t i,
	GPIO_TypeDef* enPort, uint32_t enPin, uint8_t enRev,
	GPIO_TypeDef* dirPort, uint32_t dirPin, uint8_t dirRev,
	GPIO_TypeDef* clkPort, uint32_t clkPin, uint8_t clkRev,
	GPIO_TypeDef* endPort, uint32_t endPin, uint8_t endRev)
{
	int8_t j;
	if(speedTable[1] == 0)
	{
		for(j=1;j<(MAX_SPEED+1);j++)			//40级调速(占空比)
		{
			speedTable[j] = TIM_CNT_TOP / j;//分配固定脉冲宽度
		}
	}
	menPort[i] = enPort;
	menPin[i] = enPin;
	menRev[i] = enRev;
	mdirPort[i] = dirPort;
	mdirPin[i] = dirPin;
	mdirRev[i] = dirRev;
	mclkPort[i] = clkPort;
	mclkPin[i] = clkPin;
	mclkRev[i] = clkRev;
	endsPort[i] = endPort;
	endsPin[i] = endPin;
	if(endRev)
	{
		endsPhase[i] = 0;
	}
	else
	{
		endsPhase[i] = endPin;
	}
	MDIR_HIGH(i);
	MCLK_LOW(i);
	if(menRev[i])
		MEN_HIGH(i);
	else
		MEN_LOW(i);
}

uint8_t IsMotorStop(uint8_t i)
{
	return stepCount[i] == 0;
}

void TurnMotor(uint8_t i, int32_t steps, uint8_t speed)
{
	if(stepCount[i] == 0 && speed <= MAX_SPEED)
	{
		stopCount[i] = 0;
		if(steps > 0)
		{
			if(mdirRev[i])
				MDIR_LOW(i);
			else
				MDIR_HIGH(i);
			iDir[i] = 1;
		}
		else if(steps < 0)
		{
			if(mdirRev[i])
				MDIR_HIGH(i);
			else
				MDIR_LOW(i);
			iDir[i] = -1;
			steps = -steps;
		}
		else
		{
			iDir[i] = 0;
			speed = 0;
		}
		if(speed)
		{	// 匀速
			bAccel[i] = 0;
			curSpeed[i] = speed;
		}
		else
		{	// 加减速
			bAccel[i] = 1;
			curSpeed[i] = 1;
		}
		bStop[i] = 0;
		pulseCount[i] = speedTable[curSpeed[i]];//指定脉宽
		stepAccel[i] = steps - MOVE_ACCEL_TIME;
		stepDecel[i] = 0;
		stepCount[i] = steps;
	}
}

void StopMotor(uint8_t i)
{
	bStop[i] = 1;
}
//获得当前位置
int32_t GetCurrentPos(uint8_t i)
{
	return (int32_t)iDir[i] * (stepCount[i] + stopCount[i]);
}

int32_t GetStopPos(uint8_t i)
{
	return (int32_t)iDir[i] * stopCount[i];
}

void ResetStopPos(uint8_t i)
{
	stopCount[i] = 0;
}

