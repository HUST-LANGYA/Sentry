#ifndef __TASK_DATA_RECEIVE_H
#define __TASK_DATA_RECEIVE_H

//���̽��ձ��ĵ�ID
#define MOTOR_CHASSIS_ID   0x201   //���̵��ID��	 
#define BodanMotor_Up_ID   0x203   //�ϲ��������ID��
#define BodanMotor_Dn_ID   0x204   //�ϲ��������ID��
#define Remote_Control_ID  0x705   //����ң�������ݵı�ͷID

void CAN1_DataReceive_0(void);
void CAN1_DataReceive_1(void);
void CAN2_DataReceive_0(void);
void CAN2_DataReceive_1(void);

#endif //__TASK_DATA_RECEIVE_H
