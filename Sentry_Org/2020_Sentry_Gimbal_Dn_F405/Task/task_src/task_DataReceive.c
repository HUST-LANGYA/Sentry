#include "main.h"

float Lastyawpos, Lastpitchpos; //旧版陀螺仪
//short PC_DownFlag;
//int time, tim_now;

extern gimbal_motor_t MotoPitch;
extern gimbal_motor_t MotoYaw;
extern _2006_moto_t BodanMotor;

extern CanRxMsg Can1_rx_message_0, Can1_rx_message_1, Can2_rx_message_0, Can2_rx_message_1;
gyro_Typedef Gyro_White;




int32_t test_receive;

/**
  * @brief  接拨弹电机电机的反馈报文
  * @param  None
  * @retval None
  */
void CAN1_DataReceive_0(void)
{
	if(Can1_rx_message_0.StdId == BodanMotor_ID)//拨弹电机反馈
	{
		block_disconnect.Bodan_Last_Cnt = GetSysCnt();
		BodanMotor.Angle_ABS=Can1_rx_message_0.Data[0]<<8 | Can1_rx_message_0.Data[1];
		BodanMotor.RealSpeed=Can1_rx_message_0.Data[2]<<8 | Can1_rx_message_0.Data[3];
	}
}

/**
  * @brief  接上云台发送的遥控数据报文
  * @param  None
  * @retval None
  */
REMOTE_t remote;
//int8_t remote_s1,remote_s2;
//int16_t remote_ch[4]
//注：发送来的消息是把四通道两拨杆压缩在6个字节里面一起发来的
void CAN1_DataReceive_1(void)
{
    if(Can1_rx_message_1.StdId == Remote_Control_ID) //接收从上云台来的遥控数据帧
    {
        // 这里面解码要重新写了，把四通道两拨杆的数据全部读进来，然后再在status_Act里面解码
        REMOTE_t tempRemote;
        tempRemote.s1 = ((int8_t) Can1_rx_message_1.Data[5]>>2 ) & 0x03;
        tempRemote.s2 = ((int8_t) Can1_rx_message_1.Data[5]) & 0x03;
        if(tempRemote.s2 == 0 || tempRemote.s1 == 0) 
        {    
            return;//如果传过来空的包就直接丢掉
        }
        remote.ch[0]=((int16_t)Can1_rx_message_1.Data[0]<<3 | (int16_t)Can1_rx_message_1.Data[1]>>5) & 0x7ff;
        remote.ch[1]=((int16_t)Can1_rx_message_1.Data[1]<<6 | (int16_t)Can1_rx_message_1.Data[2]>>2 ) & 0x7ff;
        remote.ch[2]=((int16_t)Can1_rx_message_1.Data[2]<<9 | (int16_t)Can1_rx_message_1.Data[3]<<1 | (int16_t)Can1_rx_message_1.Data[4]>>7 ) & 0x7ff;
        remote.ch[3]=((int16_t)Can1_rx_message_1.Data[4]<<4 | (int16_t)Can1_rx_message_1.Data[5]>>4 ) & 0x7ff;
        remote.s1 = ((int8_t) Can1_rx_message_1.Data[5]>>2 ) & 0x03;
        remote.s2 = ((int8_t) Can1_rx_message_1.Data[5]) & 0x03;
    }

    if(Can1_rx_message_1.StdId == SHOOTING_HEAT_ID)//接收枪口热量报文
    {
//        int a=1;
//        Can1_rx_message_1;
        ;/*等把裁判系统整明白再说这个，总之这个入口要留在这里*/
    }
    
}


/**
  * @brief  接云台两轴电机的反馈报文
  * @param  None
  * @retval None
  */
int32_t rx_test = 0;
void CAN2_DataReceive_0(void)
{
	//block_disconnect_t* block_disconnect =  Get_block_t();
	if (Can2_rx_message_0.StdId == MotoPitch_ID)
	{
		block_disconnect.Pitch_Last_Cnt = GetSysCnt();
		MotoPitch.Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
		MotoPitch.real_flow = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
		MotoPitch.set_flow = Can2_rx_message_0.Data[4] << 8 | Can2_rx_message_0.Data[5];
		rx_test++;
		/*******************************************************/
	}
	else if (Can2_rx_message_0.StdId == MotoYaw_ID)
	{
		block_disconnect.Yaw_Last_Cnt = GetSysCnt();
		MotoYaw.Angle_ABS = Can2_rx_message_0.Data[0] << 8 | Can2_rx_message_0.Data[1];
		MotoYaw.speed = Can2_rx_message_0.Data[2] << 8 | Can2_rx_message_0.Data[3];
		MotoYaw.real_flow = Can2_rx_message_0.Data[4] << 8 | Can2_rx_message_0.Data[5];
	}
}

/**
  * @brief  接收两轴陀螺仪报文
  * @param  None
  * @retval None
  */
int32_t gyro_cnt;
float gyro_pitch_test;
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
	}
}

int16_t TempPC;
int32_t tempfliter;
int16_t Bspeed;
extern float PC_PitchSet, PC_YawSet;
float test_Pitch = 0, test_Yaw = 0;
uint32_t tick1,tick2;
/**
  * @brief  PC信息接收
  * @param  信息数组
  * @retval None
  */
void PCReceive(uint8_t Buf[])
{
    int16_t DataTemp;
    //	Robot* sentry = GetSentry();
    if (Buf[0] == '!' && Buf[7] == '#')
    {
        if (Verify_CRC8_Check_Sum(&Buf[1], 6))
        {
            block_disconnect.PC_Last_Cnt = GetSysCnt();
            if (Sentry_Status.GimbalMode & Gimbal_Dn_PC)
            {
                if (Buf[1] == 0x05)
                {
                    //					gimbal_motor_t* MotoPitch = GetMotoPitch();
                    //					gimbal_motor_t* MotoYaw = GetMotoYaw();
                    DataTemp = Buf[2] << 8 | Buf[3];
                    PC_PitchSet = DataTemp / 100.0f;
                    DataTemp = Buf[4] << 8 | Buf[5];
                    PC_YawSet = DataTemp / 100.0f; //获得pc的角度设定值 -180 到 180
                                                    //					/*********************************/
                                                    //					while(PC_YawSet - Gyro_White.YAW_INC>180)
                                                    //					{
                                                    //						PC_YawSet -= 360;
                                                    //					}
                                                    //					while(PC_YawSet - Gyro_White.YAW_INC<-180)
                                                    //					{
                                                    //						PC_YawSet += 360;
                                                    //					}
                                                    ////					if(Sentry.hero_alarm == 0)					//原本写了 受英雄打击会把云台抢过来

                    //					MotoYaw->PidPos.SetPoint = PC_YawSet;
                    //					MotoPitch->PidPos.SetPoint = -PC_PitchSet/360*8191+PITCH_ZERO_POS;
                    //				}
                    //				else if(Buf[1] == 0x01)
                    //				{
                    //					TempPC = Buf[2]<<8|Buf[3];
                    ////					if(Sentry.State == SENTRY_SCANNING)			//之前底盘由PC控制
                    ////						Sentry.aim_speed = Buf[2]<<8|Buf[3];
                    //				}
                    //				else if(Buf[1] == 0x02)
                    //				{
                    //					int8_t temp;
                    //					temp = Buf[4]<<8|Buf[5];
                    //					if(temp == 26)
                    //					{
                    //						Sentry.FrictionSpeedSet = 12000;
                    //						Sentry.BulletSpeedSet = 26;
                    //					}
                    //					else
                    //					{
                    // 						Sentry.BulletSpeedSet = 16;
                    //						Sentry.FrictionSpeedSet = 5700;
                    //					}
                    //					(*sentry).is_shoot = Buf[2]<<8|Buf[3];
                }
            }
        }
    }
    
}
///**
//  * @brief  遥控器
//  * @param  None
//  * @retval None
//  */
//void RCDataToSpeed(void) //将遥控器接收的数据转化为未经pid处理的速度设定值
//{

//	RC_Ctl_t RC_Ctl = getRCData();
//    switch (remote.s1)
//    {
//    case 1:
//        switch (remote.s2)
//        {
//        case 1:
//            Sentry_Status.GimbalMode = Gimbal_Up_PC | Gimbal_Down_PC;
//            Sentry_Status.ChassisMode = Chassis_PC;
//            Sentry_Status.ShootMode = Shoot_Up_PC | Shoot_Down_PC;
//            break;
//        case 2:
//            Sentry_Status.GimbalMode = Gimbal_Up_PC | Gimbal_Down_PC;
//            Sentry_Status.ChassisMode = Chassis_RC;
//            Sentry_Status.ShootMode = Down;
//            break;
//        case 3:
//            Sentry_Status.GimbalMode = Gimbal_Up_RC | Gimbal_Down_RC;
//            Sentry_Status.ChassisMode = Chassis_PC;
//            Sentry_Status.ShootMode = Down;
//            break;
//        default:
//            break;
//        }
//    case 2:
//        switch (remote.s2)
//        {
//        case 1:
//            Sentry_Status.GimbalMode = Gimbal_Up_RC;
//            Sentry_Status.ChassisMode = Chassis_RC;
//            if (remote.ch1 >= 1500) //当右侧拨杆推到最上面时，为遥控器模式下开火状态
//                Sentry_Status.ShootMode = Shoot_Up_RC;
//            else
//                Sentry_Status.ShootMode = Down;
//            break;
//        case 2:
//            Sentry_Status.GimbalMode = Down;
//            Sentry_Status.ChassisMode = Down;
//            Sentry_Status.ShootMode = Down;
//            if (remote.ch0 < 380 && remote.ch1 < 380 && remote.ch2 > 1660 && remote.ch3 < 380 && PC_DownFlag == 0)
//            {
//                PC_DownFlag = 1;
//                time = GetSysCnt();
//            }
//            else if (remote.ch0 < 380 && remote.ch1 < 380 && remote.ch2 > 1660 && remote.ch3 < 380 && PC_DownFlag == 1)
//            {
//                tim_now = GetSysCnt();
//                if (tim_now - time >= 1000)
//                    PC_DownFlag = 2;
//            }
//            else
//                PC_DownFlag = 0;
//            break;
//        case 3:
//            Sentry_Status.GimbalMode = Gimbal_Down_RC;
//            Sentry_Status.ChassisMode = Chassis_RC;
//            if (remote.ch1 >= 1500)
//                Sentry_Status.ShootMode = Shoot_Down_RC;
//            else
//                Sentry_Status.ShootMode = Down;
//            break;
//        default:
//            break;
//        }
//        break;
//    case 3:
//        switch (remote.s2)
//        {
//        case 1:
//            Sentry_Status.GimbalMode = Gimbal_Up_RC;
//            Sentry_Status.ChassisMode = Chassis_RC;
//            Sentry_Status.ShootMode = Shoot_Up_PC;
//            break;
//        case 2:
//            Sentry_Status.GimbalMode = Gimbal_Up_RC | Gimbal_Down_RC;
//            Sentry_Status.ChassisMode = Down;
//            Sentry_Status.ShootMode = Shoot_Up_PC | Shoot_Down_PC;
//            break;
//        case 3:
//            Sentry_Status.GimbalMode = Gimbal_Down_RC;
//            Sentry_Status.ChassisMode = Chassis_RC;
//            Sentry_Status.ShootMode = Shoot_Down_PC;
//            break;
//        default:
//            break;
//        }
//        break;
//    default:
//        break;
//    }
//}

uint8_t SaveBuffer[60];
uint16_t DataLen;
uint16_t cmd_id;
int32_t number;
/**
  * @brief  读取裁判系统数据
  * @param  None
  * @retval None
  */
void JudgeBuffReceive(uint8_t ReceiveBuffer[])
{
	//	uint16_t cmd_id;
	//	int16_t PackPoint;
	//	static int32_t last_hp = 600;
	//	memcpy(&SaveBuffer[JudgeBufBiggestSize],&ReceiveBuffer[0],JudgeBufBiggestSize);
	//	for(PackPoint=0;PackPoint<JudgeBufBiggestSize;PackPoint++)
	//	{
	//		if(SaveBuffer[PackPoint]==0xA5)  //?? ?????
	//		{
	//			if((Verify_CRC8_Check_Sum(&SaveBuffer[PackPoint],5)==1))//  ???? CRC??  5?CRC
	//			{
	//				block_disconnect.Judge_Last_Cnt = GetSysCnt();
	//				cmd_id=(SaveBuffer[PackPoint+6])&0xff;
	//				cmd_id=(cmd_id<<8)|SaveBuffer[PackPoint+5];  //??   6 7?ID?
	//				DataLen=SaveBuffer[PackPoint+2]&0xff;
	//				DataLen=(DataLen<<8)|SaveBuffer[PackPoint+1];//??  2 3???
	//				if((cmd_id==0x0201)&&(Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint],DataLen+9)))  //???????9
	//				{
	//					JudgeReceive.robot_id = SaveBuffer[PackPoint+7];
	//					JudgeReceive.RobotLevel=SaveBuffer[PackPoint+8];//   9???????
	//					JudgeReceive.remainHP=(SaveBuffer[PackPoint+10]<<8)|SaveBuffer[PackPoint+9]; // 10,11????????
	//					if(last_hp - JudgeReceive.remainHP>=100)
	//					{
	//						if((Sentry.HurtDir&0x0c)>>2 != 0)
	//						{
	//							Sentry.hero_alarm = 1;
	//							u8 temp = (Sentry.HurtDir&0x0c)<<2;
	//							Sentry.HurtDir |= temp;
	//							last_hp = JudgeReceive.remainHP;
	//						}
	//					}
	//					else
	//					{
	//						Sentry.HurtDir &= 0x33;
	//						last_hp = JudgeReceive.remainHP;
	//					}
	//				}

	//				if((cmd_id==0x0207)&&(Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint],DataLen+9)))
	//				{
	//					if(SaveBuffer[PackPoint+7]==1)   //?8?? ????  ?????
	//					{
	//						JudgeReceive.bulletFreq=SaveBuffer[PackPoint+8];  //?9???????
	//						Chassis_a.idata=SaveBuffer[PackPoint+12];  //10 11 12 13 ??????
	//						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[PackPoint+11];
	//						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[PackPoint+10];
	//						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[PackPoint+9];
	//						JudgeReceive.bulletSpeed17=Chassis_a.fdata;
	//						HeatCal += JudgeReceive.bulletSpeed17;
	//						number++;
	//						block_disconnect.Bullect_Last_Cnt = block_disconnect.BullectCnt;
	//					}
	//					if(SaveBuffer[PackPoint+7]==2)  //?8?? ????  ?????
	//					{
	//						JudgeReceive.bulletFreq=SaveBuffer[PackPoint+8];
	//						Chassis_a.idata=SaveBuffer[PackPoint+12];
	//						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[PackPoint+11];
	//						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[PackPoint+10];
	//						Chassis_a.idata=(Chassis_a.idata<<8)|SaveBuffer[PackPoint+9];
	//						JudgeReceive.bulletSpeed42=Chassis_a.fdata;
	//					}
	//				}
	//				if((cmd_id==0x0202)&&(Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint],DataLen+9)))
	//				{
	//					Chassis_energy.idata=SaveBuffer[PackPoint+14];
	//					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[PackPoint+13];
	//					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[PackPoint+12];
	//					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[PackPoint+11];
	//					JudgeReceive.realChassispower=Chassis_energy.fdata;//????
	//					Chassis_energy.idata=SaveBuffer[PackPoint+18];
	//					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[PackPoint+17];
	//					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[PackPoint+16];
	//					Chassis_energy.idata=(Chassis_energy.idata<<8)|SaveBuffer[PackPoint+15];
	//					JudgeReceive.remainEnergy=Chassis_energy.fdata;//????
	//					memcpy(&JudgeReceive.remainEnergy,&SaveBuffer[PackPoint+7+8],2);
	//					memcpy(&JudgeReceive.shooterHeat17,&SaveBuffer[PackPoint+7+10],2);
	//				}
	//				if((cmd_id==0x0206)&&(Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint],DataLen+9)))
	//				{
	//					JudgeReceive.armor_id = SaveBuffer[PackPoint+7+0]&0x0f;
	//					JudgeReceive.hurt_type = (SaveBuffer[PackPoint+7+0]>>4)&0x0f;
	//					if(JudgeReceive.hurt_type == 0)
	//					{
	//						if(JudgeReceive.armor_id == 0)
	//							Sentry.HurtDir |= 0x05;			//第一位置1
	//						else if(JudgeReceive.armor_id == 1)
	//							Sentry.HurtDir |= 0x0a;
	//					}
	//				}
	//				if((cmd_id==0x0301)&&(Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint],DataLen+9)))
	//				{
	//					int16_t msg = (SaveBuffer[PackPoint+7+1]<<8)|SaveBuffer[PackPoint+7];
	//					int16_t send_id = (SaveBuffer[PackPoint+7+3]<<8)|SaveBuffer[PackPoint+7+2];
	//					int16_t receive_id = (SaveBuffer[PackPoint+7+5]<<8)|SaveBuffer[PackPoint+7+4];
	//					if(((JudgeReceive.robot_id == 7 && send_id == 6 && receive_id == 7)||(JudgeReceive.robot_id == 17 && send_id == 16 && receive_id == 17))&&msg == 0x0200)
	//					{
	//						block_disconnect.Drone_Last_Cnt = GetSysCnt();
	//						u8 temp = SaveBuffer[PackPoint+7+6];
	////						if(temp == 0)										//由此判断单次受100血伤害
	////						{
	////							Sentry.drone_alarm = 0;
	////							Sentry.dron_OverTime = 0;
	////						}
	////						else if(temp == 1 && Sentry.dron_OverTime == 0)
	////							Sentry.drone_alarm = 1;
	////						if(temp != 0 && temp != 1)
	////							Sentry.drone_alarm = 1;
	//						if(temp == 1||temp == 0)
	//							Sentry.drone_alarm = temp;
	//						else
	//							Sentry.drone_alarm = 1;
	//					}
	//				}
	//				if((cmd_id==0x0003)&&(Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint],DataLen+9)))  //???????9
	//				{
	//					JudgeReceive.OtherRobot.red_1_robot_HP = (SaveBuffer[PackPoint+8]<<8)|SaveBuffer[PackPoint+7];
	//					JudgeReceive.OtherRobot.red_2_robot_HP = (SaveBuffer[PackPoint+10]<<8)|SaveBuffer[PackPoint+9];
	//					JudgeReceive.OtherRobot.red_3_robot_HP = (SaveBuffer[PackPoint+12]<<8)|SaveBuffer[PackPoint+11];
	//					JudgeReceive.OtherRobot.red_4_robot_HP = (SaveBuffer[PackPoint+14]<<8)|SaveBuffer[PackPoint+13];
	//					JudgeReceive.OtherRobot.red_5_robot_HP = (SaveBuffer[PackPoint+16]<<8)|SaveBuffer[PackPoint+15];
	//					JudgeReceive.OtherRobot.red_7_robot_HP = (SaveBuffer[PackPoint+18]<<8)|SaveBuffer[PackPoint+17];
	//					JudgeReceive.OtherRobot.red_base_HP = (SaveBuffer[PackPoint+20]<<8)|SaveBuffer[PackPoint+19];
	//					JudgeReceive.OtherRobot.blue_1_robot_HP = (SaveBuffer[PackPoint+22]<<8)|SaveBuffer[PackPoint+21];
	//					JudgeReceive.OtherRobot.blue_2_robot_HP = (SaveBuffer[PackPoint+24]<<8)|SaveBuffer[PackPoint+23];
	//					JudgeReceive.OtherRobot.blue_3_robot_HP = (SaveBuffer[PackPoint+26]<<8)|SaveBuffer[PackPoint+25];
	//					JudgeReceive.OtherRobot.blue_4_robot_HP = (SaveBuffer[PackPoint+28]<<8)|SaveBuffer[PackPoint+27];
	//					JudgeReceive.OtherRobot.blue_5_robot_HP = (SaveBuffer[PackPoint+30]<<8)|SaveBuffer[PackPoint+29];
	//					JudgeReceive.OtherRobot.blue_7_robot_HP = (SaveBuffer[PackPoint+32]<<8)|SaveBuffer[PackPoint+31];
	//					JudgeReceive.OtherRobot.blue_base_HP = (SaveBuffer[PackPoint+34]<<8)|SaveBuffer[PackPoint+33];
	//				}
	//				if((cmd_id==0x0208)&&(Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint],DataLen+9)))  //???????9
	//				{
	//					JudgeReceive.bullet_remaining_num = (SaveBuffer[PackPoint+8]<<8)|SaveBuffer[PackPoint+7];
	//				}

	//			}
	//		}
	//	}

	//
	//				if(JudgeReceive.LastShooterHeat17 != JudgeReceive.shooterHeat17)
	//				{
	//						JudgeReceive.HeatUpdate_NoUpdate = 0x01;//??????0x01
	//				}
	//					 JudgeReceive.LastShooterHeat17 = JudgeReceive.shooterHeat17;
	//				if(JudgeReceive.LastbulletSpeed17 != JudgeReceive.bulletSpeed17)
	//				{
	//					 JudgeReceive.SpeedUpdate_NoUpdate = 0x01;
	//				}
	//				JudgeReceive.LastbulletSpeed17=JudgeReceive.bulletSpeed17;
	//

	//	memcpy(&SaveBuffer[0],&SaveBuffer[JudgeBufBiggestSize],JudgeBufBiggestSize);
}

///**
//  * @brief  接收陀螺仪（新版）
//  * @param  rx_buffer16位数据
//  * @retval None 	
//  */

//void GyroReceiveFunc(uint8_t gyroBuffer[], int16_t BufferNum)
//{
//	//	FloatlongType GyroReceiveYaw,GyroReceiveGZ,GyroReceivePITCH,GyroReceiveGY;
//	//  GyroReceiveYaw.idata=gyroBuffer[(BufferNum+4)%GYRO_BUF_SIZE];
//	//  GyroReceiveYaw.idata=GyroReceiveYaw.idata<<8|gyroBuffer[(BufferNum+3)%GYRO_BUF_SIZE];
//	//  GyroReceiveYaw.idata=GyroReceiveYaw.idata<<8|gyroBuffer[(BufferNum+2)%GYRO_BUF_SIZE];
//	//  GyroReceiveYaw.idata=GyroReceiveYaw.idata<<8|gyroBuffer[(BufferNum+1)%GYRO_BUF_SIZE];
//	//  Gyro_White.YAW_ABS = GyroReceiveYaw.fdata;
//	//
//	//  GyroReceiveGZ.idata=gyroBuffer[(BufferNum+8)%GYRO_BUF_SIZE];
//	//  GyroReceiveGZ.idata=GyroReceiveGZ.idata<<8|gyroBuffer[(BufferNum+7)%GYRO_BUF_SIZE];
//	//  GyroReceiveGZ.idata=GyroReceiveGZ.idata<<8|gyroBuffer[(BufferNum+6)%GYRO_BUF_SIZE];
//	//  GyroReceiveGZ.idata=GyroReceiveGZ.idata<<8|gyroBuffer[(BufferNum+5)%GYRO_BUF_SIZE];
//	//  Gyro_White.GZ = GyroReceiveGZ.fdata;

//	//  GyroReceivePITCH.idata=gyroBuffer[(BufferNum+12)%GYRO_BUF_SIZE];
//	//  GyroReceivePITCH.idata=GyroReceivePITCH.idata<<8|gyroBuffer[(BufferNum+11)%GYRO_BUF_SIZE];
//	//  GyroReceivePITCH.idata=GyroReceivePITCH.idata<<8|gyroBuffer[(BufferNum+10)%GYRO_BUF_SIZE];
//	//  GyroReceivePITCH.idata=GyroReceivePITCH.idata<<8|gyroBuffer[(BufferNum+9)%GYRO_BUF_SIZE];
//	//  Gyro_White.PITCH = GyroReceivePITCH.fdata;
//	//
//	//  GyroReceiveGY.idata=gyroBuffer[(BufferNum+16)%GYRO_BUF_SIZE];
//	//  GyroReceiveGY.idata=GyroReceiveGY.idata<<8|gyroBuffer[(BufferNum+15)%GYRO_BUF_SIZE];
//	//  GyroReceiveGY.idata=GyroReceiveGY.idata<<8|gyroBuffer[(BufferNum+14)%GYRO_BUF_SIZE];
//	//  GyroReceiveGY.idata=GyroReceiveGY.idata<<8|gyroBuffer[(BufferNum+13)%GYRO_BUF_SIZE];
//	//  Gyro_White.GY = GyroReceiveGY.fdata;

//	//  if((Gyro_White.YAW_ABS>180)||(Gyro_White.YAW_ABS<-180)||(Gyro_White.PITCH>90)||(Gyro_White.PITCH<-90))
//	//  {
//	//    Gyro_White.PITCH = Lastpitchpos;
//	//    Gyro_White.YAW_ABS=Lastyawpos;
//	//  }
//	//  else
//	//  {
//	//    Lastpitchpos=Gyro_White.PITCH;
//	//    Lastyawpos=Gyro_White.YAW_ABS;
//	//  }
//	//  Gyro_White.MPUrecvd = 1;
//	//  Gyro_White.MPUDisconnectCnt = 0;
//}

///**
//  * @brief  递推平均滤波
//  * @param  None
//  * @retval None
//  */
//float Filter(float *buf, float new_one)
//{
//	float filter_sum = 0;
//	for (int32_t i = 0; i < 19; i++)
//		buf[i] = buf[i + 1];
//	buf[19] = new_one;
//	for (int32_t i = 0; i < 20; i++)
//		filter_sum += buf[i];
//	return (float)(filter_sum / 20);
//}
