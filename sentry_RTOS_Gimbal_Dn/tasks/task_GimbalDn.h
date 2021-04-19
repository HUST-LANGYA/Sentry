#ifndef __TASK_GIMBALUP_H
#define __TASK_GIMBALUP_H

#define gyroLimitYaw   30000  //Yaw�������ֵ�޷�
#define gyroLimitPitch 30000  //Pitch�������ֵ�޷�

void task_GimbalDn(void* parameter);


//******************�ڲ���������***********************************************//
static float CombPitchOutput(void); //��ȡ�˲����pitch�Ƕȵĺ���
static float CombYawOutput(void);   //��ȡ�˲����yaw�Ƕȵĺ���
static void Gimbal_Limit(void);//��̨�Ƕ��޷�����
static void PID_Gimbal_Init(void);
static void Gimbal_GYRO_Cal(void);

static void Gimbal_RC_Act(void);
static void Gimbal_PC_Act(void);
static void Gimbal_SLEEP_Act(void);
static void Gimbal_DEBUG_Act(void);

static void Gimbal_RC_PID_Cal(void);
static void Gimbal_PC_PID_Cal(void);
inline static void Gimbal_SLEEP_PID_Cal(void);

#endif //__TASK_GIMBALUP_H
