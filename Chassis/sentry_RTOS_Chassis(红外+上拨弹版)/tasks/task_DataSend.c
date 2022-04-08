#include "main.h"
#include "task_DataSend.h"

//虽然这个文件名叫task，但按理说应该算是一个驱动库

/**
  * @brief  发送底盘电机 和 上拨弹电机 的数据
  * @param  发给底盘电调的电流值 , 发送给上拨弹电机的电流值
  * @retval None
	*3508电机电调(C620)电流范围：-16384~16384
	*2006，2310电机电调电流范围：-8000~8000  (z这个底盘现在大概是用不上了已经)
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
  * @brief  发送底盘电机的数据
  * @param  发给底盘电调的电流值
  * @retval None
	*2006，2310电机电调电流范围：-8000~8000  (这个底盘现在大概是用不上了已经)
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
  * @brief  发送到CAN总线上的上下枪口热量
  * @brief  发送底盘接收到的关于比赛的惺惺相惜，防止在比赛正式开始前，哨兵下枪管抽风
  * @param  从裁判系统传过来的枪口热量
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
  tx_message.Data[2] = is_game_start;  //把核个标志发送给上下云台，防止在正式开始前，云台打弹
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
