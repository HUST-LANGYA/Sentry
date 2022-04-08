#ifndef __TASK_DATASEND_H
#define __TASK_DATASEND_H

//上云台发送数据的报头
#ifndef Remote_Control_ID
#define Remote_Control_ID 0x705  //线上遥控器数据的报头ID
//云台协同
#define GIMBAL_UP_ID 0x305 
#endif 

void Gyro_Can1Send(int16_t pitch, int16_t yaw);
void Bodan_Can1Send(int16_t bodanVal);
void Bodan_Can2Send(int16_t bodanVal);
void Friction_Can1Send(int16_t fric0, int16_t fric1); //发摩擦轮电流
void Remote_Can2Send(void);

#endif //__TASK_DATASEND_H
