#include "main.h"
#include "task_DataSend.h"

//��Ȼ����ļ�����task��������˵Ӧ������һ��������

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
  * @brief  ���͵�CAN�����ϵ�����ǹ������
  * @brief  ���͵��̽��յ��Ĺ��ڱ�����������ϧ����ֹ�ڱ�����ʽ��ʼǰ���ڱ���ǹ�ܳ��
  * @param  �Ӳ���ϵͳ��������ǹ������
  * @retval None
  */
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
    tx_message.Data[3] = 00;
    tx_message.Data[4] = 00;
    tx_message.Data[5] = 00;
    tx_message.Data[6] = 00;
    tx_message.Data[7] = 00;

    CAN_Transmit(CAN2, &tx_message);
}
