#include "main.h"
#include "task_DataSend.h"

extern State_t Sentry_State;
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

	tx_message.Data[4] = (uint8_t)((bodanVal >> 8) & 0xff);
	tx_message.Data[5] = (uint8_t)(bodanVal & 0xff);
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

	tx_message.Data[4] = (uint8_t)((bodanVal >> 8) & 0xff);
	tx_message.Data[5] = (uint8_t)(bodanVal & 0xff);
	CAN_Transmit(CAN2, &tx_message);
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

	tx_message.Data[2] = (uint8_t)((yaw >> 8) & 0xff);
	tx_message.Data[3] = (uint8_t)(yaw & 0xff);
	tx_message.Data[0] = (uint8_t)((pitch >> 8) & 0xff);
	tx_message.Data[1] = (uint8_t)(pitch & 0xff);
	CAN_Transmit(CAN2, &tx_message);
}


/**
  * @brief  给到底盘和下云台主控板的遥控数据(MD直接把遥控器的原始数据给过去吧，压缩一下给过去)
  *         然后在下云台和底盘那里各写各的解码函数
  * @param  None
  * @retval None
  */
CanTxMsg CAN1_tx_message_remote;
extern uint8_t armor_state;   //表示辅瞄是不是有找到目标

void Remote_Can1Send(void)
{
    extern RC_Ctl_t RC_Ctl ;
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
        CAN1_tx_message_remote.Data[6] = (uint8_t)armor_state;
//        //Data[6]和Data[7]暂时预留
//        CAN1_tx_message_remote.Data[6] = 0;
//        CAN1_tx_message_remote.Data[7] = 0;    
        CAN_Transmit(CAN1, &CAN1_tx_message_remote);
    }
}
