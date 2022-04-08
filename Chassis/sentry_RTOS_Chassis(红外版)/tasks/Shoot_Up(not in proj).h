#ifndef __TASK_SHOOT_H
#define __TASK_SHOOT_H

#define BodanCurrentLimit 1000000

//射击模式
#define Shoot_Up_RC 0x01
#define Shoot_Up_PC 0x02
#define Shoot_Up_SLEEP 0x00

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
}_2006_motor_t;//拨弹电机结构体


void Shoot_Fire_Cal(void);//射击模式
void BodanMotor_Init(void);//拨弹电机的PID参数初始化
void BlockDetect(void);//这个暂时先不管

#endif //__TASK_SHOOT_H
