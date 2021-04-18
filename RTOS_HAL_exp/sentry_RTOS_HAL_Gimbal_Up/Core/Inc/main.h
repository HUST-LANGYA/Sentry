/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmsis_os.h"
#include "can.h"
#include "dma.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
    
#include "HW_CAN.h"
#include "HW_USART2_PC.h"
#include "HW_USART3_RC.h"
#include "HW_UART4_debugPC.h"
#include "HW_TIM5_runtimeCPU.h"
#include "HW_GPIO_LED.h"
#include "HW_DBUS.h"
#include "HW_FrictionWheel.h"

#include "algo_PID.h"
#include "algo_CRC.h"
#include "algo_ADT_LoopQueue.h"
#include "algo_FIR.h"

#include "taskIT_DataReceive.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include "start_task.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
//-------------------结构体定�?------------------//
typedef struct
{
  uint16_t Angle_ABS; //角度
  int16_t real_flow;  //实际转矩电流测量
  int16_t set_flow;	//这个本来也是转矩电流的设定�?�hhhh
  PID_Typedef PidSpeed;
  PID_Typedef PidPos;
  PID_Typedef PidSpeedV;      //视觉用的PID参数
  PID_Typedef PidPosV;
  ZeroCheck_Typedef zerocheck;
  int32_t Angle_Inc;  //过零�?测后得到的角�?
  int16_t I_Set;      //转矩电流设定  （估计这个命名是历史遗留的，有机会把它跟上面的实际转矩电流统�?�?下吧�?
  int16_t real_speed;  //实际转�?? 单位rpm
} gimbal_motor_t;



/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define ABS(x) ((x)>0? (x):(-(x))) 
#define LIMIT_MAX_MIN(x, max, min)	(((x) <= (min)) ? (min):(((x) >= (max)) ? (max) : (x)))  //限幅宏函�?
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/*inline */TickType_t GetSysCnt(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
