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
void Bodan_CAN1Send(int16_t I_Set)
{
  CanTxMsg tx_message;
  tx_message.IDE = CAN_ID_STD;
  tx_message.RTR = CAN_RTR_DATA;
  tx_message.DLC = 0x08;
  tx_message.StdId = 0x200;

  I_Set = LIMIT_MAX_MIN(I_Set, BodanCurrentLimit, -BodanCurrentLimit);

  tx_message.Data[4] = (uint8_t)((I_Set >> 8) & 0xff);
  tx_message.Data[5] = (uint8_t)(I_Set & 0xff);

  CAN_Transmit(CAN1, &tx_message);
}

/**
  * @brief  给到下云台主控板的遥控数据(MD直接把遥控器的原始数据给过去吧，压缩一下给过去)
  *         然后在下云台和底盘那里各写各的解码函数
  * @param  None
  * @retval None
  */
CanTxMsg CAN1_tx_message_remote;
void Remote_Can2Send(void)
{
  // !  这个函数把从遥控器接收的数据转发给底盘和下云台
  // !  但这个函数目前有很严重的问题！通信非常的不稳定！
  extern RC_Ctl_t RC_Ctl;           //= getRCData();
  if (RC_Ctl.rc.s1 && RC_Ctl.rc.s2) //如果是零包就丢掉（虽然我还是没整清楚零包是怎么出现的MD）
  {
    CAN1_tx_message_remote.IDE = CAN_ID_STD;
    CAN1_tx_message_remote.RTR = CAN_RTR_DATA;
    CAN1_tx_message_remote.DLC = 0x08;
    CAN1_tx_message_remote.StdId = Remote_Control_ID;

    CAN1_tx_message_remote.Data[0] = (uint8_t)((RC_Ctl.rc.ch0 >> 3) & 0xff);
    CAN1_tx_message_remote.Data[1] = (uint8_t)((RC_Ctl.rc.ch0 << 5) | (RC_Ctl.rc.ch1 >> 6) & 0xff);
    CAN1_tx_message_remote.Data[2] = (uint8_t)((RC_Ctl.rc.ch1 << 2) | (RC_Ctl.rc.ch2 >> 9) & 0xff);
    CAN1_tx_message_remote.Data[3] = (uint8_t)((RC_Ctl.rc.ch2 >> 1) & 0xff);
    CAN1_tx_message_remote.Data[4] = (uint8_t)((RC_Ctl.rc.ch2 << 7) | (RC_Ctl.rc.ch3 >> 4) & 0xff);
    CAN1_tx_message_remote.Data[5] = (uint8_t)((RC_Ctl.rc.ch3 << 4) | ((RC_Ctl.rc.s1 << 2) & 0x0C) | ((RC_Ctl.rc.s2) & 0x03) & 0xff);

    CAN_Transmit(CAN2, &CAN1_tx_message_remote);
  }
}

/**
  * @brief  发送到CAN总线上的上下枪口热量
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

  extern uint8_t ShootAbleFlag;
  tx_message.Data[0] = ShootAbleFlag;
  tx_message.Data[1] = 00;
  tx_message.Data[2] = 00;
  tx_message.Data[3] = 00;
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
