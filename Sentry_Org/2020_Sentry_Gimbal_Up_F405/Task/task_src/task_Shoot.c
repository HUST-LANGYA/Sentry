#include "main.h"

_2006_motor_t BodanMotor;
int16_t num, speed;
int16_t Is_Shoot;

extern int16_t Shoot_init_flag;
extern Status_t Sentry_Status;
extern block_disconnet_t block_disconnect;

//�޲���ϵͳģʽ�µļ�¼���
void Shoot_Fire_Cal(void)
{
//	float fsend;
	if (Shoot_init_flag == 0) //��ʵ��������õ����ٶȻ������������ʵûɶ���  //yysy��ȷʵ
	{
        //������������ȥ����
        //BodanMotor.I_Set = 0;
		//BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_Inc;
		Shoot_init_flag = 1;
        FrictionWheel_Set(FrictionSpeed_Low,FrictionSpeed_Low);
	}

	/**********************************����������ģʽ����********************************/
	//   if(Sentry_Status.ShootMode==Shoot_Up_RC)
	//	 {
	//		num=6;//���ϲ���ϵͳ�󣬸�Ϊ��������
	////		speed=FrictionSpeed_Low;

	//	 }
	//	 else if (Sentry_Status.ShootMode==Shoot_Up_PC)
	//	 {
	////		 	if(.is_shoot == 0 || Sentry.is_shoot >10)
	////				num=0;
	////			else if(block_disconnect.is_judge_down == 0 && block_disconnect.is_pc_down == 0)
	////			{
	////				block_disconnect.BullectCnt++;						//�ж��Ƿ�û�����ӵ�
	////        num=Sentry.is_shoot;	//				BodanMoto_PidCal(5,0);	//HeatLimit(Sentry.is_shoot)

	////			}
	////			else if(block_disconnect.is_judge_down == 1)
	////				num=6;
	////			else if(block_disconnect.is_pc_down == 1)
	////				num=0;
	//
	//		speed=0;
	//	 }
	//	 else
	//	 {
	//	 num=0;
	//	 speed=0;
	//	 }
	//      if(Sentry_Status.ShootMode == Shoot_Up_RC)
//	speed = FrictionSpeed_Low;
	//			else
	//				speed=0;
	/*************************��������������***********************************/
	//	  if(FrictionMoto[1].real_speed>4000)
	//		{
	//	 num = LIMIT_MAX_MIN(num,25,0);
	//		BodanMotor.pid_speed.SetPoint = -num*270*2;		// 36*60/8; //�Ӹ�����Ϊ�����ˣ���е������
	//		fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);
	//		BodanMotor.I_Set=(short)LIMIT_MAX_MIN(fsend,7500,-7500);
	//		}
	//		else
	
	/*******************************Ħ�����������*********************************/

//    if(block_disconnect.Bodan_block!=0)
//	{
//		float maxspeed = 25*196.3;
//		//BodanMotor.pid_pos.SetPoint=BodanMotor.Angle_Inc-1000;
//		//BodanMotor.pid_speed.SetPoint = LIMIT_MAX_MIN(PID_Calc(&BodanMotor.pid_pos,BodanMotor.Angle_Inc,0),maxspeed,-maxspeed);
//		//fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);
//		//BodanMotor.I_Set=(short)LIMIT_MAX_MIN(fsend,7500,-7500);
//	}

//	FrictionMoto[0].pid.SetPoint = -speed;
//	fsend = PID_Calc(&FrictionMoto[0].pid, FrictionMoto[0].real_speed, 0);
//	FrictionMoto[0].I_Set = (short)LIMIT_MAX_MIN(fsend, 7500, -7500);

//	FrictionMoto[1].pid.SetPoint = speed;
//	fsend = PID_Calc(&FrictionMoto[1].pid, FrictionMoto[1].real_speed, 0);
//	FrictionMoto[1].I_Set = (short)LIMIT_MAX_MIN(fsend, 7500, -7500);

//	if (ABS(FrictionMoto[0].real_speed) >= FrictionSpeed_Low * 0.9)
//		Is_Shoot = Shoot_On;
    
    
	//Bodan_Can1Send(BodanMotor.I_Set);//������������ȥ����
    
}

/**
  * @brief  �������pid��ʼ��
  * @param  None
  * @retval None
  */
void BodanMoto_Init(void)
{
    //        BodanMotor.pid_speed.P = 0;
    //        BodanMotor.pid_speed.I = 0.0f;
    //        BodanMotor.pid_speed.D =0.0f;
    //        BodanMotor.pid_speed.IMax = 500;

    //        BodanMotor.pid_pos.P = 0.0f;
    //        BodanMotor.pid_pos.I = 0.0f;
    //        BodanMotor.pid_pos.D = 0.0f;
    //        BodanMotor.pid_pos.IMax = 500.0f;
    //        BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;

    //        BodanMotor.zerocheck.Circle = 0;
    //        BodanMotor.zerocheck.CountCycle = 8191;
    //        BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;
    
    
//	BodanMotor.pid_speed.P = 4;
//	BodanMotor.pid_speed.I = 0.2f;
//	BodanMotor.pid_speed.D = 1.5f;
//	BodanMotor.pid_speed.IMax = 500;

//	BodanMotor.pid_pos.P = 0.8f;//0.22f;
//	BodanMotor.pid_pos.I = 0.05f;
//	BodanMotor.pid_pos.D = 0.0f;
//	BodanMotor.pid_pos.IMax = 500.0f;
//	BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;

//	BodanMotor.zerocheck.Circle = 0;
//	BodanMotor.zerocheck.CountCycle = 8191;
//	BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;
}
