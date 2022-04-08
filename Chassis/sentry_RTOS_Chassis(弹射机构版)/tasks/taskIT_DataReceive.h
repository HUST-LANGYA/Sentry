#ifndef __TASK_DATA_RECEIVE_H
#define __TASK_DATA_RECEIVE_H

//���̽��ձ��ĵ�ID
#define MOTOR_CHASSIS_ID 0x201   //���̵��ID��	 
#define motor_gearbox_ID  0x203   //2006���������
#define BodanMotor_Up_ID 0x203     //�ϲ��������ID��
#define BodanMotor_Dn_ID 0x204     //�ϲ��������ID��
#define Remote_Control_ID  0x705   //����ң�������ݵı�ͷID

//��CAN������ң�����ṹ��
typedef struct
{
    int8_t s1,s2;
    int16_t ch[4];
}REMOTE_t;

void CAN1_DataReceive_0(void);
void CAN1_DataReceive_1(void);
void CAN2_DataReceive_0(void);
void CAN2_DataReceive_1(void);

////�����USART1�������ж�ʱ����ĺ���,�����ϵͳ��(�Ѹ㣨����գࣩ�g)
//void JudgeBuffReceive(uint8_t ReceiveBuffer[]);



float GetPower(void);

//void RCdata2state(void);
#endif //__TASK_DATA_RECEIVE_H
