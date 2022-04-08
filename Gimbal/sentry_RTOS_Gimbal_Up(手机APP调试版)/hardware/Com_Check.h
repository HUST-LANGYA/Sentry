#ifndef COM_CHECK_H
#define COM_CHECK_H


/****************************************************************************
* �ļ�����: ʵ�ֶ����м�⺯���Ķ������ؼ��ID�ŵĶ���
* ��    ��: Hellocoded
* ʱ    ��: 2021.11.20
*****************************************************************************/



#include "main.h"


//ͨ�ż��ID��(���������л���ڵ�ID��)

#define CAN_MOTOR_PITCH 1
#define CAN_MOTOR_YAW   2
#define CAN_GYRO_PITCH  3
#define CAN_GYRO_YAW    4
#define CAN_CHASSIS     5  //��̨�͵���֮���ͨ��
#define CAN_MOTOR_BODAN 6
#define CAN_MOTOR_FRIC  7

#define UART_PC         10
#define UART_REMOTE     11
#define UART_JUDGE      12

#define MAX_NUM         10

//ͨ�ż��ģ��
typedef struct
{
	uint32_t BullectCnt;

	uint32_t Pitch_Rec_Last_Cnt;  //���ռ���
	uint32_t Yaw_Rec_Last_Cnt;
	uint32_t Chassis_Rec_Last_Cnt;
	uint32_t Bodan_Rec_Last_Cnt;
	uint32_t Gyro_Rec_Last_Cnt;
	uint32_t PC_Rec_Last_Cnt;
	uint32_t Judge_Rec_Last_Cnt;
	uint32_t FrictionLeft_Rec_Last_Cnt;
	uint32_t FrictionRight_Rec_Last_Cnt;
	uint32_t Bullect_Rec_Last_Cnt;
	uint32_t Drone_Rec_Last_Cnt;
	uint32_t Remote_Rec_Last_Cnt;

	uint32_t Pitch_Send_Last_Cnt;   //���ͼ���
	uint32_t Yaw_Send_Last_Cnt;
	uint32_t Chassis_Send_Last_Cnt;
	uint32_t Bodan_Send_Last_Cnt;
	uint32_t Gyro_Send_Last_Cnt;
	uint32_t PC_Send_Last_Cnt;
	uint32_t Judge_Send_Last_Cnt;
	uint32_t FrictionLeft_Send_Last_Cnt;
	uint32_t FrictionRight_Send_Last_Cnt;
	uint32_t Bullect_Send_Last_Cnt;
	uint32_t Drone_Send_Last_Cnt;
	
	uint32_t Pitch_Rec_Disconnect_Cnt;  //���յ��߼���
	uint32_t Yaw_Rec_Disconnect_Cnt;
	uint32_t Chassis_Rec_Disconnect_Cnt;
	uint32_t Bodan_Rec_Disconnect_Cnt;
	uint32_t Gyro_Rec_Disconnect_Cnt;
	uint32_t PC_Rec_Disconnect_Cnt;
	uint32_t Judge_Rec_Disconnect_Cnt;
	uint32_t FrictionLeft_Rec_Disconnect_Cnt;
	uint32_t FrictionRight_Rec_Disconnect_Cnt;
	uint32_t NoBullect_Rec_Cnt;
	uint32_t Drone_Disconnect_Rec_Cnt;
	uint32_t Remote_Rec_Disconnect_Cnt;

  uint32_t Pitch_Send_Disconnect_cnt; //���͵��߼���
	uint32_t Yaw_Send_Disconnect_cnt;
  uint32_t Chassis_Send_Disconnect_Cnt;
	uint32_t Bodan_Send_Disconnect_Cnt;
	uint32_t Gyro_Send_Disconnect_Cnt;
	uint32_t PC_Send_Disconnect_Cnt;
	uint32_t Judge_Send_Disconnect_Cnt;
	uint32_t FrictionLeft_Send_Disconnect_Cnt;
	uint32_t FrictionRight_Send_Disconnect_Cnt;
	uint32_t NoBullect_Cnt;
	uint32_t Drone_Disconnect_Send_Cnt;

	uint32_t Pitch_block;
	uint32_t Yaw_block;
	uint32_t Bodan_block;
	uint32_t Friction_block;

	uint8_t is_pitch_rec_down;
	uint8_t is_pitch_send_down;
	uint8_t is_yaw_rec_down;
	uint8_t is_yaw_send_down;
	uint8_t is_chassis_rec_down;
	uint8_t is_chassis_send_down;
	uint8_t is_bodan_rec_down;
	uint8_t is_bodan_send_down;
	uint8_t is_gyro_rec_down;
	uint8_t is_gyro_send_down;
	uint8_t is_pc_rec_down;
	uint8_t is_pc_send_down;
	uint8_t is_judge_rec_down;
	uint8_t is_judge_send_down;
	uint8_t is_friction_rec_down; 
	uint8_t is_friction_send_down;
	uint8_t is_drone_down;
	uint8_t is_remote_down;

	uint8_t is_pitch_block;  //��ת���
	uint8_t is_yaw_block;
	uint8_t is_bodan_block;
	uint8_t is_friction_block;

	uint8_t is_left_limit, is_right_limit;

	uint8_t is_no_bullet;
	
	/*
	  ��������Լ������������ͨ�Ŷ��󣬲����������ģ����Ӽ���
	 
	*/
	uint8_t Send_Cmd;
} block_disconnet_t;


extern block_disconnet_t block_disconnet;

void Offet_Reset(void);
void Offet_Judge(void);
void Offet_Check_Can_All(uint8_t Check_ID[],uint8_t Rec_Num[],uint8_t Send_Num[],uint8_t Filter_Num[]);
void Offet_Check_Can_One(uint8_t Check_ID,uint8_t Rec_Num,uint8_t Send_Num,uint8_t Filter_Num);
void Offet_Check_Uart_One(uint8_t Check_ID,uint8_t Num);
void Offet_Check_Uart_All(uint8_t Check_ID[],uint8_t Num[]);
void Offet_Send(void);




#endif

