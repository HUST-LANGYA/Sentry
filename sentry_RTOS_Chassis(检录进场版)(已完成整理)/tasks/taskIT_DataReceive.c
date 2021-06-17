#include "main.h"
#include "taskIT_DataReceive.h"

extern CanRxMsg Can1_rx_message_0, Can1_rx_message_1, Can2_rx_message_0, Can2_rx_message_1;

/**
  * @brief  ���յ��̵���ķ��ر���
  * @param  None
  * @retval None
  */
void CAN1_DataReceive_0(void)
{
	if (Can1_rx_message_0.StdId == MOTOR_CHASSIS_ID) //���̵����ID��
	{
        extern _3508_motor_t motor_chassis;
		motor_chassis.angle_abs  = Can1_rx_message_0.Data[0] << 8 | Can1_rx_message_0.Data[1];
		motor_chassis.real_speed = Can1_rx_message_0.Data[2] << 8 | Can1_rx_message_0.Data[3];
        motor_chassis.real_flow  = Can1_rx_message_0.Data[4] << 8 | Can1_rx_message_0.Data[5];
	}    
}

/**
  * @brief  ����չ��FIFO
  * @param  None
  * @retval None
  */
void CAN1_DataReceive_1(void){}

    
/**
  * @brief  �������ϵ�ң�������ݱ���
  * @param  None
  * @retval None
  */
void CAN2_DataReceive_0(void)
{
	if (Can2_rx_message_0.StdId == Remote_Control_ID) //���մ�����̨������ң��������֡
	{   
        extern RC_Ctl_t RC_Ctl;
        RC_Ctl.rc.s1 = ((int8_t) Can2_rx_message_0.Data[5]>>2 ) & 0x03;
        RC_Ctl.rc.s2 = ((int8_t) Can2_rx_message_0.Data[5]) & 0x03;
        if(RC_Ctl.rc.s2 == 0 || RC_Ctl.rc.s1 == 0) 
            return;//����������յİ���ֱ�Ӷ���
        RC_Ctl.rc.ch0=((int16_t)Can2_rx_message_0.Data[0]<<3 | (int16_t)Can2_rx_message_0.Data[1]>>5) & 0x7ff;
        RC_Ctl.rc.ch1=((int16_t)Can2_rx_message_0.Data[1]<<6 | (int16_t)Can2_rx_message_0.Data[2]>>2 ) & 0x7ff;
        RC_Ctl.rc.ch2=((int16_t)Can2_rx_message_0.Data[2]<<9 | (int16_t)Can2_rx_message_0.Data[3]<<1 | (int16_t)Can2_rx_message_0.Data[4]>>7 ) & 0x7ff;
        RC_Ctl.rc.ch3=((int16_t)Can2_rx_message_0.Data[4]<<4 | (int16_t)Can2_rx_message_0.Data[5]>>4 ) & 0x7ff;  
	}
}

/**
  * @brief  ����չ��FIFO
  * @param  None
  * @retval None
  */
void CAN2_DataReceive_1(void){}
