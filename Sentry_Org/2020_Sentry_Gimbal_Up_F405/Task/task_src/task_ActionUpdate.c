#include "main.h"

Status_t Sentry_Status;
block_disconnet_t block_disconnect;
int16_t Gimbal_init_flag,RC_Change_Flag_now,RC_Change_Flag_last;
int16_t Shoot_init_flag=0;
//int32_t SetSpeed,RC_Chassis_scal = 5;
void Status_Act(void)
{
    Remote_Can1Send();
    action_update();
//    Sentry_Status.ShootMode = Shoot_Up_PC;
//    Sentry_Status.GimbalMode = Gimbal_Up_PC;
	if(Sentry_Status.GimbalMode & Gimbal_Up_PC)
    {
        Change_Check();
		Gimbal_Limit();
		Gimbal_GYRO_cal();  //��̨��̬����
        Gimbal_PC_Act();
    }
	else if(Sentry_Status.GimbalMode & Gimbal_Up_RC)
    {  
        Change_Check();	
        Gimbal_Limit();
        Gimbal_GYRO_cal();  //��̨��̬����
        Gimbal_RC_Act();
    }
    else if(Sentry_Status.GimbalMode == Gimbal_Up_SLEEP)
    {
        Change_Check();
        Gyro_Can2Send(0,0);
	}
    else 
    {
        Change_Check();
        Gyro_Can2Send(0,0);
    }
    
	if(Sentry_Status.ShootMode & Shoot_Up_PC)
	{
        Shoot_Fire_Cal();
    }
    else if(Sentry_Status.ShootMode & Shoot_Up_RC)
    {
        Shoot_Fire_Cal();
    }   
    else if(Sentry_Status.ShootMode == Shoot_Up_SLEEP)
    {
        FrictionWheel_Set(0,0);
    }
    else
    {
		Shoot_init_flag=0;
        FrictionWheel_Set(0,0);
	}
}


int16_t is_locked=0;
extern _2006_motor_t BodanMotor;
/**
  * @brief  ���߼��
  * @param  None
  * @retval None
  */
void DisconnectDetect(void)
{
	int32_t timenow = GetSysCnt();
	
	if(timenow - block_disconnect.PC_Last_Cnt>10)		//pc
		block_disconnect.PC_Disconnect_Cnt++;
	else 
		block_disconnect.PC_Disconnect_Cnt = 0;
	if(block_disconnect.PC_Disconnect_Cnt>=100)
		block_disconnect.is_pc_down = 1;
	else 
		block_disconnect.is_pc_down = 0;
	
//	
//	if(timenow - block_disconnect.Judge_Last_Cnt>10)	//����ϵͳ
//		block_disconnect.Judge_Disconnect_Cnt++;
//	else 
//		block_disconnect.Judge_Disconnect_Cnt = 0;
//	if(block_disconnect.Judge_Disconnect_Cnt>=200)
//		block_disconnect.is_judge_down = 1;
//	else 
//		block_disconnect.is_judge_down = 0;
	
	
	if(timenow - block_disconnect.Yaw_Last_Cnt>10)		//yaw���߼��
		block_disconnect.Yaw_Disconnect_Cnt++;
	else 
		block_disconnect.Yaw_Disconnect_Cnt = 0;
	if(block_disconnect.Yaw_Disconnect_Cnt>=1000)
		block_disconnect.is_yaw_down = 1;
	else 
		block_disconnect.is_yaw_down = 0;
	                      
	if(timenow - block_disconnect.Pitch_Last_Cnt>10)		//pitch���߼��
		block_disconnect.Pitch_Disconnect_Cnt++;
	else 
		block_disconnect.Pitch_Disconnect_Cnt = 0;					
	if(block_disconnect.Pitch_Disconnect_Cnt>=1000)
		block_disconnect.is_pitch_down = 1;
	else 
		block_disconnect.is_pitch_down = 0;
	
	
//	if(timenow - block_disconnect.Bodan_Last_Cnt>10)		//����������߼��
//		block_disconnect.Bodan_Disconnect_Cnt++;
//	else 
//		block_disconnect.Bodan_Disconnect_Cnt = 0;					
//	if(block_disconnect.Bodan_Disconnect_Cnt>=1000)
//		block_disconnect.is_bodan_down = 1;
//	else 
//		block_disconnect.is_bodan_down = 0;
	
	
	if(timenow - block_disconnect.Gyro_Last_Cnt>10)		//��̨
		block_disconnect.Gyro_Disconnect_Cnt++;
	else 
		block_disconnect.Gyro_Disconnect_Cnt = 0;					
	if(block_disconnect.Gyro_Disconnect_Cnt>=1000)
		block_disconnect.is_gyro_down = 1;
	else 
		block_disconnect.is_gyro_down = 0;

//	if(block_disconnect.BullectCnt - block_disconnect.Bullect_Last_Cnt>10)		//No bullect
//		block_disconnect.NoBullect_Cnt++;
//	else 
//		block_disconnect.NoBullect_Cnt = 0;					
//	if(block_disconnect.NoBullect_Cnt>=2000)
//		block_disconnect.is_no_bullet = 1;
//	else 
//		block_disconnect.is_no_bullet = 0;
//	
	
	if(timenow - block_disconnect.FrictionLeft_Last_Cnt>10)		//fritction���߼��
		block_disconnect.FrictionLeft_Disconnect_Cnt++;
	else 
		block_disconnect.FrictionLeft_Disconnect_Cnt = 0;	

	if(timenow - block_disconnect.FrictionRight_Last_Cnt>10)		
		block_disconnect.FrictionRight_Disconnect_Cnt++;
	else 
		block_disconnect.FrictionRight_Disconnect_Cnt = 0;	
	
	if(block_disconnect.FrictionLeft_Disconnect_Cnt>=1000 || block_disconnect.FrictionRight_Disconnect_Cnt>=1000)
		block_disconnect.is_friction_down = 1;
	else 
		block_disconnect.is_friction_down = 0;
	/************************************************/
//		block_disconnect.is_yaw_down = 0;	                   
//		block_disconnect.is_pitch_down = 0;
//		block_disconnect.is_bodan_down = 0;
//		block_disconnect.is_gyro_down = 0;
//		block_disconnect.is_no_bullet = 0;
//		block_disconnect.is_friction_down = 0;
	/***********************************************/
//	if(timenow - block_disconnect.Drone_Last_Cnt>2000)
//		block_disconnect.Drone_Disconnect_Cnt++;
//	else 
//		block_disconnect.Drone_Disconnect_Cnt = 0;
//	if(block_disconnect.Drone_Disconnect_Cnt>=5000)
//	{
//		block_disconnect.is_drone_down = 1;
//		Sentry.drone_alarm = 1;
//	}
//	else 
//		block_disconnect.is_drone_down = 0;
	
//	//���������ת���
//	if(ABS(BodanMotor.I_Set)>=BodanCurrentLimit*0.95)
//		block_disconnect.Bodan_block++;
//	else
//		block_disconnect.Bodan_block=0;
}

void Change_Check(void)
{
    //RCģʽ��
	if(Sentry_Status.GimbalMode & Gimbal_Up_RC)
	{
      RC_Change_Flag_now=1;
	  if(RC_Change_Flag_now != RC_Change_Flag_last)
		  Gimbal_init_flag=0;
	  RC_Change_Flag_last=RC_Change_Flag_now;
    }
	else if(Sentry_Status.GimbalMode & Gimbal_Up_PC)
	{
		RC_Change_Flag_now=0;
	}
	else
	{
		RC_Change_Flag_now=0;	
	}
}

/**
  * @brief  ��ң������Ϣ���¹���״̬
  * @param  None
  * @retval None
  */
int16_t PC_DownFlag;
int32_t time, tim_now;
void action_update(void)
{
    RC_Ctl_t RC_Ctl = getRCData();
    switch (RC_Ctl.rc.s1)
    {
    case 1:
        switch (RC_Ctl.rc.s2)
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
        switch (RC_Ctl.rc.s2)
        {
        case 1:
            Sentry_Status.GimbalMode = Gimbal_Up_RC;
            Sentry_Status.ChassisMode = Chassis_RC;
            //if (RC_Ctl.rc.ch1 >= 1500) //���Ҳದ���Ƶ�������ʱ��Ϊң����ģʽ�¿���״̬
                Sentry_Status.ShootMode = Shoot_Up_RC;
            //else
            //    Sentry_Status.ShootMode = Shoot_Up_SLEEP;
            break;
        case 2:
            Sentry_Status.GimbalMode = Gimbal_Up_SLEEP|Gimbal_Dn_SLEEP;
            Sentry_Status.ChassisMode = Chassis_SLEEP;
            Sentry_Status.ShootMode = Shoot_Up_SLEEP|Shoot_Dn_SLEEP;
            if (RC_Ctl.rc.ch0 < 380 && RC_Ctl.rc.ch1 < 380 && RC_Ctl.rc.ch2 > 1660 && RC_Ctl.rc.ch3 < 380 && PC_DownFlag == 0)
            {
                PC_DownFlag = 1;
                time = GetSysCnt();
            }
            else if (RC_Ctl.rc.ch0 < 380 && RC_Ctl.rc.ch1 < 380 && RC_Ctl.rc.ch2 > 1660 && RC_Ctl.rc.ch3 < 380 && PC_DownFlag == 1)
            {
                tim_now = GetSysCnt();
                if (tim_now - time >= 1000)
                    PC_DownFlag = 2;
            }
            else
                PC_DownFlag = 0;
            break;
        case 3:
            Sentry_Status.GimbalMode = Gimbal_Dn_RC;
            Sentry_Status.ChassisMode = Chassis_RC;
            //if (RC_Ctl.rc.ch1 >= 1500)
                Sentry_Status.ShootMode = Shoot_Dn_RC;
            //else
            //    Sentry_Status.ShootMode = Shoot_Dn_SLEEP;
            break;
        default:
            break;
        }
        break;
    case 3:
        switch (RC_Ctl.rc.s2)
        {
        case 1:
            Sentry_Status.GimbalMode = Gimbal_Up_RC;
            Sentry_Status.ChassisMode = Chassis_RC;
            Sentry_Status.ShootMode = Shoot_Up_PC;
            break;
        case 2:
            Sentry_Status.GimbalMode = Gimbal_Up_RC | Gimbal_Dn_RC;
            Sentry_Status.ChassisMode = Chassis_SLEEP;
            Sentry_Status.ShootMode = Shoot_Up_PC | Shoot_Dn_PC;
            break;
        case 3:
            Sentry_Status.GimbalMode = Gimbal_Dn_RC;
            Sentry_Status.ChassisMode = Chassis_RC;
            Sentry_Status.ShootMode = Shoot_Dn_PC;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}
