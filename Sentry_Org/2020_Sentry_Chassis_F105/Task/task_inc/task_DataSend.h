#ifndef __TASK_DATA_SEND_H
#define __TASK_DATA_SEND_H

#define ChassisCurrentLimit 16000

//底盘发送报文的ID
#define SHOOTING_HEAT_ID 0x706//发送枪口热量数据的报头ID号

void ChassisISend(int16_t ChassisI_Set,int16_t BodanI_Set);
void ShootingHeat_CAN2Send(void);
//void MasterSend(void);
//可能还要发其他的裁判系统数据,做好心理准备吧

#endif  //__TASK_DATA_SEND_H

