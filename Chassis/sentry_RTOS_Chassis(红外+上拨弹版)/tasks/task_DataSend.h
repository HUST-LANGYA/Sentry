#ifndef __TASK_DATA_SEND_H
#define __TASK_DATA_SEND_H

#define ChassisCurrentLimit 16000  //电源管理模块的限制输出电流是10A
#define BodanCurrentLimit 7500

//底盘发送报文的ID
#define SHOOTING_HEAT_ID 0x706//发送枪口热量数据的报头ID号

void Chassis_Bodan_CAN1Send(int16_t ChassisI_Set,int16_t BodanI_Set);
void Chassis_CAN1Send(int16_t I_Set);
void Bodan_CAN2Send(int16_t I_Set);
void ShootingHeat_CAN2Send(void);

//void MasterSend(void);
//可能还要发其他的裁判系统数据,做好心理准备吧

#endif  //__TASK_DATA_SEND_H

