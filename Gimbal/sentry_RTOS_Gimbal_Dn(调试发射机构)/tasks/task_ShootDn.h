#ifndef __TASK_SHOOT_H
#define __TASK_SHOOT_H

#define FrictionWheel_L_Speed_Low 8500//15000
#define FrictionWheel_L_Speed_High 13000
#define FrictionWheel_L_Speed_Off 0

#define FrictionWheel_R_Speed_Low 8500//15000
#define FrictionWheel_R_Speed_High 13000
#define FrictionWheel_R_Speed_Off 0

#define BodanCurrentLimit 10000  //10000
#define FrictionCurrentLimit  10000//12000  //10000

typedef struct
{
	uint16_t Angle_ABS; //0~8191(0x1fff)
	int32_t Angle_Inc;
	uint16_t LastAngle;
	int16_t RealSpeed; //     RPM
	PID_Typedef pid_speed;
	PID_Typedef pid_pos;
	ZeroCheck_Typedef zerocheck;
	int16_t I_Set;
	uint8_t is_finish;
} _2006_motor_t;

//typedef struct
//{
//	uint16_t angle; //0~8191(0x1fff)
//	int16_t real_speed;	  //
//	PID_Typedef pid;
//	int16_t I_Set;
//} _3510_motor_t;

void task_ShootDn(void *parameter);
void PID_Shoot_Init(void);
#endif //__TASK_SHOOT_H
