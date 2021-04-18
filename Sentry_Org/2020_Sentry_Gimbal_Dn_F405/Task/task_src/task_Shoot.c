#include "main.h"

_2006_moto_t BodanMotor;
//int16_t num, speed;

extern int16_t Shoot_init_flag;
extern Status_t Sentry_Status;

//无裁判系统模式下的检录射击
int16_t FricSpeed_test;
void Shoot_Fire_Cal(void)
{
	/*******************************摩擦轮输出计算*********************************/
	//    if(block_disconnect.Bodan_block!=0)
	//		{
	//		float maxspeed = 25*196.3;
	//		BodanMotor.pid_pos.SetPoint=BodanMotor.Angle_Inc=1000;
	//		BodanMotor.pid_speed.SetPoint = LIMIT_MAX_MIN(PID_Calc(&BodanMotor.pid_pos,BodanMotor.Angle_Inc,0),maxspeed,0);
	//		fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);
	//		BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,7500,-7500);
	//
	//		}
    if (Sentry_Status.ShootMode & Shoot_Dn_RC)
    {
        FrictionWheel_Set(400); //先开摩擦轮，再开拨弹
        float fsend;
        //num = LIMIT_MAX_MIN(num,25,0);
//        BodanMotor.pid_speed.SetPoint = 36*60/8; // -num*270*2; //加负号因为方向反了，机械的问题
        BodanMotor.pid_pos.SetPoint=BodanMotor.Angle_Inc+1000;
        BodanMotor.pid_speed.SetPoint = PID_Calc(&BodanMotor.pid_pos,BodanMotor.Angle_Inc,0);
        fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);
        BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,7500,-7500);

        
        //发拨弹电流
        Bodan_Can1Send(BodanMotor.I_Set);
    }
    if (Sentry_Status.ShootMode & Shoot_Dn_PC)
    {
        //FrictionWheel_Set(FricSpeed_test);
         FrictionWheel_Set(400); //先开摩擦轮，再开拨弹
         float fsend;
        //num = LIMIT_MAX_MIN(num,25,0);
        //BodanMotor.I_Set = 0;
//        BodanMotor.pid_speed.SetPoint = 0;
//        fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);
//        BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,7500,-7500);
        
        
        //BodanMotor.pid_speed.SetPoint = 40;
        fsend = PID_Calc(&BodanMotor.pid_speed,BodanMotor.RealSpeed,0);
        BodanMotor.I_Set=(int16_t)LIMIT_MAX_MIN(fsend,7500,-7500);
        //发拨弹电流
        Bodan_Can1Send(BodanMotor.I_Set);
    }
}

/**
  * @brief  拨弹电机pid初始化
  * @param  None
  * @retval None
  */
void BodanMoto_Init(void)
{
    BodanMotor.pid_speed.SetPoint=800;
	BodanMotor.pid_speed.P = 6;
	BodanMotor.pid_speed.I = 1;
	BodanMotor.pid_speed.D = 1.5;
	BodanMotor.pid_speed.IMax = 500;
    
    BodanMotor.pid_pos.P = 0.0f;
	BodanMotor.pid_pos.I = 0.0f;
	BodanMotor.pid_pos.D = 0.0f;
	BodanMotor.pid_pos.IMax = 500.0f;
	BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;

//	BodanMotor.pid_pos.P = 0.22f;
//	BodanMotor.pid_pos.I = 0.05f;
//	BodanMotor.pid_pos.D = 0.0f;
//	BodanMotor.pid_pos.IMax = 500.0f;
//	BodanMotor.pid_pos.SetPoint = BodanMotor.Angle_ABS;

	BodanMotor.zerocheck.Circle = 0;
	BodanMotor.zerocheck.CountCycle = 8191;
	BodanMotor.zerocheck.LastValue = BodanMotor.Angle_ABS;
}
