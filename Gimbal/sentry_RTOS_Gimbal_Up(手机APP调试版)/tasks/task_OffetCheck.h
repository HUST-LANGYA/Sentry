#ifndef _TASK_OFFETCHECK_H
#define _TASK_OFFETCHECK_H


#define NUMBER_SEND 60



////Can通信检测模块
//typedef struct
//{
//	//Pitch电机通信判断
//  uint8_t Can_MotoPitch_Rec_Flag;
//	uint32_t Can_MotoPitch_Rec_CurNum;
//	uint32_t Can_MotoPitch_Rec_LastNum;
//	
//	uint8_t Can_MotoPitch_Send_Flag;
//	uint32_t Can_MotoPitch_Send_CurNum;
//	uint32_t Can_MotoPitch_Send_LastNum;
//	
//	//Yaw电机通信判断
//	uint8_t Can_MotoYaw_Rec_Flag;
//	uint32_t Can_MotoYaw_Rec_CurNum;
//	uint32_t Can_MotoYaw_Rec_LastNum;
//	
//	uint8_t Can_MotoYaw_Send_Flag;
//	uint32_t Can_MotoYaw_Send_CurNum;
//	uint32_t Can_MotoYaw_Send_LastNum;
//	
//	//陀螺仪通信判断
//	uint8_t Can_Gyro_Pitch_Flag;
//	uint32_t Can_Gyro_Pitch_CurNum;
//	uint32_t Can_Gyro_Pitch_LastNum;
//	
//	uint8_t Can_Gyro_Yaw_Flag;
//	uint32_t Can_Gyro_Yaw_CurNum;
//	uint32_t Can_Gyro_Yaw_LastNum;
//	
//	//和底盘的通信判断
//	uint8_t Can_Chassis_Rec_Flag;
//	uint32_t Can_Chassis_Rec_CurNum;
//	uint32_t Can_Chassis_Rec_LastNum;
//	
//	uint8_t Can_Remote_Send_Flag;
//	uint32_t Can_Remote_Send_CurNum;
//	uint32_t Can_Remote_Send_LastNum;
//	
//	//拨弹电机通信判断
//	uint8_t Can_BodanMoto_Rec_Flag;
//	uint32_t Can_BodanMoto_Rec_CurNum;
//	uint32_t Can_BodanMoto_Rec_LastNum;
//	
//	uint8_t Can_BodanMoto_Send_Flag;
//	uint32_t Can_BodanMoto_Send_CurNum;
//	uint32_t Can_BodanMoto_Send_LastNum;
//	
//	//摩擦轮通信判断
//	uint8_t Can_FricWheel_Rec_Flag;
//	uint32_t Can_FricWheel_Rec_CurNum;
//	uint32_t Can_FricWheel_Rec_LastNum;
//	
//	uint8_t Can_FricWheel_Send_Flag;
//	uint32_t Can_FricWheel_Send_CurNum;
//	uint32_t Can_FricWheel_Send_LastNum;
//	

//}Can_Offet_t;	







////Uart通信检测模块
//typedef struct 
//{
//	//和miniPC的通信
//  uint8_t Uart_PC_Rec_Flag;
//	uint32_t Uart_PC_Rec_CurTime;
//	uint32_t Uart_PC_Rec_LastTime;
//	
//	uint8_t Uart_PC_Send_Flag;
//	uint32_t Uart_PC_Send_CurTime;
//	uint32_t Uart_PC_Send_LastTime;
//	//遥控器通信
//	uint8_t Uart_RC_Rec_Flag;
//	uint32_t Uart_RC_Rec_CurTime;
//	uint32_t Uart_RC_Rec_LastTime;
//	
//	uint8_t Uart_RC_Send_Flag;
//	uint32_t Uart_RC_Send_CurTime;
//	uint32_t Uart_RC_Send_LastTime;

//}Uart_Offet_t;


//typedef struct 
//{
//  Can_Offet_t Can_Offet;
//  Uart_Offet_t Uart_Offet;
//  uint8_t Send_Cmd;
//}Com_Offet_t;



void Offet_Task(void*pvParameters);
//void Offet_Init(void);
//void Offet_Check(void);
//void Offet_Send(void);



#endif

