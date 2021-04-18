#include "main.h"


/**
 * @brief  获取哨兵在滑轨上的位置，用编码器的绝对值算得
 * @param  None
 * @retval 绝对位置（目前还没考虑好定位的标准和零位）
 */
int32_t roundNum;//记录编码器转过了几次机械零位
int32_t get_pos(void)
{
    return roundNum*TICK_PER_ROUND+Read_Encoder();  //大概思路就是这样了反正
}

int32_t time_out_left,time_out_right;
int32_t KeyLeftFlag,KeyRightFlag;
uint8_t leftio,rightio;//光电开关IO口
int32_t crash_L_cnt = 0,crash_R_cnt = 0;
int32_t CHASSIS_ENCODER_MAX = 3725500;

void Encoder_detect()
{

	_3508_motor_t*  motor_chassis;
	block_disconnect_t* temp = Get_block_t();
	motor_chassis = Get_MotorChassis();	
	leftio = KEY_LEFT;
	rightio = KEY_RIGHT;
	if(block_disconnect.is_left_limit == 1 || KeyLeftFlag == 1)		//防抖
	{
		if(KeyLeftFlag == 0)		//第一次进入
		{
			time_out_left = GetSysCnt();
			KeyLeftFlag = 1;
		}
		if(KEY_LEFT == 0 && KeyLeftFlag ==1)//判断为误触
		{
			int32_t now = GetSysCnt();
			if(now - time_out_left>=KEY_TIMEOUT)
			{
				block_disconnect.is_left_limit = 0;
				Sentry.Journey = START_POS;
				crash_L_cnt++;
				motor_chassis->angle_inc = CHASSIS_ENCODER_MAX;
				motor_chassis->zerocheck.LastValue = motor_chassis->angle_abs;
				motor_chassis->zerocheck.Circle = -CHASSIS_ENCODER_MAX/8191;
				KeyLeftFlag = 0;
			}
		}
		else if(KEY_LEFT == 1)
		{
				KeyLeftFlag = 0;
				block_disconnect.is_left_limit = 0;
		}
	}
	
	if(block_disconnect.is_right_limit == 1 || KeyRightFlag == 1)
	{
		if(KeyRightFlag == 0)		//第一次进入
		{
			time_out_right = GetSysCnt();
			KeyRightFlag = 1;
		}
		else if(KEY_RIGHT == 1 && KeyRightFlag ==1)//判断为误触//这里改了改了改了
		{
			int32_t now = GetSysCnt();
			if(now - time_out_right>=KEY_TIMEOUT)
			{
				block_disconnect.is_right_limit = 0;
				Sentry.Journey = END_POS;
				crash_R_cnt++;
				motor_chassis->angle_inc = motor_chassis->angle_abs;
				motor_chassis->zerocheck.Circle = 0;
				motor_chassis->zerocheck.LastValue = motor_chassis->angle_abs;					
				KeyRightFlag = 0;
			}
		}
		else if(KEY_RIGHT == 1)
		{
				KeyRightFlag = 0;
				block_disconnect.is_right_limit = 0;
		}
	}	
}




