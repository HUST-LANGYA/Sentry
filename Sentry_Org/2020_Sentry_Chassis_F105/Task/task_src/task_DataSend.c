#include "main.h"

/**
  * @brief  发送底盘电机 和 上拨弹电机 的数据
  * @param  发给底盘电调的电流值
  * @retval None
	*3508电机电调(C620)电流范围：-16384~16384
	*2006，2310电机电调电流范围：-8000~8000  (z这个底盘现在大概是用不上了已经)
  */
void ChassisISend(int16_t ChassisI_Set,int16_t BodanI_Set)
{
    CanTxMsg tx_message;
    tx_message.IDE = CAN_ID_STD;    
    tx_message.RTR = CAN_RTR_DATA; 
    tx_message.DLC = 0x08;    
    tx_message.StdId = 0x200;
	
	ChassisI_Set=LIMIT_MAX_MIN(ChassisI_Set,ChassisCurrentLimit,-ChassisCurrentLimit);
    BodanI_Set=LIMIT_MAX_MIN(BodanI_Set,BodanCurrentLimit,-BodanCurrentLimit);
    
    tx_message.Data[0] = (uint8_t)((ChassisI_Set>>8)&0xff);
    tx_message.Data[1] = (uint8_t)(ChassisI_Set&0xff);  
    
    tx_message.Data[4] = (uint8_t)((BodanI_Set>>8)&0xff);
    tx_message.Data[5] = (uint8_t)(BodanI_Set&0xff);  

    CAN_Transmit(CAN1,&tx_message);
}



void BodanISend(int16_t I_Set)
{
    CanTxMsg tx_message;
    tx_message.IDE = CAN_ID_STD;    
    tx_message.RTR = CAN_RTR_DATA; 
    tx_message.DLC = 0x08;    
    tx_message.StdId = 0x200;
	
	I_Set=LIMIT_MAX_MIN(I_Set,BodanCurrentLimit,-BodanCurrentLimit);

    tx_message.Data[4] = (uint8_t)((I_Set>>8)&0xff);
    tx_message.Data[5] = (uint8_t)(I_Set&0xff);  

    CAN_Transmit(CAN1,&tx_message);
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
    
    tx_message.Data[0] = (uint8_t)1;
    tx_message.Data[1] = (uint8_t)2;
    tx_message.Data[2] = (uint8_t)3;
    tx_message.Data[3] = (uint8_t)4;
    tx_message.Data[4] = (uint8_t)5;
    tx_message.Data[5] = (uint8_t)6;
    tx_message.Data[6] = (uint8_t)7;
    tx_message.Data[7] = (uint8_t)8;
    
    CAN_Transmit(CAN2,&tx_message);
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


