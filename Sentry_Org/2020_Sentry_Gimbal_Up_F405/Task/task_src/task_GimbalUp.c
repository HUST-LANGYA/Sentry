#include "main.h"

gimbal_motor_t MotoPitch, MotoYaw;
float RCPitch_Scal = 0.08f, RCYaw_Scal = 0.0002f;   //ң������������ƵķŴ����

int32_t PITCH_MAX_ANGLE, PITCH_MIN_ANGLE, PITCH_ZERO_POS; //pitch�޷���
int32_t YAW_MAX_ANGLE, YAW_MIN_ANGLE, YAW_ZERO_POS;		  //pitch�޷���
float Pitch_Actual, Yaw_Actual;
float PC_PitchSet, PC_YawSet;

extern int16_t Gimbal_init_flag;
extern gyro_Typedef Gyro_White;


static float CombPitchOutput(void);//��ȡ�˲����pitch�Ƕȵĺ���
static float CombYawOutput(void);//��ȡ�˲����yaw�Ƕȵĺ���

float Test_Pitch = 0, Test_Yaw = 0;
int16_t rc_flag_pitch_test = 0, rc_flag_yaw_test = 0;
/**
  * @brief  ң����ģʽ�£���������̨����״̬
  * @param  ң����ֵ
  * @retval None
  */
void Gimbal_RC_Act(void)
{
    RC_Ctl_t RC_Ctl = getRCData();
    
	Yaw_Actual = CombYawOutput();
	Pitch_Actual = CombPitchOutput();

	if (Gimbal_init_flag == 0)
	{
		MotoYaw.PidPos.SetPoint = 0;        //Yaw_Actual;	  //MotoPitch.Angle_Inc;
		MotoPitch.PidPos.SetPoint = 0;      //Pitch_Actual; // Gyro_White.YAW_INC;
		Gimbal_init_flag = 1;
	}

	if (rc_flag_yaw_test) MotoYaw.PidPos.SetPoint = Test_Yaw;
    else MotoYaw.PidPos.SetPoint  -= RCYaw_Scal *  (RC_Ctl.rc.ch2 - 1002);

	if (rc_flag_pitch_test) MotoPitch.PidPos.SetPoint = Test_Pitch;
    else MotoPitch.PidPos.SetPoint = RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024);
}

/**
  * @brief  PCģʽ�£���������̨����״̬
  * @param  None
  * @retval None
  */
float TestV_Pitch=0,TestV_Yaw=0;
int16_t pc_flag_pitch_testV=0,pc_flag_yaw_testV=0;
FilterFIR filterPC;//�˲������� 
CoeFIR testCOE; // = {  0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                 };// �������ľ���������matlab���꣨������ô��ȥ��LZCȥ��
void Gimbal_PC_Act(void)
{
    extern float PC_PitchSet,PC_YawSet;
	Yaw_Actual = CombYawOutput();
	Pitch_Actual = CombPitchOutput();
    
    if(Gimbal_init_flag==0)
	{
		MotoYaw.PidPosV.SetPoint=0;//Yaw_Actual;//MotoPitch.Angle_Inc;
		MotoPitch.PidPosV.SetPoint =-30;//Pitch_Actual;// Gyro_White.YAW_INC;
		filterPC=FIRInit(testCOE);//��ʼ���˲�����
        for(int i=0;i<20;i++)
        {
            testCOE.h[i] = 0.05f;
        }
        
        Gimbal_init_flag=1;
	}
    
    if(pc_flag_pitch_testV) MotoPitch.PidPosV.SetPoint = TestV_Pitch;
    MotoPitch.PidPosV.SetPoint = PC_PitchSet;//0;//PCPitch_Scal*(remote.ch[1] - 1024);
     
	if(pc_flag_yaw_testV) MotoYaw.PidPosV.SetPoint	= TestV_Yaw;
	MotoYaw.PidPosV.SetPoint = PC_YawSet;//0;//PCYaw_Scal*(remote.ch[0]-1043);
    
    PC_MsgSend(PC_SEND_GYRO);
}

/**
  * @brief  ����̨��ת�Ƕ���λ
  * @param  None
  * @retval None
  */
void Gimbal_Limit(void)
{
	static uint8_t Orign_PitchAngle_Flag, Orign_YawAngle_Flag;

	/************************����ϵ�λ���޷���ʼ��*******************************/
	if (Orign_PitchAngle_Flag == 0)
	{
		//			if(MotoPitch.Angle_ABS >= 5000)
		//			{
		PITCH_MAX_ANGLE = Pitch_Actual + 25;
		PITCH_ZERO_POS = Pitch_Actual + 0;
		PITCH_MIN_ANGLE = Pitch_Actual - 55;

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
		YAW_MAX_ANGLE = Yaw_Actual + 65;
		YAW_ZERO_POS = Yaw_Actual + 0;
		YAW_MIN_ANGLE = Yaw_Actual - 65;

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

float Limit = 6.0f;
int16_t testYawI = 0, testPitchI = 0;

/**
  * @brief  ����̨PID�������
  * @param  None
  * @retval None
  */
extern int32_t syscnt;
void Gimbal_Pid_Cal(void)
{   
    if(Sentry_Status.GimbalMode & Gimbal_Up_RC)
    {	
        float fsend;

        //PITCH�޷�+��ֵ  
        MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint,PITCH_MAX_ANGLE,PITCH_MIN_ANGLE);
        MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos,Pitch_Actual,0)/1000.0f;
        fsend = PID_Calc(&MotoPitch.PidSpeed,Gyro_White.GY,0);
        MotoPitch.I_Set = LIMIT_MAX_MIN(fsend,gyroLimitPitch,-gyroLimitPitch);
        
        //YAW  �޷�+��ֵ     
        MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint,YAW_MAX_ANGLE,YAW_MIN_ANGLE);
        MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos,Yaw_Actual,0);
        fsend = PID_Calc(&MotoYaw.PidSpeed,Gyro_White.GZ,0);
        MotoYaw.I_Set = LIMIT_MAX_MIN(fsend,gyroLimitYaw,-gyroLimitYaw);	
        
        if(syscnt %3==0)
            Gyro_Can2Send(MotoPitch.I_Set,MotoYaw.I_Set);
	}
    
    if(Sentry_Status.GimbalMode & Gimbal_Up_PC)
    {	
        float fsend;

        //PITCH�޷�+��ֵ  
        MotoPitch.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPosV.SetPoint,PITCH_MAX_ANGLE,PITCH_MIN_ANGLE);
        MotoPitch.PidSpeedV.SetPoint = PID_Calc(&MotoPitch.PidPosV,Pitch_Actual,0)/1000.0f;
        fsend = PID_Calc(&MotoPitch.PidSpeedV,Gyro_White.GY,0);
        MotoPitch.I_Set = LIMIT_MAX_MIN(fsend,gyroLimitPitch,-gyroLimitPitch);
        
        //YAW��ֵ
        MotoYaw.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPosV.SetPoint,0,0/*YAW_MAX_ANGLE,YAW_MIN_ANGLE*/);
        MotoYaw.PidSpeedV.SetPoint = PID_Calc(&MotoYaw.PidPosV,Yaw_Actual,0);
        fsend = PID_Calc(&MotoYaw.PidSpeedV,Gyro_White.GZ,0);
        MotoYaw.I_Set = LIMIT_MAX_MIN(fsend,gyroLimitYaw,-gyroLimitYaw);	
        
        if(syscnt %3==0)
           //Gyro_Can2Send(MotoPitch.I_Set,0);
           Gyro_Can2Send(MotoPitch.I_Set,MotoYaw.I_Set);
	}   
}


float moto_pitch, moto_yaw, moto_pitch_init, moto_yaw_init;
float gyro_pitch, gyro_yaw, gyro_pitch_init, gyro_yaw_init;
float comb_pitch, comb_yaw;
float k_pitch = 0;
float k_yaw = 0;//0.02;
int8_t init_comb_flag = 1;
/**
  * @brief  ����̨��̬������㣨������ֵ�͵��ֵ�Ļ����˲���
  * @param  None
  * @retval None
  */
void Gimbal_GYRO_cal(void)
{
	if (init_comb_flag)
	{
		//moto_pitch_init=PitchAngleOutPut();
		moto_pitch_init = 5780.0f;
		gyro_pitch_init = GyroPitchOutPut();
		//moto_yaw_init=YawAngleOutPut();
		moto_yaw_init = 2700.0f;
		gyro_yaw_init = GyroYawOutPut();
		init_comb_flag = 0;
	}

    
	moto_pitch = (PitchAngleOutPut() - moto_pitch_init) / 8192 * 360;
	gyro_pitch = -(GyroPitchOutPut() - gyro_pitch_init);
	comb_pitch = k_pitch * gyro_pitch + (1 - k_pitch) * moto_pitch;     //һ�׻����˲�

	moto_yaw = (YawAngleOutPut() - moto_yaw_init) / 8192 * 360;
	gyro_yaw = -(GyroYawOutPut() - gyro_yaw_init);
	comb_yaw = k_yaw * gyro_yaw + (1 - k_yaw) * moto_yaw;               //һ�׻����˲�
}

/**
  * @brief  ��̨���pid��ʼ��
  * @param  None
  * @retval None
  */
void Pid_Gimbal_Init(void)
{
//	MotoPitch.PidPos.P =0;
//	MotoPitch.PidPos.I =0;
//	MotoPitch.PidPos.D =0;
//	MotoPitch.PidPos.IMax = 10.0f;
//	
//	MotoPitch.PidSpeed.P =0;
//	MotoPitch.PidSpeed.I =0;
//	MotoPitch.PidSpeed.D =0;
//	MotoPitch.PidSpeed.IMax = 50.0f;

	MotoPitch.PidPos.P = -45.0f;
	MotoPitch.PidPos.I = 0;
	MotoPitch.PidPos.D = 0;
	MotoPitch.PidPos.IMax = 10.0f;

	MotoPitch.PidSpeed.P = 850;
	MotoPitch.PidSpeed.I = 30.0f;
	MotoPitch.PidSpeed.D = 0;
	MotoPitch.PidSpeed.IMax = 30.0f;
      
    
    MotoPitch.PidPosV.P=-55.0f;
    MotoPitch.PidPosV.I=0.0f;
    MotoPitch.PidPosV.D=0.0f;
    MotoPitch.PidPosV.IMax=10.0f;
    
    MotoPitch.PidSpeedV.P=1600.0f;
    MotoPitch.PidSpeedV.I=15.0f;
    MotoPitch.PidSpeedV.D=0.0f;
    MotoPitch.PidSpeedV.IMax=15.0f;

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

	MotoYaw.PidPos.P = -0.7f;
	MotoYaw.PidPos.I = 0;
	MotoYaw.PidPos.D = 1;
	MotoYaw.PidPos.I_Limit = 0.5f;
	MotoYaw.PidPos.IMax = 10.0f;

	MotoYaw.PidSpeed.P = -7000;        
	MotoYaw.PidSpeed.I = 0;
	MotoYaw.PidSpeed.D = 10;
	MotoYaw.PidSpeed.I_Limit = 1;
	MotoYaw.PidSpeed.IMax = 500;
    
//60����Ӧ�õ�PID�����������
//  MotoYaw.PidPos.P = -0.7f;
//	MotoYaw.PidPos.I = 0;
//	MotoYaw.PidPos.D = 1;
//	MotoYaw.PidPos.I_Limit = 0.5f;
//	MotoYaw.PidPos.IMax = 10.0f;

//	MotoYaw.PidSpeed.P = -7500;        
//	MotoYaw.PidSpeed.I = 0;
//	MotoYaw.PidSpeed.D = 10;
//	MotoYaw.PidSpeed.I_Limit = 1;
//	MotoYaw.PidSpeed.IMax = 500;
    
    
//    MotoYaw.PidPosV.P=-0.5f;
//    MotoYaw.PidPosV.I=0.0f;
//    MotoYaw.PidPosV.D=1.0f;
//    MotoYaw.PidPosV.I_Limit = 0.5f;
//    MotoYaw.PidPosV.IMax=10.0f;
//	
//    MotoYaw.PidSpeedV.P=-5000.0f;
//    MotoYaw.PidSpeedV.I=0.0f;
//    MotoYaw.PidSpeedV.D=10.0f;
//    MotoYaw.PidSpeedV.I_Limit=1.0f;
//    MotoYaw.PidSpeedV.IMax=10.0f;

//��������PID�����Ǹ����Ӿ���ʱ���õģ�
//��������Ĳ�ͬ���ǣ���ͬ�����Ӿ����ݸ��µ�Ƶ���ϣ�
//���⽵����p������ֵ������ʹ��Ӧ���������������Ƹ�Ƶ����
    MotoYaw.PidPosV.P=-0.1f;
    MotoYaw.PidPosV.I=0.0f;
    MotoYaw.PidPosV.D=1.0f;
    MotoYaw.PidPosV.I_Limit = 0.5f;
    MotoYaw.PidPosV.IMax=10.0f;
	
    MotoYaw.PidSpeedV.P=-100.0f;
    MotoYaw.PidSpeedV.I=0.0f;
    MotoYaw.PidSpeedV.D=5.0f;
    MotoYaw.PidSpeedV.I_Limit=1.0f;
    MotoYaw.PidSpeedV.IMax=5.0f;


	MotoPitch.zerocheck.CountCycle = 8191;
	MotoPitch.zerocheck.Circle = 0;
	MotoPitch.zerocheck.LastValue = MotoPitch.Angle_ABS;

	MotoYaw.zerocheck.CountCycle = 8191;
	MotoYaw.zerocheck.Circle = 0;
	MotoYaw.zerocheck.LastValue = MotoYaw.Angle_ABS; //����Ƕ�

	Gyro_White.zerocheck_yaw.CountCycle = 360;
	Gyro_White.zerocheck_yaw.Circle = 0;
	Gyro_White.zerocheck_yaw.LastValue = Gyro_White.YAW_ABS; //�����ǵĽǶ�

	Gyro_White.zerocheck_pitch.CountCycle = 360;
	Gyro_White.zerocheck_pitch.Circle = 0;
	Gyro_White.zerocheck_pitch.LastValue = Gyro_White.PITCH; //�����ǵĽǶ�
															 //
	//	MotoPitch.PidPos.SetPoint = MotoPitch.Angle_ABS;
	//	MotoYaw.PidPos.SetPoint = Gyro_White.YAW_ABS;
}


/**
  * @brief  ��ȡ�˲������̨������̬��pitch��yaw��
  * @param  None
  * @retval pitch��yaw�ĽǶ�
  */
static float CombPitchOutput(void)
{
	return comb_pitch;
}
static float CombYawOutput(void)
{
	return comb_yaw;
}
