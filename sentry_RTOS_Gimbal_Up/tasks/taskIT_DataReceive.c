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
}

/**
  * @brief  ������Ϣ����
  * @param  None
  * @retval None    
  */
uint32_t chassis_offline_tick = 0;  //���̵������������������20000����Ϊ���̵���
uint8_t chassis_offline_flag = 0; //���̵����־λ������������1
void CAN1_DataReceive_1(void) //���յ�������
{
    extern uint8_t Heat_ShootAbleFlag_0;
    extern uint8_t is_game_start;
    if (Can1_rx_message_1.StdId == SHOOTING_HEAT_ID)
    {
        Heat_ShootAbleFlag_0 = Can1_rx_message_1.Data[0];
        is_game_start = Can1_rx_message_1.Data[2];
        chassis_offline_tick = 0;  //����������㣬��ʾ��������
    }
}

/**
  * @brief  ����̨��������Ϣ�����������Ϣ����
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
    if (Can2_rx_message_1.StdId == Gyro_Yaw_ID)
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
short distance;
short armor_state = 0; //��ʾ�����ǲ������ҵ�Ŀ��
float aim_yaw, aim_pitch;
short tx2_last_receive_flag; //��ʾ��û�����ݸ���
short tx2_receive_flag;
float aim_yaw_Limit = 10000.0f, aim_pitch_Limit = 3000.0f;
uint16_t distance_Limit = 100 * 10;
uint8_t CV_Shoot_ABLE = 0; //�ж��Ӿ������Ƿ��ܹ����ӵ�
PC_Receive_t PC_Receive;
void PCReceive(uint8_t PCReceivebuffer[])
{
    run_time_check++;
    pc_pitch = (short)(PCReceivebuffer[2] << 8 | PCReceivebuffer[3]); //���ﲻ��תΪfloat����Ȼ�������������Ϊ����
    pc_yaw = (int)(PCReceivebuffer[4] << 24 | PCReceivebuffer[5] << 16 | PCReceivebuffer[6] << 8 | PCReceivebuffer[7] << 0);
    distance = (uint8_t)PCReceivebuffer[8]; //7~1λ����distance

    if (pc_yaw >= aim_yaw_Limit || pc_yaw <= -aim_yaw_Limit ||
        pc_pitch >= aim_pitch_Limit || pc_pitch <= -aim_pitch_Limit ||
        distance >= distance_Limit)
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

    if (run_time_check % 400== 0) //��һ��ʱ���ж�һ��
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
}
