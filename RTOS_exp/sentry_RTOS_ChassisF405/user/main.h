#ifndef __MAIN_H
#define __MAIN_H

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

/* exact-width unsigned integer types */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef unsigned char bool_t;
typedef float fp32;
typedef double fp64;

#define ABS(x) ((x)>0? (x):(-(x))) 
#define LIMIT_MAX_MIN(x, max, min)	(((x) <= (min)) ? (min):(((x) >= (max)) ? (max) : (x)))


#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


//Hardware
#include "HW_TIM2_delay.h"
#include "HW_EXTI_EncoderZ.h"
#include "HW_TIM3_Encoder.h"
#include "HW_TIM5_runtimeCPU.h"
#include "HW_DBUS.h"
#include "HW_Encoder.h"
#include "HW_INA260.h"
#include "HWbsp_IIC.h"
#include "HW_LED.h"
#include "HW_PhotoelectricSwitch.h"
#include "HW_LimitSwitch.h"
#include "HW_CAN.h"
#include "HW_USART1_debugPC.h"
//#include "HW_USART2_judge.h"
#include "HW_Judgement_Decode.h"
//#include "bsp_USART2.h"
//#include "bsp_USART3_Remote.h"
#include "HW_IWDG.h"


//Algorithm
#include "algo_PID.h"
#include "algo_CRC.h"

//Task
#include "task_ZeroCheck.h"
#include "task_ActionUpdate.h"
#include "task_DataSend.h"
#include "task_Chassis.h"
#include "task_BlockDisconnect.h"
#include "task_StateLED.h"
#include "task_UsageCPU.h"
#include "task_PowerReadINA.h"
#include "task_PhotoEleSwRead.h"
#include "task_JudgeDecode.h"

#include "taskIT_DataReceive.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include "start_task.h"

//typedef struct
//{
//	int16_t ChassisMode;
//	int16_t ShootMode;
//	int16_t GimbalMode;
//}Status_t;

/*inline */TickType_t GetSysCnt(void);

    
////通用参数
//extern Robot Sentry;
//extern block_disconnect_t block_disconnect;
//extern Status_t Sentry_State;

////功率参数
//extern float PowerNow;
//extern PID_Typedef PowerLimit;

#endif //__MAIN_H
