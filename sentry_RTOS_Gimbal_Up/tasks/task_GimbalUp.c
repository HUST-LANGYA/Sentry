#include "main.h"
#include "task_GimbalUp.h"

//��������̨��ʱδװ�����ǣ����õ�����ص�ת�ӽǶȺ�ת���ٶ���PID
#define PITCH_GYRO_OFFSET  330.0f//(7600.0f/8192*360.0f)  //7600��ˮƽʱ��pitchת�ӻ�е�Ƕ�
#define YAW_GYRO_OFFSET    90.0f//(2090.0f/8192*360.0f)  //2090��ָ����ǰ��ʱ��yawת�ӻ�е�Ƕ�   //��Ϊyawң������������ʽ�ģ������ƫ��ֵֻ���ڳ�ʼ������


gimbal_motor_t MotoPitch, MotoYaw;
float RCPitch_Scal = 0.05f, RCYaw_Scal = 0.0002f; //ң������������ƵķŴ����

int32_t PITCH_MAX_ANGLE, PITCH_MIN_ANGLE, PITCH_ZERO_POS; //pitch�޷��Ȳ���
int32_t YAW_MAX_ANGLE, YAW_MIN_ANGLE, YAW_ZERO_POS;       //yaw�޷��Ȳ���

float Pitch_Actual, Yaw_Actual;
float PC_PitchSet, PC_YawSet;

extern int16_t Gimbal_init_flag;
extern gyro_Typedef Gyro_White;
extern State_t Sentry_State;


//�ں˶�����
//extern QueueHandle_t anglePitch_MsgQueue;//��ǣ���̬������Ϣ���о��

/**
 * @brief ��̨��������
 * @param ��
 * @retval ��
 */
void task_GimbalUp(void *parameter)
{
    //��������ʱִ��һ�θ�λ
    PID_Gimbal_Init(); //��һ��ִ����̨������ ����̨��PID�������г�ʼ��
    Gimbal_Limit();//�޷�У��
    while (1)
    {
        Gimbal_Limit();
        Gimbal_GYRO_Cal();
        
        Yaw_Actual = CombYawOutput();
        Pitch_Actual = CombPitchOutput();
          
        if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_RC) Gimbal_RC_Act();
        else if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_PC) Gimbal_PC_Act();
        else if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_SLEEP) Gimbal_SLEEP_Act();
         else if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_DEBUG) Gimbal_DEBUG_Act();
        //������Լ�����չ��������ģʽ��Ȼ��Ҫ�ǵ���ͷ�ļ�����Ѷ�Ӧ�ĺ궨�����
        
        vTaskDelay(1); 
    }
}

float Test_Pitch_Pos = 0, Test_Yaw_Pos = 0;
float Test_Pitch_Veloc = 0, Test_Yaw_Veloc = 0;
static void Gimbal_DEBUG_Act(void)
{
    //RC_Ctl_t RC_Ctl = getRCData();

    MotoYaw.PidPos.SetPoint = Test_Yaw_Pos;//+ YAW_GYRO_OFFSET;       //RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
    MotoPitch.PidPos.SetPoint = Test_Pitch_Pos/*+ PITCH_GYRO_OFFSET*/;   //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;
//   MotoPitch.PidSpeed.SetPoint = Test_Pitch_Veloc/*+ PITCH_GYRO_OFFSET*/;   //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;
//   MotoYaw.PidSpeed.SetPoint = Test_Yaw_Veloc;
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

    Gyro_Can2Send(MotoPitch.I_Set, MotoYaw.I_Set);
}


/**
  * @brief  ң����ģʽ�£���������̨����״̬
  * @param  ң����ֵ
  * @retval None
  */
static void Gimbal_RC_Act(void)
{
    extern RC_Ctl_t RC_Ctl ;//= getRCData();

    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPos.SetPoint = 0 ;   //Yaw_Actual;	  //MotoPitch.Angle_Inc;
        MotoPitch.PidPos.SetPoint = 0 ; //Pitch_Actual; // Gyro_White.YAW_INC;
        Gimbal_init_flag = 1;
    }

    MotoYaw.PidPos.SetPoint += RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
    MotoPitch.PidPos.SetPoint = RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) ;

    Gimbal_RC_PID_Cal();
}

/**
  * @brief  PCģʽ�£���������̨����״̬
  * @param  None
  * @retval None
  */
float TestV_Pitch = 0, TestV_Yaw = 0;
//int16_t pc_flag_pitch_testV = 0, pc_flag_yaw_testV = 0;
float patrol_step_pitch = 0.015f,patrol_step_yaw = 0.001f;
uint8_t patrol_dir_yaw = 0,patrol_dir_pitch = 0;  //��־���ܵ����ǰѲ�ߵķ���0��ʾ--��1��ʾ++
float patrol_set_pitch = 0.0f,patrol_set_yaw = 0.0f;  //Ѳ���趨ֵ����
int32_t PATROL_PITCH_MAX_ANGLE, PATROL_PITCH_MIN_ANGLE, PATROL_PITCH_ZERO_POS; //pitch�޷��Ȳ���
int32_t PATROL_YAW_MAX_ANGLE, PATROL_YAW_MIN_ANGLE, PATROL_YAW_ZERO_POS;       //yaw�޷��Ȳ���
//FilterFIR filterPC;//�˲�������
//CoeFIR testCOE; // = {  0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                 };// �������ľ���������matlab���꣨������ô��ȥ��LZCȥ��
static void Gimbal_PC_Act(void)
{
    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPosV.SetPoint = 0;     //Yaw_Actual;//MotoPitch.Angle_Inc;
        MotoPitch.PidPosV.SetPoint = 0; //Pitch_Actual;// Gyro_White.YAW_INC;
        patrol_set_pitch = 0;
        patrol_set_yaw = 0;
                                          //		filterPC=FIRInit(testCOE);//��ʼ���˲�����
                                          //        for(int i=0;i<20;i++)
                                          //        {
                                          //            testCOE.h[i] = 0.05f;
                                          //        }

        Gimbal_init_flag = 1;
    }
    
    extern uint8_t armor_state;
    
    if(armor_state == ARMOR_AIMED)
    {//�������׽��װ�ף��������׼ģʽ����̨��̬����CV��Ϣ������
        //extern float PC_PitchSet, PC_YawSet;
        extern float aim_yaw, aim_pitch;
        MotoPitch.PidPosV.SetPoint = aim_pitch;//PC_PitchSet; //0;
        patrol_set_pitch = aim_pitch;
        patrol_set_pitch = LIMIT_MAX_MIN(patrol_set_pitch,PATROL_PITCH_MAX_ANGLE,PATROL_PITCH_MIN_ANGLE);
        MotoYaw.PidPosV.SetPoint = aim_yaw;//PC_YawSet; //0;
        patrol_set_yaw = aim_yaw;
        patrol_set_yaw = LIMIT_MAX_MIN(patrol_set_yaw,PATROL_YAW_MAX_ANGLE,PATROL_YAW_MIN_ANGLE);
    }
    else //if(armor_state == ARMOR_NO_AIM)
    {//�����δ��׽��װ�ף������Ѳ��ģʽ����̨��̬��һ���ض��Ĳ���������
        if((patrol_set_pitch + patrol_step_pitch >= PATROL_PITCH_MAX_ANGLE) &&  patrol_dir_pitch) patrol_dir_pitch = 0; //��������޷��� 
        if((patrol_set_pitch - patrol_step_pitch <= PATROL_PITCH_MIN_ANGLE) && !patrol_dir_pitch) patrol_dir_pitch = 1; //��������޷��� 
        if((patrol_set_yaw   + patrol_step_yaw   >=  PATROL_YAW_MAX_ANGLE ) &&    patrol_dir_yaw) patrol_dir_yaw   = 0; //��������޷��� 
        if((patrol_set_yaw   - patrol_step_yaw   <=  PATROL_YAW_MIN_ANGLE ) &&   !patrol_dir_yaw) patrol_dir_yaw   = 1; //��������޷��� 
        patrol_set_pitch += (patrol_dir_pitch==1)?(+patrol_step_pitch):(-patrol_step_pitch);
        patrol_set_yaw   += (patrol_dir_yaw == 1)?( +patrol_step_yaw ):(-patrol_step_yaw);
        
        MotoPitch.PidPosV.SetPoint = patrol_set_pitch;
        MotoYaw.PidPosV.SetPoint   = patrol_set_yaw;
//        MotoPitch.PidPosV.SetPoint = 0;
//        MotoYaw.PidPosV.SetPoint = 0;
    }
    
    Gimbal_PC_PID_Cal();
}

static void Gimbal_SLEEP_Act(void)
{
    //! Ӧ�û���Ҫ����һ��flag֮��Ķ���������
    Gimbal_SLEEP_PID_Cal();
}


float Limit = 6.0f;
int16_t testYawI = 0, testPitchI = 0;

/**
  * @brief  ����̨PID�������
  * @param  None
  * @retval None
  */
//? ����Ҫ��Ҫ�Ѳ�ͬģʽ��PID_Cal�ֿ���д�������Լ��ټ����ж�
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

    Gyro_Can2Send(MotoPitch.I_Set, MotoYaw.I_Set);
}
static void Gimbal_PC_PID_Cal(void)
{
     float fsend;

    //PITCH�޷�+��ֵ
    MotoPitch.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoPitch.PidPosV.SetPoint, PITCH_MAX_ANGLE, PITCH_MIN_ANGLE);
    MotoPitch.PidSpeedV.SetPoint = PID_Calc(&MotoPitch.PidPosV, Pitch_Actual, 0) / 1000.0f;
    fsend = PID_Calc(&MotoPitch.PidSpeedV, Gyro_White.GY, 0);
    MotoPitch.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitPitch, -gyroLimitPitch);

    //YAW��ֵ
    MotoYaw.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPosV.SetPoint, YAW_MAX_ANGLE,YAW_MIN_ANGLE);
    MotoYaw.PidSpeedV.SetPoint = PID_Calc(&MotoYaw.PidPosV, Yaw_Actual, 0);
    fsend = PID_Calc(&MotoYaw.PidSpeedV, Gyro_White.GZ, 0);
    MotoYaw.I_Set = LIMIT_MAX_MIN(fsend, gyroLimitYaw, -gyroLimitYaw);

    //if (GetSysCnt() % 3 == 0)
    //Gyro_Can2Send(MotoPitch.I_Set,0);
        Gyro_Can2Send(MotoPitch.I_Set, MotoYaw.I_Set);
}
/*inline*/ static void Gimbal_SLEEP_PID_Cal(void)
{
    Gyro_Can2Send(0,0);
}


float moto_pitch, moto_yaw, moto_pitch_init, moto_yaw_init;
float gyro_pitch, gyro_yaw, gyro_pitch_init, gyro_yaw_init;
float comb_pitch, comb_yaw;
float k_pitch = 0;
float k_yaw = 0; //0.02;
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
        moto_pitch_init = 7400.0f;
        gyro_pitch_init = GyroPitchOutPut();
        //moto_yaw_init=YawAngleOutPut();
        moto_yaw_init = 2090.0f;
        gyro_yaw_init = GyroYawOutPut();
        init_comb_flag = 0;
    }

    moto_pitch = (PitchAngleOutPut() - moto_pitch_init) / 8192.0f * 360.0f;
    gyro_pitch = -(GyroPitchOutPut() - gyro_pitch_init);
    comb_pitch = k_pitch * gyro_pitch + (1 - k_pitch) * moto_pitch; //һ�׻����˲�

    moto_yaw = (YawAngleOutPut() - moto_yaw_init) / 8192 * 360;
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
        PITCH_MAX_ANGLE = /*PITCH_GYRO_OFFSET +*/ +0;//ʡ������Ҫɨ���Ƕ�
        PITCH_ZERO_POS = /*PITCH_GYRO_OFFSET */+ 0;
        PITCH_MIN_ANGLE = /*PITCH_GYRO_OFFSET -*/ -36;//5+Yaw_Actual/3.0f;  //�и�С���⣬����yaw����ֵ�ϴ��ʱ��pitch��ͷ45���򵽲��˰壬���������ø�������һ��ʵʱ��λֵ
        
        YAW_MAX_ANGLE = /*YAW_GYRO_OFFSET +*/ 40;
        YAW_ZERO_POS  = /*YAW_GYRO_OFFSET + */ 0;
        YAW_MIN_ANGLE = /*YAW_GYRO_OFFSET */- 40;


        //Ѳ�ߵķ�Χһ�����λ�ķ�ΧҪխ����Ϊ����Ĺ���㹻��
        PATROL_PITCH_MAX_ANGLE = /*PITCH_GYRO_OFFSET +*/ 0;//ʡ������Ҫɨ���Ƕ�
        PATROL_PITCH_ZERO_POS = /*PITCH_GYRO_OFFSET */+ 0;
        PATROL_PITCH_MIN_ANGLE = /*PITCH_GYRO_OFFSET -*/ -30;//5+Yaw_Actual/3.0f;  //�и�С���⣬����yaw����ֵ�ϴ��ʱ��pitch��ͷ45���򵽲��˰壬���������ø�������һ��ʵʱ��λֵ
        
        PATROL_YAW_MAX_ANGLE = /*YAW_GYRO_OFFSET +*/ 30;
        PATROL_YAW_ZERO_POS  = /*YAW_GYRO_OFFSET + */ 0;
        PATROL_YAW_MIN_ANGLE = /*YAW_GYRO_OFFSET */- 30;
}


/**
  * @brief  ��̨���pid��ʼ��
  * @param  None
  * @retval None
  */
static void PID_Gimbal_Init(void)
{
    MotoPitch.PidPos.P =356.0f;
    MotoPitch.PidPos.I =0.0f;
    MotoPitch.PidPos.D =1245.0f;
    MotoPitch.PidPos.IMax = 10.0f;

    MotoPitch.PidSpeed.P =5000.0f;
    MotoPitch.PidSpeed.I =165.0f;
    MotoPitch.PidSpeed.D =0.0f;
    MotoPitch.PidSpeed.IMax = 50.0f;

//    MotoPitch.PidPos.P = 400.0f;
//    MotoPitch.PidPos.I = 100;
//    MotoPitch.PidPos.D = 0;
//    MotoPitch.PidPos.IMax = 50.0f;

//    MotoPitch.PidSpeed.P = 400.0f;
//    MotoPitch.PidSpeed.I = 30.0f;
//    MotoPitch.PidSpeed.D = 0;
//    MotoPitch.PidSpeed.IMax = 50.0f;

    MotoPitch.PidPosV.P = 350.0f;//356.0f;
    MotoPitch.PidPosV.I = 180.0f;
    MotoPitch.PidPosV.D = 1800.0f;//1245.0f;
    MotoPitch.PidPosV.IMax = 5.0f;

    MotoPitch.PidSpeedV.P = 3000.0f;//5000.0f;
    MotoPitch.PidSpeedV.I = 0.0f;//200.0f;//165.0f;
    MotoPitch.PidSpeedV.D = 0.0f;
    MotoPitch.PidSpeedV.IMax = 50.0f;



//////////////////////////////////////////////////////////////
    MotoYaw.PidPos.P = 0;
    MotoYaw.PidPos.I =0;
    MotoYaw.PidPos.D = 0;
    MotoYaw.PidPos.I_Limit = 0.5;
    MotoYaw.PidPos.IMax = 10.0f;

    MotoYaw.PidSpeed.P =8000.0f;
    MotoYaw.PidSpeed.I =0;
    MotoYaw.PidSpeed.D = 1000.0f;
    MotoYaw.PidSpeed.I_Limit = 1;
    MotoYaw.PidSpeed.IMax = 50.0f;

//    MotoYaw.PidPos.P = 40.0f;
//    MotoYaw.PidPos.I = 0.0f;
//    MotoYaw.PidPos.D = 200.0f;
//    MotoYaw.PidPos.I_Limit = 0.5f;
//    MotoYaw.PidPos.IMax = 10.0f;

//    MotoYaw.PidSpeed.P = 60;
//    MotoYaw.PidSpeed.I = 0;
//    MotoYaw.PidSpeed.D = 120.0f;
//    MotoYaw.PidSpeed.I_Limit = 1;
//    MotoYaw.PidSpeed.IMax = 500;


    MotoYaw.PidPosV.P = 0.25f;
    MotoYaw.PidPosV.I = 0.0f;
    MotoYaw.PidPosV.D = 0.0f;
    MotoYaw.PidPosV.I_Limit = 0.0f;
    MotoYaw.PidPosV.IMax = 0.0f;

    MotoYaw.PidSpeedV.P = 8000.0f;
    MotoYaw.PidSpeedV.I = 0.0f;
    MotoYaw.PidSpeedV.D = 3000.0f;
    MotoYaw.PidSpeedV.I_Limit = 0.0f;
    MotoYaw.PidSpeedV.IMax = 0.0f;
    
    
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
