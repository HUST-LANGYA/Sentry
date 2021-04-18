#ifndef __TASK_GIMBAL_H
#define __TASK_GIMBAL_H

//#include "stm32f4xx.h"
//#include "dbus.h"

#define GyroLimit 5000 

void Gimbal_RC_Act(void /*这个到时候用CAN解码从上云台传来的数据*/);
void Gimbal_PC_Act(void);
void Gimbal_PID_Cal(void);

void Gimbal_Limit(void);
void Gimbal_GYRO_cal(void);

void Pid_Gimbal_Init(void);

float CombPitchOutput(void);
float CombYawOutput(void);


#endif //v
