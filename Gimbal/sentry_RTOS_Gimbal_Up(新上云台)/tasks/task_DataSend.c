#include "main.h"
#include "task_DataSend.h"

extern State_t Sentry_State;
extern float Pitch_Actual, Yaw_Actual;
CanTxMsg tx_message;
CanTxMsg tx_message_gimbal;
/**
  * @brief  发送拨弹电机的信息，设置拨弹电机的转速
  * @param  给到拨弹电机的电流值
  * @retval None
  */
void Bodan_Can1Send(int16_t bodanVal)
{
	//CanTxMsg tx_message;
	tx_message.IDE = CAN_ID_STD;
	tx_message.RTR = CAN_RTR_DATA;
	tx_message.DLC = 0x08;
	tx_message.StdId = 0x200;

	bodanVal = LIMIT_MAX_MIN(bodanVal, BodanCurrentLimit, -BodanCurrentLimit);

	tx_message.Data[4] = (uint8_t)((bodanVal >> 8) & 0xff);
	tx_message.Data[5] = (uint8_t)(bodanVal & 0xff);
	CAN_Transmit(CAN1, &tx_message);
}
void Bodan_Can2Send(int16_t bodanVal)
{
	//CanTxMsg tx_message;
	tx_message.IDE = CAN_ID_STD;
	tx_message.RTR = CAN_RTR_DATA;
	tx_message.DLC = 0x08;
	tx_message.StdId = 0x200;

	bodanVal = LIMIT_MAX_MIN(bodanVal, BodanCurrentLimit, -BodanCurrentLimit);

	tx_message.Data[4] = (uint8_t)((bodanVal >> 8) & 0xff);
	tx_message.Data[5] = (uint8_t)(bodanVal & 0xff);
	CAN_Transmit(CAN2, &tx_message);
}

/**
  * @brief  发送摩擦轮电机的信息，设置摩擦轮电机的转速
  * @param  给到摩擦轮电机的电流值
  * @retval None
  */


void Friction_Can1Send(int16_t fric0, int16_t fric1) //发摩擦轮电流
{
	//CanTxMsg tx_message;
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
uint16_t TX_Number=1;
TickType_t Last_Tick,Now_Tick,Interval;

void Gyro_Can1Send(int16_t pitch, int16_t yaw)
{
	//CanTxMsg tx_message;
	Last_Tick = Now_Tick;
	Now_Tick = xTaskGetTickCount();
	Interval = Now_Tick - Last_Tick;
	
	tx_message_gimbal.IDE = CAN_ID_STD;
	tx_message_gimbal.RTR = CAN_RTR_DATA;
	tx_message_gimbal.DLC = 0x08;
	tx_message_gimbal.StdId = 0x1FF;

	yaw = LIMIT_MAX_MIN(yaw, gyroLimitYaw, -gyroLimitYaw);
	pitch = LIMIT_MAX_MIN(pitch, gyroLimitPitch, -gyroLimitPitch);

//	pitch = -20000;
	tx_message_gimbal.Data[0] = (uint8_t)((pitch >> 8) & 0xff);
	tx_message_gimbal.Data[1] = (uint8_t)(pitch & 0xff);
	tx_message_gimbal.Data[2] = (uint8_t)((yaw >> 8) & 0xff); //
	tx_message_gimbal.Data[3] = (uint8_t)(yaw & 0xff);
	TX_Number = !TX_Number;
	CAN_Transmit(CAN1, &tx_message_gimbal);
}


/**
  * @brief  给到底盘和下云台主控板的遥控数据(MD直接把遥控器的原始数据给过去吧，压缩一下给过去)
  *         然后在下云台和底盘那里各写各的解码函数
  * @param  None
  * @retval None
  */
CanTxMsg CAN2_tx_message_remote;
CanTxMsg CAN2_tx_down;
extern uint8_t armor_state;   //表示辅瞄是不是有找到目标

void Remote_Can2Send(void)
{
    extern RC_Ctl_t RC_Ctl ;
    if(RC_Ctl.rc.s1 && RC_Ctl.rc.s2)  //如果是零包就丢掉（虽然我还是没整清楚零包是怎么出现的MD）
    {
        CAN2_tx_message_remote.IDE = CAN_ID_STD;
        CAN2_tx_message_remote.RTR = CAN_RTR_DATA;
        CAN2_tx_message_remote.DLC = 0x08;
        CAN2_tx_message_remote.StdId = Remote_Control_ID;

        CAN2_tx_message_remote.Data[0] = (uint8_t)((RC_Ctl.rc.ch0>>3) & 0xff);
        CAN2_tx_message_remote.Data[1] = (uint8_t)((RC_Ctl.rc.ch0<<5)|(RC_Ctl.rc.ch1>>6) & 0xff);
        CAN2_tx_message_remote.Data[2] = (uint8_t)((RC_Ctl.rc.ch1<<2)|(RC_Ctl.rc.ch2>>9) & 0xff);
        CAN2_tx_message_remote.Data[3] = (uint8_t)((RC_Ctl.rc.ch2>>1) & 0xff);
        CAN2_tx_message_remote.Data[4] = (uint8_t)((RC_Ctl.rc.ch2<<7)|(RC_Ctl.rc.ch3>>4) & 0xff);
        CAN2_tx_message_remote.Data[5] = (uint8_t)((RC_Ctl.rc.ch3<<4)|((RC_Ctl.rc.s1<<2) & 0x0C)|((RC_Ctl.rc.s2) & 0x03) & 0xff);
        //CAN2_tx_message_remote.Data[6] = (uint8_t)armor_state;
//        //Data[6]和Data[7]暂时预留
//        CAN1_tx_message_remote.Data[6] = 0;
//        CAN1_tx_message_remote.Data[7] = 0;    
        CAN_Transmit(CAN2, &CAN2_tx_message_remote);
    }
}



/**************************************************************
* function：给下云台发送瞄准到的角度值
*
***************************************************************/
short send_pitch;
int   send_yaw;
void Down_CAN2Send(void)
{
   //extern Down_Receive_t Down_Receive_Now;
	// extern float aim_yaw, aim_pitch;
	 extern int pc_yaw_down;
   extern short pc_pitch_down;
   //extern uint8_t Send_Up_buffer[Send_Up_Size];
	 extern uint8_t last_armor_state;
	 extern uint8_t armor_state;
	
	 send_pitch = (short)(pc_pitch_down);
	 send_yaw = (int)(pc_yaw_down);
	
	 CAN2_tx_down.IDE = CAN_ID_STD;
	 CAN2_tx_down.RTR = CAN_RTR_DATA;
	 CAN2_tx_down.DLC = 0x08;
	 CAN2_tx_down.StdId = GIMBAL_UP_ID;
	 
	 CAN2_tx_down.Data[0] = (uint8_t)(send_pitch >> 8 & 0x00FF);
	 CAN2_tx_down.Data[1] = (uint8_t)(send_pitch & 0x00FF);
	 CAN2_tx_down.Data[2] = (uint8_t)(send_yaw >> 24 & 0x000000FF);
	 CAN2_tx_down.Data[3] = (uint8_t)(send_yaw >> 16 & 0x000000FF);
	 CAN2_tx_down.Data[4] = (uint8_t)(send_yaw >> 8 & 0x000000FF);
	 CAN2_tx_down.Data[5] = (uint8_t)(send_yaw & 0x000000FF);
	 //CAN2_tx_down.Data[6] = (uint8_t)(last_armor_state==ARMOR_NO_AIM && armor_state == ARMOR_AIMED);//瞄准状态上升沿时数据数据才是有效的
	
	 //last_armor_state = armor_state;
	 CAN_Transmit(CAN2, &CAN2_tx_down);
//	 Send_Up_buffer[0] = '!';
//	 Send_Up_buffer[1] = (unsigned char)(send_pitch >> 8 & 0x00FF);
//   Send_Up_buffer[2] = (unsigned char) (send_pitch & 0x00FF);
//	
//	 Send_Up_buffer[3] = (unsigned char)(send_yaw >> 24 & 0x000000FF);
//	 Send_Up_buffer[4] = (unsigned char)(send_yaw >> 16 & 0x000000FF);
//	 Send_Up_buffer[5] = (unsigned char)(send_yaw >> 8 & 0x000000FF);
//	 Send_Up_buffer[6] = (unsigned char)(send_yaw >> 0 & 0x000000FF);
//	 Send_Up_buffer[7] = (unsigned char) (last_armor_state==ARMOR_NO_AIM && armor_state == ARMOR_AIMED);//瞄准状态上升沿
//	 last_armor_state = armor_state;
//   Append_CRC8_Check_Sum(Send_Up_buffer, Send_Up_Size);
//   DMA_Cmd(DMA1_Stream7,ENABLE);

}

