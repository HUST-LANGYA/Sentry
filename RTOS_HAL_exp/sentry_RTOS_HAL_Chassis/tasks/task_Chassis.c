#include "main.h"
#include "task_Chassis.h"


_3508_motor_t motor_chassis;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;

uint8_t chassisFlag_Init=0;
int16_t test_init_flag=0;

////���ʲ���
//float PowerNow;
//PID_Typedef PowerLimit;
//float PowerLimit_k=1;

static void Chassis_PC_Act(void);
static void Chassis_RC_Act(void);
static void Chassis_SLEEP_Act(void);
static void Chassis_DEBUG_Act(void);
static void Chassis_PC_PID_Cal(void);
static void Chassis_RC_PID_Cal(void);
inline static void Chassis_SLEEP_PID_Cal(void);
static void Chassis_DEBUG_PID_Cal(void);
static void PID_Chassis_Init(void);

void task_Chassis(void* parameter)
{
    //��������ʱִ��һ�θ�λ
    PID_Chassis_Init(); //��һ��ִ�е�������ʱ �����̵�PID�������г�ʼ��
    
    while (1)
    {
        if (Sentry_State.Chassis_Mode == Chassis_PC) Chassis_PC_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_RC) Chassis_RC_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_SLEEP) Chassis_SLEEP_Act();
        /*else if (Sentry_State.Chassis_Mode == Chassis_DEBUG)*/else Chassis_DEBUG_Act();
        vTaskDelay(1);
    }
}


/**
  * @brief  ң��ģʽ�µ��̵��˶�
  * @param  None
  * @retval None
  */
static void Chassis_RC_Act(void)
{
    static float kChassis = 4.0f;   //���ҳ���Ϊ����ϵ����
    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=0;
        chassisFlag_Init=1; 
    }
    
    RC_Ctl_t RC_Ctl = getRCData();

    //����+����̨ң�أ� ��ҡ�˿�����̨����ҡ�˿��Ƶ��̺ͷ���
    if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_RC /* && Sentry_State.ChassisMode & Chassis_RC*/)
        motor_chassis.pid.SetPoint=kChassis*(RC_Ctl.rc.ch0-1024/*1043*/); 
    
    //����+����̨ң�أ� ��ҡ�˿�����̨����ҡ�˿��Ƶ��̺ͷ���
    if(Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_RC /*&& Sentry_State.ChassisMode & Chassis_RC*/)
        motor_chassis.pid.SetPoint=kChassis*(RC_Ctl.rc.ch2-1024/*1002*/); 

    Chassis_RC_PID_Cal();
}

/**
  * @brief  PCģʽ�µ��̵��˶�
  * @param  None
  * @retval None
  */
static void Chassis_PC_Act(void)
{
//    extern uint8_t startFlag, destinationFlag;
//    extern int32_t movingTick;
//    extern uint8_t reverseFlag;

//    if(chassisFlag_Init == 0)
//    {
//        motor_chassis.pid.SetPoint=0;
//        chassisFlag_Init=1; 
//    }
//    
//    //PCģʽ�µ����Զ�����������һ������
//    //ע��ֻ��һ����翪�صĽӿ��Ǻõ�
//    
//    extern Robot Sentry;
//    
//    if(Sentry.MovingDir == FORWARD) //�����ƶ�ʱ
//    {
//        startFlag = 1;//��ʼ�ܶ�
//        movingTick++;
//        motor_chassis.pid.SetPoint= -2000; //����ܹ���������
//    }
//    else if(Sentry.MovingDir == BACKWARD) //�����ƶ�ʱ
//    {
//        startFlag = 1;//��ʼ�ܶ�
//        movingTick--;
//        motor_chassis.pid.SetPoint= 2000; //����ܹ���������
//    }
    
    Chassis_PC_PID_Cal();
}


static void Chassis_SLEEP_Act(void)
{
    //����Ҫ����һЩflag������
    Chassis_SLEEP_PID_Cal();
}

int16_t debug_SetPoint = 0;
static void Chassis_DEBUG_Act(void)
{   
    motor_chassis.pid.SetPoint = debug_SetPoint;
    Chassis_DEBUG_PID_Cal();
}

static void Chassis_RC_PID_Cal(void)
{
    float Chassis_Iset;
    
    //����3508 �޷�+��ֵ
    motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint,ChassisCurrentLimit , -ChassisCurrentLimit);
    Chassis_Iset = PID_Calc( &motor_chassis.pid, motor_chassis.real_speed);
//    PowerLimit_k += PID_Calc(&PowerLimit, PowerNow) / PowerLimit.SetPoint;
//    PowerLimit_k = LIMIT_MAX_MIN(PowerLimit_k, 1, 0);
    Chassis_Iset = LIMIT_MAX_MIN(Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    //  Chassis_Iset = PowerLimit_k * Chassis_Iset;
    
    Chassis_CAN1Send(Chassis_Iset);
}

static void Chassis_PC_PID_Cal(void)
{
    ;
}

inline static void Chassis_SLEEP_PID_Cal(void)
{
    Chassis_CAN1Send(0);
}

int16_t debug_Chassis_Iset;
static void Chassis_DEBUG_PID_Cal(void)
{
    extern INA260 INA260_1;
    debug_Chassis_Iset = (int16_t)PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
    debug_Chassis_Iset = LIMIT_MAX_MIN(debug_Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    
    Chassis_CAN1Send(debug_Chassis_Iset);
}

/**
  * @brief  ����PID��ʼ��
  * @param  None
  * @retval None
  */
static void PID_Chassis_Init(void)
{ 
    motor_chassis.pid.P = 0.8f;
	motor_chassis.pid.I = 0.6f;
	motor_chassis.pid.D = 0.7f;
	motor_chassis.pid.IMax = 1000.0f;
	motor_chassis.zerocheck.CountCycle = 8191;
	motor_chassis.zerocheck.LastValue = motor_chassis.angle_abs;
	motor_chassis.zerocheck.Circle = 0;
	
//	PowerLimit.SetPoint = 20;
//	PowerLimit.P = 0.01f;
//	PowerLimit.I = 0.01f;
//	PowerLimit.D = 0.03f;
//	PowerLimit.IMax = 0.5f;
}

void PID_Chassis_ClearError(void)
{  //����˵�ǿ��Բ�����������ģ���Ŀǰ����һ�������Է���һ
    motor_chassis.pid.dError = 0.0f;
    motor_chassis.pid.PreError = 0.0f;
    motor_chassis.pid.SumError = 0.0f;
}


/**
 *  @brief  ���̵�����
 *  @param  None
 *  @retval None
 */
//void Disconnect_Check(void)
//{
//	int32_t time = GetSysCnt();
//	if(time - block_disconnect.Chassis_Last_Cnt>=10)
//		block_disconnect.Chassis_Sleep_Cnt++;
//	if(block_disconnect.Chassis_Sleep_Cnt>=500)
//		block_disconnect.is_Chassis_Sleep = 1;	
//    
//    /*�����ĵ�����Ҳ�ӵ������Ժ�*/
//}

///**
//  * @brief  ���ص��̵Ľṹ��ָ��
//  * @param  None
//  * @retval None
//  */
//_3508_motor_t* Get_MotorChassis(void)
//{
//	return &motor_chassis;
//}
//
///**
//  * @brief  ����setpoint���ݵ�ָ��
//  * @param  None
//  * @retval None
//  */
//int16_t* GetChassisSetspeed(void)
//{
//	return &ChassisSetSpeed;
//}


