#ifndef __TASK_GIMBALUP_H
#define __TASK_GIMBALUP_H

#define gyroLimitYaw   30000  //Yaw电机电流值限幅
#define gyroLimitPitch 30000  //Pitch电机电流值限幅

void task_GimbalDn(void* parameter);


//******************内部函数声明***********************************************//
static float CombPitchOutput(void); //获取滤波后的pitch角度的函数
static float CombYawOutput(void);   //获取滤波后的yaw角度的函数
static void Gimbal_Limit(void);//云台角度限幅函数
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
