#ifndef __TASK_DATA_RECEIVE_H
#define __TASK_DATA_RECEIVE_H

//���̽��ձ��ĵ�ID
#define MOTOR_CHASSIS_ID 0x201   //���̵��ID��	 
#define BodanMotor_Up_ID 0x203     //�ϲ��������ID��
#define Remote_Control_ID  0x705   //����ң�������ݵı�ͷID

//��CAN������ң�����ṹ��
typedef struct
{
    int8_t s1,s2;
    int16_t ch[4];
}REMOTE_t;

//yysy,�⼸��CAN���յĺ�����������CAN��Ӧ�Ľ����ж�������õ�,��Ӧ�÷ŵ�task��һ����
//�����������澭��Ҫ��,����ǰ���Ƕ��������,������Ҳ�����ø���
void CAN1_DataReceive_0(void);
void CAN1_DataReceive_1(void);
void CAN2_DataReceive_0(void);
void CAN2_DataReceive_1(void);

////�����USART1�������ж�ʱ����ĺ���,�����ϵͳ��(�Ѹ㣨����գࣩ�g)
//void JudgeBuffReceive(uint8_t ReceiveBuffer[]);



float GetPower(void);

//�ҹ���,��ʱ��Ӳ���ϵͳ��USARt�ӵ�����
//void RCdata2state(void);
#endif //__TASK_DATA_RECEIVE_H
