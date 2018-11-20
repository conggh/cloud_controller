#include "main.h"
#include "stm32f1xx_hal.h"
#include "iocntl.h"
#include "stepmotor.h"


// 步进角1.8度，8细分，1600个脉冲一步，200个脉冲0.225°
// 360/1.8 = 200 步  
int32_t MovePos_0 = 0;
int32_t MovePos_1 = 0;

void InitIO()
{
	InitMotor(MOVE_ID_0, DRV1_EN_GPIO_Port, DRV1_EN_Pin, 0,
						DRV1_DIR_GPIO_Port, DRV1_DIR_Pin, 0,
						DRV1_CLK_GPIO_Port, DRV1_CLK_Pin, 0,
						ENDS_A_GPIO_Port, ENDS_A_Pin, 1);

	InitMotor(MOVE_ID_1, DRV2_EN_GPIO_Port, DRV2_EN_Pin, 0,
						DRV2_DIR_GPIO_Port, DRV2_DIR_Pin, 0,
						DRV2_CLK_GPIO_Port, DRV2_CLK_Pin, 0,
						ENDS_B_GPIO_Port, ENDS_B_Pin, 1);
	UsbPlug();
}
// 设置当前位置
void SetPosition(float Left_Pos,float Up_Pos)
{
	if(GetMoveState() == 0)
	{
		MovePos_0 = (int32_t)(Left_Pos * Left_STEP_PER_MM);
		MovePos_1 = (int32_t)(Up_Pos * Up_STEP_PER_MM);
	}
}

// 移动到目标位置
void Move(float Left_Pos,float Up_Pos, uint8_t spd)
{

	int32_t L_iPos = (int32_t)(Left_Pos * Left_STEP_PER_MM);
	int32_t U_iPos = (int32_t)(Up_Pos * Up_STEP_PER_MM);
	if(L_iPos != MovePos_0)
	{
		TurnMotor(MOVE_ID_0, L_iPos - MovePos_0, spd);
		MovePos_0 = L_iPos;
	}

	if(U_iPos != MovePos_1)
	{
		TurnMotor(MOVE_ID_1, U_iPos - MovePos_1, spd);	
		MovePos_1 = U_iPos;
	}
}

// 停止移动
void StopMove()
{
	StopMotor(MOVE_ID_0);
	StopMotor(MOVE_ID_1);
}
// 获取移动状态
uint8_t GetMoveState()
{
	uint8_t run = 0;
	if(IsMotorStop(MOVE_ID_0))
	{
		MovePos_0 = GetMovePos_0();
		ResetStopPos(MOVE_ID_0);
	}
	else
	{
		run = 1;
	}
	if(IsMotorStop(MOVE_ID_1))
	{
		MovePos_1 = GetMovePos_1();
		ResetStopPos(MOVE_ID_1);
	}
	else if(run == 1)
	{
		run = 3;
	}
	else
	{
		run = 2;
	}
	return run;
}

// 获取左右当前位置
int32_t GetMovePos_0()
{
	int32_t pos = GetCurrentPos(MOVE_ID_0);
	return (MovePos_0 - pos);
}
// 获取上下当前位置
int32_t GetMovePos_1()
{
	int32_t pos = GetCurrentPos(MOVE_ID_1);
	return (MovePos_1 - pos);
}


// 校准
void calibration()
{
	Move(-360.f, -120.f, 20);
}

// USB
void UsbPlug()
{
	HAL_GPIO_WritePin(USB_EN_GPIO_Port, USB_EN_Pin, GPIO_PIN_RESET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(USB_EN_GPIO_Port, USB_EN_Pin, GPIO_PIN_SET);
}


