#include "main.h"

//����һ���ܵ��۵����顣��ΪӲ���������Ƶ�ԭ��������������һ��CAN������������ң�ر��ĵĻ���
//��ô����̨�Ĳ�������͵��������̰��ϣ��õ��̰�������������Ϣ��������������
//����Ħ��������������̨���ϣ�������ǹ�ܵĿ����߼��ͻ��úܹ���
//������ϰ��ң�������ݣ������ݴ������CAN���ϣ�Ȼ����̰����ʱ�����������Ҫ�������ģʽ��
//��ô�Լ���һ���������ٶȻ�������һ��ʱ����Ħ����������������Ϸ���
//��ˣ����̰������̨�嶼Ҫдshoot_fire_cal , �������ݻ���һ����һ����Ħ���֣�һ����������


_2006_motor_t BodanMotor;
int16_t num;

int16_t Shoot_init_flag;

//�޲���ϵͳģʽ�µļ�¼���
void Shoot_Fire_Cal(void)
{
//	int32_t i;

	float fsend;
	if(Shoot_init_flag==0)//��ʵ��������õ����ٶȻ������������ʵûɶ���
	{
		BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_Inc;		
		//BodanMotor[1].pid_pos.SetPoint = BodanMotor[1].Angle_Inc;	//����ֻ��һ���������
		Shoot_init_flag=1;
    }
	
		

/**********************************����������ģʽ����********************************/		
     if(Sentry_Status.ShootMode & Shoot_Up_RC)
	 {		
		num=6;//���ϲ���ϵͳ�󣬸�Ϊ��������
//		speed=FrictionSpeed_Low;

	 }
	 else if (Sentry_Status.ShootMode & Shoot_Up_PC)
	 {
//		 	if(Sentry.is_shoot == 0 || Sentry.is_shoot >10)
//				num=0;
//			else if(block_disconnect.is_judge_down == 0 && block_disconnect.is_pc_down == 0)
//			{
//				block_disconnect.BullectCnt++;						//�ж��Ƿ�û�����ӵ�
//        num=Sentry.is_shoot;	//				BodanMoto_PidCal(5,0);	//HeatLimit(Sentry.is_shoot)

//			}
//			else if(block_disconnect.is_judge_down == 1)
//				num=6;
//			else if(block_disconnect.is_pc_down == 1)
//				num=0;
 
		num=0;
	 }
	 else
		 num=0;
//	 if ((Sentry_Status.ShootMode & Shoot_Down_RC)==Shoot_Down_RC)
//	 {
//        num[1]=6;
//	 }
//	 else if((Sentry_Status.ShootMode & Shoot_Down_PC)==Shoot_Down_PC)
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
//		num[1]=0;
//	 }
//	 else
//		num[1]=0;

/*************************��������������***********************************/	 
//     for( i=0;i < 2;i++)
//	 {
    float maxspeed = 25*196.3;
    if(Sentry_Status.ShootMode != Shoot_Up_SLEEP/*Sentry.is_shoot != Shoot_Up_SLEEP*/)
    {	
//        BodanMotor.pid_pos.SetPoint -= 8192;
//		BodanMotor.pid_speed.SetPoint = LIMIT_MAX_MIN(PID_Calc(&BodanMotor.pid_pos,BodanMotor.Angle_Inc),5000,0);
//		fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed);
//		BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,7500,-7500);

            
        num = LIMIT_MAX_MIN(num,25,0);
        //BodanMotor.pid_speed.SetPoint = 800;//-num*270*2;		// 36*60/8; //�Ӹ�����Ϊ�����ˣ���е������
        fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed);
        BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,7500,-7500);
    }
    else
    BodanMotor.I_Set=0;
//	}
/*******************************Ħ�����������*********************************/
	 
//	 
////    if(block_disconnect.Bodan_block!=0)
////		{ 	
//	if(Sentry.is_shoot == Shoot_On)
//	{
//		float maxspeed = 25*196.3;
//		
//	}
//	else 
//		BodanMotor[0].I_Set=0;
////		}
		

	 
// 	 Bodan_Can1Send(BodanMotor[0].I_Set,BodanMotor[1].I_Set);

}

/**
  * @brief  �������pid��ʼ��
  * @param  None
  * @retval None
  */

void BodanMotor_Init(void)
{
//	 int32_t i;

//	for(i=0;i<2;i++)
//	{
    BodanMotor.pid_speed.SetPoint = -3600;
	BodanMotor.pid_speed.P = 84;
	BodanMotor.pid_speed.I = 8;
	BodanMotor.pid_speed.D = 1.5;
	BodanMotor.pid_speed.IMax = 500;
	
	BodanMotor.pid_pos.P=10.0f;
	BodanMotor.pid_pos.I=0.0f;
	BodanMotor.pid_pos.D=0.0f;
	BodanMotor.pid_pos.IMax=500.0f;
	BodanMotor.pid_pos.SetPoint=BodanMotor.Angle_ABS;
	
	BodanMotor.zerocheck.Circle = 0;
	BodanMotor.zerocheck.CountCycle =8191;
	BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;
//	}
}

/**
  * @brief  ���·������������⣨��������˻���
  * @param  None
  * @retval None
  */
void BlockDetect()
{
//	if(ABS(MotoPitch.I_Set) >= GyroLimit*0.95)
//		block_disconnect.Pitch_block++;
//	else 
//		block_disconnect.Pitch_block = 0;
//	if(ABS(MotoYaw.I_Set) >= GyroLimit*0.95)
//		block_disconnect.Yaw_block++;
//	else 
//		block_disconnect.Yaw_block = 0;
//	if(ABS(BodanMotor[0].I_Set) >= BodanCurrentLimit*0.95)
//		block_disconnect.Bodan_block++;
//	else 
//		block_disconnect.Bodan_block = 0;
	
//	if(ABS(FrictionMoto[0].I_Set) >= 7500*0.95 || (ABS(FrictionMoto[1].I_Set) >= 7500*0.95))
//		block_disconnect.Friction_block++;
//	else 
//		block_disconnect.Friction_block = 0;


//	if(block_disconnect.Pitch_block>=5000)
//		block_disconnect.is_pitch_block = 1;
//	else 
//		block_disconnect.is_pitch_block = 0;
//	
//	if(block_disconnect.Yaw_block>=5000)
//		block_disconnect.is_yaw_block = 1;
//	else 
//		block_disconnect.is_yaw_block = 0;
//	
//	if(block_disconnect.Bodan_block>=5000)
//		block_disconnect.is_bodan_block = 1;
//	else 
//		block_disconnect.is_bodan_block = 0;
//	
//	if(block_disconnect.Friction_block>=5000)
//		block_disconnect.is_friction_block = 1;
//	else 
//		block_disconnect.is_friction_block = 0;
}





