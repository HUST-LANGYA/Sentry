#include "main.h"
/*
    视觉对接的部分，留一个UART的口子，用中断周期性的读
    然后专门取两个变量用于接收从视觉接收，发送到yaw轴和pitch轴电机的位置环设定值
    然后在包含视觉的act部分里，调试出适用于视觉节奏的两轴电机PID参数
*/

/*
UART部分，包含
    在bsp层的硬件配置（包括硬件初始化、dma配置和中断服务函数）、
    在dev层的解码函数（把视觉发过来的报文解码成pitch和yaw的SetPoint）
*/

/*************************内部变量*****************************/
gimbal_motor_t MotoPitch,MotoYaw;
float RCPitch_Scal = 0.04f,RCYaw_Scal = 0.0003f;

int32_t PITCH_MAX_ANGLE,PITCH_MIN_ANGLE,PITCH_ZERO_POS;										//pitch限幅度	
int32_t YAW_MAX_ANGLE,YAW_MIN_ANGLE,YAW_ZERO_POS;										//pitch限幅度	
float Pitch_Actual,Yaw_Actual;
float PC_PitchSet, PC_YawSet;

extern int16_t Gimbal_init_flag;
extern  gyro_Typedef Gyro_White;

float CombPitchOutput(void);
float CombYawOutput(void);


/**
  * @brief  RC模式下，从连有上云台的CAN线上获取遥控数据
  * @param  None
  * @retval None
  */
float Test_Pitch=0,Test_Yaw=0;
int16_t rc_flag_pitch_test=0,rc_flag_yaw_test=0;
void Gimbal_RC_Act(void)
{	
    extern REMOTE_t remote;
	Yaw_Actual   =  CombYawOutput();
    Pitch_Actual =	CombPitchOutput();

	if(Gimbal_init_flag==0)
	{
		MotoYaw.PidPos.SetPoint=0;//Yaw_Actual;//MotoPitch.Angle_Inc;
		MotoPitch.PidPos.SetPoint =0;//Pitch_Actual;// Gyro_White.YAW_INC;
		Gimbal_init_flag=1;
	}

//	if(temp<=20&&temp>=-20)
//		temp = 0;
    
//    if(rc_flag_pitch_test) MotoPitch.PidPos.SetPoint = Test_Pitch;
//	MotoPitch.PidPos.SetPoint = RCPitch_Scal*(remote.ch[1] - 1024);
//    
//    
//	if(rc_flag_yaw_test) MotoYaw.PidPos.SetPoint	= Test_Yaw;
//	MotoYaw.PidPos.SetPoint += RCYaw_Scal*(remote.ch[0]-1043);
}

/**
  * @brief  PC模式下，更新下云台工作状态
  * @param  None
  * @retval None
  */
float TestV_Pitch=0,TestV_Yaw=0;
int16_t pc_flag_pitch_testV=0,pc_flag_yaw_testV=0;
void Gimbal_PC_Act(void)
{
    extern float PC_PitchSet,PC_YawSet;
	Yaw_Actual = CombYawOutput();
	Pitch_Actual = CombPitchOutput();
    
    if(Gimbal_init_flag==0)
	{
		MotoYaw.PidPosV.SetPoint=0;//Yaw_Actual;//MotoPitch.Angle_Inc;
		MotoPitch.PidPosV.SetPoint =0;//Pitch_Actual;// Gyro_White.YAW_INC;
		Gimbal_init_flag=1;
	}
    
     if(pc_flag_pitch_testV) MotoPitch.PidPosV.SetPoint = TestV_Pitch;
     MotoPitch.PidPosV.SetPoint = PC_PitchSet;//0;//PCPitch_Scal*(remote.ch[1] - 1024);
    
    
	if(pc_flag_yaw_testV) MotoYaw.PidPosV.SetPoint	= TestV_Yaw;
	MotoYaw.PidPosV.SetPoint = PC_YawSet;//0;//PCYaw_Scal*(remote.ch[0]-1043);
    
    
    PC_MsgSend(PC_SEND_GYRO);
    
}

void Gimbal_Limit(void)
{
		static uint8_t Orign_PitchAngle_Flag,Orign_YawAngle_Flag;
/************************电机上电位置限幅初始化*******************************/
		if(Orign_PitchAngle_Flag == 0)
		{
//			if(MotoPitch.Angle_ABS >= 5000)
//			{
				PITCH_MAX_ANGLE = Pitch_Actual+30;
				PITCH_ZERO_POS = Pitch_Actual+0;
				PITCH_MIN_ANGLE =Pitch_Actual-60;	

//			}
//			else
//			{
//				PITCH_MAX_ANGLE = 8491;
//				PITCH_ZERO_POS = 7791;
//				PITCH_MIN_ANGLE = 7291;
//			}
//			Orign_PitchAngle_Flag = 1;
//		}
//		
//		if(Orign_YawAngle_Flag == 0)
//		{
//			if(MotoYaw.Angle_ABS >= 5000)
//			{
//				YAW_MAX_ANGLE = Yaw_Actual+80;
//				YAW_ZERO_POS = Yaw_Actual+0;
//				YAW_MIN_ANGLE = Yaw_Actual-80;		//最低150开始

//			}
//			else
//			{
//				YAW_MAX_ANGLE = 709;
//				YAW_ZERO_POS = 650;
//				YAW_MIN_ANGLE = -91;
//			}
			Orign_PitchAngle_Flag = 1;			
		}
}

extern int32_t syscnt;
void Gimbal_PID_Cal(void)
{
    if(Sentry_Status.GimbalMode & Gimbal_Dn_RC)
    {	
        float fsend;

        //PITCH限幅+赋值  
        MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint,PITCH_MAX_ANGLE,PITCH_MIN_ANGLE);
        MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos,Pitch_Actual,0)/1000.0f;
        fsend = PID_Calc(&MotoPitch.PidSpeed,Gyro_White.GY,0);
        MotoPitch.I_Set = LIMIT_MAX_MIN(fsend,GyroLimit,-GyroLimit);
        
        //YAW赋值     
        MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos,Yaw_Actual,0);
        fsend = PID_Calc(&MotoYaw.PidSpeed,Gyro_White.GZ,0);
        MotoYaw.I_Set = LIMIT_MAX_MIN(fsend,30000,-30000);	
        
        if(syscnt %3==0)
            Gyro_Can2Send(MotoPitch.I_Set,MotoYaw.I_Set);
	}
    
    if(Sentry_Status.GimbalMode & Gimbal_Dn_PC)
    {	
        float fsend;

        //PITCH限幅+赋值  
        MotoPitch.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPosV.SetPoint,PITCH_MAX_ANGLE,PITCH_MIN_ANGLE);
        MotoPitch.PidSpeedV.SetPoint = PID_Calc(&MotoPitch.PidPosV,Pitch_Actual,0)/1000.0f;
        fsend = PID_Calc(&MotoPitch.PidSpeedV,Gyro_White.GY,0);
        MotoPitch.I_Set = LIMIT_MAX_MIN(fsend,GyroLimit,-GyroLimit);
        
        //YAW赋值     
        MotoYaw.PidSpeedV.SetPoint = PID_Calc(&MotoYaw.PidPosV,Yaw_Actual,0);
        fsend = PID_Calc(&MotoYaw.PidSpeedV,Gyro_White.GZ,0);
        MotoYaw.I_Set = LIMIT_MAX_MIN(fsend,30000,-30000);	
        
        if(syscnt %3==0)
            Gyro_Can2Send(MotoPitch.I_Set,MotoYaw.I_Set);
	}   
	
//测试模式      
//    Gyro_Can2Send(0,0);

}

//Pitch_Comb
float moto_pitch,moto_yaw,moto_pitch_init,moto_yaw_init;
float gyro_pitch,gyro_yaw,gyro_pitch_init,gyro_yaw_init;
float comb_pitch,comb_yaw;
float k_pitch=0;
float k_yaw=0;
int8_t init_comb_flag=1;
void Gimbal_GYRO_cal()
{//互补滤波解算两轴姿态
	if(init_comb_flag)
	{
		//moto_pitch_init=PitchAngleOutPut();
        moto_pitch_init=2040;
		gyro_pitch_init=GyroPitchOutPut();
        //moto_yaw_init=YawAngleOutPut();
        moto_yaw_init=1325;
		gyro_yaw_init=GyroYawOutPut();
		init_comb_flag=0;
	}
	
	moto_pitch=(PitchAngleOutPut()-moto_pitch_init)/8192*360;
	gyro_pitch=-(GyroPitchOutPut()-gyro_pitch_init);
	comb_pitch=k_pitch*gyro_pitch+(1-k_pitch)*moto_pitch;
	
	moto_yaw=(YawAngleOutPut()-moto_yaw_init)/8192*360;
	gyro_yaw=-(GyroYawOutPut()-gyro_yaw_init);
	comb_yaw=k_yaw*gyro_yaw+(1-k_yaw)*moto_yaw; //k_yaw=1,就滤波的时候只看陀螺仪的角度了
	
}


/**
  * @brief  云台电机pid初始化
  * @param  None
  * @retval None
  */
void Pid_Gimbal_Init(void)
{
//	MotoPitch.PidPos.P =0;//-150;		//电机角度				// -15																				
//	MotoPitch.PidPos.I =0;//-25;
//	MotoPitch.PidPos.D =0;// 0;			
//	MotoPitch.PidPos.IMax = 10.0f;	
//	
//	MotoPitch.PidSpeed.P =0;//3500;
//	MotoPitch.PidSpeed.I =0;//20;	
//	MotoPitch.PidSpeed.D =0;// 0;
//	MotoPitch.PidSpeed.IMax = 50.0f;
    
    
    MotoPitch.PidPos.P =-300;																			
	MotoPitch.PidPos.I =0;
	MotoPitch.PidPos.D =0;			
	MotoPitch.PidPos.IMax = 10.0f;	
	
	MotoPitch.PidSpeed.P =1500;
	MotoPitch.PidSpeed.I =10;	
	MotoPitch.PidSpeed.D =0;
	MotoPitch.PidSpeed.IMax = 50.0f;
    
    MotoPitch.PidPosV.P=0.0f;
    MotoPitch.PidPosV.I=0.0f;
    MotoPitch.PidPosV.D=0.0f;
    MotoPitch.PidPosV.IMax=0.0f;
    
    MotoPitch.PidSpeedV.P=0.0f;
    MotoPitch.PidSpeedV.I=0.0f;
    MotoPitch.PidSpeedV.D=0.0f;
    MotoPitch.PidSpeedV.IMax=0.0f;
    
    
//	MotoYaw.PidPos.P = 0;
//	MotoYaw.PidPos.I =0;
//	MotoYaw.PidPos.D = 0;
//	MotoYaw.PidPos.I_Limit = 0.5;
//	MotoYaw.PidPos.IMax = 10.0f;
//	
//	MotoYaw.PidSpeed.P =0;	
//	MotoYaw.PidSpeed.I =0;
//	MotoYaw.PidSpeed.D = 0;
//	MotoYaw.PidSpeed.I_Limit = 1;
//	MotoYaw.PidSpeed.IMax = 500;	

	MotoYaw.PidPos.P = 0.3f;
	MotoYaw.PidPos.I =0;
	MotoYaw.PidPos.D = 0;
	MotoYaw.PidPos.I_Limit = 0.5;
	MotoYaw.PidPos.IMax = 10.0f;
	
	MotoYaw.PidSpeed.P = 13000;	
	MotoYaw.PidSpeed.I = 0;
	MotoYaw.PidSpeed.D = 3000;
	MotoYaw.PidSpeed.I_Limit = 1;
	MotoYaw.PidSpeed.IMax = 500;	
    
    MotoYaw.PidPosV.P=0.0f;
    MotoYaw.PidPosV.I=0.0f;
    MotoYaw.PidPosV.D=0.0f;
    MotoYaw.PidPosV.IMax=0.0f;
	
    MotoYaw.PidSpeedV.P=0.0f;
    MotoYaw.PidSpeedV.I=0.0f;
    MotoYaw.PidSpeedV.D=0.0f;
    MotoYaw.PidSpeedV.IMax=0.0f;
 
    MotoPitch.zerocheck.CountCycle = 8191;
	MotoPitch.zerocheck.Circle = 0;
	MotoPitch.zerocheck.LastValue = MotoPitch.Angle_ABS;
	
	MotoYaw.zerocheck.CountCycle = 8191;
	MotoYaw.zerocheck.Circle = 0;
	MotoYaw.zerocheck.LastValue = MotoYaw.Angle_ABS;			//电机角度

	Gyro_White.zerocheck_yaw.CountCycle = 360;
	Gyro_White.zerocheck_yaw.Circle = 0;
	Gyro_White.zerocheck_yaw.LastValue = Gyro_White.YAW_ABS;	//陀螺仪的角度
	
	Gyro_White.zerocheck_pitch.CountCycle = 360;
	Gyro_White.zerocheck_pitch.Circle = 0;
	Gyro_White.zerocheck_pitch.LastValue = Gyro_White.PITCH;	//陀螺仪的角度	
//	
//	MotoPitch.PidPos.SetPoint = MotoPitch.Angle_ABS;
//	MotoYaw.PidPos.SetPoint = Gyro_White.YAW_ABS;
}
float CombPitchOutput(void)
{
	return comb_pitch;
}
float CombYawOutput(void)
{
	return comb_yaw;
}

