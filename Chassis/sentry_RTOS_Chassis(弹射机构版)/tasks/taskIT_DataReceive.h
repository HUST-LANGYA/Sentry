#ifndef __TASK_DATA_RECEIVE_H
#define __TASK_DATA_RECEIVE_H

//底盘接收报文的ID
#define MOTOR_CHASSIS_ID 0x201   //底盘电机ID号	 
#define motor_gearbox_ID  0x203   //2006电机减速轮
#define BodanMotor_Up_ID 0x203     //上拨弹电机的ID号
#define BodanMotor_Dn_ID 0x204     //上拨弹电机的ID号
#define Remote_Control_ID  0x705   //线上遥控器数据的报头ID

//从CAN接来的遥控器结构体
typedef struct
{
    int8_t s1,s2;
    int16_t ch[4];
}REMOTE_t;

void CAN1_DataReceive_0(void);
void CAN1_DataReceive_1(void);
void CAN2_DataReceive_0(void);
void CAN2_DataReceive_1(void);

////这个是USART1跳接收中断时进入的函数,解裁判系统的(难搞（；′д｀）ゞ)
//void JudgeBuffReceive(uint8_t ReceiveBuffer[]);



float GetPower(void);

//void RCdata2state(void);
#endif //__TASK_DATA_RECEIVE_H
