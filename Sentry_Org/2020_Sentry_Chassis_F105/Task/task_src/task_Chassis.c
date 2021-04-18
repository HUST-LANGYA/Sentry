#include "main.h"
//���̲���
_3508_motor_t motor_chassis;
int16_t ChassisSetSpeed;//�����ٶ��趨ֵ
float Chassis_Iset;  //���̵������ֵ

extern int16_t Journey_direction;//�˶�����
extern _2006_motor_t BodanMotor;
//�ڵ��̵�cal����ͬʱ���������������ֵ��
//�������������Ҫ��ִ��bodan_cal
//(��Ϊ�����Ľ��յı�ͷ��һ���ģ���ͺ�SB)
uint8_t chassisFlag_Init=0;

int16_t test_init_flag=0;

//���ʲ���
float PowerNow;
PID_Typedef PowerLimit;
float PowerLimit_k=1;

//���߼��
block_disconnect_t block_disconnect;

float kChassis = 4.0f;
/**
  * @brief  ң��ģʽ�µ��̵��˶�
  * @param  None
  * @retval None
  */
void Chassis_RC_Act(void)
{
    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=0;
        chassisFlag_Init=1; 
    }
    
    extern REMOTE_t remote;//��CAN���Ͻ��յ�ң���������ĵ��̵���ٶ�
    motor_chassis.pid.SetPoint=0;
    //����+����̨ң�أ� ��ҡ�˿�����̨����ҡ�˿��Ƶ��̺ͷ���
    if(Sentry_Status.GimbalMode & Gimbal_Up_RC  && Sentry_Status.ChassisMode & Chassis_RC)
        motor_chassis.pid.SetPoint=kChassis*(remote.ch[0]-1043); 
    
    //����+����̨ң�أ� ��ҡ�˿�����̨����ҡ�˿��Ƶ��̺ͷ���
    if(Sentry_Status.GimbalMode & Gimbal_Dn_RC && Sentry_Status.ChassisMode & Chassis_RC)
        motor_chassis.pid.SetPoint=kChassis*(remote.ch[2]-1002); 
}

/**
  * @brief  PCģʽ�µ��̵��˶�
  * @param  None
  * @retval None
  */
extern uint8_t startFlag,destinationFlag;
        extern int32_t movingTick;
        extern uint8_t reverseFlag;
void Chassis_PC_Act(void)
{

    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=0;
        chassisFlag_Init=1; 
    }
    
    //PCģʽ�µ����Զ�����������һ������
    //ע��ֻ��һ����翪�صĽӿ��Ǻõ�
    
    
    if(Sentry.MovingDir == FORWARD) //�����ƶ�ʱ
    {
        startFlag = 1;//��ʼ�ܶ�
        movingTick++;
        motor_chassis.pid.SetPoint= -2000; //����ܹ���������
    }
    else if(Sentry.MovingDir == BACKWARD) //�����ƶ�ʱ
    {
        startFlag = 1;//��ʼ�ܶ�
        movingTick--;
        motor_chassis.pid.SetPoint= 2000; //����ܹ���������
    }
    
}


int16_t test_ChassisI,test_BodanI=0;
/**
  * @brief  ����PID����,�����͵�can��
  * @param  None
  * @retval None
  */
void Chassis_PID_Cal(void)
{	
	PowerNow = GetPower();
	
	if(Sentry.State == SENTRY_SCANNING)
		PowerLimit.SetPoint = 30;
//	else if(Drone_alarm == 0)		
//	{
//		PowerLimit.SetPoint = 30;
//	}
//	else 
//	{
////		if(recover_flag == 1)
////			PowerLimit.SetPoint = 35;
////		else 
////			PowerLimit.SetPoint = 30;	//37
//	}
//	
//	if(remainEnergy<= 50)
//		PowerLimit.SetPoint = 28; 
	
	if((Sentry_Status.ChassisMode & Chassis_PC) && test_init_flag==0)
	{
		Sentry.Journey=START_POS;
		test_init_flag=1;
	}
	if(!(Sentry_Status.ChassisMode & Chassis_PC) && test_init_flag==1)
	{	
		test_init_flag=0;
	}

	
	if(Sentry_Status.ChassisMode != Chassis_SLEEP )
	{
		if(Journey_direction == R_direction  )
		{
			ChassisSetSpeed -= ChassisTempSpeed;
			if(ChassisSetSpeed <= -ChassisMaxSpeed)
				ChassisSetSpeed= -ChassisMaxSpeed;
		}
		else if(Journey_direction == L_direction)
		{
			ChassisSetSpeed+= ChassisTempSpeed;
			if(ChassisSetSpeed >= ChassisMaxSpeed)
				ChassisSetSpeed= ChassisMaxSpeed;
		}
	}
	//motor_chassis.pid.SetPoint =ChassisSetSpeed;
    
//    if(Sentry_Status.ChassisMode & Chassis_PC)
//    {        
        if(reverseFlag && movingTick <= 50) //�Ѿ��س��ҿ쵽ͷ��
        {
            Chassis_Iset = 0;//ͣ�µ��̵��˶�(����������û�����ᣬ��ƾ�����ٻ�һ��)
            destinationFlag = 1;
            motor_chassis.pid.SetPoint=0;//ͣ�µ��̵��˶�
            //Sentry_Status.ChassisMode = Chassis_SLEEP;
        }
//        

//    }

    
	Chassis_Iset=PID_Calc(&motor_chassis.pid,motor_chassis.real_speed); 
	PowerLimit_k+=PID_Calc(&PowerLimit,PowerNow)/PowerLimit.SetPoint;			
	PowerLimit_k=LIMIT_MAX_MIN(PowerLimit_k,1,0);
	Chassis_Iset=LIMIT_MAX_MIN(Chassis_Iset,ChassisCurrentLimit,-ChassisCurrentLimit);
	Chassis_Iset=PowerLimit_k*Chassis_Iset;

    
//     if(destinationFlag == 1)
//       motor_chassis.pid.SetPoint = 0;
    
    if((Sentry_Status.ShootMode & Shoot_Up_PC) == 0)   //û����������͸�����������ĵ���Ϊ0
     ChassisISend(Chassis_Iset,0/*BodanMotor.I_Set*/);
    
    if(Sentry_Status.ShootMode & Shoot_Up_PC )      //�������������ͬʱ�����̵���Ͳ�������ĵ���
     ChassisISend(Chassis_Iset,BodanMotor.I_Set);
    
    //ChassisISend(test_ChassisI,test_BodanI);
    //����ģʽ����test_ChassisI,test_BodanI
    //ChassisISend(test_ChassisItest_ChassisI,test_BodanI);

}	

/**
  * @brief  ����PID��ʼ��
  * @param  None
  * @retval None
  */
void PID_Chassis_Init(void)
{
//  motor_chassis.pid.P = 0;
//	motor_chassis.pid.I = 0;
//	motor_chassis.pid.D = 0;
//	motor_chassis.pid.IMax = 1000;
//	motor_chassis.zerocheck.CountCycle = 8191;
//	motor_chassis.zerocheck.LastValue = motor_chassis.angle_abs;
//	motor_chassis.zerocheck.Circle = 0;
	
	PowerLimit.SetPoint = 20;
	PowerLimit.P = 0.01f;
	PowerLimit.I = 0.01f;
	PowerLimit.D = 0.03f;
	PowerLimit.IMax = 0.5f;
    
	motor_chassis.pid.P = 8;
	motor_chassis.pid.I = 0.8;
	motor_chassis.pid.D = 1;
	motor_chassis.pid.IMax = 1000;
	motor_chassis.zerocheck.CountCycle = 8191;
	motor_chassis.zerocheck.LastValue = motor_chassis.angle_abs;
	motor_chassis.zerocheck.Circle = 0;
	
//	PowerLimit.SetPoint = 20;
//	PowerLimit.P = 0.01f;
//	PowerLimit.I = 0.01f;
//	PowerLimit.D = 0.03f;
//	PowerLimit.IMax = 0.5f;
}

/**
 *  @brief  ��ȡ�������������ṹ��ĵ�ǰ����
 *  @param  None
 *  @retval None
 */
block_disconnect_t* Get_block_t(void)
{
	return &block_disconnect;
}


void ChassisZeroCheck(void)
{
	motor_chassis.angle_inc = ZeroCheck(&motor_chassis.zerocheck,motor_chassis.angle_abs,Position);
}

/**
 *  @brief  ���̵�����
 *  @param  None
 *  @retval None
 */
void Disconnect_Check(void)
{
	int32_t time = GetSysCnt();
	if(time - block_disconnect.Chassis_Last_Cnt>=10)
		block_disconnect.Chassis_Sleep_Cnt++;
	if(block_disconnect.Chassis_Sleep_Cnt>=500)
		block_disconnect.is_Chassis_Sleep = 1;	
    
    /*�����ĵ�����Ҳ�ӵ������Ժ�*/
}

/**
  * @brief  ���ص��̵Ľṹ��ָ��
  * @param  None
  * @retval None
  */
_3508_motor_t* Get_MotorChassis(void)
{
	return &motor_chassis;
}

/**
  * @brief  ����setpoint���ݵ�ָ��
  * @param  None
  * @retval None
  */
int16_t* GetChassisSetspeed(void)
{
	return &ChassisSetSpeed;
}




/**
  * @brief  ��ң������Ϣ���¹���״̬
  * @param  None
  * @retval None
  */
int16_t PC_DownFlag;
int32_t time, time_now;
void action_update(void)
{
    extern REMOTE_t remote; // = getRemoteData();
    switch (remote.s1)
    {
    case 1:
        switch (remote.s2)
        {
        case 1:
            Sentry_Status.GimbalMode = Gimbal_Up_PC | Gimbal_Dn_PC;
            Sentry_Status.ChassisMode = Chassis_PC;
            Sentry_Status.ShootMode = Shoot_Up_PC | Shoot_Dn_PC;
            break;
        case 2:
            Sentry_Status.GimbalMode = Gimbal_Up_PC | Gimbal_Dn_PC;
            Sentry_Status.ChassisMode = Chassis_RC;
            Sentry_Status.ShootMode = Shoot_Up_SLEEP|Shoot_Dn_SLEEP;
            break;
        case 3:
            Sentry_Status.GimbalMode = Gimbal_Up_RC | Gimbal_Dn_RC;
            Sentry_Status.ChassisMode = Chassis_PC;
            Sentry_Status.ShootMode = Shoot_Up_SLEEP|Shoot_Dn_SLEEP;
            break;
        default:
            break;
        }
        break;
    case 2:
        switch (remote.s2)
        {
        case 1:
            Sentry_Status.GimbalMode = Gimbal_Up_RC|Gimbal_Dn_SLEEP;
            Sentry_Status.ChassisMode = Chassis_RC;
            //if (remote.ch[1] >= 1500) //���Ҳದ���Ƶ�������ʱ��Ϊң����ģʽ�£�����̨����״̬
                Sentry_Status.ShootMode = Shoot_Up_RC|Shoot_Dn_SLEEP;
            //else
            //    Sentry_Status.ShootMode = Shoot_Up_SLEEP|Shoot_Dn_SLEEP;
            break;
        case 2:
            Sentry_Status.GimbalMode = Gimbal_Up_SLEEP|Gimbal_Dn_SLEEP;
            Sentry_Status.ChassisMode = Chassis_SLEEP;
            Sentry_Status.ShootMode = Shoot_Up_SLEEP|Shoot_Dn_SLEEP;
//            if (remote.ch[0] < 380 && remote.ch[1] < 380 && remote.ch[2] > 1660 && remote.ch[3] < 380 && PC_DownFlag == 0)
//            {
//                PC_DownFlag = 1;
//                time = GetSysCnt();
//            }
//            else if (remote.ch[0] < 380 && remote.ch[1] < 380 && remote.ch[2] > 1660 && remote.ch[3] < 380 && PC_DownFlag == 1)
//            {
//                time_now = GetSysCnt();
//                if (time_now - time >= 1000)
//                    PC_DownFlag = 2;
//            }
//            else
//                PC_DownFlag = 0;
            break;
        case 3:
            Sentry_Status.GimbalMode = Gimbal_Dn_RC;
            Sentry_Status.ChassisMode = Chassis_RC;
            //if (remote.ch[3] >= 1500) //����ದ���Ƶ�������ʱ��Ϊң����ģʽ�£�����̨����״̬
                Sentry_Status.ShootMode = Shoot_Up_SLEEP|Shoot_Dn_RC;
            //else
            //    Sentry_Status.ShootMode = Shoot_Up_SLEEP|Shoot_Dn_SLEEP;
            break;
        default:
            break;
        }
        break;
    case 3:
        switch (remote.s2)
        {
        case 1:
            Sentry_Status.GimbalMode = Gimbal_Up_RC|Gimbal_Dn_SLEEP;
            Sentry_Status.ChassisMode = Chassis_RC;
            Sentry_Status.ShootMode = Shoot_Up_PC|Shoot_Dn_SLEEP;
            break;
        case 2:
            Sentry_Status.GimbalMode = Gimbal_Up_RC | Gimbal_Dn_RC;
            Sentry_Status.ChassisMode = Chassis_SLEEP;
            Sentry_Status.ShootMode = Shoot_Up_PC | Shoot_Dn_PC;
            break;
        case 3:
            Sentry_Status.GimbalMode = Gimbal_Dn_RC;
            Sentry_Status.ChassisMode = Chassis_RC;
            Sentry_Status.ShootMode = Shoot_Up_SLEEP | Shoot_Dn_PC;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}


/**
  * @brief  ң�����������ݵĳ�ʼ��
  * @param  None
  * @retval None
  */
void remote_reset(void)
{
    extern REMOTE_t remote;
    remote.s1=2;
    remote.s2=2;
    remote.ch[0]=1043;
    remote.ch[1]=1024;
    remote.ch[2]=1002;
    remote.ch[3]=1024;
}

