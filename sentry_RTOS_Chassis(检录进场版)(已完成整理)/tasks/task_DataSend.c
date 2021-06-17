#include "main.h"
#include "task_DataSend.h"

//虽然这个文件名叫task，但按理说应该算是一个驱动库

/**
  * @brief  发送底盘电机的数据
  * @param  发给底盘电调的电流值
  * @retval None
	*3508电机电调(C620)电流范围：-16384~16384
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
  * @brief  发送到CAN总线上的上下枪口热量
  * @brief  发送底盘接收到的关于比赛的惺惺相惜，防止在比赛正式开始前，哨兵下枪管抽风
  * @param  从裁判系统传过来的枪口热量
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
    tx_message.Data[2] = is_game_start;  //把核个标志发送给上下云台，防止在正式开始前，云台打弹
    tx_message.Data[3] = 00;
    tx_message.Data[4] = 00;
    tx_message.Data[5] = 00;
    tx_message.Data[6] = 00;
    tx_message.Data[7] = 00;

    CAN_Transmit(CAN2, &tx_message);
}
