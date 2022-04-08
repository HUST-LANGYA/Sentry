#include "main.h"
#include "task_DataSend.h"

extern State_t Sentry_State;
extern float Pitch_Actual, Yaw_Actual;
CanTxMsg tx_message;
CanTxMsg tx_message_gimbal;
/**
  * @brief  ���Ͳ����������Ϣ�����ò��������ת��
  * @param  ������������ĵ���ֵ
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
  * @brief  ����Ħ���ֵ������Ϣ������Ħ���ֵ����ת��
  * @param  ����Ħ���ֵ���ĵ���ֵ
  * @retval None
  */


void Friction_Can1Send(int16_t fric0, int16_t fric1) //��Ħ���ֵ���
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
  * @brief  ��̨�����Ϣ����	
  * @param  ������ת�ٵ��趨ֵ
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
  * @brief  �������̺�����̨���ذ��ң������(MDֱ�Ӱ�ң������ԭʼ���ݸ���ȥ�ɣ�ѹ��һ�¸���ȥ)
  *         Ȼ��������̨�͵��������д���Ľ��뺯��
  * @param  None
  * @retval None
  */
CanTxMsg CAN2_tx_message_remote;
CanTxMsg CAN2_tx_down;
extern uint8_t armor_state;   //��ʾ�����ǲ������ҵ�Ŀ��

void Remote_Can2Send(void)
{
    extern RC_Ctl_t RC_Ctl ;
    if(RC_Ctl.rc.s1 && RC_Ctl.rc.s2)  //���������Ͷ�������Ȼ�һ���û������������ô���ֵ�MD��
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
//        //Data[6]��Data[7]��ʱԤ��
//        CAN1_tx_message_remote.Data[6] = 0;
//        CAN1_tx_message_remote.Data[7] = 0;    
        CAN_Transmit(CAN2, &CAN2_tx_message_remote);
    }
}



/**************************************************************
* function��������̨������׼���ĽǶ�ֵ
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
	 //CAN2_tx_down.Data[6] = (uint8_t)(last_armor_state==ARMOR_NO_AIM && armor_state == ARMOR_AIMED);//��׼״̬������ʱ�������ݲ�����Ч��
	
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
//	 Send_Up_buffer[7] = (unsigned char) (last_armor_state==ARMOR_NO_AIM && armor_state == ARMOR_AIMED);//��׼״̬������
//	 last_armor_state = armor_state;
//   Append_CRC8_Check_Sum(Send_Up_buffer, Send_Up_Size);
//   DMA_Cmd(DMA1_Stream7,ENABLE);

}

