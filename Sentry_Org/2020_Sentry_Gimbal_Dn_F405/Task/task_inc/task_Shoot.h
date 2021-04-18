#ifndef __TASK_SHOOT_H
#define __TASK_SHOOT_H

#define FrictionSpeed_Low 4000
#define FrictionSpeed_High 12000

#define BodanCurrentLimit 10000
#define FrictionCurrentLimit 16000

typedef struct
{
	uint16_t Angle_ABS;//0~8191(0x1fff)
	int32_t Angle_Inc;
	uint16_t LastAngle;
	int16_t RealSpeed;//     RPM
	PID_Typedef pid_speed;
	PID_Typedef pid_pos;
	ZeroCheck_Typedef zerocheck;
	int16_t I_Set;
	uint8_t is_finish;
}_2006_moto_t;

void Shoot_Fire_Cal(void);
void BodanMoto_Init(void);



#endif //__TASK_SHOOT_H
