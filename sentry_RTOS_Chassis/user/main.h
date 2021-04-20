#ifndef __MAIN_H
#define __MAIN_H

#define ABS(x) ((x)>0? (x):(-(x))) 
#define LIMIT_MAX_MIN(x, max, min)	(((x) <= (min)) ? (min):(((x) >= (max)) ? (max) : (x)))

#define SENTRY_SCANNING 0   //哨兵巡逻模式


//底盘运动方向
#define FORWARD  0x00
#define BACKWARD 0x01

#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

//Hardware
#include "HW_TIM2_delay.h"
#include "HW_TIM_EncoderZ.h"  //TIM1
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
#include "HW_IWDG.h"
//#include "HW_USART2_judge.h"
#include "HW_Judgement_Decode.h"
//#include "bsp_USART2.h"
//#include "bsp_USART3_Remote.h"
#include "HW_USART1_debugPC.h"

//Algorithm
#include "algo_PID.h"
#include "algo_CRC.h"
#include "algo_HeatControl.h"


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
#include "task_LimitSwRead.h"
#include "taskIT_DataReceive.h"
#include "task_JudgeDecode.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include "start_task.h"


typedef struct
{
	uint8_t MovingDir;
	int32_t Journey;
    uint8_t is_shootable; 
	uint8_t is_heat_ok;
	uint8_t PC_DownFlag;	//关机标志
	uint8_t State;
	float BulletSpeedSet;
	uint8_t position;   	//0 1 2
	uint8_t HurtDir;		
	uint8_t aim_pos;
	uint8_t hero_alarm;
	int32_t FrictionRealSpeed[2];
	int16_t KS_103_DIS;
}Robot;//这个结构体感觉跟裁判系统的返回信息有关系

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
