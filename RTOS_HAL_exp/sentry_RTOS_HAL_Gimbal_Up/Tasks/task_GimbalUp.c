#include "main.h"
#include "task_GimbalUp.h"

//��������̨��ʱδװ�����ǣ����õ�����ص�ת�ӽǶȺ�ת���ٶ���PID
#define PITCH_GYRO_OFFSET (830.0f/8192*360.0f)//830��ˮƽʱ��pitchת�ӻ�е�Ƕ�
#define YAW_GYRO_OFFSET   (6712.0f/8192*360.0f) //6712��ָ������ʱ��yawת�ӻ�е�Ƕ�   //��Ϊyawң������������ʽ�ģ������ƫ��ֵֻ���ڳ�ʼ������

gimbal_motor_t MotoPitch, MotoYaw;
float RCPitch_Scal = 0.08f, RCYaw_Scal = 0.0002f; //ң������������ƵķŴ����

int32_t PITCH_MAX_ANGLE, PITCH_MIN_ANGLE, PITCH_ZERO_POS; //pitch�޷��Ȳ���
int32_t YAW_MAX_ANGLE, YAW_MIN_ANGLE, YAW_ZERO_POS;       //yaw�޷��Ȳ���

float Pitch_Actual, Yaw_Actual;
float PC_PitchSet, PC_YawSet;

extern int16_t Gimbal_init_flag;
extern gyro_Typedef Gyro_White;
extern State_t Sentry_State;


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
        if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_RC) Gimbal_RC_Act();
        else if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_PC) Gimbal_PC_Act();
        else if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_SLEEP) Gimbal_SLEEP_Act();
         else if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_DEBUG) Gimbal_DEBUG_Act();
        //������Լ�����չ��������ģʽ��Ȼ��Ҫ�ǵ���ͷ�ļ�����Ѷ�Ӧ�ĺ궨�����
        
        vTaskDelay(1); 
    }
}

float Test_Pitch_Pos = 0, Test_Yaw_Pos = 0;
static void Gimbal_DEBUG_Act(void)
{
    RC_Ctl_t RC_Ctl = getRCData();

    Yaw_Actual = CombYawOutput();
    Pitch_Actual = CombPitchOutput();

    if (Gimbal_init_flag == 0)
    { 
        MotoYaw.PidPos.SetPoint = 0 + YAW_GYRO_OFFSET;     //Yaw_Actual;	  //MotoPitch.Angle_Inc;
        MotoPitch.PidPos.SetPoint = 0 + PITCH_GYRO_OFFSET;   //Pitch_Actual; // Gyro_White.YAW_INC;
        Gimbal_init_flag = 1;
    }

    MotoYaw.PidPos.SetPoint = Test_Yaw_Pos;//+ YAW_GYRO_OFFSET;       //RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
    MotoPitch.PidPos.SetPoint = Test_Pitch_Pos+ PITCH_GYRO_OFFSET;   //RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;
   

    Gimbal_RC_PID_Cal();
}


/**
  * @brief  ң����ģʽ�£���������̨����״̬
  * @param  ң����ֵ
  * @retval None
  */
static void Gimbal_RC_Act(void)
{
    extern RC_Ctl_t RC_Ctl ;//= getRCData();

    Yaw_Actual = CombYawOutput();
    Pitch_Actual = CombPitchOutput();

    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPos.SetPoint = 0 + YAW_GYRO_OFFSET;   //Yaw_Actual;	  //MotoPitch.Angle_Inc;
        MotoPitch.PidPos.SetPoint = 0 + PITCH_GYRO_OFFSET; //Pitch_Actual; // Gyro_White.YAW_INC;
        Gimbal_init_flag = 1;
    }

    MotoYaw.PidPos.SetPoint -= RCYaw_Scal * (RC_Ctl.rc.ch2 - 1024);
    MotoPitch.PidPos.SetPoint = RCPitch_Scal * (RC_Ctl.rc.ch3 - 1024) + PITCH_GYRO_OFFSET;

    Gimbal_RC_PID_Cal();
}

/**
  * @brief  PCģʽ�£���������̨����״̬
  * @param  None
  * @retval None
  */
float TestV_Pitch = 0, TestV_Yaw = 0;
int16_t pc_flag_pitch_testV = 0, pc_flag_yaw_testV = 0;
//FilterFIR filterPC;//�˲�������
//CoeFIR testCOE; // = {  0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                    0.05,0.05,0.05,0.05,0.05,
//                 };// �������ľ���������matlab���꣨������ô��ȥ��LZCȥ��
static void Gimbal_PC_Act(void)
{
    extern float PC_PitchSet, PC_YawSet;
    Yaw_Actual = CombYawOutput();
    Pitch_Actual = CombPitchOutput();

    if (Gimbal_init_flag == 0)
    {
        MotoYaw.PidPosV.SetPoint = 0;     //Yaw_Actual;//MotoPitch.Angle_Inc;
        MotoPitch.PidPosV.SetPoint = -30; //Pitch_Actual;// Gyro_White.YAW_INC;
                                          //		filterPC=FIRInit(testCOE);//��ʼ���˲�����
                                          //        for(int i=0;i<20;i++)
                                          //        {
                                          //            testCOE.h[i] = 0.05f;
                                          //        }

        Gimbal_init_flag = 1;
    }

    if (pc_flag_pitch_testV)
        MotoPitch.PidPosV.SetPoint = TestV_Pitch;
    MotoPitch.PidPosV.SetPoint = PC_PitchSet; //0;//PCPitch_Scal*(remote.ch[1] - 1024);

    if (pc_flag_yaw_testV)
        MotoYaw.PidPosV.SetPoint = TestV_Yaw;
    MotoYaw.PidPosV.SetPoint = PC_YawSet; //0;//PCYaw_Scal*(remote.ch[0]-1043);

   // PC_MsgSend(PC_SEND_GYRO);

    Gimbal_PC_PID_Cal();
}

static void Gimbal_SLEEP_Act(void)
{
    //! Ӧ�û���Ҫ����һ��flag֮��Ķ���������
    Gimbal_SLEEP_PID_Cal();
}

/**
  * @brief  ����̨��ת�Ƕ���λ
  * @param  None
  * @retval None
  */
static void Gimbal_Limit(void)
{
//    static uint8_t Origin_PitchAngle_Flag, Origin_YawAngle_Flag;

    /************************����ϵ�λ���޷���ʼ��*******************************/
//    if (Origin_PitchAngle_Flag == 0)
//    {

        PITCH_MAX_ANGLE = PITCH_GYRO_OFFSET + 25;
        PITCH_ZERO_POS = PITCH_GYRO_OFFSET + 0;
        PITCH_MIN_ANGLE = PITCH_GYRO_OFFSET - 25;

        YAW_MAX_ANGLE = YAW_GYRO_OFFSET + 65;
        YAW_ZERO_POS  = YAW_GYRO_OFFSET + 0;
        YAW_MIN_ANGLE = YAW_GYRO_OFFSET - 65;

//        Origin_PitchAngle_Flag = 1;
//    }
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

//        if (GetSysCnt() % 3 == 0)
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
        MotoYaw.PidPosV.SetPoint = LIMIT_MAX_MIN(MotoYaw.PidPosV.SetPoint, 0, 0 /*YAW_MAX_ANGLE,YAW_MIN_ANGLE*/);
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
        moto_pitch_init=PitchAngleOutPut();
        //moto_pitch_init = 5780.0f;
        gyro_pitch_init = GyroPitchOutPut();
        moto_yaw_init=YawAngleOutPut();
        //moto_yaw_init = 2700.0f;
        gyro_yaw_init = GyroYawOutPut();
        init_comb_flag = 0;
    }

    moto_pitch = (PitchAngleOutPut() - moto_pitch_init) / 8192 * 360;
    gyro_pitch = -(GyroPitchOutPut() - gyro_pitch_init);
    comb_pitch = k_pitch * gyro_pitch + (1 - k_pitch) * moto_pitch; //һ�׻����˲�

    moto_yaw = (YawAngleOutPut() - moto_yaw_init) / 8192 * 360;
    gyro_yaw = -(GyroYawOutPut() - gyro_yaw_init);
    comb_yaw = k_yaw * gyro_yaw + (1 - k_yaw) * moto_yaw; //һ�׻����˲�
}

/**
  * @brief  ��̨���pid��ʼ��
  * @param  None
  * @retval None
  */
static void PID_Gimbal_Init(void)
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

    MotoPitch.PidPos.P = 400.0f;
    MotoPitch.PidPos.I = 100;
    MotoPitch.PidPos.D = 0;
    MotoPitch.PidPos.IMax = 20.0f;

    MotoPitch.PidSpeed.P = 400.0f;
    MotoPitch.PidSpeed.I = 30.0f;
    MotoPitch.PidSpeed.D = 0;
    MotoPitch.PidSpeed.IMax = 50.0f;

    MotoPitch.PidPosV.P = -55.0f;
    MotoPitch.PidPosV.I = 0.0f;
    MotoPitch.PidPosV.D = 0.0f;
    MotoPitch.PidPosV.IMax = 10.0f;

    MotoPitch.PidSpeedV.P = 1600.0f;
    MotoPitch.PidSpeedV.I = 15.0f;
    MotoPitch.PidSpeedV.D = 0.0f;
    MotoPitch.PidSpeedV.IMax = 15.0f;

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

    MotoYaw.PidPos.P = 40.0f;
    MotoYaw.PidPos.I = 0.0f;
    MotoYaw.PidPos.D = 200.0f;
    MotoYaw.PidPos.I_Limit = 0.5f;
    MotoYaw.PidPos.IMax = 10.0f;

    MotoYaw.PidSpeed.P = 60;
    MotoYaw.PidSpeed.I = 0;
    MotoYaw.PidSpeed.D = 120.0f;
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
    MotoYaw.PidPosV.P = -0.1f;
    MotoYaw.PidPosV.I = 0.0f;
    MotoYaw.PidPosV.D = 0.0f;
    MotoYaw.PidPosV.I_Limit = 0.5f;
    MotoYaw.PidPosV.IMax = 10.0f;

    MotoYaw.PidSpeedV.P = -100.0f;
    MotoYaw.PidSpeedV.I = 0.0f;
    MotoYaw.PidSpeedV.D = 5.0f;
    MotoYaw.PidSpeedV.I_Limit = 1.0f;
    MotoYaw.PidSpeedV.IMax = 5.0f;

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