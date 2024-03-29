/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       main.c/h
  * @brief      stm32初始化以及开始任务freeRTOS。
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. 完成
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */
#ifndef MAIN_H
#define MAIN_H

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

////云台电机可能can发送失败的情况，尝试使用随机延迟发送控制指令的方式解决
//#define GIMBAL_MOTOR_6020_CAN_LOSE_SLOVE 0


#define Latitude_At_ShenZhen 22.57025f

//#ifndef NULL
//#define NULL 0
//#endif

#ifndef PI
#define PI 3.14159265358979f
#endif


#define ABS(x) ((x)>0? (x):(-(x))) 
#define LIMIT_MAX_MIN(x, max, min)	(((x) <= (min)) ? (min):(((x) >= (max)) ? (max) : (x)))  //限幅宏函数



//-------------------------------------头文件包含---------------------------------------//
//Standard Lib
#include <stm32f4xx.h>	 
#include <stm32f4xx_conf.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//hardware
#include "HW_CAN.h"
#include "HW_USART2_PC.h"
#include "HW_USART3_RC.h"
#include "HW_UART4_debugPC.h"
#include "HW_TIM8_pwm.h"
#include "HW_TIM2_delay.h"
#include "HW_TIM5_runtimeCPU.h"
#include "HW_GPIO_LED.h"
#include "HW_DBUS.h"
#include "HW_FrictionWheel.h"
#include "HW_IWDG.h"

//Algorithm
#include "algo_PID.h"
#include "algo_CRC.h"
#include "algo_ADT_LoopQueue.h"
#include "algo_FIR.h"

//tasks
#include "task_ZeroCheck.h"
#include "task_ActionUpdate.h"
#include "task_DataSend.h"
#include "task_GimbalUp.h"
#include "task_ShootUp.h"
#include "task_StateLED.h"
#include "task_UsageCPU.h"
#include "task_BlockDisconnect.h"
#include "DataSendTask.h"

#include "taskIT_DataReceive.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include "start_task.h"


//-------------------结构体定义------------------//
typedef struct
{
  uint16_t Angle_ABS; //角度
  int16_t real_flow;  //实际转矩电流测量
  int16_t set_flow;	//这个本来也是转矩电流的设定值hhhh
  PID_Typedef PidSpeed;
  PID_Typedef PidPos;
  PID_Typedef PidSpeedV;      //视觉用的PID参数
  PID_Typedef PidPosV;
  ZeroCheck_Typedef zerocheck;
  int32_t Angle_Inc;  //过零检测后得到的角度
  int16_t I_Set;      //转矩电流设定  （估计这个命名是历史遗留的，有机会把它跟上面的实际转矩电流统一一下吧）
  int16_t real_speed;  //实际转速 单位rpm
} gimbal_motor_t;


/*inline */TickType_t GetSysCnt(void);
    
#endif /* __MAIN_H */
