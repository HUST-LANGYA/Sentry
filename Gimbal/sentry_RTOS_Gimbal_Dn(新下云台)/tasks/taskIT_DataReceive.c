#include "main.h"
#include "taskIT_DataReceive.h"

extern CanRxMsg Can1_rx_message_0, Can1_rx_message_1, Can2_rx_message_0, Can2_rx_message_1;

/**
  * @brief  接收摩擦轮数据
  * @param  None
  * @retval None
  */
uint16_t RX_Number=1;
void CAN1_DataReceive_0(void)
{
    extern RC_Ctl_t RC_Ctl;
	  extern gimbal_motor_t MotoPitch;
    extern gimbal_motor_t MotoYaw;
    if (Can1_rx_message_0.StdId == MotoPitch_ID_Dn)
    {
        MotoPitch.Angle_ABS = Can1_rx_message_0.Data[0] << 8 | Can1_rx_message_0.Data[1];
        MotoPitch.real_speed = Can1_rx_message_0.Data[2] << 8 | Can1_rx_message_0.Data[3];
        MotoPitch.real_flow = Can1_rx_message_0.Data[4] << 8 | Can1_rx_message_0.Data[5];
    }
    else if (Can1_rx_message_0.StdId == MotoYaw_ID_Dn)
    {
        MotoYaw.Angle_ABS = Can1_rx_message_0.Data[0] << 8 | Can1_rx_message_0.Data[1];
        MotoYaw.real_speed = Can1_rx_message_0.Data[2] << 8 | Can1_rx_message_0.Data[3];
        MotoYaw.real_flow = Can1_rx_message_0.Data[4] << 8 | Can1_rx_message_0.Data[5];
    }
	  RX_Number= !RX_Number;
}

/**
  * @brief  底盘信息接收
  * @param  None
  * @retval None
  */
uint32_t chassis_offline_tick = 0;  //底盘掉电检测计数器，计数到20000后认为底盘掉电
uint8_t chassis_offline_flag = 0; //底盘掉电标志位，若掉电则置1
float Bullet_Speed;

//云台协同
int aim_Down_Pitch_100;
short aim_Down_Yaw_100;
float aim_Down_Pitch;//下云台发来的Pitch协同值
float aim_Down_Yaw;//下云台发来的Yaw协同值
uint8_t aim_Valid_Flag;
float Bullet_Speed;
void CAN1_DataReceive_1(void) //接收底盘数据
{
    extern uint8_t Heat_ShootAbleFlag_0;
    extern uint8_t is_game_start;
	  extern int16_t chassis_speed;
    if (Can1_rx_message_1.StdId == SHOOTING_HEAT_ID)
    {
        Heat_ShootAbleFlag_0 = Can1_rx_message_1.Data[0];
        is_game_start = Can1_rx_message_1.Data[2];
			  chassis_speed = (int16_t)(((int16_t)Can1_rx_message_1.Data[5] << 8  | (int16_t)Can1_rx_message_1.Data[6]) & 0xffff);
        chassis_offline_tick = 0;//掉电计数清零
			   
    }
		else if(Can1_rx_message_1.StdId == BULLET_SPEED_ID)
	  {
			
		    memcpy(&Bullet_Speed ,&(Can1_rx_message_1.Data[0]),4);   //此时接收到的弹速为100倍
		    chassis_offline_tick = 0;//掉电计数清零
		}		
		else if(Can1_rx_message_1.StdId == GIMBAL_DOWN_ID)
		{
		    aim_Down_Pitch_100 = (short)(Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1]); 
		    aim_Down_Yaw_100 = (int)(Can2_rx_message_0.Data[2] << 24 | Can2_rx_message_0.Data[3] << 16 | Can2_rx_message_0.Data[4] << 8 | Can2_rx_message_0.Data[5]);
				aim_Down_Pitch = (float)aim_Down_Pitch / 100.0f;
				aim_Down_Yaw = (float)aim_Down_Yaw_100 / 100.0f;
				aim_Valid_Flag = (uint8_t)Can2_rx_message_0.Data[6];
		
		
		}
	
}

/**
  * @brief  下云台两轴电机信息、拨弹电机信息接收
  * @param  None
  * @retval None
  */
void CAN2_DataReceive_0(void)
{
    //extern gimbal_motor_t MotoPitch;
    //extern gimbal_motor_t MotoYaw;
    extern _2006_motor_t BodanMotor;
	  extern _2006_motor_t FrictionMotor[2];
    extern block_disconnect_t block_disconnect;
		
    if (Can2_rx_message_0.StdId == FrictionMotor_Up_0_ID)
    {
        FrictionMotor[0].Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
        FrictionMotor[0].RealSpeed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
    }
    else if (Can2_rx_message_0.StdId == FrictionMotor_Up_1_ID)
    {
        FrictionMotor[1].Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
        FrictionMotor[1].RealSpeed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
    }
    else if (Can2_rx_message_0.StdId == BodanMotor_Dn_ID)
    {
        BodanMotor.Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
        BodanMotor.RealSpeed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
    }
}

/**
  * @brief  下云台陀螺仪信息接收
  * @param  None
  * @retval None
  */
gyro_Typedef Gyro_White;
uint32_t gyro_cnt;
float gyro_pitch_test;
uint16_t Gyro_Rx_Number;
uint32_t cnt_temp, cnt1;      //float cnt2,cnt2temp;
float gz10e6;                 //?????
void CAN2_DataReceive_1(void) //接收陀螺仪
{
    if (Can2_rx_message_1.StdId == Gyro_Pitch_ID)
    {
        memcpy(&Gyro_White.PITCH, &Can2_rx_message_1.Data[0], 4);
        memcpy(&Gyro_White.GY, &Can2_rx_message_1.Data[4], 4);
        gyro_cnt++;
        gyro_pitch_test = Gyro_White.GY * 100000;
        //cnt1 = xTaskGetTickCountFromISR() - cnt_temp;
        //cnt_temp = syscnt;
    }
    if (Can2_rx_message_1.StdId == Gyro_Yaw_ID)
    {
        memcpy(&Gyro_White.YAW_ABS, &Can2_rx_message_1.Data[0], 4);
        memcpy(&Gyro_White.GZ, &Can2_rx_message_1.Data[4], 4);
        gz10e6 = Gyro_White.GZ * 1000000;
    }
		Gyro_Rx_Number++;
}

/**
  * @brief  TX2信息接收
  * @param  None
  * @retval None
  */
volatile long run_time_check;
int pc_yaw;
short pc_pitch;
int pc_yaw_up;
short pc_pitch_up;
short distance;
uint8_t armor_state = 0; //表示辅瞄是不是有找到目标
uint8_t last_armor_state=ARMOR_NO_AIM;
float aim_yaw, aim_pitch;
short tx2_last_receive_flag; //表示有没有数据更新
short tx2_receive_flag;
float aim_yaw_Limit = 18001.0f, aim_pitch_Limit = 3000.0f;
uint16_t distance_Limit = 100*10;
uint8_t CV_Shoot_ABLE = 0; //判定视觉方面是否能够打子弹
PC_Receive_t PC_Receive;
uint32_t Pc_RX_Num=0;
uint32_t Last_Pc_Rx_Num=0;
void PCReceive(uint8_t PCReceivebuffer[])
{
    run_time_check++;
	  Pc_RX_Num++;
    pc_pitch = (short)(PCReceivebuffer[2] << 8 | PCReceivebuffer[3]); //这里不能转为float，不然负数传过来会变为正数
    pc_yaw = (int)(PCReceivebuffer[4] << 24 | PCReceivebuffer[5] << 16 | PCReceivebuffer[6] << 8 | PCReceivebuffer[7] << 0);
    //distance = (short)PCReceivebuffer[8] /*>>1*/; //7~0位给到distance
    pc_pitch_up = (short)(PCReceivebuffer[8] << 8 | PCReceivebuffer[9]);
	  pc_yaw_up = (int)(PCReceivebuffer[10] << 24 | PCReceivebuffer[11] << 16 | PCReceivebuffer[12] << 8 | PCReceivebuffer[13] << 0);
	
    if (pc_yaw >= aim_yaw_Limit|| pc_yaw <= -aim_yaw_Limit ||
        pc_pitch >= aim_pitch_Limit || pc_pitch <= -aim_pitch_Limit ||
        distance >= distance_Limit )
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
    
    if (run_time_check % 3 == 0) //隔一段时间判断一次
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
