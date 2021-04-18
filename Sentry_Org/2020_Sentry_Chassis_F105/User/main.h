#ifndef __MAIN_H
#define __MAIN_H

#define ABS(x) ((x)>0? (x):(-(x))) 
#define LIMIT_MAX_MIN(x, max, min)	(((x) <= (min)) ? (min):(((x) >= (max)) ? (max) : (x)))

#define SENTRY_SCANNING 0   //哨兵巡逻模式

//底盘模式
#define Chassis_RC 0x01
#define Chassis_PC 0x02
#define Chassis_SLEEP 0x00

//射击模式
#define Shoot_Up_RC     0x01
#define Shoot_Up_PC     0x02
#define Shoot_Up_SLEEP 0x00
#define Shoot_Dn_RC   0x04
#define Shoot_Dn_PC   0x08
#define Shoot_Dn_SLEEP 0x00

//#define Shoot_On    0x1
//#define Shoot_Off   0x0

//云台模式
#define Gimbal_Up_RC    0x01
#define Gimbal_Up_PC    0x02
#define Gimbal_Up_SLEEP 0x00
#define Gimbal_Dn_RC  0x04
#define Gimbal_Dn_PC  0x08
#define Gimbal_Dn_SLEEP 0x00

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

//Algorithm
#include "algo_PID.h"
#include "algo_CRC.h"

//Hardware
#include "bsp_CAN.h"
#include "bsp_USART1_Judge.h"
#include "bsp_USART2.h"
//#include "bsp_USART3_Remote.h"
#include "bsp_UART4.h"
#include "bsp_TIM2_timer.h"
#include "bsp_EXTI.h"
#include "bsp_TIM3_encoder.h"
#include "bsp_GPIO_limit_switch.h"
#include "bsp_GPIO_photoelectric_switch.h"

//Device
#include "dev_Encoder.h"
#include "dev_INA260.h"
#include "dev_IIC.h"
//#include "dev_DBUS.h"  //底盘不直接接收遥控器的值,而是从上云台发过来的CAN报文获取
#include "dev_LED.h"
#include "dev_PhotoelectricSwitch.h"
#include "dev_LimitSwitch.h"

//Task
#include "task_ZeroCheck.h"
#include "task_Chassis.h"
#include "task_DataReceive.h"
#include "task_DataSend.h"
#include "task_Shoot_Up.h"  //现在底盘要管上云台Shoot的拨弹电机了

typedef struct
{
	uint8_t MovingDir;
	int32_t Journey;
    uint8_t is_shoot; 
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
}Robot;

typedef struct
{
	int16_t ChassisMode;
	int16_t ShootMode;
	int16_t GimbalMode;
}Status_t;

uint32_t GetSysCnt(void);
void idelay(uint32_t t);
    
//通用参数
extern Robot Sentry;
extern block_disconnect_t block_disconnect;
extern Status_t Sentry_Status;

//功率参数
extern float PowerNow;
extern PID_Typedef PowerLimit;

#endif //__MAIN_H
