#ifndef __TASK_DATA_SEND_H
#define __TASK_DATA_SEND_H

#define ChassisCurrentLimit 16000
#define BodanCurrentLimit 7500

//���̷��ͱ��ĵ�ID
#define SHOOTING_HEAT_ID 0x706//����ǹ���������ݵı�ͷID��

void Chassis_CAN1Send(int16_t I_Set);
void ShootingHeat_CAN2Send(void);

#endif  //__TASK_DATA_SEND_H

