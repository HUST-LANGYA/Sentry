#include "main.h"

//这是一个很蛋疼的事情。因为硬件连接限制的原因，如果把三块板用一根CAN线连起来，走遥控报文的话，
//那么上云台的拨弹电机就得连到底盘板上，用底盘板给拨弹电机发信息来控制它拨弹，
//不过摩擦轮又连在上云台板上，所以上枪管的控制逻辑就会变得很诡异
//大概是上板接遥控器数据，把数据打包发到CAN线上，然后底盘板接收时解码如果发现要开上射击模式，
//那么自己算一波拨弹的速度环（先延一下时等上摩擦轮启动），配合上发射
//因此，底盘板和上云台板都要写shoot_fire_cal , 而且内容还不一样（一个开摩擦轮，一个开拨弹）


_2006_motor_t BodanMotor;
int16_t num;

int16_t Shoot_init_flag;

//无裁判系统模式下的检录射击
void Shoot_Fire_Cal(void)
{
//	int32_t i;

	float fsend;
	if(Shoot_init_flag==0)//其实拨弹电机用的是速度环，所以这个其实没啥用嗷
	{
		BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_Inc;		
		//BodanMotor[1].pid_pos.SetPoint = BodanMotor[1].Angle_Inc;	//现在只有一个拨弹电机
		Shoot_init_flag=1;
    }
	
		

/**********************************拨弹电机输出模式计算********************************/		
     if(Sentry_Status.ShootMode & Shoot_Up_RC)
	 {		
		num=6;//加上裁判系统后，改为热量限制
//		speed=FrictionSpeed_Low;

	 }
	 else if (Sentry_Status.ShootMode & Shoot_Up_PC)
	 {
//		 	if(Sentry.is_shoot == 0 || Sentry.is_shoot >10)
//				num=0;
//			else if(block_disconnect.is_judge_down == 0 && block_disconnect.is_pc_down == 0)
//			{
//				block_disconnect.BullectCnt++;						//判断是否没有了子弹
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
////				block_disconnect.BullectCnt++;						//判断是否没有了子弹
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

/*************************拨弹电机输出计算***********************************/	 
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
        //BodanMotor.pid_speed.SetPoint = 800;//-num*270*2;		// 36*60/8; //加负号因为方向反了，机械的问题
        fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed);
        BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,7500,-7500);
    }
    else
    BodanMotor.I_Set=0;
//	}
/*******************************摩擦轮输出计算*********************************/
	 
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
  * @brief  拨弹电机pid初始化
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
  * @brief  上下发射机构卡弹检测（这会整不了还）
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





