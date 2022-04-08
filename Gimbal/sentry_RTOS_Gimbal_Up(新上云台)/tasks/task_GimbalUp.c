#include "main.h"
#include "task_GimbalUp.h"

gimbal_motor_t MotoPitch, MotoYaw;
float RCPitch_Scal = 0.00005f, RCYaw_Scal = 0.00004f; //ң������������ƵķŴ����

int32_t PITCH_MAX_ANGLE, PITCH_MIN_ANGLE, PITCH_ZERO_POS; //pitch�޷��Ȳ���
int32_t YAW_MAX_ANGLE, YAW_MIN_ANGLE, YAW_ZERO_POS;       //yaw�޷��Ȳ���

float Pitch_Actual, Yaw_Actual;
float PC_PitchSet, PC_YawSet;

extern int16_t Gimbal_init_flag;
extern gyro_Typedef Gyro_White;
extern State_t Sentry_State;
extern uint8_t aim_flag;
//�����л�modeʱ��pitch��yaw����λ��
uint8_t GimbalUp_ModeUpdate_Flag = 0;                 //��־�ϴ�ģʽ�͵�ǰģʽ�Ƿ�һ��
volatile uint8_t GimbalUp_LastMode = Gimbal_Up_SLEEP; //����ϴν���Gimbal_Dn_taskʱ��״̬

//******************�ڲ���������***********************************************//
static float CombPitchOutput(void); //��ȡ�˲����pitch�Ƕȵĺ���
static float CombYawOutput(void);   //��ȡ�˲����yaw�Ƕȵĺ���
static void Gimbal_Limit(void);//��̨�Ƕ��޷�����
static void PID_Gimbal_Init(void);
static void Gimbal_GYRO_Cal(void);

static void Gimbal_RC_Act(void);
static void Gimbal_PC_Act(void);
static void Gimbal_SLEEP_Act(void);
static void Gimbal_DEBUG_Act(void);

static void Gimbal_RC_PID_Cal(void);
static void Gimbal_PC_PID_Cal(void);
inline static void Gimbal_SLEEP_PID_Cal(void);
static void Gimbal_DEBUG_PID_Cal(void);
/**
 * @brief ��̨��������
 * @param ��
 * @retval ��
 */
void task_GimbalUp(void *parameter)
{
    //��������ʱִ��һ�θ�λ
    PID_Gimbal_Init(); //��һ��ִ����̨������ ����̨��PID�������г�ʼ��
    Gimbal_Limit();    //�޷�У��
    while (1)
    {
        GimbalUp_ModeUpdate_Flag = (GimbalUp_LastMode != Sentry_State.Gimbal_Up_Mode); //?  0: 1;
        GimbalUp_LastMode = Sentry_State.Gimbal_Up_Mode;                               //�����ϴ�״̬

       // Gimbal_Limit();
        Gimbal_GYRO_Cal();

        Yaw_Actual = CombYawOutput();
        Pitch_Actual = CombPitchOutput();

        if (Sentry_State.Gimbal_Up_Mode == Gimbal_Up_RC)      Gimbal_RC_Act();
        else if (Sentry_State.Gimbal_Up_Mode == Gimbal_Up_PC) Gimbal_PC_Act();
        else if (Sentry_State.Gimbal_Up_Mode == Gimbal_Up_SLEEP) Gimbal_SLEEP_Act();
        else if (Sentry_State.Gimbal_Up_Mode == Gimbal_Up_DEBUG) Gimbal_DEBUG_Act();
        vTaskDelay(1);
    }
}

float Test_Pitch_Pos = 0, Test_Yaw_Pos = 0;
float Test_Pitch_Veloc = 0, Test_Yaw_Veloc = 0;
static void Gimbal_DEBUG_Act(void)
{
    //RC_Ctl_t RC_Ctl = getRCData();

    MotoYaw.PidPos.SetPoint = Test_Yaw_Pos;                             //+ YAW_GYRO_OFFSET;       //RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
    MotoPitch.PidPos.SetPoint = Test_Pitch_Pos;       /*+ PITCH_GYRO_OFFSET*/; //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;	//   MotoPitch.PidSpeed.SetPoint = Test_Pitch_Veloc/*+ PITCH_GYRO_OFFSET*/;   //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;                                                          //   MotoYaw.PidSpeed.SetPoint = Test_Yaw_Veloc;
	
    Gimbal_DEBUG_PID_Cal();
}

/**
  * @brief  ң����ģʽ�£���������̨����״̬
  * @param  ң����ֵ
  * @retval None
  */
static void Gimbal_RC_Act(void)
{
    extern RC_Ctl_t RC_Ctl;

    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPos.SetPoint = 0;  
        MotoPitch.PidPos.SetPoint = 0; 
        Gimbal_init_flag = 1;
    }

    if (GimbalUp_ModeUpdate_Flag) //Ӧ����������ֹģʽ�л���ʱ���˶�����
    {
        MotoYaw.PidPos.SetPoint = Yaw_Actual;
        MotoPitch.PidPos.SetPoint = Pitch_Actual;
    }
    else
    { //���ģʽû�з����仯��������ִ��ң������ֵ�Ĳ���
        MotoYaw.PidPos.SetPoint -= RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
        MotoPitch.PidPos.SetPoint += RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024);
    }
 
    Gimbal_RC_PID_Cal();
}

/**
  * @brief  PCģʽ�£���������̨����״̬
  * @param  None
  * @retval None
  */
float patrol_step_pitch = 0.035f/*0.035f */, patrol_step_yaw =0.03f;// 0.005f;
uint8_t patrol_dir_yaw = 0, patrol_dir_pitch = 0;                              //��־���ܵ����ǰѲ�ߵķ���0��ʾ--��1��ʾ++
float patrol_set_pitch = 0.0f, patrol_set_yaw = 0.0f;                          //Ѳ���趨ֵ����
int32_t PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE, PATROL_PITCH_ZERO_POS; //pitch�޷��Ȳ���
int32_t PATROL_YAW_MAX_ANGLE, PATROL_YAW_MIN_ANGLE, PATROL_YAW_ZERO_POS;       //yaw�޷��Ȳ���
static void Gimbal_PC_Act(void)
{
    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPosV_Fuzzy.SetPoint = 0; 
        MotoPitch.PidPosV_Fuzzy.SetPoint = 0; 
        patrol_set_pitch = 0;
        patrol_set_yaw = 0;

        Gimbal_init_flag = 1;
    }

    if (GimbalUp_ModeUpdate_Flag)
    {
        MotoPitch.PidPosV_Fuzzy.SetPoint = Pitch_Actual;
        MotoYaw.PidPosV_Fuzzy.SetPoint = Yaw_Actual;

        patrol_set_yaw = Yaw_Actual;
        patrol_set_pitch = Pitch_Actual;
    }
    else
    {
        extern  uint8_t armor_state;
        extern  short tx2_last_receive_flag; //��ʾ��û�����ݸ���
        extern  short tx2_receive_flag;
			  extern uint8_t aim_Valid_Flag;
			  //extern uint8_t PCbuffer[PC_RECVBUF_SIZE];
			  //tx2_receive_flag = (uint8_t)(PCbuffer[1]&0xff);         //��Ϊ���±�־λ
//        if (tx2_receive_flag == tx2_last_receive_flag) //���û�����ݸ���
//        {
//            armor_state = ARMOR_NO_AIM; //ûĿ��
//        }
//        else
//        {
//            armor_state = ARMOR_AIMED; //��Ŀ��
//        }
//			
			
        if (armor_state == ARMOR_AIMED)
        { //�������׽��װ�ף��������׼ģʽ����̨��̬����CV��Ϣ������
            extern float aim_yaw, aim_pitch;

//            MotoPitch.PidPosV.SetPoint = aim_pitch; 
//            MotoYaw.PidPosV.SetPoint   = aim_yaw;

					  MotoPitch.PidPosV_Fuzzy.SetPoint = aim_pitch;
					  MotoYaw.PidPosV_Fuzzy.SetPoint = aim_yaw;
					 //MotoYaw.PidPosV.SetPoint =0;
           // patrol_set_pitch = LIMIT_MAX_MIN(aim_pitch, PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE);
					  patrol_set_pitch = aim_pitch;
            patrol_set_yaw = LIMIT_MAX_MIN(aim_yaw, PATROL_YAW_MAX_ANGLE, PATROL_YAW_MIN_ANGLE);
        }
				
        else
        {   
   					extern float aim_Up_Pitch;//����̨������PitchЭֵͬ
            extern float aim_Up_Yaw;//����̨������YawЭֵͬ
		         
					  if(aim_Valid_Flag==1)
						{
            MotoPitch.PidPosV_Fuzzy.SetPoint = aim_Up_Pitch;
					  MotoYaw.PidPosV_Fuzzy.SetPoint = aim_Up_Yaw;
						}
					 else
					 {
            if ((patrol_set_pitch + patrol_step_pitch >= PATROL_PITCH_MAX_ANGLE) && patrol_dir_pitch)
                patrol_dir_pitch = 0; //��������޷���
            if ((patrol_set_pitch - patrol_step_pitch <= PATROL_PITCH_MIN_ANGLE) && !patrol_dir_pitch)
                patrol_dir_pitch = 1; //��������޷���
            if ((patrol_set_yaw + patrol_step_yaw >= PATROL_YAW_MAX_ANGLE) && patrol_dir_yaw)
                patrol_dir_yaw = 0; //��������޷���
            if ((patrol_set_yaw - patrol_step_yaw <= PATROL_YAW_MIN_ANGLE) && !patrol_dir_yaw)
                patrol_dir_yaw = 1; //��������޷���
            patrol_set_pitch += (patrol_dir_pitch == 1) ? (+patrol_step_pitch) : (-patrol_step_pitch);
            patrol_set_yaw += (patrol_dir_yaw == 1) ? (+patrol_step_yaw) : (-patrol_step_yaw);

            //MotoPitch.PidPosV.SetPoint = patrol_set_pitch;
//						MotoPitch.PidPosV.SetPoint = patrol_set_pitch;
//            MotoYaw.PidPosV.SetPoint = patrol_set_yaw;
						MotoPitch.PidPosV_Fuzzy.SetPoint = patrol_set_pitch;
            MotoYaw.PidPosV_Fuzzy.SetPoint = patrol_set_yaw;
						//MotoYaw.PidPosV.SetPoint=0;
					}
        }
    }
    Gimbal_PC_PID_Cal();
}

static void Gimbal_SLEEP_Act(void)
{
    Gimbal_SLEEP_PID_Cal();
}


/**
  * @brief  ����̨PID�������
  * @param  None
  * @retval None
  */
static void Gimbal_RC_PID_Cal(void)
{
    float fsend;
  
    //PITCH�޷�+��ֵ 
	  
    MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  �޷�+��ֵ
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeed, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can1Send(MotoPitch.I_Set, MotoYaw.I_Set);
}

static void Gimbal_PC_PID_Cal(void)
{
    float fsend;
	  float fsend_fuzzy;
    extern uint8_t armor_state; 
    //PITCH�޷�+��ֵ
//	  if(armor_state==ARMOR_NO_AIM)
//		{
//     MotoPitch.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPosV.SetPoint, PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE);
//		//}
//    MotoPitch.PidSpeedV.SetPoint = PID_Calc(&MotoPitch.PidPosV, Pitch_Actual, 0) / 1000.0f;
//    fsend = PID_Calc(&MotoPitch.PidSpeedV, Gyro_White.GY, 0);
//    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);
	
	   //ģ��PID
	  MotoPitch.PidPosV_Fuzzy.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPosV_Fuzzy.SetPoint,PATROL_PITCH_MAX_ANGLE,PATROL_PITCH_MIN_ANGLE);
	  MotoPitch.PidSpeedV_Fuzzy.SetPoint = FuzzyPID_Calc(&MotoPitch.PidPosV_Fuzzy,Pitch_Actual) / 1000.0f;
	  fsend_fuzzy = FuzzyPID_Calc(&MotoPitch.PidSpeedV_Fuzzy,Gyro_White.GY);
	  MotoPitch.I_Set_Fuzzy = LIMIT_MAX_MIN(fsend_fuzzy,gyroLimitPitch,-gyroLimitPitch);

    //YAW��ֵ
		//if(armor_state==ARMOR_NO_AIM)
		//{
//     MotoYaw.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPosV.SetPoint, PATROL_YAW_MAX_ANGLE,  PATROL_YAW_MIN_ANGLE);
//		//}
//    MotoYaw.PidSpeedV.SetPoint = PID_Calc(&MotoYaw.PidPosV, Yaw_Actual, 0);
//    fsend = PID_Calc(&MotoYaw.PidSpeedV, Gyro_White.GZ, 0);
//    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);
    MotoYaw.PidPosV_Fuzzy.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPosV_Fuzzy.SetPoint,PATROL_YAW_MAX_ANGLE,PATROL_YAW_MIN_ANGLE);
    MotoYaw.PidSpeedV_Fuzzy.SetPoint = FuzzyPID_Calc(&MotoYaw.PidPosV_Fuzzy,Yaw_Actual);
	  fsend_fuzzy = FuzzyPID_Calc(&MotoYaw.PidSpeedV_Fuzzy,Gyro_White.GZ);
    MotoYaw.I_Set_Fuzzy = LIMIT_MAX_MIN(fsend_fuzzy,gyroLimitYaw,-gyroLimitYaw);
    
     
    Gyro_Can1Send(MotoPitch.I_Set_Fuzzy, MotoYaw.I_Set_Fuzzy);
}


static void Gimbal_SLEEP_PID_Cal(void)
{
    Gyro_Can1Send(0, 0);
}


static void Gimbal_DEBUG_PID_Cal(void)
{
     float fsend;
  
    //PITCH�޷�+��ֵ 
	  
    MotoPitch.PidPos.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPos.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeed.SetPoint = PID_Calc(&MotoPitch.PidPos, Pitch_Actual, 0) / 1000.0f;  //1000.0f�����������ǵķ��صĽ��ٶ�
    fsend = PID_Calc(&MotoPitch.PidSpeed, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW  �޷�+��ֵ
    MotoYaw.PidPos.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPos.SetPoint, YAW_MAX_ANGLE, YAW_MIN_ANGLE);
    MotoYaw.PidSpeed.SetPoint = PID_Calc(&MotoYaw.PidPos, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeed, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    Gyro_Can1Send(MotoPitch.I_Set, 0);


}



float moto_pitch, moto_yaw, moto_pitch_init, moto_yaw_init;
float gyro_pitch, gyro_yaw, gyro_pitch_init, gyro_yaw_init;
float comb_pitch, comb_yaw;
float k_pitch = 0;
float k_yaw = 0;
int8_t init_comb_flag = 1;
/**
  * @brief  ����̨��̬������㣨������ֵ�͵��ֵ�Ļ����˲���
  * @param  None
  * @retval None
  */
static void Gimbal_GYRO_Cal(void)
{
    if (init_comb_flag)
    {
        //moto_pitch_init=PitchAngleOutPut();
        moto_pitch_init = 4810.0f;
        gyro_pitch_init = GyroPitchOutPut();
        //moto_yaw_init=YawAngleOutPut();
        moto_yaw_init = 1387.0f;
        gyro_yaw_init = GyroYawOutPut();
        init_comb_flag = 0;
    }

    moto_pitch = (PitchAngleOutPut() - moto_pitch_init) / 8192.0f * 360.0f;
    gyro_pitch = -(GyroPitchOutPut() - gyro_pitch_init);
    comb_pitch = k_pitch * gyro_pitch + (1 - k_pitch) * moto_pitch; //һ�׻����˲�

    moto_yaw = (YawAngleOutPut() - moto_yaw_init) / 8192 * 360.0f;
    gyro_yaw = -(GyroYawOutPut() - gyro_yaw_init);
    comb_yaw = k_yaw * gyro_yaw + (1 - k_yaw) * moto_yaw; //һ�׻����˲�
}

/**
  * @brief  ����̨��ת�Ƕ���λ
  * @param  None
  * @retval None
  */
static void Gimbal_Limit(void)
{
    PITCH_MAX_ANGLE =+20.0f; //������Ҫ��̫Զ���ᱻ����ĸߵص�ס
    PITCH_ZERO_POS = +0;
    PITCH_MIN_ANGLE =-35.0f;

    YAW_MAX_ANGLE = 60;
    YAW_ZERO_POS = 0;
    YAW_MIN_ANGLE = -60;

    //Ѳ�ߵķ�Χһ�����λ�ķ�ΧҪխ����Ϊ����Ĺ���㹻��
    PATROL_PITCH_MAX_ANGLE =+20.0f; //-3.0f;
    PATROL_PITCH_ZERO_POS = +0;
    PATROL_PITCH_MIN_ANGLE =-40.0f; //-30.0f;

    PATROL_YAW_MAX_ANGLE = 65;//364
    PATROL_YAW_ZERO_POS =  0;
    PATROL_YAW_MIN_ANGLE =-65;//-36;
}

/**
  * @brief  ��̨���pid��ʼ��
  * @param  None
  * @retval None
  */
static void PID_Gimbal_Init(void)
{
    MotoPitch.PidPos.P = -290.0f;//-550.0f;//-450.0f;//-500.0f;   //-450.0f
    MotoPitch.PidPos.I = -10.0f;//-1.2f;//-1.5f;     //-0.7f
    MotoPitch.PidPos.D = 0.0f;
    MotoPitch.PidPos.IMax = 120.0f;//1000.0f;

    MotoPitch.PidSpeed.P = -6000.0f;//-2500.0f;//-6500.0f;//-8000.0f; //-2500.0f
    MotoPitch.PidSpeed.I = -6.0f;//-10.0f;//-16.0f;//-5.0f;               //-16.0f
    MotoPitch.PidSpeed.D = 0.0f;                
    MotoPitch.PidSpeed.IMax = 500.0f;           //800.0f

//    MotoPitch.PidPosV.P = -300.0f;
//    MotoPitch.PidPosV.I = -0.4f;
//    MotoPitch.PidPosV.D = -2.0f; //1800.0f;
//    MotoPitch.PidPosV.IMax = 1200.0f;
    
    MotoPitch.PidPosV.P = -350.0f;//-380.0f;//-300.0f;//-280.0f;//-100.0f;//-200.0f; //-300.0f;
    MotoPitch.PidPosV.I = -4.0f;//-4.0f;//-2.8f;//-3.0f;//-1.5f;//-0.25f; //-5.0f;
    MotoPitch.PidPosV.D = 0.0f;//-2.0f; //1800.0f;
    MotoPitch.PidPosV.IMax = 120.0f;//200.0f;//300.0f; //50.0f;

    MotoPitch.PidSpeedV.P = -4500.0f;//-5000.0f;//-6000.0f; //-5000.0f;
    MotoPitch.PidSpeedV.I = -15.0f;//-1.5f;    //15
    MotoPitch.PidSpeedV.D = 0.0f;
    MotoPitch.PidSpeedV.IMax = 2000.0f;//500.0f; //2000.0f;

    //FUZZY_PID
		MotoPitch.PidPos_Fuzzy.Kp = -290.0f;
		MotoPitch.PidPos_Fuzzy.Ki = -10.0f;
		MotoPitch.PidPos_Fuzzy.Kd = 0.0f;
		MotoPitch.PidPos_Fuzzy.IMax = 120.0f;
		
		MotoPitch.PidSpeed_Fuzzy.Kp = -6000.0f;
		MotoPitch.PidSpeed_Fuzzy.Ki = -6.0f;
		MotoPitch.PidSpeed_Fuzzy.Kd = 0.0f;
		MotoPitch.PidSpeed_Fuzzy.IMax = 500.0f;
		
		MotoPitch.PidPosV_Fuzzy.Kp = -350.0f;//-350.0f;
    MotoPitch.PidPosV_Fuzzy.Ki = -3.5f;//-4.0f;//3.8f;
		MotoPitch.PidPosV_Fuzzy.Kd = 0.0f;
		MotoPitch.PidPosV_Fuzzy.IMax = 1500.0f;
		
		MotoPitch.PidSpeedV_Fuzzy.Kp = -5500.0f;//-4500.0f;
		MotoPitch.PidSpeedV_Fuzzy.Ki = -15.0f;
		MotoPitch.PidSpeedV_Fuzzy.Kd = 0.0f;
		MotoPitch.PidSpeedV_Fuzzy.IMax = 2000.0f;
     
    //    MotoPitch.PidPosV.P = 0.0f;/
    //    MotoPitch.PidPosV.I = 0.0f;
    //    MotoPitch.PidPosV.D = 0.0f;
    //    MotoPitch.PidPosV.IMax = 5.0f;

    //    MotoPitch.PidSpeedV.P = 0.0f;
    //    MotoPitch.PidSpeedV.I = 0.0f;
    //    MotoPitch.PidSpeedV.D = 0.0f;
    //    MotoPitch.PidSpeedV.IMax = 50.0f;

    //////////////////////////////////////////////////////////////
    MotoYaw.PidPos.P = -0.36f;//-0.33f;//0.35f;//0.65f; //0.5f//-0.36f;//-0.32f//-0.5f
    MotoYaw.PidPos.I = -0.0000f;//-0.0012f;//0.0008f; //0.0008f//-0.001f //-0.0004f//-0.005
    MotoYaw.PidPos.D = 0.0f;//3.8f;
    MotoYaw.PidPos.I_Limit = 0.5;
    MotoYaw.PidPos.IMax = 250.0f;//200.0f; //250

    MotoYaw.PidSpeed.P = -9500.0f;//-9000.0f;//7000.0f;//8000.0f; //7000.0f;//-0.36f//-9500.0f;
    MotoYaw.PidSpeed.I = -10.0f;//10.0f;//2.0f;    //10.0f;//������˸�����
    MotoYaw.PidSpeed.D = 0.0f;    
    MotoYaw.PidSpeed.I_Limit = 1;
    MotoYaw.PidSpeed.IMax = 1000.0f;

    MotoYaw.PidPosV.P = -0.4f;//-0.36f;//0.01f;//0.5f;//0.01f;//-0.05f;//-0.45f
    MotoYaw.PidPosV.I = 0.0f;//0.002f;//-0.0015f;//0.0008f;//-0.005f;
    MotoYaw.PidPosV.D = 0.0f;//2.0f;//3.8f;//2.0f;
    MotoYaw.PidPosV.I_Limit = 0.0f;
    MotoYaw.PidPosV.IMax =100.0f; //5000.0f;//0.0f;//400.0f

    MotoYaw.PidSpeedV.P = -8000.0f; //8000.0f
    MotoYaw.PidSpeedV.I = -2.0f;//-15.0f;//2.0f;
    MotoYaw.PidSpeedV.D = 0.0f;
    MotoYaw.PidSpeedV.I_Limit = 0.0f;
    MotoYaw.PidSpeedV.IMax = 1500.0f;
		
		//FUZZY_PID
    MotoYaw.PidPos_Fuzzy.Kp = -0.36f;
		MotoYaw.PidPos_Fuzzy.Ki = -0.001f;
		MotoYaw.PidPos_Fuzzy.Kd = 0.0f;
		MotoYaw.PidPos_Fuzzy.IMax = 250.0f;
		
		MotoYaw.PidSpeed_Fuzzy.Kp = -9500.0f;
		MotoYaw.PidSpeed_Fuzzy.Ki = -10.0f;
		MotoYaw.PidSpeed_Fuzzy.Kd = 0.0f;
		MotoYaw.PidSpeed_Fuzzy.IMax = 300.0f;
		
		MotoYaw.PidPosV_Fuzzy.Kp = -0.34f;
    MotoYaw.PidPosV_Fuzzy.Ki = -0.003f;//0.0f;
		MotoYaw.PidPosV_Fuzzy.Kd = 0.0f;
		MotoYaw.PidPosV_Fuzzy.IMax = 100.0f;
		
	  MotoYaw.PidSpeedV_Fuzzy.Kp =  -8000.0f;
		MotoYaw.PidSpeedV_Fuzzy.Ki = -2.0f;
		MotoYaw.PidSpeedV_Fuzzy.Kd = 0.0f;
		MotoYaw.PidSpeedV_Fuzzy.IMax = 1500.0f;
		
		

    //    MotoYaw.PidPosV.P = 0.0f;
    //    MotoYaw.PidPosV.I = 0.0f;
    //    MotoYaw.PidPosV.D = 0.0f;
    //    MotoYaw.PidPosV.I_Limit = 0.0f;
    //    MotoYaw.PidPosV.IMax = 0.0f;

    //    MotoYaw.PidSpeedV.P = 0.0f;
    //    MotoYaw.PidSpeedV.I = 0.0f;
    //    MotoYaw.PidSpeedV.D = 0.0f;
    //    MotoYaw.PidSpeedV.I_Limit = 0.0f;
    //    MotoYaw.PidSpeedV.IMax = 0.0f;
    //
    ////////////////////////////////////////////////////////////////////////
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
