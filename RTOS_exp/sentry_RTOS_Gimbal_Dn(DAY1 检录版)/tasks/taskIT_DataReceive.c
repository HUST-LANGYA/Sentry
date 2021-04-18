#include "main.h"
#include "taskIT_DataReceive.h"

extern CanRxMsg Can1_rx_message_0, Can1_rx_message_1, Can2_rx_message_0, Can2_rx_message_1;

int32_t test_receive;
/**
  * @brief  ��������̨�͵��̷�������Ϣ��ң������Ϣ��
  * @param  None
  * @retval None
  */
void CAN1_DataReceive_0(void)
{
    extern RC_Ctl_t RC_Ctl;
    if(Can1_rx_message_0.StdId == Remote_Control_ID) //���մ�����̨����ң������֡
    {
        RC_Ctl.rc.s1 = ((int8_t) Can1_rx_message_0.Data[5]>>2 ) & 0x03;
        RC_Ctl.rc.s2 = ((int8_t) Can1_rx_message_0.Data[5]) & 0x03;
        if(RC_Ctl.rc.s2 == 0 || RC_Ctl.rc.s1 == 0) 
            return;//����������յİ���ֱ�Ӷ���
        RC_Ctl.rc.ch0=((int16_t)Can1_rx_message_0.Data[0]<<3 | (int16_t)Can1_rx_message_0.Data[1]>>5) & 0x7ff;
        RC_Ctl.rc.ch1=((int16_t)Can1_rx_message_0.Data[1]<<6 | (int16_t)Can1_rx_message_0.Data[2]>>2 ) & 0x7ff;
        RC_Ctl.rc.ch2=((int16_t)Can1_rx_message_0.Data[2]<<9 | (int16_t)Can1_rx_message_0.Data[3]<<1 | (int16_t)Can1_rx_message_0.Data[4]>>7 ) & 0x7ff;
        RC_Ctl.rc.ch3=((int16_t)Can1_rx_message_0.Data[4]<<4 | (int16_t)Can1_rx_message_0.Data[5]>>4 ) & 0x7ff;
    }
}

/**
  * @brief  ������Ϣ����
  * @param  None
  * @retval None
  */
void CAN1_DataReceive_1(void) //���յ�������
{
    if (Can1_rx_message_1.StdId == SHOOTING_HEAT_ID)
    {
        /*���������ǹ������������*/
    }
}


/**
  * @brief  ����̨��������Ϣ�����������Ϣ����
  * @param  None
  * @retval None
  */
//int32_t rx_test = 0;
void CAN2_DataReceive_0(void)
{
    extern gimbal_motor_t MotoPitch;
    extern gimbal_motor_t MotoYaw;
    extern _2006_motor_t BodanMotor;
    extern block_disconnect_t block_disconnect;
    if (Can2_rx_message_0.StdId == MotoPitch_ID)
    {
        //block_disconnect.Pitch_Last_Cnt = GetSysCnt();
        MotoPitch.Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
        MotoPitch.real_speed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
        MotoPitch.real_flow = Can2_rx_message_0.Data[4] << 8 | Can2_rx_message_0.Data[5];
        //rx_test++;
    }
    else if (Can2_rx_message_0.StdId == MotoYaw_ID)
    {
        //block_disconnect.Yaw_Last_Cnt = GetSysCnt();
        MotoYaw.Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
        MotoYaw.real_speed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
        MotoYaw.real_flow = Can2_rx_message_0.Data[4] << 8 | Can2_rx_message_0.Data[5];
    }
    else if (Can2_rx_message_0.StdId == BodanMotor_Dn_ID)
    {
        //block_disconnect.Bodan_Last_Cnt = GetSysCnt();
        BodanMotor.Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
        BodanMotor.RealSpeed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
    }
    //	else if(Can1_rx_message.StdId == MASTER_ID)
    //	{
    //		Robot* sentry = GetSentry();
    //		memcpy(&sentry->Journey,Can1_rx_message.Data,4);
    //		sentry->Chassis_Speed = sentry->Journey - sentry->Journey_last;
    //		sentry->Journey_last = sentry->Journey;
    //		(*block_disconnect).is_right_limit = Can1_rx_message.Data[4];
    //		(*block_disconnect).is_left_limit = Can1_rx_message.Data[5];
    //		(*block_disconnect).is_chassis_down = Can1_rx_message.Data[6];
    //	}
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
float gz10e6;  //?????
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
}



/*extern */volatile long run_time_check;
/**********************************************************************************************************
*�� �� ��: PCReceive
*����˵��: USART6��PCͨ�Ž������ݴ���   
*��    ��: rx_buffer[]
*�� �� ֵ: ��
**********************************************************************************************************/
int pc_yaw;
short pc_pitch;
short distance;
short armor_state = 0;		//��ʾ�����ǲ������ҵ�Ŀ��
float aim_yaw, aim_pitch;
short tx2_last_receive_flag;	//��ʾ��û�����ݸ���
short tx2_receive_flag;	
//uint8_t CV_is_Catching = 0; //�Ӿ�����һ��bit�ж��Ƿ����(��armor_state�жϾ��У�
uint8_t CV_Shoot_ABLE = 0; //�ж��Ӿ������Ƿ��ܹ����ӵ�
PC_Receive_t PC_Receive;
void PCReceive(uint8_t/*unsigned char*/ PCReceivebuffer[])
{				
    run_time_check++;
    pc_pitch = (short)(PCReceivebuffer[2]<<8|PCReceivebuffer[3]);//���ﲻ��תΪfloat����Ȼ�������������Ϊ����
    pc_yaw = (int)(PCReceivebuffer[4]<<24|PCReceivebuffer[5]<<16|PCReceivebuffer[6]<<8|PCReceivebuffer[7]<<0);
    distance = (short)PCReceivebuffer[8]/*>>1*/;//7~0λ����distance
    //CV_is_Catching = (uint8_t) PCReceivebuffer[8] & 0x01; //�ж��Ƿ���뼤��״̬
    aim_yaw =  (float)pc_yaw/100.0f;
    aim_pitch = (float)pc_pitch/100.0f;
    if(run_time_check % 500 == 0)	//��һ��ʱ���ж�һ��
    {
        tx2_receive_flag = PCReceivebuffer[1];	//��Ϊ���±�־λ
        if(tx2_receive_flag == tx2_last_receive_flag)		//���û�����ݸ���
        {
            armor_state = ARMOR_NO_AIM;		//ûĿ��
        }
        else
        {
            armor_state = ARMOR_AIMED;		//��Ŀ��
        }
    }
    PC_Receive.RCPitch = (float)(aim_pitch);	//����ֵ
    PC_Receive.RCYaw = (float)(aim_yaw);
    
    PC_Receive.RCdistance = (float)distance/10.0f;
    
    PC_Receive.DisConnect=0;
    tx2_last_receive_flag = tx2_receive_flag;
}


//int16_t TempPC;
//int32_t tempfliter;
//int16_t Bspeed;
//extern float PC_PitchSet, PC_YawSet;
//float test_Pitch = 0, test_Yaw = 0;
//uint32_t tick1,tick2;
//float pitchFilterBuf[20],yawFilterBuf[20];//�Ŵ�PC���յĴ��˲���������


///**
//  * @brief  PC��Ϣ����
//  * @param  ��Ϣ����
//  * @retval None
//  */
//int16_t DataTemp;
//void PCReceive(uint8_t Buf[])
//{

//    extern block_disconnect_t block_disconnect;
//    extern State_t Sentry_State;
//    if (Buf[0] == '!' && Buf[7] == '#')
//    {
//        if (Verify_CRC8_Check_Sum(&Buf[1], 6))
//        {
//            //block_disconnect.PC_Last_Cnt = GetSysCnt();
//            if (Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_PC)
//            {
//                if (Buf[1] == 0x05)
//                {                     
//                    //extern FilterFIR filterPC;
//                        
//                    DataTemp = Buf[2] << 8 | Buf[3];
//                    //PC_PitchSet = DataTemp / 100.0f;
//                    //PC_PitchSet = FIR_filter_RT(filterPC, DataTemp / 100.0f); //�˲��㷨
//                    
//                    DataTemp = Buf[4] << 8 | Buf[5];
//                    //PC_YawSet = DataTemp / 100.0f;//FIR_filter_RT();
//                    //PC_YawSet = FIR_filter_RT(filterPC, DataTemp / 100.0f);
//                }   
//                    //					/*********************************/
//                    //					while(PC_YawSet - Gyro_White.YAW_INC>180)
//                    //					{
//                    //						PC_YawSet -= 360;
//                    //					}
//                    //					while(PC_YawSet - Gyro_White.YAW_INC<-180)
//                    //					{
//                    //						PC_YawSet += 360;
//                    //					}
//                    ////					if(Sentry.hero_alarm == 0)					//ԭ��д�� ��Ӣ�۴�������̨������

//                    //					MotoYaw->PidPos.SetPoint = PC_YawSet;
//                    //					MotoPitch->PidPos.SetPoint = -PC_PitchSet/360*8191+PITCH_ZERO_POS;
//                    //				}
//                    //				else if(Buf[1] == 0x01)
//                    //				{
//                    //					TempPC = Buf[2]<<8|Buf[3];
//                    ////					if(Sentry.State == SENTRY_SCANNING)			//֮ǰ������PC����
//                    ////						Sentry.aim_speed = Buf[2]<<8|Buf[3];
//                    //				}
//                    //				else if(Buf[1] == 0x02)
//                    //				{
//                    //					int8_t temp;
//                    //					temp = Buf[4]<<8|Buf[5];
//                    //					if(temp == 26)
//                    //					{
//                    //						Sentry.FrictionSpeedSet = 12000;
//                    //						Sentry.BulletSpeedSet = 26;
//                    //					}
//                    //					else
//                    //					{
//                    // 						Sentry.BulletSpeedSet = 16;
//                    //						Sentry.FrictionSpeedSet = 5700;
//                    //					}
//                    //					(*sentry).is_shoot = Buf[2]<<8|Buf[3];
//            }
//        }
//    }
//}



//uint8_t SaveBuffer[60];
//uint16_t DataLen;
//uint16_t cmd_id;
//int32_t number;
///**
//  * @brief  ��ȡ����ϵͳ����
//  * @param  None
//  * @retval None
//  */
//void JudgeBuffReceive(uint8_t ReceiveBuffer[])
//{
//    //	uint16_t cmd_id;
//    //	int8_t ptrPackHead;
//    //	static int16_t last_hp = 600;
//    //	memcpy(&SaveBuffer[JudgeBufBiggestSize],&ReceiveBuffer[0],JudgeBufBiggestSize);
//    //	for(ptrPackHead=0;ptrPackHead<JudgeBufBiggestSize;ptrPackHead++)
//    //	{
//    //		if(SaveBuffer[ptrPackHead]==0xA5)  //?? ?????
//    //		{
//    //			if((Verify_CRC8_Check_Sum(&SaveBuffer[ptrPackHead],5)==1))//  ???? CRC??  5?CRC
//    //			{
//    //				block_disconnect.Judge_Last_Cnt = GetSysCnt();
//    //				cmd_id=(SaveBuffer[ptrPackHead+6])&0xff;
//    //				cmd_id=(cmd_id<<8)|SaveBuffer[ptrPackHead+5];  //??   6 7?ID?
//    //				DataLen=SaveBuffer[ptrPackHead+2]&0xff;
//    //				DataLen=(DataLen<<8)|SaveBuffer[ptrPackHead+1];//??  2 3???
//    //				if((cmd_id==0x0201)&&(Verify_CRC16_Check_Sum(&SaveBuffer[ptrPackHead],DataLen+9)))  //???????9
//    //				{
//    //					JudgeReceive.robot_id = SaveBuffer[ptrPackHead+7];
//    //					JudgeReceive.RobotLevel=SaveBuffer[ptrPackHead+8];//   9???????
//    //					JudgeReceive.remainHP=(SaveBuffer[ptrPackHead+10]<<8)|SaveBuffer[ptrPackHead+9]; // 10,11????????
//    //					if(last_hp - JudgeReceive.remainHP>=100)
//    //					{
//    //						if((Sentry.HurtDir&0x0c)>>2 != 0)
//    //						{
//    //							Sentry.hero_alarm = 1;
//    //							u8 temp = (Sentry.HurtDir&0x0c)<<2;
//    //							Sentry.HurtDir |= temp;
//    //							last_hp = JudgeReceive.remainHP;
//    //						}
//    //					}
//    //					else
//    //					{
//    //						Sentry.HurtDir &= 0x33;
//    //						last_hp = JudgeReceive.remainHP;
//    //					}
//    //				}

//    //				if((cmd_id==0x0207)&&(Verify_CRC16_Check_Sum(&SaveBuffer[ptrPackHead],DataLen+9)))
//    //				{
//    //					if(SaveBuffer[ptrPackHead+7]==1)   //?8?? ????  ?????
//    //					{
//    //						JudgeReceive.bulletFreq=SaveBuffer[ptrPackHead+8];  //?9???????
//    //						Chassis_a.idata=SaveBuffer[ptrPackHead+12];  //10 11 12 13 ??????
//    //						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[ptrPackHead+11];
//    //						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[ptrPackHead+10];
//    //						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[ptrPackHead+9];
//    //						JudgeReceive.bulletSpeed17=Chassis_a.fdata;
//    //						HeatCal += JudgeReceive.bulletSpeed17;
//    //						number++;
//    //						block_disconnect.Bullect_Last_Cnt = block_disconnect.BullectCnt;
//    //					}
//    //					if(SaveBuffer[ptrPackHead+7]==2)  //?8?? ????  ?????
//    //					{
//    //						JudgeReceive.bulletFreq=SaveBuffer[ptrPackHead+8];
//    //						Chassis_a.idata=SaveBuffer[ptrPackHead+12];
//    //						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[ptrPackHead+11];
//    //						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[ptrPackHead+10];
//    //						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[ptrPackHead+9];
//    //						JudgeReceive.bulletSpeed42=Chassis_a.fdata;
//    //					}
//    //				}
//    //				if((cmd_id==0x0202)&&(Verify_CRC16_Check_Sum(&SaveBuffer[ptrPackHead],DataLen+9)))
//    //				{
//    //					Chassis_energy.idata=SaveBuffer[ptrPackHead+14];
//    //					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[ptrPackHead+13];
//    //					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[ptrPackHead+12];
//    //					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[ptrPackHead+11];
//    //					JudgeReceive.realChassispower=Chassis_energy.fdata;//????
//    //					Chassis_energy.idata=SaveBuffer[ptrPackHead+18];
//    //					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[ptrPackHead+17];
//    //					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[ptrPackHead+16];
//    //					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[ptrPackHead+15];
//    //					JudgeReceive.remainEnergy=Chassis_energy.fdata;//????
//    //					memcpy(&JudgeReceive.remainEnergy,&SaveBuffer[ptrPackHead+7+8],2);
//    //					memcpy(&JudgeReceive.shooterHeat17,&SaveBuffer[ptrPackHead+7+10],2);
//    //				}
//    //				if((cmd_id==0x0206)&&(Verify_CRC16_Check_Sum(&SaveBuffer[ptrPackHead],DataLen+9)))
//    //				{
//    //					JudgeReceive.armor_id = SaveBuffer[ptrPackHead+7+0]&0x0f;
//    //					JudgeReceive.hurt_type = (SaveBuffer[ptrPackHead+7+0]>>4)&0x0f;
//    //					if(JudgeReceive.hurt_type == 0)
//    //					{
//    //						if(JudgeReceive.armor_id == 0)
//    //							Sentry.HurtDir |= 0x05;			//��һλ��1
//    //						else if(JudgeReceive.armor_id == 1)
//    //							Sentry.HurtDir |= 0x0a;
//    //					}
//    //				}
//    //				if((cmd_id==0x0301)&&(Verify_CRC16_Check_Sum(&SaveBuffer[ptrPackHead],DataLen+9)))
//    //				{
//    //					int8_t msg = (SaveBuffer[ptrPackHead+7+1]<<8)|SaveBuffer[ptrPackHead+7];
//    //					int8_t send_id = (SaveBuffer[ptrPackHead+7+3]<<8)|SaveBuffer[ptrPackHead+7+2];
//    //					int8_t receive_id = (SaveBuffer[ptrPackHead+7+5]<<8)|SaveBuffer[ptrPackHead+7+4];
//    //					if(((JudgeReceive.robot_id == 7 && send_id == 6 && receive_id == 7)||(JudgeReceive.robot_id == 17 && send_id == 16 && receive_id == 17))&&msg == 0x0200)
//    //					{
//    //						block_disconnect.Drone_Last_Cnt = GetSysCnt();
//    //						u8 temp = SaveBuffer[ptrPackHead+7+6];
//    ////						if(temp == 0)										//�ɴ��жϵ�����100Ѫ�˺�
//    ////						{
//    ////							Sentry.drone_alarm = 0;
//    ////							Sentry.dron_OverTime = 0;
//    ////						}
//    ////						else if(temp == 1 && Sentry.dron_OverTime == 0)
//    ////							Sentry.drone_alarm = 1;
//    ////						if(temp != 0 && temp != 1)
//    ////							Sentry.drone_alarm = 1;
//    //						if(temp == 1||temp == 0)
//    //							Sentry.drone_alarm = temp;
//    //						else
//    //							Sentry.drone_alarm = 1;
//    //					}
//    //				}
//    //				if((cmd_id==0x0003)&&(Verify_CRC16_Check_Sum(&SaveBuffer[ptrPackHead],DataLen+9)))  //???????9
//    //				{
//    //					JudgeReceive.OtherRobot.red_1_robot_HP = (SaveBuffer[ptrPackHead+8]<<8)|SaveBuffer[ptrPackHead+7];
//    //					JudgeReceive.OtherRobot.red_2_robot_HP = (SaveBuffer[ptrPackHead+10]<<8)|SaveBuffer[ptrPackHead+9];
//    //					JudgeReceive.OtherRobot.red_3_robot_HP = (SaveBuffer[ptrPackHead+12]<<8)|SaveBuffer[ptrPackHead+11];
//    //					JudgeReceive.OtherRobot.red_4_robot_HP = (SaveBuffer[ptrPackHead+14]<<8)|SaveBuffer[ptrPackHead+13];
//    //					JudgeReceive.OtherRobot.red_5_robot_HP = (SaveBuffer[ptrPackHead+16]<<8)|SaveBuffer[ptrPackHead+15];
//    //					JudgeReceive.OtherRobot.red_7_robot_HP = (SaveBuffer[ptrPackHead+18]<<8)|SaveBuffer[ptrPackHead+17];
//    //					JudgeReceive.OtherRobot.red_base_HP = (SaveBuffer[ptrPackHead+20]<<8)|SaveBuffer[ptrPackHead+19];
//    //					JudgeReceive.OtherRobot.blue_1_robot_HP = (SaveBuffer[ptrPackHead+22]<<8)|SaveBuffer[ptrPackHead+21];
//    //					JudgeReceive.OtherRobot.blue_2_robot_HP = (SaveBuffer[ptrPackHead+24]<<8)|SaveBuffer[ptrPackHead+23];
//    //					JudgeReceive.OtherRobot.blue_3_robot_HP = (SaveBuffer[ptrPackHead+26]<<8)|SaveBuffer[ptrPackHead+25];
//    //					JudgeReceive.OtherRobot.blue_4_robot_HP = (SaveBuffer[ptrPackHead+28]<<8)|SaveBuffer[ptrPackHead+27];
//    //					JudgeReceive.OtherRobot.blue_5_robot_HP = (SaveBuffer[ptrPackHead+30]<<8)|SaveBuffer[ptrPackHead+29];
//    //					JudgeReceive.OtherRobot.blue_7_robot_HP = (SaveBuffer[ptrPackHead+32]<<8)|SaveBuffer[ptrPackHead+31];
//    //					JudgeReceive.OtherRobot.blue_base_HP = (SaveBuffer[ptrPackHead+34]<<8)|SaveBuffer[ptrPackHead+33];
//    //				}
//    //				if((cmd_id==0x0208)&&(Verify_CRC16_Check_Sum(&SaveBuffer[ptrPackHead],DataLen+9)))  //???????9
//    //				{
//    //					JudgeReceive.bullet_remaining_num = (SaveBuffer[ptrPackHead+8]<<8)|SaveBuffer[ptrPackHead+7];
//    //				}

//    //			}
//    //		}
//    //	}

//    //
//    //				if(JudgeReceive.LastShooterHeat17 != JudgeReceive.shooterHeat17)
//    //				{
//    //						JudgeReceive.HeatUpdate_NoUpdate = 0x01;//??????0x01
//    //				}
//    //					 JudgeReceive.LastShooterHeat17 = JudgeReceive.shooterHeat17;
//    //				if(JudgeReceive.LastbulletSpeed17 != JudgeReceive.bulletSpeed17)
//    //				{
//    //					 JudgeReceive.SpeedUpdate_NoUpdate = 0x01;
//    //				}
//    //				JudgeReceive.LastbulletSpeed17=JudgeReceive.bulletSpeed17;
//    //

//    //	memcpy(&SaveBuffer[0],&SaveBuffer[JudgeBufBiggestSize],JudgeBufBiggestSize);
//}



///**
//  * @brief  ���������ǣ��°棩��Ϣ
//  * @param  rx_buffer16λ����
//  * @retval None
//  */
//void GyroReceiveFunc(uint8_t gyroBuffer[], int8_t BufferNum)
//{
//    //	FloatlongType GyroReceiveYaw,GyroReceiveGZ,GyroReceivePITCH,GyroReceiveGY;
//    //  GyroReceiveYaw.idata=gyroBuffer[(BufferNum+4)%GYRO_BUF_SIZE];
//    //  GyroReceiveYaw.idata=GyroReceiveYaw.idata<<8|gyroBuffer[(BufferNum+3)%GYRO_BUF_SIZE];
//    //  GyroReceiveYaw.idata=GyroReceiveYaw.idata<<8|gyroBuffer[(BufferNum+2)%GYRO_BUF_SIZE];
//    //  GyroReceiveYaw.idata=GyroReceiveYaw.idata<<8|gyroBuffer[(BufferNum+1)%GYRO_BUF_SIZE];
//    //  Gyro_White.YAW_ABS = GyroReceiveYaw.fdata;
//    //
//    //  GyroReceiveGZ.idata=gyroBuffer[(BufferNum+8)%GYRO_BUF_SIZE];
//    //  GyroReceiveGZ.idata=GyroReceiveGZ.idata<<8|gyroBuffer[(BufferNum+7)%GYRO_BUF_SIZE];
//    //  GyroReceiveGZ.idata=GyroReceiveGZ.idata<<8|gyroBuffer[(BufferNum+6)%GYRO_BUF_SIZE];
//    //  GyroReceiveGZ.idata=GyroReceiveGZ.idata<<8|gyroBuffer[(BufferNum+5)%GYRO_BUF_SIZE];
//    //  Gyro_White.GZ = GyroReceiveGZ.fdata;

//    //  GyroReceivePITCH.idata=gyroBuffer[(BufferNum+12)%GYRO_BUF_SIZE];
//    //  GyroReceivePITCH.idata=GyroReceivePITCH.idata<<8|gyroBuffer[(BufferNum+11)%GYRO_BUF_SIZE];
//    //  GyroReceivePITCH.idata=GyroReceivePITCH.idata<<8|gyroBuffer[(BufferNum+10)%GYRO_BUF_SIZE];
//    //  GyroReceivePITCH.idata=GyroReceivePITCH.idata<<8|gyroBuffer[(BufferNum+9)%GYRO_BUF_SIZE];
//    //  Gyro_White.PITCH = GyroReceivePITCH.fdata;
//    //
//    //  GyroReceiveGY.idata=gyroBuffer[(BufferNum+16)%GYRO_BUF_SIZE];
//    //  GyroReceiveGY.idata=GyroReceiveGY.idata<<8|gyroBuffer[(BufferNum+15)%GYRO_BUF_SIZE];
//    //  GyroReceiveGY.idata=GyroReceiveGY.idata<<8|gyroBuffer[(BufferNum+14)%GYRO_BUF_SIZE];
//    //  GyroReceiveGY.idata=GyroReceiveGY.idata<<8|gyroBuffer[(BufferNum+13)%GYRO_BUF_SIZE];
//    //  Gyro_White.GY = GyroReceiveGY.fdata;

//    //  if((Gyro_White.YAW_ABS>180)||(Gyro_White.YAW_ABS<-180)||(Gyro_White.PITCH>90)||(Gyro_White.PITCH<-90))
//    //  {
//    //    Gyro_White.PITCH = Lastpitchpos;
//    //    Gyro_White.YAW_ABS=Lastyawpos;
//    //  }
//    //  else
//    //  {
//    //    Lastpitchpos=Gyro_White.PITCH;
//    //    Lastyawpos=Gyro_White.YAW_ABS;
//    //  }
//    //  Gyro_White.MPUrecvd = 1;
//    //  Gyro_White.MPUDisconnectCnt = 0;
//}

/////**
////  * @brief  ����ƽ���˲�(����Ǹ�����PC��Ϣʱ�˲��������õ�)������ƽ���˲����ʺϴ�������壩
////  * @param  �洢���˲�ֵ�����飬����ӵ�ֵ�����������˲�����������ɵ�ֵ��
////  * @retval None
////  * �����˲����������ഺ�c(*��>��<)o�b
////  */

////static float Filter(float* buf, float new_one)
//////Recursion average filtering method
//////Ϊʲô�����������DataReceive���ļ�����???
//////����ԭ������Ϊֻ������������ݲŻ��õ��˲���ԭ��
////{
////    int32_t i;
////    float filter_sum = 0;//�˲���
////    for (i = 0; i < 19; i++)//��ǰ��Ԫ��λ�ã�Ҫ��Ҫ���ǻ�����������������������ʽ�����ټ�������
////        buf[i] = buf[i + 1];
////    buf[19] = new_one;          //�����µ�Ԫ��
////    return (float)(filter_sum / 20);    //�����˲��Ľ��
////}
