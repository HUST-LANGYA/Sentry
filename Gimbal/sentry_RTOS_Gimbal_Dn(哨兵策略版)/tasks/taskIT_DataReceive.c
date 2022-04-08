#include "main.h"
#include "taskIT_DataReceive.h"

extern CanRxMsg Can1_rx_message_0, Can1_rx_message_1, Can2_rx_message_0, Can2_rx_message_1;

/**
  * @brief  ����Ħ��������
  * @param  None
  * @retval None
  */
uint16_t RX_Number;
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
	  RX_Number++;
}

/**
  * @brief  ������Ϣ����
  * @param  None
  * @retval None
  */
uint32_t chassis_offline_tick = 0;  //���̵������������������20000����Ϊ���̵���
uint8_t chassis_offline_flag = 0; //���̵����־λ������������1
uint8_t defend_flag;

//����̨���ݽ���
uint16_t Up_Rec_Number;
short Up_pitch;
int Up_yaw;
uint8_t Aim_positive_flag;
float rec_pitch;
float rec_yaw;
uint8_t Up_distance;
Up_Receive_t Up_Receive_Now;

void CAN1_DataReceive_1(void) //���յ�������
{
    extern uint8_t Heat_ShootAbleFlag_2;
    extern uint8_t is_game_start;
	  extern float aim_yaw_Limit, aim_pitch_Limit;
    if (Can1_rx_message_1.StdId == SHOOTING_HEAT_ID)
    {
        Heat_ShootAbleFlag_2 = Can1_rx_message_1.Data[1];
        is_game_start = Can1_rx_message_1.Data[2];
			  defend_flag = Can1_rx_message_1.Data[4];
        chassis_offline_tick = 0;//�����������
    }
		else if(Can1_rx_message_1.StdId == GIMBAL_UP_ID)
		{
		   Up_Rec_Number++;
	     Up_pitch = (short)(Can1_rx_message_1.Data[0] << 8 | Can1_rx_message_1.Data[1]);
	     Up_yaw = (int)(Can1_rx_message_1.Data[2] << 24 | Can1_rx_message_1.Data[3] << 16 | Can1_rx_message_1.Data[4] << 8 | Can1_rx_message_1.Data[5] <<0);
       Up_distance = (unsigned char)(Can1_rx_message_1.Data[6]);
	     Aim_positive_flag = (uint8_t)Can1_rx_message_1.Data[7];
	   
	     if (Up_yaw >= aim_yaw_Limit || Up_yaw <= -aim_yaw_Limit ||
        Up_pitch >= aim_pitch_Limit || Up_pitch <= -aim_pitch_Limit 
        /*distance >= distance_Limit*/)
      {
        //armor_state = ARMOR_NO_AIM;
			  //������
			  Up_Receive_Now.Valid_flag = 0;
        return; //�ӵ��������ֱ����ջ
      }
		 
			 rec_pitch  = (Up_pitch) / 100.0f;
			 rec_yaw = (Up_yaw) / 100.0f;
			 
			 Up_Receive_Now.Up_pitch = (float)rec_pitch;
			 Up_Receive_Now.Up_yaw = (float)rec_yaw;
			 Up_Receive_Now.Up_distance = (float)Up_distance;
			 Up_Receive_Now.Valid_flag = Aim_positive_flag;
			 Angle_Trans();
		}
}
/***************************************************
* Function : ��ȡ����̨��ָʾ�ǶȲ���ת��
***************************************************/

float robot_length; //������������̨֮�����ֱ����
float k_angle; //tan(up)��ֵ
void Angle_Trans(void)
{
	
   extern Up_Receive_t Up_Receive_Now;
   
	 k_angle = (robot_length/Up_Receive_Now.Up_distance - sinf(Up_Receive_Now.Up_pitch)) \
	            / (cosf(Up_Receive_Now.Up_yaw));
	
	 //Up_Receive_Now.Down_Trans_yaw = acosf(sqrtf(1/(k_angle * k_angle + 1)));
	 Up_Receive_Now.Down_Trans_yaw =  Up_Receive_Now.Up_yaw;//ֱ�Ӳ�����ͬ��YAW��ʹ��������̨��ͬһƽ����
	 Up_Receive_Now.Down_Trans_pitch = asinf(k_angle * sqrtf(1/(k_angle * k_angle + 1)));


}


/**
  * @brief  ����̨��������Ϣ�����������Ϣ����
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
  * @brief  ����̨��������Ϣ����
  * @param  None
  * @retval None
  */
gyro_Typedef Gyro_White;
uint32_t gyro_cnt;
float gyro_pitch_test;
uint16_t Gyro_Rx_Number;
uint32_t cnt_temp, cnt1;      //float cnt2,cnt2temp;
float gz10e6;                 //?????
void CAN2_DataReceive_1(void) //����������
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
  * @brief  TX2��Ϣ����
  * @param  None
  * @retval None
  */
volatile long run_time_check;
int pc_yaw;
short pc_pitch;
short distance;
uint8_t armor_state = 0; //��ʾ�����ǲ������ҵ�Ŀ��
uint8_t last_armor_state = 0;
float aim_yaw, aim_pitch;
short tx2_last_receive_flag; //��ʾ��û�����ݸ���
short tx2_receive_flag;
float aim_yaw_Limit = 18001.0f, aim_pitch_Limit = 3000.0f;
uint16_t distance_Limit = 100*10;
uint8_t CV_Shoot_ABLE = 0; //�ж��Ӿ������Ƿ��ܹ����ӵ�
PC_Receive_t PC_Receive;
void PCReceive(uint8_t PCReceivebuffer[])
{
    run_time_check++;
    pc_pitch = (short)(PCReceivebuffer[2] << 8 | PCReceivebuffer[3]); //���ﲻ��תΪfloat����Ȼ�������������Ϊ����
    pc_yaw = (int)(PCReceivebuffer[4] << 24 | PCReceivebuffer[5] << 16 | PCReceivebuffer[6] << 8 | PCReceivebuffer[7] << 0);
    distance = (short)PCReceivebuffer[8] /*>>1*/; //7~0λ����distance
   
    if (pc_yaw >= aim_yaw_Limit|| pc_yaw <= -aim_yaw_Limit ||
        pc_pitch >= aim_pitch_Limit || pc_pitch <= -aim_pitch_Limit ||
        distance >= distance_Limit )
    {
        armor_state = ARMOR_NO_AIM;
        return; //�ӵ��������ֱ����ջ
    }
    
    aim_yaw = (float)pc_yaw / 100.0f;
    aim_pitch = (float)pc_pitch / 100.0f;
  
    if (aim_yaw >= aim_yaw_Limit || aim_yaw <= -aim_yaw_Limit ||
        aim_pitch >= aim_pitch_Limit || aim_pitch <= -aim_pitch_Limit)
    {
        armor_state = ARMOR_NO_AIM;
        return; //�ӵ��������ֱ����ջ
    }
    
    if (run_time_check % 400 == 0) //��һ��ʱ���ж�һ��
    {
        tx2_receive_flag = PCReceivebuffer[1];         //��Ϊ���±�־λ
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

    PC_Receive.RCdistance = (float)distance / 10.0f;

    PC_Receive.DisConnect = 0;
    tx2_last_receive_flag = tx2_receive_flag;
		//last_armor_state = armor_state;
}



uint16_t Up_Rec_Number;
short Up_pitch;
int Up_yaw;
uint8_t Aim_positive_flag;
float rec_pitch;
float rec_yaw;
uint8_t Up_distance;
Up_Receive_t Up_Receive_Now;

void Up_Receive(uint8_t Up_Receiver[])
{
     Up_Rec_Number++;
	   Up_pitch = (short)(Up_Receiver[1] << 8 | Up_Receiver[2]);
	   Up_yaw = (int)(Up_Receiver[3] << 24 | Up_Receiver[4] << 16 | Up_Receiver[5] << 8 | Up_Receiver[6] <<0);
     Up_distance = (unsigned char)(Up_Receiver[7]);
	   Aim_positive_flag = (uint8_t)Up_Receiver[8];
	   
	   if (Up_yaw >= aim_yaw_Limit || Up_yaw <= -aim_yaw_Limit ||
        Up_pitch >= aim_pitch_Limit || Up_pitch <= -aim_pitch_Limit 
        /*distance >= distance_Limit*/)
     {
        //armor_state = ARMOR_NO_AIM;
			  //������
			  Up_Receive_Now.Valid_flag = 0;
        return; //�ӵ��������ֱ����ջ
     }
		 
     rec_pitch  = (Up_pitch) / 100.0f;
		 rec_yaw = (Up_yaw) / 100.0f;
     
     Up_Receive_Now.Up_pitch = (float)rec_pitch;
		 Up_Receive_Now.Up_yaw = (float)rec_yaw;
		 Up_Receive_Now.Up_distance = (float)Up_distance;
     Up_Receive_Now.Valid_flag = Aim_positive_flag;
     Angle_Trans();

}




















