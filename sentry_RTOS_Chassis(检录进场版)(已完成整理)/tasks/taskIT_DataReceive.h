#ifndef __TASK_DATA_RECEIVE_H
#define __TASK_DATA_RECEIVE_H

//底盘接收报文的ID
#define MOTOR_CHASSIS_ID   0x201   //底盘电机ID号	 
#define BodanMotor_Up_ID   0x203   //上拨弹电机的ID号
#define BodanMotor_Dn_ID   0x204   //上拨弹电机的ID号
#define Remote_Control_ID  0x705   //线上遥控器数据的报头ID

void CAN1_DataReceive_0(void);
void CAN1_DataReceive_1(void);
void CAN2_DataReceive_0(void);
void CAN2_DataReceive_1(void);

#endif //__TASK_DATA_RECEIVE_H
