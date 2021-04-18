#include "main.h"
#include "HW_Encoder.h"

//尚未测试该底层是否是正确的
//可能可以在过零检测结构体里面一起解决


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
extern block_disconnect_t block_disconnect;
extern _3508_motor_t  motor_chassis;
    extern Robot Sentry;
    //block_disconnect_t* temp = Get_block_t();
    //motor_chassis = Get_MotorChassis();	
  block_disconnect_t temp_block =   block_disconnect;
 _3508_motor_t* temp_motor = &motor_chassis;
    
    leftio = LimitSw_L;
    rightio = LimitSw_R;
    if(temp_block.is_left_limit == 1 || KeyLeftFlag == 1)		//防抖
    {
        if(KeyLeftFlag == 0)		//第一次进入
        {
            time_out_left = GetSysCnt();
            KeyLeftFlag = 1;
        }
        if(LimitSw_L == 0 && KeyLeftFlag ==1)//判断为误触
        {
            int32_t now = GetSysCnt();
            if(now - time_out_left>=LimitSw_TIMEOUT)
            {
                temp_block.is_left_limit = 0;
                Sentry.Journey = START_POS;
                crash_L_cnt++;
                temp_motor->angle_inc = CHASSIS_ENCODER_MAX;
                temp_motor->zerocheck.LastValue = temp_motor->angle_abs;
                temp_motor->zerocheck.Circle = -CHASSIS_ENCODER_MAX/8191;
                KeyLeftFlag = 0;
            }
        }
        else if(LimitSw_L == 1)
        {
                KeyLeftFlag = 0;
                temp_block.is_left_limit = 0;
        }
    }
    
    if(temp_block.is_right_limit == 1 || KeyRightFlag == 1)
    {
        if(KeyRightFlag == 0)		//第一次进入
        {
            time_out_right = GetSysCnt();
            KeyRightFlag = 1;
        }
        else if(LimitSw_R == 1 && KeyRightFlag ==1)//判断为误触//这里改了改了改了
        {
            int32_t now = GetSysCnt();
            if(now - time_out_right>=LimitSw_TIMEOUT)
            {
                temp_block.is_right_limit = 0;
                Sentry.Journey = END_POS;
                crash_R_cnt++;
                temp_motor->angle_inc = temp_motor->angle_abs;
                temp_motor->zerocheck.Circle = 0;
                temp_motor->zerocheck.LastValue = temp_motor->angle_abs;					
                KeyRightFlag = 0;
            }
        }
        else if(LimitSw_R == 1)
        {
                KeyRightFlag = 0;
                temp_block.is_right_limit = 0;
        }
    }	
}




