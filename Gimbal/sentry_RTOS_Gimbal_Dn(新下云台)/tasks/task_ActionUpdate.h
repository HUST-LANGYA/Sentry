#ifndef __TASK_ACTIONUPDATE_H
#define __TASK_ACTIONUPDATE_H

typedef struct
{
	uint8_t Chassis_Mode;
	uint8_t Shoot_Up_Mode;
	uint8_t Gimbal_Up_Mode;
    uint8_t Shoot_Dn_Mode;
	uint8_t Gimbal_Dn_Mode;
} State_t;

//射击模式
#define Shoot_Up_RC 0x01
#define Shoot_Up_PC 0x02
#define Shoot_Up_SLEEP 0x00

//云台模式
#define Gimbal_Up_RC 0x01   //0001b
#define Gimbal_Up_PC 0x02   //0010b
#define Gimbal_Up_DEBUG 0x03 //xx11b
#define Gimbal_Up_SLEEP 0x00  //xx00b

#define Chassis_RC 0x01
#define Chassis_Patrol 0x02
#define Chassis_SLEEP 0x00
#define Chassis_DEBUG 0x03

#define Gimbal_Dn_RC 0x04   //0100b
#define Gimbal_Dn_PC 0x08   //1000b
#define Gimbal_Dn_SLEEP 0x00 //00xxb
#define Gimbal_Dn_DEBUG 0x0C //11xxb

#define Shoot_Dn_RC 0x04 
#define Shoot_Dn_PC 0x08
#define Shoot_Dn_SLEEP 0x00
#define Shoot_Dn_DEBUG 0x0C

void task_ActionUpdate(void);

#endif //__TASK_ACTIONUPDATE_H
