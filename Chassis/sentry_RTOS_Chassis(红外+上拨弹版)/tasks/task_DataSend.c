#include "main.h"
#include "task_DataSend.h"

//��Ȼ����ļ�����task��������˵Ӧ������һ��������

/**
  * @brief  ���͵��̵�� �� �ϲ������ ������
  * @param  �������̵���ĵ���ֵ , ���͸��ϲ�������ĵ���ֵ
  * @retval None
	*3508������(C620)������Χ��-16384~16384
	*2006��2310������������Χ��-8000~8000  (z����������ڴ�����ò������Ѿ�)
  */
void Chassis_Bodan_CAN1Send(int16_t ChassisI_Set, int16_t BodanI_Set)
{
  CanTxMsg tx_message;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 0x08;
  tx_message.StdId = 0x200;

  ChassisI_Set = LIMIT_MAX_MIN(ChassisI_Set, ChassisCurrentLimit, -ChassisCurrentLimit);
  //    BodanI_Set=LIMIT_MAX_MIN(BodanI_Set,BodanCurrentLimit,-BodanCurrentLimit);

  tx_message.Data[0] = (uint8_t)((ChassisI_Set >> 8) & 0xff);
  tx_message.Data[1] = (uint8_t)(ChassisI_Set & 0xff);

  tx_message.Data[4] = (uint8_t)((BodanI_Set >> 8) & 0xff);
  tx_message.Data[5] = (uint8_t)(BodanI_Set & 0xff);

  CAN_Transmit(CAN1, &tx_message);
}

/**
  * @brief  ���͵��̵��������
  * @param  �������̵���ĵ���ֵ
  * @retval None
	*3508������(C620)������Χ��-16384~16384
  */
void Chassis_CAN1Send(int16_t I_Set)
{
  CanTxMsg tx_message;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 0x08;
  tx_message.StdId = 0x200;

  I_Set = LIMIT_MAX_MIN(I_Set, ChassisCurrentLimit, -ChassisCurrentLimit);

  tx_message.Data[0] = (uint8_t)((I_Set >> 8) & 0xff);
  tx_message.Data[1] = (uint8_t)(I_Set & 0xff);

  CAN_Transmit(CAN1, &tx_message);
}

/**
  * @brief  ���͵��̵��������
  * @param  �������̵���ĵ���ֵ
  * @retval None
	*2006��2310������������Χ��-8000~8000  (����������ڴ�����ò������Ѿ�)
  */
void Bodan_CAN2Send(int16_t I_Set)
{
  CanTxMsg tx_message;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 0x08;
  tx_message.StdId = 0x200;

  I_Set = LIMIT_MAX_MIN(I_Set, BodanCurrentLimit, -BodanCurrentLimit);
  
  tx_message.Data[0] = 0;
  tx_message.Data[1] = 0;
  tx_message.Data[2] = 0;
	tx_message.Data[3] = 0;
	tx_message.Data[4] = (uint8_t)((I_Set >> 8) & 0xff);
	tx_message.Data[5] = (uint8_t)(I_Set & 0xff);
	tx_message.Data[6] = 0;
	tx_message.Data[7] = 0;
	
  CAN_Transmit(CAN2, &tx_message);
}

/**
  * @brief  ���͵�CAN�����ϵ�����ǹ������
  * @brief  ���͵��̽��յ��Ĺ��ڱ�����������ϧ����ֹ�ڱ�����ʽ��ʼǰ���ڱ���ǹ�ܳ��
  * @param  �Ӳ���ϵͳ��������ǹ������
  * @retval None
  */

extern uint8_t aim_flag;

void ShootingHeat_CAN2Send(void)
{
  CanTxMsg tx_message;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 0x08;
  tx_message.StdId = SHOOTING_HEAT_ID;

  extern uint8_t ShootAbleFlag_0, ShootAbleFlag_2;
  extern uint8_t is_game_start;
  tx_message.Data[0] = ShootAbleFlag_0;
  tx_message.Data[1] = ShootAbleFlag_2;
  tx_message.Data[2] = is_game_start;  //�Ѻ˸���־���͸�������̨����ֹ����ʽ��ʼǰ����̨��
  tx_message.Data[3] = aim_flag;
  tx_message.Data[4] = 00;
  tx_message.Data[5] = 00;
  tx_message.Data[6] = 00;
  tx_message.Data[7] = 00;

  CAN_Transmit(CAN2, &tx_message);
}



//void MasterSend(void)
//{
//	block_disconnect_t* temp = Get_block_t();
//	_3508_motor_t* chassis = Get_MotorChassis();
//	int32_t journey = Sentry.Journey;
//	CanTxMsg tx_message;
//	tx_message.IDE = CAN_ID_STD;
//	tx_message.RTR = CAN_RTR_DATA;
//	tx_message.DLC = 0x08;
//	tx_message.StdId = SHOOTING_HEAT_ID;

//	memcpy(tx_message.Data,&(journey),4);
//	tx_message.Data[4] = (*temp).is_left_limit;
//	tx_message.Data[5] = (*temp).is_right_limit;
//    CAN_Transmit(CAN2,&tx_message);
//	(*temp).is_left_limit = 0;
//	(*temp).is_right_limit = 0;
//}
