#include "main.h"
#include "task_DataSend.h"

///**********************************************/
//uint8_t DataScopeSend_Buf[DataScopeSend_BufSize];
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
//extern uint8_t PC_SendBuf[PC_SENDBUF_SIZE];
//extern float Gyro_PitchAngle,Gyro_PitchSpeed,Gyro_YawAngle,Gyro_YawSpeed;
//extern u8 recover_flag;
extern uint8_t SendToPC_Buff[];
extern State_t Sentry_State;
//extern int32_t SetSpeed;
extern int16_t Is_Shoot;
extern float Pitch_Actual, Yaw_Actual;

/**
  * @brief  发送拨弹电机的信息，设置拨弹电机的转速
  * @param  给到拨弹电机的电流值
  * @retval None
  */
void Bodan_Can1Send(int16_t bodanVal)
{
	CanTxMsg tx_message;
	tx_message.IDE = CAN_ID_STD;
	tx_message.RTR = CAN_RTR_DATA;
	tx_message.DLC = 0x08;
	tx_message.StdId = 0x200;

	bodanVal = LIMIT_MAX_MIN(bodanVal, BodanCurrentLimit, -BodanCurrentLimit);

	tx_message.Data[6] = (uint8_t)((bodanVal >> 8) & 0xff);
	tx_message.Data[7] = (uint8_t)(bodanVal & 0xff);
	CAN_Transmit(CAN1, &tx_message);
}
void Bodan_Can2Send(int16_t bodanVal)
{
	CanTxMsg tx_message;
	tx_message.IDE = CAN_ID_STD;
	tx_message.RTR = CAN_RTR_DATA;
	tx_message.DLC = 0x08;
	tx_message.StdId = 0x200;

	bodanVal = LIMIT_MAX_MIN(bodanVal, BodanCurrentLimit, -BodanCurrentLimit);

	tx_message.Data[6] = (uint8_t)((bodanVal >> 8) & 0xff);
	tx_message.Data[7] = (uint8_t)(bodanVal & 0xff);
	CAN_Transmit(CAN2, &tx_message);
}

void Friction_Can1Send(int16_t fric0, int16_t fric1) //发拨弹电流
{
	CanTxMsg tx_message;
	tx_message.IDE = CAN_ID_STD;
	tx_message.RTR = CAN_RTR_DATA;
	tx_message.DLC = 0x08;
	tx_message.StdId = 0x200;

	fric0 = LIMIT_MAX_MIN(fric0, FrictionCurrentLimit, -FrictionCurrentLimit);
	fric1 = LIMIT_MAX_MIN(fric1, FrictionCurrentLimit, -FrictionCurrentLimit);

	tx_message.Data[0] = (uint8_t)((fric0 >> 8) & 0xff);
	tx_message.Data[1] = (uint8_t)(fric0 & 0xff);
	tx_message.Data[2] = (uint8_t)((fric1 >> 8) & 0xff);
	tx_message.Data[3] = (uint8_t)(fric1 & 0xff);

	CAN_Transmit(CAN1, &tx_message);
}

/**
  * @brief  云台电机消息发送	
  * @param  两轴电机转速的设定值
  * @retval None
  */
void Gyro_Can2Send(int16_t pitch, int16_t yaw)
{
	CanTxMsg tx_message;
	tx_message.IDE = CAN_ID_STD;
	tx_message.RTR = CAN_RTR_DATA;
	tx_message.DLC = 0x08;
	tx_message.StdId = 0x1FF;

	yaw = LIMIT_MAX_MIN(yaw, gyroLimitYaw, -gyroLimitYaw);
	pitch = LIMIT_MAX_MIN(pitch, gyroLimitPitch, -gyroLimitPitch);

    tx_message.Data[0] = (uint8_t)((pitch >> 8) & 0xff);
	tx_message.Data[1] = (uint8_t)(pitch & 0xff);
	tx_message.Data[2] = (uint8_t)((yaw >> 8) & 0xff);
	tx_message.Data[3] = (uint8_t)(yaw & 0xff);

	CAN_Transmit(CAN2, &tx_message);
}

/**
  * @brief  给到底盘和下云台主控板的遥控数据(MD直接把遥控器的原始数据给过去吧，压缩一下给过去)
  *         然后在下云台和底盘那里各写各的解码函数
  * @param  None
  * @retval None
  */
CanTxMsg CAN1_tx_message_remote;
void Remote_Can1Send(void)
{
	// !  这个函数把从遥控器接收的数据转发给底盘和下云台
	// !  但这个函数目前有很严重的问题！通信非常的不稳定！
    extern RC_Ctl_t RC_Ctl ;//= getRCData();
    if(RC_Ctl.rc.s1 && RC_Ctl.rc.s2)  //如果是零包就丢掉（虽然我还是没整清楚零包是怎么出现的MD）
    {
        CAN1_tx_message_remote.IDE = CAN_ID_STD;
        CAN1_tx_message_remote.RTR = CAN_RTR_DATA;
        CAN1_tx_message_remote.DLC = 0x08;
        CAN1_tx_message_remote.StdId = Remote_Control_ID;
           
        CAN1_tx_message_remote.Data[0] = (uint8_t)((RC_Ctl.rc.ch0>>3) & 0xff);
        CAN1_tx_message_remote.Data[1] = (uint8_t)((RC_Ctl.rc.ch0<<5)|(RC_Ctl.rc.ch1>>6) & 0xff);
        CAN1_tx_message_remote.Data[2] = (uint8_t)((RC_Ctl.rc.ch1<<2)|(RC_Ctl.rc.ch2>>9) & 0xff);
        CAN1_tx_message_remote.Data[3] = (uint8_t)((RC_Ctl.rc.ch2>>1) & 0xff);
        CAN1_tx_message_remote.Data[4] = (uint8_t)((RC_Ctl.rc.ch2<<7)|(RC_Ctl.rc.ch3>>4) & 0xff);
        CAN1_tx_message_remote.Data[5] = (uint8_t)((RC_Ctl.rc.ch3<<4)|((RC_Ctl.rc.s1<<2) & 0x0C)|((RC_Ctl.rc.s2) & 0x03) & 0xff);

        if(RC_Ctl.rc.s1 ==1 && RC_Ctl.rc.s2 == 1)   CAN1_tx_message_remote.Data[5] = (uint8_t)((RC_Ctl.rc.ch3<<4)|((1<<2) & 0x0C)|((2) & 0x03) & 0xff);
        
//        //Data[6]和Data[7]暂时预留
//        CAN1_tx_message_remote.Data[6] = 0;
//        CAN1_tx_message_remote.Data[7] = 0;    
        CAN_Transmit(CAN1, &CAN1_tx_message_remote);
    }
}
/**
  * @brief  给到底盘和下云台主控板的遥控数据(MD直接把遥控器的原始数据给过去吧，压缩一下给过去)
  *         然后在下云台和底盘那里各写各的解码函数
  * @param  None
  * @retval None
  */
//CanTxMsg CAN1_tx_message_remote;
//void Remote_Can1Send(void)
//{

//	// !  这个函数把从遥控器接收的数据转发给底盘和下云台
//	// !  但这个函数目前有很严重的问题！通信非常的不稳定！
//    RC_Ctl_t RC_Ctl = getRCData();
//    if(RC_Ctl.rc.s1 && RC_Ctl.rc.s2)  //如果是零包就丢掉（虽然我还是没整清楚零包是怎么出现的MD）
//    {
//        CAN1_tx_message_remote.IDE = CAN_ID_STD;
//        CAN1_tx_message_remote.RTR = CAN_RTR_DATA;
//        CAN1_tx_message_remote.DLC = 0x06;
//        CAN1_tx_message_remote.StdId = Remote_Control_ID;

//        CAN1_tx_message_remote.Data[0] = (uint8_t)((RC_Ctl.rc.ch0>>3) & 0xff);
//        CAN1_tx_message_remote.Data[1] = (uint8_t)((RC_Ctl.rc.ch0<<5)|(RC_Ctl.rc.ch1>>6) & 0xff);
//        CAN1_tx_message_remote.Data[2] = (uint8_t)((RC_Ctl.rc.ch1<<2)|(RC_Ctl.rc.ch2>>9) & 0xff);
//        CAN1_tx_message_remote.Data[3] = (uint8_t)((RC_Ctl.rc.ch2>>1) & 0xff);
//        CAN1_tx_message_remote.Data[4] = (uint8_t)((RC_Ctl.rc.ch2<<7)|(RC_Ctl.rc.ch3>>4) & 0xff);
//        CAN1_tx_message_remote.Data[5] = (uint8_t)((RC_Ctl.rc.ch3<<4)|((RC_Ctl.rc.s1<<2) & 0x0C)|((RC_Ctl.rc.s2) & 0x03) & 0xff);
////        //Data[6]和Data[7]暂时预留
////        CAN1_tx_message_remote.Data[6] = 0;
////        CAN1_tx_message_remote.Data[7] = 0;    
//        CAN_Transmit(CAN1, &CAN1_tx_message_remote);
//    }
//}


//void USART3SEND(uint8_t* msg)
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

///**
//  * @brief  向PC发信息
//  * @param  状态值
//  * @retval None
//  */
//void PC_MsgSend(uint8_t State)
//{
//	float TempP, TempY;
//	int16_t floattoshort;
//	PC_SendBuf[0] = '!';
//	PC_SendBuf[1] = State;
//	if (State == PC_SEND_OTHER)
//	{
//		//	Robot* Sentry = GetSentry();
//		int8_t tempshort = 0; //(*Sentry).Journey;
//		PC_SendBuf[2] = (uint8_t)((tempshort >> 8) & 0xff);
//		PC_SendBuf[3] = (uint8_t)(tempshort & 0xff);
//		//	if(JudgeReceive.robot_id == 7)			//RED
//		//	PC_SendBuf[4] = 1;
//		//	else if(JudgeReceive.robot_id == 17)		//BLUE
//		PC_SendBuf[4] = 2;
//		//	else
//		//	PC_SendBuf[4] = 0;					//None

//		PC_SendBuf[5] = 0; //Sentry->hero_alarm;
//	}
//	else if (State == PC_SEND_GYRO)
//	{
//		//	gimbal_motor_t* MotoYaw = GetMotoYaw();
//		//	gimbal_motor_t* MotoPitch = GetMotoPitch();
//		PC_SendBuf[1] = 0x10;
//		TempP = Pitch_Actual; //向下是负变成向下是正 跟陀螺仪一样
//		//TempP = TempP * 360 / 8191;
//		floattoshort = (short)(TempP * 100);
//		PC_SendBuf[2] = (uint8_t)((floattoshort >> 8) & 0xff);
//		PC_SendBuf[3] = (uint8_t)(floattoshort & 0xff);
//		TempY = Yaw_Actual;
//		floattoshort = (short)(TempY * 100);
//		PC_SendBuf[4] = (uint8_t)((floattoshort >> 8) & 0xff);
//		PC_SendBuf[5] = (uint8_t)(floattoshort & 0xff);
//	}
//	Append_CRC8_Check_Sum(PC_SendBuf + 1, PC_SEND_BUF_SIZE - 2);
//	PC_SendBuf[7] = '#';
//    
//    for(int t=0;t<8;t++)
//    {
////        if(t>0&&t<7)
////            PC_SendBuf[t] = 'A'+t-1;        
//        USART_SendData(USART2,PC_SendBuf[t]);
//        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
//    }
//	DMA_Cmd(DMA1_Stream6, ENABLE);
//}

///**
//  * @brief 	给云台板发信息 （猜测是裁判系统模块？）
//  * @param  None
//  * @retval None
//  */
//void SlaveMsgSend(void)
//{
//	CanTxMsg tx_message;
//	tx_message.IDE = CAN_ID_STD;
//	tx_message.RTR = CAN_RTR_DATA;
//	tx_message.DLC = 0x08;
//	tx_message.StdId = SLAVE_ID;
//	//
//	tx_message.Data[0] = Sentry_Status.ChassisMode;
//	tx_message.Data[1] = (uint8_t)(SetSpeed >> 8) & 0xff;
//	tx_message.Data[2] = (uint8_t)SetSpeed & 0xff;
//	tx_message.Data[3] = Sentry_Status.GimbalMode;
//	tx_message.Data[4] = Sentry_Status.ShootMode;
//	//		short temp = JudgeReceive.remainEnergy*100;
//	tx_message.Data[5] = Is_Shoot; //剩余能量
//								   //		tx_message.Data[6] = (uint8_t)temp&0xff;
//								   //		tx_message.Data[7] = (recover_flag&0x0f)<<4|(Sentry.drone_alarm&0x0f);
//	CAN_Transmit(CAN1, &tx_message);
//}

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
//		tx_message.Data[0] = (uint8_t)(temp>>8)&0xff;		//剩余能量
//		tx_message.Data[1] = (uint8_t)temp&0xff;
//
//		tx_message.Data[2] = Sentry.State;		//子弹数
//		tx_message.Data[3] = Sentry.HurtDir;
//		tx_message.Data[4] = Sentry.aim_pos;		//子弹数
//		tx_message.Data[5] = Sentry.hero_alarm;
//		temp = JudgeReceive.bulletSpeed17;
//		tx_message.Data[6] = (uint8_t)(temp>>8)&0xff;		//子弹数
//		tx_message.Data[7] = (uint8_t)temp&0xff;
//
//		CAN_Transmit(CAN1,&tx_message);
//}
