#include "main.h"
#include "taskIT_DataReceive.h"

extern CanRxMsg Can1_rx_message_0, Can1_rx_message_1, Can2_rx_message_0, Can2_rx_message_1;

/**
 * @brief  待扩展FIFO
 * @param  None
 * @retval None
 */
void CAN1_DataReceive_0(void)
{
}

/**
  * @brief  底盘信息接收
  * @param  None
  * @retval None    
  */
uint32_t chassis_offline_tick = 0;  //底盘掉电检测计数器，计数到20000后认为底盘掉电
uint8_t chassis_offline_flag = 0; //底盘掉电标志位，若掉电则置1
void CAN1_DataReceive_1(void) //接收底盘数据
{
    extern uint8_t Heat_ShootAbleFlag_0;
    extern uint8_t is_game_start;
    if (Can1_rx_message_1.StdId == SHOOTING_HEAT_ID)
    {
        Heat_ShootAbleFlag_0 = Can1_rx_message_1.Data[0];
        is_game_start = Can1_rx_message_1.Data[2];
        chassis_offline_tick = 0;  //掉电计数清零，表示底盘在线
    }
}

/**
  * @brief  上云台两轴电机信息、拨弹电机信息接收
  * @param  None
  * @retval None
  */
void CAN2_DataReceive_0(void)
{
    extern gimbal_motor_t MotoPitch;
    extern gimbal_motor_t MotoYaw;
    extern _2006_motor_t BodanMotor;
    extern block_disconnect_t block_disconnect;
    if (Can2_rx_message_0.StdId == MotoPitch_ID)
    {
        MotoPitch.Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
        MotoPitch.real_speed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
        MotoPitch.real_flow = Can2_rx_message_0.Data[4] << 8 | Can2_rx_message_0.Data[5];
    }
    else if (Can2_rx_message_0.StdId == MotoYaw_ID)
    {
        MotoYaw.Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
        MotoYaw.real_speed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
        MotoYaw.real_flow = Can2_rx_message_0.Data[4] << 8 | Can2_rx_message_0.Data[5];
    }
    else if (Can2_rx_message_0.StdId == BodanMotor_Up_ID)
    {
        BodanMotor.Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
        BodanMotor.RealSpeed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
    }
}

/**
  * @brief  上云台陀螺仪信息接收
  * @param  None
  * @retval None
  */
gyro_Typedef Gyro_White;
uint32_t gyro_cnt;
float gyro_pitch_test;

uint32_t cnt_temp, cnt1; //float cnt2,cnt2temp;
float gz10e6;            //?????
void CAN2_DataReceive_1(void) //接收陀螺仪
{
    if (Can2_rx_message_1.StdId == Gyro_Pitch_ID)
    {
        memcpy(&Gyro_White.PITCH, &Can2_rx_message_1.Data[0], 4);
        memcpy(&Gyro_White.GY, &Can2_rx_message_1.Data[4], 4);
        gyro_cnt++;
        gyro_pitch_test = Gyro_White.GY * 100000;
    }
    if (Can2_rx_message_1.StdId == Gyro_Yaw_ID)
    {
        memcpy(&Gyro_White.YAW_ABS, &Can2_rx_message_1.Data[0], 4);
        memcpy(&Gyro_White.GZ, &Can2_rx_message_1.Data[4], 4);
        gz10e6 = Gyro_White.GZ * 1000000;
    }
}

/**
  * @brief  TX2信息接收
  * @param  None
  * @retval None
  */
volatile long run_time_check;
int pc_yaw;
short pc_pitch;
short distance;
short armor_state = 0; //表示辅瞄是不是有找到目标
float aim_yaw, aim_pitch;
short tx2_last_receive_flag; //表示有没有数据更新
short tx2_receive_flag;
float aim_yaw_Limit = 10000.0f, aim_pitch_Limit = 3000.0f;
uint16_t distance_Limit = 100 * 10;
uint8_t CV_Shoot_ABLE = 0; //判定视觉方面是否能够打子弹
PC_Receive_t PC_Receive;
void PCReceive(uint8_t PCReceivebuffer[])
{
    run_time_check++;
    pc_pitch = (short)(PCReceivebuffer[2] << 8 | PCReceivebuffer[3]); //这里不能转为float，不然负数传过来会变为正数
    pc_yaw = (int)(PCReceivebuffer[4] << 24 | PCReceivebuffer[5] << 16 | PCReceivebuffer[6] << 8 | PCReceivebuffer[7] << 0);
    distance = (uint8_t)PCReceivebuffer[8]; //7~1位给到distance

    if (pc_yaw >= aim_yaw_Limit || pc_yaw <= -aim_yaw_Limit ||
        pc_pitch >= aim_pitch_Limit || pc_pitch <= -aim_pitch_Limit ||
        distance >= distance_Limit)
    {
        armor_state = ARMOR_NO_AIM;
        return; //接到乱码包，直接退栈
    }

    aim_yaw = (float)pc_yaw / 100.0f;
    aim_pitch = (float)pc_pitch / 100.0f;

    if (aim_yaw >= aim_yaw_Limit || aim_yaw <= -aim_yaw_Limit ||
        aim_pitch >= aim_pitch_Limit || aim_pitch <= -aim_pitch_Limit)
    {
        armor_state = ARMOR_NO_AIM;
        return; //接到乱码包，直接退栈
    }

    if (run_time_check % 400== 0) //隔一段时间判断一次
    {
        tx2_receive_flag = PCReceivebuffer[1];         //作为更新标志位
        if (tx2_receive_flag == tx2_last_receive_flag) //如果没有数据更新
        {
            armor_state = ARMOR_NO_AIM; //没目标
        }
        else
        {
            armor_state = ARMOR_AIMED; //有目标
        }
    }
    PC_Receive.RCPitch = (float)(aim_pitch); //更新值
    PC_Receive.RCYaw = (float)(aim_yaw);

    PC_Receive.RCdistance = (float)distance / 10.0f;

    PC_Receive.DisConnect = 0;
    tx2_last_receive_flag = tx2_receive_flag;
}
