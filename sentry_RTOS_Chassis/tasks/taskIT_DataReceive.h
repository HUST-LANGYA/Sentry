#ifndef __TASK_DATA_RECEIVE_H
#define __TASK_DATA_RECEIVE_H

//底盘接收报文的ID
#define MOTOR_CHASSIS_ID 0x201   //底盘电机ID号	 
#define BodanMotor_Up_ID 0x203     //上拨弹电机的ID号
#define Remote_Control_ID  0x705   //线上遥控器数据的报头ID

//从CAN接来的遥控器结构体
typedef struct
{
    int8_t s1,s2;
    int16_t ch[4];
}REMOTE_t;

//yysy,这几个CAN接收的函数本来是在CAN对应的接收中断里面调用的,不应该放到task这一层来
//但由于它里面经常要改,而且前辈们都这样搞的,所以我也就懒得改了
void CAN1_DataReceive_0(void);
void CAN1_DataReceive_1(void);
void CAN2_DataReceive_0(void);
void CAN2_DataReceive_1(void);

////这个是USART1跳接收中断时进入的函数,解裁判系统的(难搞（；′д｀）ゞ)
//void JudgeBuffReceive(uint8_t ReceiveBuffer[]);



float GetPower(void);

//我估计,到时候从裁判系统用USARt接的数据
//void RCdata2state(void);
#endif //__TASK_DATA_RECEIVE_H
