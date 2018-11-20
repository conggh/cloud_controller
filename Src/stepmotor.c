#include "main.h"
#include "stm32f1xx_hal.h"
#include "stepmotor.h"


/* �ٶȶ��� */
#define	MAX_SPEED			40
#define TIM_CNT_TOP		2520

/* ������ٿ��� */
#define MOVE_ACCEL_TIME   ((curSpeed[i]<<2)*curSpeed[i])  // ÿ���ٶ��µ�������

uint16_t speedTable[MAX_SPEED + 1] = {0};

GPIO_TypeDef* menPort[MOTOR_NUM] = {NULL};		// ���ʹ�������˿�
uint32_t menPin[MOTOR_NUM] = {0};							// ���ʹ�������ܽ�
uint8_t menRev[MOTOR_NUM] = {0};
GPIO_TypeDef* mdirPort[MOTOR_NUM] = {NULL};		// ������������˿�
uint32_t mdirPin[MOTOR_NUM] = {0};							// ������������ܽ�
uint8_t mdirRev[MOTOR_NUM] = {0};
GPIO_TypeDef* mclkPort[MOTOR_NUM] = {NULL};		// ������������˿�
uint32_t mclkPin[MOTOR_NUM] = {0};							// ������������ܽ�
uint8_t mclkRev[MOTOR_NUM] = {0};
GPIO_TypeDef* endsPort[MOTOR_NUM] = {NULL};		// ��λ���ض˿�
uint32_t endsPin[MOTOR_NUM] = {0};							// ��λ���عܽ�
uint32_t endsPhase[MOTOR_NUM] = {0};							// ��λ���عܽ�

uint8_t bStop[MOTOR_NUM] = {0};					// �Ƿ�ֹͣ
uint8_t bAccel[MOTOR_NUM] = {0};				// �Ƿ�Ӽ���
int8_t iDir[MOTOR_NUM] = {0};						// ����
uint8_t curSpeed[MOTOR_NUM] = {0};			// ��ǰ�ٶ�
uint16_t pulseCount[MOTOR_NUM] = {0};		// �����ȿ���
uint32_t stepCount[MOTOR_NUM] = {0};		// ����
uint32_t stepDecel[MOTOR_NUM] = {0};		// ���ﲽ��ʱ��ʼ����
uint32_t stepAccel[MOTOR_NUM] = {0};		// ���ﲽ��ʱ��ʼ����
uint32_t stopCount[MOTOR_NUM] = {0};		// ��;ֹͣʱʣ�ಽ��

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
	{	// ��������ÿ�����
		
		if(mclkPort[i] != NULL && stepCount[i])
		{	// �в���ʱ����
			if(endsPort[i] != NULL && IS_ENDS(i) == endsPhase[i] && iDir[i] == -1)
			{	// ������λ,stopCount��¼ֹͣʱʣ����������Ȼ��stepCount���㣬ֹͣת��
				stopCount[i] = stepCount[i];
				stepCount[i] = 0;
				MCLK_HIGH(i);
				continue;
			}
			if(pulseCount[i] == 0)
			{	// �����������������
				pulseCount[i] = speedTable[curSpeed[i]];	// ָ������
				if(!bAccel[i])
				{ // �����˶�
					if(bStop[i])
					{   // ֹͣ����¼δִ��λ��
						stopCount[i] = stepCount[i] - 1;
						stepCount[i] = 0;
						MCLK_HIGH(i);
						continue;
					}
				}
				else if(stepCount[i] > stepDecel[i])
				{ // ���ٽ׶�
					if(bStop[i])
					{   // ֹͣ����¼δִ��λ�ã�ֱ�ӽ������
						stopCount[i] = stepCount[i] - stepDecel[i] - 1;
						stepCount[i] = stepDecel[i] + 1;
					}
					else if(stepCount[i] == stepAccel[i])
					{ // ������ٵ�
						if(curSpeed[i] < MAX_SPEED)
						{ // ����
							stepDecel[i] += MOVE_ACCEL_TIME;
							if(stepDecel[i] < stepCount[i])
							{ // ����
								curSpeed[i]++;
								stepAccel[i] -= MOVE_ACCEL_TIME;
							}
							else
							{ // ȡ������
								stepDecel[i] -= MOVE_ACCEL_TIME;
								stepAccel[i] -= MOVE_ACCEL_TIME;
							}
						}
						else
						{ // ���õ��ٵ�
							stepAccel[i] -= MOVE_ACCEL_TIME;
						}
					}
				}
				else if(stepCount[i] == stepDecel[i])
				{ // ��ʼ����
					if(curSpeed[i] > 1)
					{
						curSpeed[i]--;
						stepAccel[i] = stepDecel[i] - MOVE_ACCEL_TIME;
					}
				}
				else if(stepCount[i] == stepAccel[i])
				{ // ������ٵ�
					if(curSpeed[i] > 1)
					{
						curSpeed[i]--;
						stepAccel[i] -= MOVE_ACCEL_TIME;
					}
				}
				MCLK_HIGH(i);
			}
			else
			{	// �������
				if(pulseCount[i] == 50)
				{	// ����һ��ʱ������
					MCLK_LOW(i);
					stepCount[i]--;		// ����
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
		for(j=1;j<(MAX_SPEED+1);j++)			//40������(ռ�ձ�)
		{
			speedTable[j] = TIM_CNT_TOP / j;//����̶�������
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
		{	// ����
			bAccel[i] = 0;
			curSpeed[i] = speed;
		}
		else
		{	// �Ӽ���
			bAccel[i] = 1;
			curSpeed[i] = 1;
		}
		bStop[i] = 0;
		pulseCount[i] = speedTable[curSpeed[i]];//ָ������
		stepAccel[i] = steps - MOVE_ACCEL_TIME;
		stepDecel[i] = 0;
		stepCount[i] = steps;
	}
}

void StopMotor(uint8_t i)
{
	bStop[i] = 1;
}
//��õ�ǰλ��
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

