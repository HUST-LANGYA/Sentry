#ifndef __TASK_DATA_SEND_H
#define __TASK_DATA_SEND_H

#define ChassisCurrentLimit 16000
#define motor_gearbox_CurrentLimit 10000
#define BodanCurrentLimit 7500


//���̷��ͱ��ĵ�ID
#define SHOOTING_HEAT_ID 0x706//����ǹ���������ݵı�ͷID��

void Chassis_Bodan_CAN1Send(int16_t ChassisI_Set,int16_t BodanI_Set);
void Chassis_CAN2Send(int16_t I_Set);
void Deline_CAN2Send(int16_t I_set);
void Bodan_CAN1Send(int16_t I_Set);
void ShootingHeat_CAN2Send(void);

//void MasterSend(void);
//���ܻ�Ҫ�������Ĳ���ϵͳ����,��������׼����

#endif  //__TASK_DATA_SEND_H

