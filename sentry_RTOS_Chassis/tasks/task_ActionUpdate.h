#ifndef __TASK_ACTIONUPDATE_H
#define __TASK_ACTIONUPDATE_H

#define Down 0x0000


//* 如果有机会 看看能不能把这个状态结构体 做成一个长的信号量
typedef struct
{
	uint8_t Chassis_Mode;
	uint8_t Shoot_Up_Mode;
    uint8_t Shoot_Dn_Mode;
	uint8_t Gimbal_Up_Mode;
    uint8_t Gimbal_Dn_Mode;
} State_t;

#define Chassis_RC 0x01
#define Chassis_Patrol 0x02
#define Chassis_SLEEP 0x00
#define Chassis_DEBUG 0x03


#define Shoot_Up_RC 0x01
#define Shoot_Up_PC 0x02
#define Shoot_Up_SLEEP 0x00
#define Gimbal_Up_RC 0x01
#define Gimbal_Up_PC 0x02
#define Gimbal_Up_SLEEP 0x00

#define Gimbal_Dn_RC 0x04
#define Gimbal_Dn_PC 0x08
#define Gimbal_Dn_SLEEP 0x00
#define Shoot_Dn_RC 0x04
#define Shoot_Dn_PC 0x08
#define Shoot_Dn_SLEEP 0x00

void task_ActionUpdate(void);


static void update_state_with_RCdata(void);
static void sentry_state_reflect(uint8_t gimbal_up_mode,
                                 uint8_t gimbal_dn_mode,
                                 uint8_t shoot_up_mode,
                                 uint8_t shoot_dn_mode,
                                 uint8_t chassis_mode); //reflect当作做一个映射的意思吧

#endif //__TASK_ACTIONUPDATE_H
