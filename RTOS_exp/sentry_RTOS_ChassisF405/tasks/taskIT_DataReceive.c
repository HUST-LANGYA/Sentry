#include "main.h"

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
//		block_disconnect.Chassis_Last_Cnt = GetSysCnt();
		motor_chassis.angle_abs = Can1_rx_message_0.Data[0] << 8 | Can1_rx_message_0.Data[1];
		motor_chassis.real_speed = Can1_rx_message_0.Data[2] << 8 | Can1_rx_message_0.Data[3];
        motor_chassis.real_flow = Can1_rx_message_0.Data[4] << 8 | Can1_rx_message_0.Data[5];
	}    
}

/**
  * @brief  ��������̨��������ķ�������
  * @param  None
  * @retval None
  */
void CAN1_DataReceive_1(void)
{
//	if (Can1_rx_message_1.StdId == BodanMotor_Up_ID)  //�����ϲ�������ķ�����Ϣ
//    {
//        extern _2006_motor_t BodanMotor;
//        block_disconnect.Bodan_Last_Cnt = GetSysCnt();
//        BodanMotor.Angle_ABS = Can1_rx_message_1.Data[0] << 8 | Can1_rx_message_0.Data[1];
//        BodanMotor.RealSpeed = Can1_rx_message_1.Data[2] << 8 | Can1_rx_message_0.Data[3];
//    }
}

/**
  * @brief  �������ϵ�ң�������ݱ���
  * @param  None
  * @retval None
  */
void CAN2_DataReceive_0(void)
{
    extern RC_Ctl_t RC_Ctl;
	if (Can2_rx_message_0.StdId == Remote_Control_ID) //���մ�����̨������ң��������֡
	{   
        RC_Ctl.rc.s1 = (((int16_t)Can2_rx_message_0.Data[5] >> 6)& 0x0003); //!< Switch left
        RC_Ctl.rc.s2 = (((int16_t)Can2_rx_message_0.Data[5] >> 4)& 0x0003); //!< Switch right
        if(RC_Ctl.rc.s2 == 0 || RC_Ctl.rc.s1 == 0) 
            return;//����������յİ���ֱ�Ӷ���        
        RC_Ctl.rc.ch0 = ((int16_t)Can2_rx_message_0.Data[0]| ((int16_t)Can2_rx_message_0.Data[1] << 8)) & 0x07ff; //!< Channel 0
        RC_Ctl.rc.ch1 = (((int16_t)Can2_rx_message_0.Data[1] >> 3) | ((int16_t)Can2_rx_message_0.Data[2] << 5)) & 0x07ff; //!< Channel 1
        RC_Ctl.rc.ch2 = (((int16_t)Can2_rx_message_0.Data[2] >> 6) | ((int16_t)Can2_rx_message_0.Data[3] << 2) | ((int16_t)Can2_rx_message_0.Data[4] << 10)) & 0x07ff;//!< Channel 2
        RC_Ctl.rc.ch3 = (((int16_t)Can2_rx_message_0.Data[4] >> 1) | ((int16_t)Can2_rx_message_0.Data[5] << 7)) & 0x07ff; //!< Channel 3
        
//        RC_Ctl.rc.s1 = ((int8_t) Can2_rx_message_0.Data[5]>>2 ) & 0x03;
//        RC_Ctl.rc.s2 = ((int8_t) Can2_rx_message_0.Data[5]) & 0x03;
//        if(RC_Ctl.rc.s2 == 0 || RC_Ctl.rc.s1 == 0) 
//            return;//����������յİ���ֱ�Ӷ���
//        RC_Ctl.rc.ch0=((int16_t)Can2_rx_message_0.Data[0]<<3 | (int16_t)Can2_rx_message_0.Data[1]>>5) & 0x7ff;
//        RC_Ctl.rc.ch1=((int16_t)Can2_rx_message_0.Data[1]<<6 | (int16_t)Can2_rx_message_0.Data[2]>>2 ) & 0x7ff;
//        RC_Ctl.rc.ch2=((int16_t)Can2_rx_message_0.Data[2]<<9 | (int16_t)Can2_rx_message_0.Data[3]<<1 | (int16_t)Can2_rx_message_0.Data[4]>>7 ) & 0x7ff;
//        RC_Ctl.rc.ch3=((int16_t)Can2_rx_message_0.Data[4]<<4 | (int16_t)Can2_rx_message_0.Data[5]>>4 ) & 0x7ff;         
	}
}

/**
  * @brief  blblblbl
  * @param  None
  * @retval None
  */
void CAN2_DataReceive_1(void)
{
	;
}




///**
//  * @brief  �����յ��Ĺ���PowerNow
//  * @param  None
//  * @retval None
//  */
//float GetPower()
//{
//	return PowerNow;
//}
