#ifndef	_IOCNTL_H
#define	_IOCNTL_H
#include "stm32f1xx_hal.h"


#define	MOVE_ID_0	0
#define	MOVE_ID_1	1


// 步进角1.8度，8细分，1600个脉冲一圈
// 360/1.8 = 200 步  1.8/8 = 0.225°

#define	Left_STEP_PER_MM		183.f
#define	Up_STEP_PER_MM		360.f

void InitIO(void);
uint8_t GetMoveState(void);
int32_t GetMovePos_0(void);
int32_t GetMovePos_1(void);
void SetPosition(float Left_Pos,float Up_Pos);// 设置当前位置
void Move(float Left_Pos,float Up_Pos, uint8_t spd);
void UsbPlug(void);
void calibration(void);
void StopMove(void);


#endif
