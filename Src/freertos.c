/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
//#include "main.h"
//#include "stepmotor.h"
//#include "stm32f1xx_hal.h"
//#include "usbd_cdc_if.h"
//#include "iocntl.h"

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */
//extern uint8_t usb_buf[64];
//extern uint32_t usb_len;
//uint8_t sbuf[64];
//uint8_t g_snum = 0;

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
//void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
//void ProcCommand(char*usb_buf, char usb_len);
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
//  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
	
  /* Infinite loop */
  for(;;)
  {
		/*
 		// receive command
		if(usb_len > 0)
		{
			ProcCommand((char*)usb_buf, usb_len);
			memset(usb_buf,0,sizeof(usb_buf));
			usb_len = 0;
		}
//		printf("ProcCommand usb_buf:%s\n",usb_buf);
		sbuf[0] = 0x55;
		sbuf[1] = g_snum;	// sync number
		sbuf[2] = GetMoveState();
//		if(GetMoveState())
//			printf("GetMoveState 1\n");
//		else printf("GetMoveState 0\n");
		if(!(ENDS_A_GPIO_Port->IDR & ENDS_A_Pin))
		{
			sbuf[3] = 0x01;//printf("ends_a interrupt\n");
		}
		else if(!(ENDS_B_GPIO_Port->IDR & ENDS_B_Pin))
		{
			sbuf[3] = 0x02;//printf("ends_b interrupt\n");	
		}
		else sbuf[3] = 0;
		sbuf[4] = 0;
		sbuf[8] = 0;
		CDC_Transmit_FS(sbuf, 12);
		HAL_GPIO_WritePin(RUN_GPIO_Port, RUN_Pin, (GPIO_PinState)!HAL_GPIO_ReadPin(RUN_GPIO_Port,RUN_Pin));
		
    osDelay(50);
		//printf("LED is running\n");
		*/

  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */
/*  void ProcCommand(char*usb_buf, char usb_len)
	{
		float Left_val;
		float Up_val;
		int speed;
		
		printf("ProcCommand usb_buf:%s\n",usb_buf);
		switch(usb_buf[1])
		{
			case 'p'://设置水平角，上下角
				if(sscanf(&usb_buf[2], "%f,%f", &Left_val, &Up_val) == 2)
				SetPosition(Left_val,Up_val);
				break;
			case 't':	// 水平角，上下角，速度
			if(sscanf(&usb_buf[2], "%f,%f,%d", &Left_val, &Up_val,&speed) == 3)
			{		
				Move(Left_val,Up_val,speed);
				printf("Left_val:%f,Up_val:%f,speed:%d\n",Left_val, Up_val,speed);
			}
			break;
			case 's':	// stop		
				StopMove();
			break;
			case 'c':	// 校准
				calibration();
			break;
			default:
			break;
		}
			// 命令收到，更新状态后，再同步序号
		g_snum = usb_buf[0] == '0' ? 0 : 1;
	}	*/	
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
