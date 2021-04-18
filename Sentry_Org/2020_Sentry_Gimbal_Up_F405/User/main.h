#ifndef __MAIN_H
#define __MAIN_H

#define ABS(x) ((x)>0? (x):(-(x))) 
#define LIMIT_MAX_MIN(x, max, min)	(((x) <= (min)) ? (min):(((x) >= (max)) ? (max) : (x)))  //限幅宏函数

//Standard Lib
#include <stm32f4xx.h>	 
#include <stm32f4xx_conf.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//Mylib
#include "bsp_CAN.h"
#include "bsp_USART2_PC.h"
#include "bsp_USART3_RC.h"
#include "bsp_UART4.h"//手误，懒得改了 ，应该是usart4.h  //神TM懒得改了2333 ε=ε=ε=(~￣▽￣)~
#include "bsp_TIM4_pwm.h"
#include "bsp_TIM2_delay.h"
#include "bsp_GPIO_LED.h"
//#include "bsp_TIM8_DSHOT.h"

//Algorithm
#include "algo_PID.h"
#include "algo_CRC.h"
#include "algo_ADT_LoopQueue.h"
#include "algo_FIR.h"

//Device
#include "dev_DBUS.h"
#include "dev_FrictionWheel.h"

//Task
#include "task_ZeroCheck.h"
#include "task_DataReceive.h"
#include "task_DataSend.h"
#include "task_GimbalUp.h"
#include "task_Shoot.h"
#include "task_ActionUpdate.h"
#include "task_StateLED.h"

//通用参数
extern Status_t Sentry_Status;
extern block_disconnet_t block_disconnect;

uint32_t GetSysCnt(void);
void delay_ms(uint32_t t);

typedef struct
{
	uint16_t Angle_ABS; //角度
	int16_t real_flow;		  //实际转矩电流测量
	int16_t set_flow;			  //转矩电流设定
	PID_Typedef PidSpeed;
	PID_Typedef PidPos;
    PID_Typedef PidSpeedV;      //视觉用的PID参数
    PID_Typedef PidPosV;
	ZeroCheck_Typedef zerocheck;
	int32_t Angle_Inc;
	int16_t I_Set;
	int16_t speed;
} gimbal_motor_t;

#endif //__MAIN_H
