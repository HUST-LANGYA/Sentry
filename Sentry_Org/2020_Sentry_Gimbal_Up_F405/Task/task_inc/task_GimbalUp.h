#ifndef __TASK_GIMBALUP_H
#define __TASK_GIMBALUP_H

#define gyroLimitYaw   30000
#define gyroLimitPitch 5000 

void Gimbal_RC_Act(void);
void Gimbal_PC_Act(void);

void Gimbal_Pid_Cal(void);

void Gimbal_Limit(void);
void Gimbal_GYRO_cal(void);

void Pid_Gimbal_Init(void);

// float CombPitchOutput(void);
// float CombYawOutput(void);

#endif //__TASK_GIMBALUP_H
