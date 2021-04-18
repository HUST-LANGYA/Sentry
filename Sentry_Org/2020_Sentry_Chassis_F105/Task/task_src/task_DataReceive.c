#include "main.h"

extern CanRxMsg Can1_rx_message_0, Can1_rx_message_1, Can2_rx_message_0, Can2_rx_message_1;

/**
  * @brief  接收底盘电机的返回报文
  * @param  None
  * @retval None
  */
void CAN1_DataReceive_0(void)
{
	if (Can1_rx_message_0.StdId == MOTOR_CHASSIS_ID) //底盘电机的ID号
	{
        extern _3508_motor_t motor_chassis;
		block_disconnect.Chassis_Last_Cnt = GetSysCnt();
		motor_chassis.angle_abs = Can1_rx_message_0.Data[0] << 8 | Can1_rx_message_0.Data[1];
		motor_chassis.real_speed = Can1_rx_message_0.Data[2] << 8 | Can1_rx_message_0.Data[3];
	}    
}

/**
  * @brief  接收上云台拨弹电机的反馈报文
  * @param  None
  * @retval None
  */
void CAN1_DataReceive_1(void)
{
	if (Can1_rx_message_1.StdId == BodanMotor_Up_ID)  //接收上拨弹电机的反馈信息
    {
        extern _2006_motor_t BodanMotor;
//        block_disconnect.Bodan_Last_Cnt = GetSysCnt();
        BodanMotor.Angle_ABS = Can1_rx_message_1.Data[0] << 8 | Can1_rx_message_0.Data[1];
        BodanMotor.RealSpeed = Can1_rx_message_1.Data[2] << 8 | Can1_rx_message_0.Data[3];
    }
}

/**
  * @brief  接收线上的遥控器数据报文
  * @param  None
  * @retval None
  */
REMOTE_t remote;
void CAN2_DataReceive_0(void)
{
	if (Can2_rx_message_0.StdId == Remote_Control_ID) //接收从上云台传来的遥控器数据帧
	{
		// 这里面解码要重新写了，把四通道两拨杆的数据全部读进来，然后再在status_Act里面解码
        REMOTE_t tempRemote;
        tempRemote.s1 = ((int8_t) Can2_rx_message_0.Data[5]>>2 ) & 0x03;
        tempRemote.s2 = ((int8_t) Can2_rx_message_0.Data[5]) & 0x03;
        if(tempRemote.s2 == 0 || tempRemote.s1 == 0) 
        {    
            return;//如果传过来空的包就直接丢掉
        }
        remote.s1 = ((int8_t) Can2_rx_message_0.Data[5]>>2 ) & 0x03;
        remote.s2 = ((int8_t) Can2_rx_message_0.Data[5]) & 0x03;
        remote.ch[0]=((int16_t)Can2_rx_message_0.Data[0]<<3 | (int16_t)Can2_rx_message_0.Data[1]>>5) & 0x7ff;
        remote.ch[1]=((int16_t)Can2_rx_message_0.Data[1]<<6 | (int16_t)Can2_rx_message_0.Data[2]>>2 ) & 0x7ff;
        remote.ch[2]=((int16_t)Can2_rx_message_0.Data[2]<<9 | (int16_t)Can2_rx_message_0.Data[3]<<1 | (int16_t)Can2_rx_message_0.Data[4]>>7 ) & 0x7ff;
        remote.ch[3]=((int16_t)Can2_rx_message_0.Data[4]<<4 | (int16_t)Can2_rx_message_0.Data[5]>>4 ) & 0x7ff;         
	}
}

/**
  * @brief  目前不晓得是接啥的
  * @param  None
  * @retval None
  */
void CAN2_DataReceive_1(void)
{
	;
}


/**
  * @brief  读取裁判系统数据(这一串是前辈们写的,我也看不明白)
  *         (但愿能用,不能用我也没办法,主要是半天找不到一个手册)
  *         (MD用不了,到时候问LZC)
  * @param  用来存储接收数据的数组
  * @retval None
  */
void JudgeBuffReceive(uint8_t ReceiveBuffer[])
{
//    uint8_t SaveBuffer[60];
//    uint16_t DataLen;
//    int32_t number;
//	uint16_t cmd_id;
//	int16_t PackPoint;
//	static int last_hp = 600;
//    
//	memcpy(&SaveBuffer[JudgeBufBiggestSize], &ReceiveBuffer[0], JudgeBufBiggestSize);
//	for (PackPoint = 0; PackPoint < JudgeBufBiggestSize; PackPoint++)
//	{
//		if (SaveBuffer[PackPoint] == 0xA5) //解裁判系统的报头
//		{
//			if ((Verify_CRC8_Check_Sum(&SaveBuffer[PackPoint], 5) == 1)) //  ???? CRC??  5?CRC
//			{
//				block_disconnect.Judge_Last_Cnt = GetSysCnt();
//				cmd_id = (SaveBuffer[PackPoint + 6]) & 0xff;
//				cmd_id = (cmd_id << 8) | SaveBuffer[PackPoint + 5]; //??   6 7?ID?
//				DataLen = SaveBuffer[PackPoint + 2] & 0xff;
//				DataLen = (DataLen << 8) | SaveBuffer[PackPoint + 1];									 //??  2 3???
//				if ((cmd_id == 0x0201) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9))) //???????9
//				{
//					JudgeReceive.robot_id = SaveBuffer[PackPoint + 7];
//					JudgeReceive.RobotLevel = SaveBuffer[PackPoint + 8];								   //   9???????
//					JudgeReceive.remainHP = (SaveBuffer[PackPoint + 10] << 8) | SaveBuffer[PackPoint + 9]; // 10,11????????
//					if (last_hp - JudgeReceive.remainHP >= 100)
//					{
//						if ((Sentry.HurtDir & 0x0c) >> 2 != 0)
//						{
//							Sentry.hero_alarm = 1;
//							u8 temp = (Sentry.HurtDir & 0x0c) << 2;
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

//				if ((cmd_id == 0x0207) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
//				{
//					if (SaveBuffer[PackPoint + 7] == 1) //?8?? ????  ?????
//					{
//						JudgeReceive.bulletFreq = SaveBuffer[PackPoint + 8]; //?9???????
//						Chassis_a.idata = SaveBuffer[PackPoint + 12];		 //10 11 12 13 ??????
//						Chassis_a.idata = (Chassis_a.idata << 8) | SaveBuffer[PackPoint + 11];
//						Chassis_a.idata = (Chassis_a.idata << 8) | SaveBuffer[PackPoint + 10];
//						Chassis_a.idata = (Chassis_a.idata << 8) | SaveBuffer[PackPoint + 9];
//						JudgeReceive.bulletSpeed17 = Chassis_a.fdata;
//						HeatCal += JudgeReceive.bulletSpeed17;
//						number++;
//						block_disconnect.Bullect_Last_Cnt = block_disconnect.BullectCnt;
//					}
//					if (SaveBuffer[PackPoint + 7] == 2) //?8?? ????  ?????
//					{
//						JudgeReceive.bulletFreq = SaveBuffer[PackPoint + 8];
//						Chassis_a.idata = SaveBuffer[PackPoint + 12];
//						Chassis_a.idata = (Chassis_a.idata << 8) | SaveBuffer[PackPoint + 11];
//						Chassis_a.idata = (Chassis_a.idata << 8) | SaveBuffer[PackPoint + 10];
//						Chassis_a.idata = (Chassis_a.idata << 8) | SaveBuffer[PackPoint + 9];
//						JudgeReceive.bulletSpeed42 = Chassis_a.fdata;
//					}
//				}
//				if ((cmd_id == 0x0202) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
//				{
//					Chassis_energy.idata = SaveBuffer[PackPoint + 14];
//					Chassis_energy.idata = (Chassis_energy.idata << 8) | SaveBuffer[PackPoint + 13];
//					Chassis_energy.idata = (Chassis_energy.idata << 8) | SaveBuffer[PackPoint + 12];
//					Chassis_energy.idata = (Chassis_energy.idata << 8) | SaveBuffer[PackPoint + 11];
//					JudgeReceive.realChassispower = Chassis_energy.fdata; //????
//					Chassis_energy.idata = SaveBuffer[PackPoint + 18];
//					Chassis_energy.idata = (Chassis_energy.idata << 8) | SaveBuffer[PackPoint + 17];
//					Chassis_energy.idata = (Chassis_energy.idata << 8) | SaveBuffer[PackPoint + 16];
//					Chassis_energy.idata = (Chassis_energy.idata << 8) | SaveBuffer[PackPoint + 15];
//					JudgeReceive.remainEnergy = Chassis_energy.fdata; //????
//					memcpy(&JudgeReceive.remainEnergy, &SaveBuffer[PackPoint + 7 + 8], 2);
//					memcpy(&JudgeReceive.shooterHeat17, &SaveBuffer[PackPoint + 7 + 10], 2);
//				}
//				if ((cmd_id == 0x0206) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
//				{
//					JudgeReceive.armor_id = SaveBuffer[PackPoint + 7 + 0] & 0x0f;
//					JudgeReceive.hurt_type = (SaveBuffer[PackPoint + 7 + 0] >> 4) & 0x0f;
//					if (JudgeReceive.hurt_type == 0)
//					{
//						if (JudgeReceive.armor_id == 0)
//							Sentry.HurtDir |= 0x05; //第一位置1
//						else if (JudgeReceive.armor_id == 1)
//							Sentry.HurtDir |= 0x0a;
//					}
//				}
//				if ((cmd_id == 0x0301) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9)))
//				{
//					int16_t msg = (SaveBuffer[PackPoint + 7 + 1] << 8) | SaveBuffer[PackPoint + 7];
//					int16_t send_id = (SaveBuffer[PackPoint + 7 + 3] << 8) | SaveBuffer[PackPoint + 7 + 2];
//					int16_t receive_id = (SaveBuffer[PackPoint + 7 + 5] << 8) | SaveBuffer[PackPoint + 7 + 4];
//					if (((JudgeReceive.robot_id == 7 && send_id == 6 && receive_id == 7) || (JudgeReceive.robot_id == 17 && send_id == 16 && receive_id == 17)) && msg == 0x0200)
//					{
//						block_disconnect.Drone_Last_Cnt = GetSysCnt();
//						u8 temp = SaveBuffer[PackPoint + 7 + 6];
//						//						if(temp == 0)										//由此判断单次受100血伤害
//						//						{
//						//							Sentry.drone_alarm = 0;
//						//							Sentry.dron_OverTime = 0;
//						//						}
//						//						else if(temp == 1 && Sentry.dron_OverTime == 0)
//						//							Sentry.drone_alarm = 1;
//						//						if(temp != 0 && temp != 1)
//						//							Sentry.drone_alarm = 1;
//						if (temp == 1 || temp == 0)
//							Sentry.drone_alarm = temp;
//						else
//							Sentry.drone_alarm = 1;
//					}
//				}
//				if ((cmd_id == 0x0003) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9))) //???????9
//				{
//					JudgeReceive.OtherRobot.red_1_robot_HP = (SaveBuffer[PackPoint + 8] << 8) | SaveBuffer[PackPoint + 7];
//					JudgeReceive.OtherRobot.red_2_robot_HP = (SaveBuffer[PackPoint + 10] << 8) | SaveBuffer[PackPoint + 9];
//					JudgeReceive.OtherRobot.red_3_robot_HP = (SaveBuffer[PackPoint + 12] << 8) | SaveBuffer[PackPoint + 11];
//					JudgeReceive.OtherRobot.red_4_robot_HP = (SaveBuffer[PackPoint + 14] << 8) | SaveBuffer[PackPoint + 13];
//					JudgeReceive.OtherRobot.red_5_robot_HP = (SaveBuffer[PackPoint + 16] << 8) | SaveBuffer[PackPoint + 15];
//					JudgeReceive.OtherRobot.red_7_robot_HP = (SaveBuffer[PackPoint + 18] << 8) | SaveBuffer[PackPoint + 17];
//					JudgeReceive.OtherRobot.red_base_HP = (SaveBuffer[PackPoint + 20] << 8) | SaveBuffer[PackPoint + 19];
//					JudgeReceive.OtherRobot.blue_1_robot_HP = (SaveBuffer[PackPoint + 22] << 8) | SaveBuffer[PackPoint + 21];
//					JudgeReceive.OtherRobot.blue_2_robot_HP = (SaveBuffer[PackPoint + 24] << 8) | SaveBuffer[PackPoint + 23];
//					JudgeReceive.OtherRobot.blue_3_robot_HP = (SaveBuffer[PackPoint + 26] << 8) | SaveBuffer[PackPoint + 25];
//					JudgeReceive.OtherRobot.blue_4_robot_HP = (SaveBuffer[PackPoint + 28] << 8) | SaveBuffer[PackPoint + 27];
//					JudgeReceive.OtherRobot.blue_5_robot_HP = (SaveBuffer[PackPoint + 30] << 8) | SaveBuffer[PackPoint + 29];
//					JudgeReceive.OtherRobot.blue_7_robot_HP = (SaveBuffer[PackPoint + 32] << 8) | SaveBuffer[PackPoint + 31];
//					JudgeReceive.OtherRobot.blue_base_HP = (SaveBuffer[PackPoint + 34] << 8) | SaveBuffer[PackPoint + 33];
//				}
//				if ((cmd_id == 0x0208) && (Verify_CRC16_Check_Sum(&SaveBuffer[PackPoint], DataLen + 9))) //???????9
//				{
//					JudgeReceive.bullet_remaining_num = (SaveBuffer[PackPoint + 8] << 8) | SaveBuffer[PackPoint + 7];
//				}
//			}
//		}
//	}

//	if (JudgeReceive.LastShooterHeat17 != JudgeReceive.shooterHeat17)
//	{
//		JudgeReceive.HeatUpdate_NoUpdate = 0x01; //??????0x01
//	}
//	JudgeReceive.LastShooterHeat17 = JudgeReceive.shooterHeat17;
//	if (JudgeReceive.LastbulletSpeed17 != JudgeReceive.bulletSpeed17)
//	{
//		JudgeReceive.SpeedUpdate_NoUpdate = 0x01;
//	}
//	JudgeReceive.LastbulletSpeed17 = JudgeReceive.bulletSpeed17;

//	memcpy(&SaveBuffer[0], &SaveBuffer[JudgeBufBiggestSize], JudgeBufBiggestSize);
}

/**
  * @brief  返回收到的功率PowerNow
  * @param  None
  * @retval None
  */
float GetPower()
{
	return PowerNow;
}
