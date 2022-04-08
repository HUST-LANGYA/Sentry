#ifndef __TASK_SHOOT_H
#define __TASK_SHOOT_H

#define BodanCurrentLimit 1000000

//���ģʽ
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
}_2006_motor_t;//��������ṹ��


void Shoot_Fire_Cal(void);//���ģʽ
void BodanMotor_Init(void);//���������PID������ʼ��
void BlockDetect(void);//�����ʱ�Ȳ���

#endif //__TASK_SHOOT_H
