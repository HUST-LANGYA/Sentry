#include "main.h"
#include "task_Chassis.h"


_3508_motor_t motor_chassis;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;

uint8_t chassisFlag_Init=0;
int16_t test_init_flag=0;

////���ʲ���
float PowerNow;
PID_Typedef PowerLimit;
float PowerLimit_k=0.8;



int16_t encoder_veloc = 0;
int32_t encoder_distance = 0;//�ñ�������¼�����ٶ�֮ǰ��������
void task_Chassis(void* parameter)
{
    //��������ʱִ��һ�θ�λ
    PID_Chassis_Init(); //��һ��ִ�е�������ʱ �����̵�PID�������г�ʼ��
    
    while (1)
    {
        HeatUpdate();   //����ǹ������ֵ��������������ֵ����ͨ��CAN���͵�������̨����ȥ
        ShootingHeat_CAN2Send();        
        encoder_distance = Read_Encoder();
        
        if (Sentry_State.Chassis_Mode == Chassis_Patrol) Chassis_Patrol_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_RC) Chassis_RC_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_SLEEP) Chassis_SLEEP_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_DEBUG) Chassis_DEBUG_Act();
        
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
    static float kChassis = 4.0f;   //����ϵ��
    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=0;
        chassisFlag_Init=1; 
    }
    
    RC_Ctl_t RC_Ctl = getRCData();

//    //����+����̨ң�أ� ��ҡ�˿�����̨����ҡ�˿��Ƶ��̺ͷ���
//    if(Sentry_State.Gimbal_Up_Mode == Gimbal_Up_RC /* && Sentry_State.ChassisMode & Chassis_RC*/)
//        motor_chassis.pid.SetPoint=kChassis*(RC_Ctl.rc.ch0-1024/*1043*/); 
//    
//    //����+����̨ң�أ� ��ҡ�˿�����̨����ҡ�˿��Ƶ��̺ͷ���
//    if(Sentry_State.Gimbal_Dn_Mode == Gimbal_Dn_RC /*&& Sentry_State.ChassisMode & Chassis_RC*/)
//        motor_chassis.pid.SetPoint=kChassis*(RC_Ctl.rc.ch2-1024/*1002*/); 

    Chassis_SLEEP_PID_Cal();
}

/**
  * @brief  PCģʽ�µ��̵��˶�
  * @param  None
  * @retval None
  */
uint16_t waitingTick;//�ڶ�ͷ��ͣ��ʱ��
uint32_t movingTick;//��¼�˶���ʱ��
uint16_t movingDir;//��ǰ�н��ķ���  1Ϊ���� 0Ϊ����
static void Chassis_Patrol_Act(void)
{
    extern uint8_t startFlag, destinationFlag;
    extern uint8_t reverseFlag;

    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=1;
        chassisFlag_Init=1; 
    }
        
    extern PhotoEleSWState_t PhotoEle_State;  //��翪��״̬
    
    
    //�����˶�״̬��������������
    if(PhotoEle_State == PhotoEle_Both_IDLE) 
    {//���߶�δ����
//        if(motor_chassis.pid.SetPoint)
//        {//���������˶���waitingTick���㣬movingTick++
            waitingTick = 0;
            movingTick++;
//        }
//        else 
//        {
//           //������Ϊ��ͣ����ͬʱ����waitingTick��movingTick
//           waitingTick = 0;
//           movingTick = 0;
//        }
    }
    else if(PhotoEle_State == PhotoEle_Left_Active)  
    {//��ഥ������������J�����˶���ͣ��movingTick���㣬waitingTick++, �˶�����
        waitingTick++;
        movingTick = 0;
        movingDir = 1;
    }
    else if( PhotoEle_State == PhotoEle_Right_Active )
    {//�Ҳഥ������������J�����˶���ͣ��movingTick���㣬waitingTick++, �˶�����
        waitingTick++;
        movingTick = 0;
        movingDir = 0;
    }
    else if(PhotoEle_State == PhotoEle_Both_Active ) 
    {//˫�ߴ���������Ϊ��Ϊ��ͣ���
        waitingTick = 1;//����������
        movingTick = 0;
    }
    
    
    
    //���ݼ���ֵ���ı��˶�״̬
    if(waitingTick>0 && waitingTick<200 && movingTick == 0) 
    {//��ͣ״̬0��
        motor_chassis.pid.SetPoint = 0;
    }
    else if(waitingTick>200 && movingTick == 0)
    {//����������1��״̬
        motor_chassis.pid.SetPoint = (movingDir)?(-1000):(1000);    
    }
    else
    {
        if(waitingTick == 0 && movingTick > 300)
        {//��1�ٿ�ʼ�ټ�һ���ٵ�2��״̬
            motor_chassis.pid.SetPoint = (movingDir)?(-2000):(2000);    
        }
        if(waitingTick == 0 && movingTick > 600)
        {//��1�ٿ�ʼ�ټ�һ���ٵ�2��״̬
            motor_chassis.pid.SetPoint = (movingDir)?(-2500):(2500);    
        }
        if(waitingTick == 0 && movingTick > 1000)
        {//��2�ٿ�ʼ�ټ�һ���ٵ�3��״̬
            motor_chassis.pid.SetPoint = (movingDir)?(-3200):(3200);    
        }
//        if(waitingTick == 0 && movingTick > 2000)
//        {//��3�ٿ�ʼ�ټ�һ���ٵ�4��״̬
//            motor_chassis.pid.SetPoint = (movingDir)?(-4000):(4000);    
//        }
    }
    
    Chassis_Patrol_PID_Cal();
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

static void Chassis_Patrol_PID_Cal(void)
{
    float Chassis_Iset;
    extern INA260 INA260_1;
//  motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint, ChassisCurrentLimit, -ChassisCurrentLimit);
    Chassis_Iset = PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
//    PowerLimit_k += PID_Calc(&PowerLimit, INA260_1.Power) / PowerLimit.SetPoint;  //�ù��ʻ�PID�������űȣ�
//    PowerLimit_k = LIMIT_MAX_MIN(PowerLimit_k, 1, 0);
    Chassis_Iset = LIMIT_MAX_MIN(Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
//    Chassis_Iset = PowerLimit_k * Chassis_Iset;

    Chassis_CAN1Send(Chassis_Iset);
}

inline static void Chassis_SLEEP_PID_Cal(void)
{
    Chassis_CAN1Send(0);
}

int32_t debug_Chassis_Iset;
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
    motor_chassis.pid.P = 20.0f;//0.8f;
	motor_chassis.pid.I = 0;//0.6f;
	motor_chassis.pid.D = 0;//0.7f;
	motor_chassis.pid.IMax = 10.0f;
    motor_chassis.pid.OutMax = 5000.0f;
	motor_chassis.zerocheck.CountCycle = 8191;
	motor_chassis.zerocheck.LastValue = motor_chassis.angle_abs;
	motor_chassis.zerocheck.Circle = 0;
	
	PowerLimit.SetPoint = 25;  //�ڱ����̹���������30W�����⻹��200J��������
	PowerLimit.P = 0.01f;
	PowerLimit.I = 0.01f;
	PowerLimit.D = 0.03f;
	PowerLimit.IMax = 0.5f;
}

void PID_Chassis_ClearError(void)
{  //����˵�ǿ��Բ�����������ģ���Ŀǰ����һ�������Է���һ
    motor_chassis.pid.dError = 0.0f;
    motor_chassis.pid.PreError = 0.0f;
    motor_chassis.pid.SumError = 0.0f;
    PowerLimit.dError = 0.0f;
    PowerLimit.PreError = 0.0f;
    PowerLimit.SumError = 0.0f;
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



