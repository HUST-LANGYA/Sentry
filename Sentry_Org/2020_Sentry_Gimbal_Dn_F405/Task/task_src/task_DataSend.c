#include "main.h"

///**********************************************/
//unsigned char DataScopeSend_Buf[DataScopeSend_BufSize];
//u8 Send_Count =0;
//short floattoshort;
//short ShootAlready = 0;
///*********************extern**********************/
//extern int PITCH_ZERO_POS;//693
//extern tGameInfo JudgeReceive;
//extern block_disconnect_t block_disconnect;
//extern gyro_Typedef Gyro_White;
//extern int MaxSpeed;
//extern float AvgMaxSpeed;
//extern float speed_check;
//extern float Gyro_YawSpeed;
//extern float PC_YawSet,PC_PitchSet;
//extern unsigned char PC_SendBuf[PC_SEND_BUF_SIZE];
//extern float Gyro_PitchAngle,Gyro_PitchSpeed,Gyro_YawAngle,Gyro_YawSpeed;
//extern u8 recover_flag;
extern Status_t Sentry_Status;
extern int SetSpeed;
/**
  * @brief  拨弹电机+摩擦轮电调
  * @param  None
  * @retval None
  */
void Bodan_Can1Send(short c)
{
  CanTxMsg tx_message;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 0x08;
  tx_message.StdId = 0x200;

//  a = LIMIT_MAX_MIN(a, FrictionCurrentLimit, -FrictionCurrentLimit);
//  b = LIMIT_MAX_MIN(b, FrictionCurrentLimit, -FrictionCurrentLimit);
  c = LIMIT_MAX_MIN(c, BodanCurrentLimit, -BodanCurrentLimit);

//  tx_message.Data[0] = (unsigned char)((a >> 8) & 0xff);
//  tx_message.Data[1] = (unsigned char)(a & 0xff);
//  tx_message.Data[2] = (unsigned char)((b >> 8) & 0xff);
//  tx_message.Data[3] = (unsigned char)(b & 0xff);
  tx_message.Data[4] = (unsigned char)((c >> 8) & 0xff);
  tx_message.Data[5] = (unsigned char)(c & 0xff);
  CAN_Transmit(CAN1, &tx_message);
}

/**
  * @brief  云台电机消息发送	
  * @param  发给 pitch 和 yaw 电机的电流值
  * @retval None
  */

void Gyro_Can2Send(int16_t pitch, int16_t yaw)
{
  CanTxMsg tx_message;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 0x08;
  tx_message.StdId = 0x1FF;

  yaw = LIMIT_MAX_MIN(yaw, 30000, -30000);
  pitch = LIMIT_MAX_MIN(pitch, GyroLimit, -GyroLimit);

  tx_message.Data[0] = (unsigned char)((pitch >> 8) & 0xff);
  tx_message.Data[1] = (unsigned char)(pitch & 0xff);

  //    tx_message.Data[2] = (unsigned char)((a>>8)&0xff);
  //    tx_message.Data[3] = (unsigned char)(a&0xff);
  
  tx_message.Data[4] = (unsigned char)((yaw >> 8) & 0xff);
  tx_message.Data[5] = (unsigned char)(yaw & 0xff);
  CAN_Transmit(CAN2, &tx_message);
}

//void USART3SEND(unsigned char* msg)
//{
//		DataScope_Get_Channel_Data(1, 1 );  //??? 1.0  ???? 1
//		DataScope_Get_Channel_Data(2, 2 );  //??? 2.0  ???? 2
//		DataScope_Get_Channel_Data(3, 3 );  //??? 3.0  ???? 3
//		DataScope_Get_Channel_Data(4, 4 );  //??? 4.0  ???? 4
//		DataScope_Get_Channel_Data(5, 5 );  //??? 5.0  ???? 5
//		DataScope_Get_Channel_Data(6, 6 );  //??? 6.0  ???? 6
//		DataScope_Get_Channel_Data(7, 7 );  //??? 7.0  ???? 7
//		DataScope_Get_Channel_Data( 8.0 , 8 );  //??? 8.0  ???? 8
//		DataScope_Get_Channel_Data( 9.0 , 9 );  //??? 9.0  ???? 9
//		DataScope_Get_Channel_Data( 10.0 , 10); //??? 10.0 ???? 10
//		Send_Count = DataScope_Data_Generate(10); //??10???? ??????,???????
//		for( int i = 0 ; i < Send_Count; i++)  //????,??????
//		{
//			while((USART3->SR&0X40)==0);
//			USART3->DR = DataScope_OutPut_Buffer[i]; //????????????
//		}
//}

/**
  * @brief  向PC发信息
  * @param  状态值
  * @retval None
  */
extern uint8_t PC_SendBuf[PC_SENDBUF_SIZE];
extern float Pitch_Actual, Yaw_Actual;
void PC_MsgSend(uint8_t State)
{
	float TempP, TempY;
	int16_t floattoshort;
	PC_SendBuf[0] = '!';
	PC_SendBuf[1] = State;
	if (State == PC_SEND_OTHER)
	{
		//	Robot* Sentry = GetSentry();
		int8_t tempshort = 0; //(*Sentry).Journey;
		PC_SendBuf[2] = (uint8_t)((tempshort >> 8) & 0xff);
		PC_SendBuf[3] = (uint8_t)(tempshort & 0xff);
		//	if(JudgeReceive.robot_id == 7)			//RED
		//	PC_SendBuf[4] = 1;
		//	else if(JudgeReceive.robot_id == 17)		//BLUE
		PC_SendBuf[4] = 2;
		//	else
		//	PC_SendBuf[4] = 0;					//None

		PC_SendBuf[5] = 0; //Sentry->hero_alarm;
	}
	else if (State == PC_SEND_GYRO)
	{
		//	gimbal_motor_t* MotoYaw = GetMotoYaw();
		//	gimbal_motor_t* MotoPitch = GetMotoPitch();
		PC_SendBuf[1] = 0x10;
		TempP = Pitch_Actual; //向下是负变成向下是正 跟陀螺仪一样
		//TempP = TempP * 360 / 8191;
		floattoshort = (short)(TempP * 100);
		PC_SendBuf[2] = (uint8_t)((floattoshort >> 8) & 0xff);
		PC_SendBuf[3] = (uint8_t)(floattoshort & 0xff);
		TempY = Yaw_Actual;
		floattoshort = (short)(TempY * 100);
		PC_SendBuf[4] = (uint8_t)((floattoshort >> 8) & 0xff);
		PC_SendBuf[5] = (uint8_t)(floattoshort & 0xff);
	}
	Append_CRC8_Check_Sum(PC_SendBuf + 1, PC_SEND_BUF_SIZE - 2);
	PC_SendBuf[7] = '#';
    
    for(int t=0;t<8;t++)
    {
//        if(t>0&&t<7)
//            PC_SendBuf[t] = 'A'+t-1;        
        USART_SendData(USART2,PC_SendBuf[t]);
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    }
	DMA_Cmd(DMA1_Stream6, ENABLE);
}

/**
  * @brief 	给云台板发信息
  * @param  None
  * @retval None
  */
void SlaveMsgSend(void)
{
  CanTxMsg tx_message;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 0x08;
  tx_message.StdId = SLAVE_ID;
  //
  tx_message.Data[0] = Sentry_Status.GimbalMode;
  tx_message.Data[1] = (unsigned char)(SetSpeed >> 8) & 0xff;
  tx_message.Data[2] = (unsigned char)SetSpeed & 0xff;
  //		tx_message.Data[3] = Sentry.State;
  //		tx_message.Data[4] = Sentry.aim_pos;
  //		short temp = JudgeReceive.remainEnergy*100;
  //		tx_message.Data[5] = (unsigned char)(temp>>8)&0xff;		//剩余能量
  //		tx_message.Data[6] = (unsigned char)temp&0xff;
  //		tx_message.Data[7] = (recover_flag&0x0f)<<4|(Sentry.drone_alarm&0x0f);
  CAN_Transmit(CAN1, &tx_message);
}

///**
//  * @brief  调试使用 蓝牙
//  * @param  None
//  * @retval None
//  */
//extern int ShootNumtest;
//void BlueMsgSend()
//{
//	  CanTxMsg tx_message;
//    tx_message.IDE = CAN_ID_STD;
//    tx_message.RTR = CAN_RTR_DATA;
//    tx_message.DLC = 0x08;
//    tx_message.StdId = BLUE_ID;
//
//		short temp = JudgeReceive.remainEnergy*100;
//		tx_message.Data[0] = (unsigned char)(temp>>8)&0xff;		//剩余能量
//		tx_message.Data[1] = (unsigned char)temp&0xff;
//
//		tx_message.Data[2] = Sentry.State;		//子弹数
//		tx_message.Data[3] = Sentry.HurtDir;
//		tx_message.Data[4] = Sentry.aim_pos;		//子弹数
//		tx_message.Data[5] = Sentry.hero_alarm;
//		temp = JudgeReceive.bulletSpeed17;
//		tx_message.Data[6] = (unsigned char)(temp>>8)&0xff;		//子弹数
//		tx_message.Data[7] = (unsigned char)temp&0xff;
//
//		CAN_Transmit(CAN1,&tx_message);
//}
