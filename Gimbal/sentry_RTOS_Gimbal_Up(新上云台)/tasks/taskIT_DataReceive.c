#include "main.h"
#include "taskIT_DataReceive.h"

extern CanRxMsg Can1_rx_message_0, Can1_rx_message_1, Can2_rx_message_0, Can2_rx_message_1;

/**
 * @brief  ����չFIFO
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
  * @brief  ����̨��������Ϣ����
  * @param  None
  * @retval None    
  */
uint32_t chassis_offline_tick = 0;  //���̵������������������20000����Ϊ���̵���
uint8_t chassis_offline_flag = 0; //���̵����־λ������������1
uint8_t aim_flag;
uint16_t RX_Number=1;
void CAN1_DataReceive_1(void) //��������������
{
	  extern gimbal_motor_t MotoPitch;
    extern gimbal_motor_t MotoYaw;
	    if (Can1_rx_message_1.StdId == MotoPitch_ID_Up)
    {
        MotoPitch.Angle_ABS = Can1_rx_message_1.Data[0] << 8 | Can1_rx_message_1.Data[1];
        MotoPitch.real_speed = Can1_rx_message_1.Data[2] << 8 | Can1_rx_message_1.Data[3];
        MotoPitch.real_flow = Can1_rx_message_1.Data[4] << 8 | Can1_rx_message_1.Data[5]; 
			//RX_Number++;
    }
    else if (Can1_rx_message_1.StdId == MotoYaw_ID_Up)
    {
        MotoYaw.Angle_ABS = Can1_rx_message_1.Data[0] << 8 | Can1_rx_message_1.Data[1];
        MotoYaw.real_speed = Can1_rx_message_1.Data[2] << 8 | Can1_rx_message_1.Data[3];
        MotoYaw.real_flow = Can1_rx_message_1.Data[4] << 8 | Can1_rx_message_1.Data[5];
			//RX_Number++;
    }
		RX_Number = !RX_Number;
 
}

/**
  * @brief  ����̨��������Ϣ�����������Ϣ����
  * @param  None
  * @retval None
  */
uint16_t RX_Number;
int aim_Up_Pitch_100;
short aim_Up_Yaw_100;
float aim_Up_Pitch;//����̨������PitchЭֵͬ
float aim_Up_Yaw;//����̨������YawЭֵͬ
uint8_t aim_Valid_Flag;
float Bullet_Speed;
void CAN2_DataReceive_0(void)
{
    
//	  extern gimbal_motor_t MotoPitch;
//    extern gimbal_motor_t MotoYaw;
	  extern uint8_t Heat_ShootAbleFlag_2;
    extern uint8_t is_game_start;
	 // extern _2006_motor_t BodanMotor;
	  extern int16_t chassis_speed;
	  
	 if (Can2_rx_message_0.StdId == SHOOTING_HEAT_ID)
    {
        Heat_ShootAbleFlag_2 = Can2_rx_message_0.Data[0];
        is_game_start = Can2_rx_message_0.Data[2];
			  aim_flag = Can2_rx_message_0.Data[3]; //��׼�µ���ֹͣ�ź�
			  chassis_speed = (int16_t)(((int16_t)Can2_rx_message_0.Data[5] << 8  | (int16_t)Can2_rx_message_0.Data[6]) & 0xffff);
        chassis_offline_tick = 0;  //����������㣬��ʾ��������
    }
		else if(Can2_rx_message_0.StdId == GIMBAL_DOWN_ID)
		{
		    aim_Up_Pitch_100 = (short)(Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1]); 
		    aim_Up_Yaw_100 = (int)(Can2_rx_message_0.Data[2] << 24 | Can2_rx_message_0.Data[3] << 16 | Can2_rx_message_0.Data[4] << 8 | Can2_rx_message_0.Data[5]);
				aim_Up_Pitch = (float)aim_Up_Pitch / 100.0f;
				aim_Up_Yaw = (float)aim_Up_Yaw_100 / 100.0f;
				aim_Valid_Flag = (uint8_t)Can2_rx_message_0.Data[6];
		
		}
		else if(Can2_rx_message_0.StdId == BULLET_SPEED_ID)
	  {
			
		    memcpy(&Bullet_Speed ,&(Can1_rx_message_1.Data[0]),4);   //��ʱ���յ��ĵ���Ϊ100��
		    chassis_offline_tick = 0;//�����������
		}		
	
//		else if(Can2_rx_message_0.StdId == MotoPitch_ID)
//	  {
//		    MotoPitch.Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
//        MotoPitch.real_speed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
//        MotoPitch.real_flow = Can2_rx_message_0.Data[4] << 8 | Can2_rx_message_0.Data[5];
//		}
}

/**
  * @brief  ����̨��������Ϣ����
  * @param  None
  * @retval None
  */
gyro_Typedef Gyro_White;
uint32_t gyro_cnt;
float gyro_pitch_test;

uint32_t cnt_temp, cnt1; //float cnt2,cnt2temp;
float gz10e6;            //?????
void CAN2_DataReceive_1(void) //����������
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
  * @brief  TX2��Ϣ����
  * @param  None
  * @retval None
  */
volatile long run_time_check;
int pc_yaw;
short pc_pitch;
int pc_yaw_down;
short pc_pitch_down;
short distance;
short Armor_Aimed;
uint8_t armor_state = 0; //��ʾ�����ǲ������ҵ�Ŀ��
uint8_t last_armor_state=ARMOR_NO_AIM;
float aim_yaw, aim_pitch;
float aim_yaw_down,aim_pitch_down;
uint32_t Pc_RX_Num=0;
uint32_t Last_Pc_Rx_Num=0;
//float Last_aim_yaw,Last_aim_pitch;
short tx2_last_receive_flag; //��ʾ��û�����ݸ���
short tx2_receive_flag;
float aim_yaw_Limit = 10000.0f, aim_pitch_Limit = 3000.0f;//��Ӧ�� aim_yaw��-100��100֮�䣬aim_pitch��-30-30֮��
float aim_yaw_Limit_V = 100.0f,aim_pitch_Limit_V= 30.0f;
uint16_t distance_Limit = 100 * 10;
uint8_t CV_Shoot_ABLE = 0; //�ж��Ӿ������Ƿ��ܹ����ӵ�
PC_Receive_t PC_Receive;
//uint16_t Pc_Rx_Num;
void PCReceive(uint8_t PCReceivebuffer[])
{
    run_time_check++;
	  Pc_RX_Num++;
    pc_pitch = (short)(PCReceivebuffer[2] << 8 | PCReceivebuffer[3]); //���ﲻ��תΪfloat����Ȼ�������������Ϊ����
    pc_yaw = (int)(PCReceivebuffer[4] << 24 | PCReceivebuffer[5] << 16 | PCReceivebuffer[6] << 8 | PCReceivebuffer[7] << 0);
   // distance = (uint8_t)PCReceivebuffer[8]; //7~1λ����distance
    pc_pitch_down = (short)(PCReceivebuffer[9] << 8 | PCReceivebuffer[10]); 
	  pc_yaw_down = (int)(PCReceivebuffer[11] << 24 | PCReceivebuffer[12] << 16 | PCReceivebuffer[13] << 8 | PCReceivebuffer[14] << 0);  
	
    if (pc_yaw >= aim_yaw_Limit || pc_yaw <= -aim_yaw_Limit ||
        pc_pitch >= aim_pitch_Limit || pc_pitch <= -aim_pitch_Limit 
        /*distance >= distance_Limit*/)
    {
        armor_state = ARMOR_NO_AIM;
        return; //�ӵ��������ֱ����ջ
    }

    aim_yaw = (float)pc_yaw / 100.0f;
    aim_pitch = (float)pc_pitch / 100.0f;
//    aim_yaw_down = (float)pc_yaw_down / 100.0f;
//		aim_pitch_down = (float)aim_pitch_down / 100.0f;
		
    if (aim_yaw >= aim_yaw_Limit_V || aim_yaw <= -aim_yaw_Limit_V ||       
        aim_pitch >= aim_pitch_Limit_V || aim_pitch <= -aim_pitch_Limit_V)
    {
        armor_state = ARMOR_NO_AIM;
        return; //�ӵ��������ֱ����ջ
    }
//		if((armor_state==ARMOR_AIMED)&&(ABS(aim_yaw-Last_aim_yaw)<2)&&(ABS(aim_pitch-Last_aim_pitch)<2.7)) //����ʶ��ʱ���ĵ�ı仯
//		{ 
//			aim_pitch=Last_aim_pitch;
//		  aim_yaw=Last_aim_yaw;
//		}
    Armor_Aimed=/*(armor_state==ARMOR_AIMED)?(80):*/(3); //��⵽����֮��һ�αȽϳ���ʱ���ڶ�����TX2���������������˶�
   // tx2_last_receive_flag = tx2_receive_flag;
		if (run_time_check %(Armor_Aimed)== 0) //��һ��ʱ���ж�һ��
    {
        tx2_receive_flag = (uint8_t)(PCReceivebuffer[1]&0xff);         //��Ϊ���±�־λ
        if (tx2_receive_flag == tx2_last_receive_flag) //���û�����ݸ���
        {
            armor_state = ARMOR_NO_AIM; //ûĿ��
        }
        else
        {
            armor_state = ARMOR_AIMED; //��Ŀ��
        }
    }
    PC_Receive.RCPitch = (float)(aim_pitch); //����ֵ
    PC_Receive.RCYaw = (float)(aim_yaw);

//    PC_Receive.RCdistance = (float)distance / 10.0f;

    PC_Receive.DisConnect = 0;
    tx2_last_receive_flag = tx2_receive_flag;
//		Last_aim_yaw=aim_yaw;  //��¼yaw��pitch�ı仯
//		Last_aim_pitch=aim_pitch;  
		//Pc_Rx_Num = !Pc_Rx_Num;
		//Last_Pc_Rx_Num = Pc_RX_Num;
		armor_state = ((Pc_RX_Num==1)&&(Last_Pc_Rx_Num==0))? ARMOR_NO_AIM:armor_state;
		//Last_Pc_Rx_Num = Pc_RX_Num;
}
