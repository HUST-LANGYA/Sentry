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
		extern RC_Ctl_t RC_Ctl;
    extern _2006_motor_t FrictionMotor[2];
    extern _2006_motor_t BodanMotor;
	   //extern gimbal_motor_t MotoPitch;
    extern block_disconnect_t block_disconnect;
    
    if (Can1_rx_message_0.StdId == FrictionMotor_Up_0_ID)
    {
        FrictionMotor[0].Angle_ABS = Can1_rx_message_0.Data[0] << 8 | Can1_rx_message_0.Data[1];
        FrictionMotor[0].RealSpeed = Can1_rx_message_0.Data[2] << 8 | Can1_rx_message_0.Data[3];
    }
    else if (Can1_rx_message_0.StdId == FrictionMotor_Up_1_ID)
    {
        FrictionMotor[1].Angle_ABS = Can1_rx_message_0.Data[0] << 8 | Can1_rx_message_0.Data[1];
        FrictionMotor[1].RealSpeed = Can1_rx_message_0.Data[2] << 8 | Can1_rx_message_0.Data[3];
    }
    else if (Can1_rx_message_0.StdId == BodanMotor_Up_ID)
    {
        BodanMotor.Angle_ABS = Can1_rx_message_0.Data[0] << 8 | Can1_rx_message_0.Data[1];
        BodanMotor.RealSpeed = Can1_rx_message_0.Data[2] << 8 | Can1_rx_message_0.Data[3];
    }
//	  else if(Can1_rx_message_0.StdId == BodanMotor_Up_ID)
//	  {
//		    BodanMotor.Angle_ABS = Can1_rx_message_0.Data[0] << 8 | Can1_rx_message_0.Data[1];
//        BodanMotor.RealSpeed = Can1_rx_message_0.Data[2] << 8 | Can1_rx_message_0.Data[3];
//		
//		}
	
	
}

//gyro_Typedef Gyro_White;
//uint32_t gyro_cnt;
//float gyro_pitch_test;
//uint32_t cnt_temp, cnt1; //float cnt2,cnt2temp;
//float gz10e6;  
/**
  * @brief  上云台两轴电机信息接收
  * @param  None
  * @retval None    
  */
uint32_t chassis_offline_tick = 0;  //底盘掉电检测计数器，计数到20000后认为底盘掉电
uint8_t chassis_offline_flag = 0; //底盘掉电标志位，若掉电则置1
uint8_t aim_flag;
uint16_t RX_Number;
void CAN1_DataReceive_1(void) //接收两轴电机数据
{
	  extern gimbal_motor_t MotoPitch;
    extern gimbal_motor_t MotoYaw;
	    if (Can1_rx_message_1.StdId == MotoPitch_ID_Up)
    {
        MotoPitch.Angle_ABS = Can1_rx_message_1.Data[0] << 8 | Can1_rx_message_1.Data[1];
        MotoPitch.real_speed = Can1_rx_message_1.Data[2] << 8 | Can1_rx_message_1.Data[3];
        MotoPitch.real_flow = Can1_rx_message_1.Data[4] << 8 | Can1_rx_message_1.Data[5]; 
			RX_Number++;
    }
    else if (Can1_rx_message_1.StdId == MotoYaw_ID_Up)
    {
        MotoYaw.Angle_ABS = Can1_rx_message_1.Data[0] << 8 | Can1_rx_message_1.Data[1];
        MotoYaw.real_speed = Can1_rx_message_1.Data[2] << 8 | Can1_rx_message_1.Data[3];
        MotoYaw.real_flow = Can1_rx_message_1.Data[4] << 8 | Can1_rx_message_1.Data[5];
			RX_Number++;
    }
 
}

/**
  * @brief  上云台两轴电机信息、拨弹电机信息接收
  * @param  None
  * @retval None
  */
uint16_t RX_Number;
uint8_t defend_flag; //前哨站被打爆的标志位
//接收来着下云台的信息
uint16_t Down_Rec_Number;
short Down_pitch;
int Down_yaw;
uint8_t Aim_positive_flag;
float rec_pitch;
float rec_yaw;
uint8_t Down_distance;
Down_Receive_t Down_Receive_Now;
void CAN2_DataReceive_0(void)
{
    
//	  extern gimbal_motor_t MotoPitch;
//    extern gimbal_motor_t MotoYaw;
	  extern uint8_t Heat_ShootAbleFlag_0;
    extern uint8_t is_game_start;
	  extern float aim_yaw_Limit,aim_pitch_Limit;
	 // extern _2006_motor_t BodanMotor;
	
	  
	 if (Can2_rx_message_0.StdId == SHOOTING_HEAT_ID)
    {
        Heat_ShootAbleFlag_0 = Can2_rx_message_0.Data[0];
        is_game_start = Can2_rx_message_0.Data[2];
			  aim_flag = Can2_rx_message_0.Data[3]; //瞄准下底盘停止信号
			  defend_flag = Can2_rx_message_0.Data[4];
        chassis_offline_tick = 0;  //掉电计数清零，表示底盘在线
    }
		else if(Can2_rx_message_0.StdId == GIMBAL_DOWN_ID)
		{
		   Down_Rec_Number++;
			 Down_pitch = (short)(Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1]);
			 Down_yaw = (int)(Can2_rx_message_0.Data[2] << 24 | Can2_rx_message_0.Data[3] << 16 | Can2_rx_message_0.Data[4] << 8 | Can2_rx_message_0.Data[5] <<0);
			 Down_distance = (unsigned char)(Can2_rx_message_0.Data[6]);
			 Aim_positive_flag = (uint8_t)Can2_rx_message_0.Data[7];
			 
			 if (Down_yaw >= aim_yaw_Limit || Down_yaw <= -aim_yaw_Limit ||
					Down_pitch >= aim_pitch_Limit || Down_pitch <= -aim_pitch_Limit 
					/*distance >= distance_Limit*/)
			 {
					//armor_state = ARMOR_NO_AIM;
					//还在想
					Down_Receive_Now.Valid_flag = 0;
					return; //接到乱码包，直接退栈
			 }
			 
			 rec_pitch  = (Down_pitch) / 100.0f;
			 rec_yaw = (Down_yaw) / 100.0f;
			 
			 Down_Receive_Now.Down_pitch = (float)rec_pitch;
			 Down_Receive_Now.Down_yaw = (float)rec_yaw;
			 Down_Receive_Now.Down_distance = (float)Down_distance;
			 Down_Receive_Now.Valid_flag = Aim_positive_flag;
			 Angle_Trans();

		
		
		}
    
}

/***************************************************
* Function : 读取上云台的指示角度并做转化
***************************************************/
float robot_length=0.628; //机器人上下云台之间的竖直距离,单位为m
float k_angle; //tan(up)的值
void Angle_Trans(void)
{
	
   extern Down_Receive_t Down_Receive_Now;
   
	 k_angle = (robot_length/Down_Receive_Now.Down_distance - sinf(Down_Receive_Now.Down_pitch)) \
	            / (cosf(Down_Receive_Now.Down_yaw));
	
	// Down_Receive_Now.Up_Trans_yaw = acosf(sqrtf(1/(k_angle * k_angle + 1)));
	 Down_Receive_Now.Up_Trans_yaw = Down_Receive_Now.Down_yaw;//直接采用相同的YAW，使得两个云台在同一平面内
	 Down_Receive_Now.Up_Trans_pitch = asinf(k_angle * sqrtf(1/(k_angle * k_angle + 1)));


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
    else if (Can2_rx_message_1.StdId == Gyro_Yaw_ID)
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
short Armor_Aimed;
uint8_t armor_state = 0; //表示辅瞄是不是有找到目标
uint8_t last_armor_state = ARMOR_NO_AIM;
float aim_yaw, aim_pitch;
uint16_t Pc_RX_Num;
uint16_t Last_Pc_Rx_Num;
//float Last_aim_yaw,Last_aim_pitch;
short tx2_last_receive_flag; //表示有没有数据更新
short tx2_receive_flag;
float aim_yaw_Limit = 10000.0f, aim_pitch_Limit = 3000.0f;//对应着 aim_yaw在-100到100之间，aim_pitch在-30-30之间
float aim_yaw_Limit_V = 100.0f,aim_pitch_Limit_V= 30.0f;
uint16_t distance_Limit = 100 * 10;
uint8_t CV_Shoot_ABLE = 0; //判定视觉方面是否能够打子弹
PC_Receive_t PC_Receive;
uint16_t Pc_Rx_Num;
void PCReceive(uint8_t PCReceivebuffer[])
{
    run_time_check++;
	  Pc_RX_Num++;
    pc_pitch = (short)(PCReceivebuffer[2] << 8 | PCReceivebuffer[3]); //这里不能转为float，不然负数传过来会变为正数
    pc_yaw = (int)(PCReceivebuffer[4] << 24 | PCReceivebuffer[5] << 16 | PCReceivebuffer[6] << 8 | PCReceivebuffer[7] << 0);
   // distance = (uint8_t)PCReceivebuffer[8]; //7~1位给到distance
     
    if (pc_yaw >= aim_yaw_Limit || pc_yaw <= -aim_yaw_Limit ||
        pc_pitch >= aim_pitch_Limit || pc_pitch <= -aim_pitch_Limit 
        /*distance >= distance_Limit*/)
    {
        armor_state = ARMOR_NO_AIM;
        return; //接到乱码包，直接退栈
    }

    aim_yaw = (float)pc_yaw / 100.0f;
    aim_pitch = (float)pc_pitch / 100.0f;

    if (aim_yaw >= aim_yaw_Limit_V || aim_yaw <= -aim_yaw_Limit_V ||       
        aim_pitch >= aim_pitch_Limit_V || aim_pitch <= -aim_pitch_Limit_V)
    {
        armor_state = ARMOR_NO_AIM;
        return; //接到乱码包，直接退栈
    }
//		if((armor_state==ARMOR_AIMED)&&(ABS(aim_yaw-Last_aim_yaw)<2)&&(ABS(aim_pitch-Last_aim_pitch)<2.7)) //抑制识别时中心点的变化
//		{ 
//			aim_pitch=Last_aim_pitch;
//		  aim_yaw=Last_aim_yaw;
//		}
    Armor_Aimed=/*(armor_state==ARMOR_AIMED)?(80):*/(3); //检测到数据之后一段比较长的时间内都随着TX2发送来的数据来运动
   // tx2_last_receive_flag = tx2_receive_flag;
		if (run_time_check %(Armor_Aimed)== 0) //隔一段时间判断一次
    {
        tx2_receive_flag = (uint8_t)(PCReceivebuffer[1]&0xff);         //作为更新标志位
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

//    PC_Receive.RCdistance = (float)distance / 10.0f;

    PC_Receive.DisConnect = 0;
    tx2_last_receive_flag = tx2_receive_flag;
//		Last_aim_yaw=aim_yaw;  //记录yaw和pitch的变化
//		Last_aim_pitch=aim_pitch;  
		Pc_Rx_Num = !Pc_Rx_Num;
		Last_Pc_Rx_Num = Pc_RX_Num;
		//last_armor_state = armor_state; //感觉写在这里还是有问题
}


/*******************************************
* function: 接收来着下云台的瞄准角度
* 
********************************************/

uint16_t Down_Rec_Number;
short Down_pitch;
int Down_yaw;
uint8_t Aim_positive_flag;
float rec_pitch;
float rec_yaw;
uint8_t Down_distance;
Down_Receive_t Down_Receive_Now;

void Down_Receive(uint8_t Down_Receiver[])
{
     Down_Rec_Number++;
	   Down_pitch = (short)(Down_Receiver[1] << 8 | Down_Receiver[2]);
	   Down_yaw = (int)(Down_Receiver[3] << 24 | Down_Receiver[4] << 16 | Down_Receiver[5] << 8 | Down_Receiver[6] <<0);
     Down_distance = (unsigned char)(Down_Receiver[7]);
	   Aim_positive_flag = (uint8_t)Down_Receiver[8];
	   
	   if (Down_yaw >= aim_yaw_Limit || Down_yaw <= -aim_yaw_Limit ||
        Down_pitch >= aim_pitch_Limit || Down_pitch <= -aim_pitch_Limit 
        /*distance >= distance_Limit*/)
     {
        //armor_state = ARMOR_NO_AIM;
			  //还在想
			  Down_Receive_Now.Valid_flag = 0;
        return; //接到乱码包，直接退栈
     }
		 
     rec_pitch  = (Down_pitch) / 100.0f;
		 rec_yaw = (Down_yaw) / 100.0f;
     
     Down_Receive_Now.Down_pitch = (float)rec_pitch;
		 Down_Receive_Now.Down_yaw = (float)rec_yaw;
		 Down_Receive_Now.Down_distance = (float)Down_distance;
     Down_Receive_Now.Valid_flag = Aim_positive_flag;
     Angle_Trans();

}



















