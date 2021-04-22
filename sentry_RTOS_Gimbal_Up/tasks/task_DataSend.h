#ifndef __TASK_DATASEND_H
#define __TASK_DATASEND_H

//上云台发送数据的报头
#ifndef Remote_Control_ID
#define Remote_Control_ID 0x705  //线上遥控器数据的报头ID
#endif 

void Gyro_Can2Send(int16_t pitch, int16_t yaw);
void Bodan_Can1Send(int16_t bodanVal);
void Bodan_Can2Send(int16_t bodanVal);
void Remote_Can1Send(void);

void SlaveMsgSend(void);
void PC_MsgSend(uint8_t State);

#endif //__TASK_DATASEND_H

