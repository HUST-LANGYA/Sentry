#include "main.h"
#include "task_ActionUpdate.h"

State_t Sentry_State;
extern block_disconnect_t block_disconnect;
int16_t Gimbal_init_flag, RC_Change_Flag_now, RC_Change_Flag_last;
int16_t Shoot_init_flag = 0;
//int32_t SetSpeed,RC_Chassis_scal = 5;

static void update_state_with_RCdata(void);
static void sentry_state_reflect(uint8_t gimbal_up_mode,
                                 uint8_t gimbal_dn_mode,
                                 uint8_t shoot_up_mode,
                                 uint8_t shoot_dn_mode,
                                 uint8_t chassis_mode);  //reflect当作做一个映射的意思吧

/**
 * @brief 状态机本体，执行每次的状态刷新
 * @param 无
 * @retval 无
 */
void task_ActionUpdate(void)
{
    reset_remote_control_msg();

//    vTaskDelay(900);
//    Sentry_State.Shoot_Dn_Mode = Shoot_Dn_SLEEP;
//    Sentry_State.Gimbal_Dn_Mode = Gimbal_Dn_SLEEP;
        

    while (1)
    {
        update_state_with_RCdata(); 


        vTaskDelay(1);
    }
}

/**
  * @brief  用遥控器信息更新工作状态
  * @param  None
  * @retval None
  */
// int16_t PC_DownFlag;
// int32_t time, tim_now;
static void update_state_with_RCdata(void)
{
    RC_Ctl_t RC_Ctl = getRCData();

    switch (RC_Ctl.rc.s1)
    {
    case 1:
        switch (RC_Ctl.rc.s2)
        {
        case 1:
            sentry_state_reflect(Gimbal_Up_PC,
                                 Gimbal_Dn_PC,
                                 Shoot_Up_PC,
                                 Shoot_Dn_PC,
                                 Chassis_Patrol);
            break;
        case 2:
            sentry_state_reflect(Gimbal_Up_PC,
                                 Gimbal_Dn_PC,
                                 Shoot_Up_SLEEP,
                                 Shoot_Dn_SLEEP,
                                 Chassis_RC);
            break;
        case 3:
            sentry_state_reflect(Gimbal_Up_RC,
                                 Gimbal_Dn_RC,
                                 Shoot_Up_SLEEP,
                                 Shoot_Dn_SLEEP,
                                 Chassis_Patrol); 
            break;
        default:
            break;
        }
        break;
    case 2:
        switch (RC_Ctl.rc.s2)
        {
        case 1:
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 Shoot_Up_RC,
                                 Shoot_Dn_SLEEP,
                                 Chassis_SLEEP); 
            break;
        case 2:
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 Shoot_Up_SLEEP,
                                 Shoot_Dn_SLEEP,
                                 Chassis_SLEEP);             
            break;
        case 3:
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 Shoot_Up_SLEEP,
                                 Shoot_Dn_RC,
                                 Chassis_SLEEP);        
            break;
        default:
            break;
        }
        break;
    case 3:
        switch (RC_Ctl.rc.s2)
        {
        case 1:
            sentry_state_reflect(Gimbal_Up_RC,
                                 Gimbal_Dn_SLEEP,
                                 Shoot_Up_PC,
                                 Shoot_Dn_SLEEP,
                                 Chassis_RC);             
            break;
        case 2:
            sentry_state_reflect(Gimbal_Up_RC,
                                 Gimbal_Dn_RC,
                                 Shoot_Up_PC,
                                 Shoot_Dn_PC,
                                 Chassis_SLEEP);              
            break;
        case 3:
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 Shoot_Up_SLEEP,
                                 Shoot_Dn_SLEEP,
                                 Chassis_Patrol);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

static void sentry_state_reflect(uint8_t gimbal_up_mode,
                                 uint8_t gimbal_dn_mode,
                                 uint8_t shoot_up_mode,
                                 uint8_t shoot_dn_mode,
                                 uint8_t chassis_mode)
{
    Sentry_State.Gimbal_Up_Mode =  gimbal_up_mode ;
    Sentry_State.Gimbal_Dn_Mode =  gimbal_dn_mode ;
    Sentry_State.Shoot_Up_Mode  =  shoot_up_mode  ;
    Sentry_State.Shoot_Dn_Mode  =  shoot_dn_mode  ;
    Sentry_State.Chassis_Mode   =  chassis_mode   ;  
}

// void Change_Check(void)
// {
//     RC模式下
//     if(Sentry_State.GimbalMode & Gimbal_Up_RC)
//     {
//       RC_Change_Flag_now=1;
//       if(RC_Change_Flag_now != RC_Change_Flag_last)
//           Gimbal_init_flag=0;
//       RC_Change_Flag_last=RC_Change_Flag_now;
//     }
//     else if(Sentry_State.GimbalMode & Gimbal_Up_PC)
//     {
//         RC_Change_Flag_now=0;
//     }
//     else
//     {
//         RC_Change_Flag_now=0;
//     }
// }

