#include "main.h"
#include "task_DataSend.h"

extern State_t Sentry_State;
extern float Pitch_Actual, Yaw_Actual;

/**
  * @brief  ���Ͳ����������Ϣ�����ò��������ת��
  * @param  ������������ĵ���ֵ
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
  * @brief  ��̨�����Ϣ����	
  * @param  ������ת�ٵ��趨ֵ
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
  * @brief  �������̺�����̨���ذ��ң������(MDֱ�Ӱ�ң������ԭʼ���ݸ���ȥ�ɣ�ѹ��һ�¸���ȥ)
  *         Ȼ��������̨�͵��������д���Ľ��뺯��
  * @param  None
  * @retval None
  */
CanTxMsg CAN1_tx_message_remote;
extern uint8_t armor_state;   //��ʾ�����ǲ������ҵ�Ŀ��

void Remote_Can1Send(void)
{
    extern RC_Ctl_t RC_Ctl ;
    if(RC_Ctl.rc.s1 && RC_Ctl.rc.s2)  //���������Ͷ�������Ȼ�һ���û������������ô���ֵ�MD��
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
//        //Data[6]��Data[7]��ʱԤ��
//        CAN1_tx_message_remote.Data[6] = 0;
//        CAN1_tx_message_remote.Data[7] = 0;    
        CAN_Transmit(CAN1, &CAN1_tx_message_remote);
    }
}
