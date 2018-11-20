#ifndef	_STEPMOTOR_H
#define	_STEPMOTOR_H

#include "stm32f1xx_hal.h"

#define	MOTOR_NUM	3

#define	MEN_HIGH(i)		menPort[i]->BSRR = menPin[i]<<16U
#define	MEN_LOW(i)		menPort[i]->BSRR = menPin[i]
#define	MDIR_HIGH(i)	mdirPort[i]->BSRR = mdirPin[i]<<16U
#define	MDIR_LOW(i)		mdirPort[i]->BSRR = mdirPin[i]
#define	MCLK_HIGH(i)	mclkPort[i]->BSRR = mclkPin[i]<<16U
#define	MCLK_LOW(i)		mclkPort[i]->BSRR = mclkPin[i]
#define	IS_ENDS(i)		(endsPort[i]->IDR & endsPin[i])

uint8_t IsEndStop(uint8_t i);
void TickMotor(void);
void InitMotor(uint8_t i,
	GPIO_TypeDef* enPort, uint32_t enPin, uint8_t enRev,
	GPIO_TypeDef* dirPort, uint32_t dirPin, uint8_t dirRev,
	GPIO_TypeDef* clkPort, uint32_t clkPin, uint8_t clkRev,
	GPIO_TypeDef* endPort, uint32_t endPin, uint8_t endRev);
uint8_t IsMotorStop(uint8_t i);
void TurnMotor(uint8_t i, int32_t steps, uint8_t speed);
void StopMotor(uint8_t i);
int32_t GetCurrentPos(uint8_t i);
int32_t GetStopPos(uint8_t i);
void ResetStopPos(uint8_t i);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif
