#ifndef __TASK_CHASSIS_H
#define __TASK_CHASSIS_H

#define ChassisCurrentLimit 16000
#define ChassisMaxSpeed 5800
#define ChassisTempSpeed 150
#define L_direction 1
#define R_direction 0

typedef struct
{
	uint16_t angle_abs;//0~8191(0x1fff)
	int16_t real_speed;//ʵ��ת��
    int16_t real_flow; //ʵ�ʵ���
	PID_Typedef pid;
	ZeroCheck_Typedef zerocheck;
	int32_t angle_inc;
}_3508_motor_t;//3508�����ṹ��


typedef struct
{
	uint16_t Angle_ABS; //0~8191(0x1fff)
	int32_t Angle_Inc;
	uint16_t LastAngle;
	int16_t RealSpeed; //RPM
	PID_Typedef pid_speed;
	PID_Typedef pid_pos;
	ZeroCheck_Typedef zerocheck;
	int16_t I_Set;
	uint8_t is_finish;
} _2006_motor_t;



typedef struct
{
    int32_t angle_inc;
    int16_t angle_abs;//0~65535(0xffff)
    ZeroCheck_Typedef zerocheck;
}_encoder_t;

typedef enum 
{
   MovingDir_L=0, //��������
   MovingDir_R=1  //��������
}MovingDir_t;

typedef enum 
{
   RailSeg_L=0,  //�ڹ�����
   RailSeg_M,    //�ڹ���ж�
   RailSeg_R     //�ڹ���Ҷ�
}RailSeg_Flag_t;

void task_Chassis(void* parameter);
void PID_Chassis_ClearError(void);

#endif //__TASK_CHASSIS_H
