#ifndef __TASK_DATASEND_H
#define __TASK_DATASEND_H

//上云台发送数据的报头
#ifndef Remote_Control_ID
#define Remote_Control_ID 0x705  //线上遥控器数据的报头ID
//云台协同工作的
#define GIMBAL_DOWN_ID   0x306

#endif

void Gyro_Can1Send(int16_t pitch, int16_t yaw);
void Bodan_Can1Send(int16_t bodanVal);
void Bodan_Can2Send(int16_t bodanVal);
void Remote_Can1Send(void);
void Down_CAN1Send(void);
void Friction_Can2Send(int16_t fric0, int16_t fric1);

#endif //__TASK_DATASEND_H
