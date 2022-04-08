#include "main.h"
#include "task_ActionUpdate.h"

State_t Sentry_State;
extern block_disconnect_t block_disconnect;

static void update_state_with_RCdata(void);
static void sentry_state_reflect(uint8_t gimbal_up_mode,uint8_t gimbal_dn_mode,
                                 uint8_t shoot_up_mode,uint8_t shoot_dn_mode,
                                 uint8_t chassis_mode); //reflect当作做一个映射的意思

/**
 * @brief 状态机本体，执行每次的状态刷新
 * @param 无
 * @retval 无
 */
void task_ActionUpdate(void)
{
    reset_remote_control_msg(); //刚进任务时，执行一次清零
    //Sentry_State.Chassis_Mode = Chassis_SLEEP;
    while (1)
    {
        update_state_with_RCdata(); //从遥控器更新当前状态
        vTaskDelay(1);
    }
}

/**
  * @brief  用遥控器信息更新工作状态
  * @param  None
  * @retval None
  */
static void update_state_with_RCdata(void)
{
    RC_Ctl_t RC_Ctl = getRCData();

    switch (RC_Ctl.rc.s1)
    {
    case 1:
        switch (RC_Ctl.rc.s2)
        {
        case 1: //正式上轨运行模式，比赛用
            sentry_state_reflect(Gimbal_Up_PC,
                                 Gimbal_Dn_PC,
                                 Shoot_Up_SLEEP,
                                 Shoot_Dn_SLEEP,
                                 Chassis_Patrol);
            break;
        case 2:
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 Shoot_Up_SLEEP,
                                 Shoot_Dn_SLEEP,
                                 Chassis_RC);
            break;
        case 3: //挂在短滑轨上测辅瞄的模式
            sentry_state_reflect(Gimbal_Up_PC,
                                 Gimbal_Dn_PC,
                                 ((RC_Ctl.rc.ch1 > 1500) ? Shoot_Up_PC : Shoot_Up_SLEEP),
                                 ((RC_Ctl.rc.ch0 > 1500) ? Shoot_Dn_PC : Shoot_Dn_SLEEP), //记得改下云台的这个地方！！！！
                                 Chassis_SLEEP);
            break;
        default:
            break;
        }
        break;
    case 2:
        switch (RC_Ctl.rc.s2)
        {
        case 1: //上云台检录打子弹
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 ((RC_Ctl.rc.ch1 > 1500) ? Shoot_Up_RC : Shoot_Up_SLEEP),
                                 Shoot_Dn_SLEEP,
                                 Chassis_SLEEP);
            break;
        case 2: //掉电模式
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 Shoot_Up_SLEEP,
                                 Shoot_Dn_SLEEP,
                                 Chassis_SLEEP);
            break;
        case 3: //下云台检录打子弹
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 Shoot_Up_SLEEP,
                                 ((RC_Ctl.rc.ch1 > 1500) ? Shoot_Dn_RC : Shoot_Dn_SLEEP),
                                 Chassis_SLEEP);
            break;
        default:
            break;
        }
        break;
    case 3:
        switch (RC_Ctl.rc.s2)
        {
        case 1: //上云台遥控器调试
            sentry_state_reflect(Gimbal_Up_RC,
                                 Gimbal_Dn_SLEEP,
                                 ((RC_Ctl.rc.ch1 > 1500) ? Shoot_Up_RC : Shoot_Up_SLEEP),
                                 Shoot_Dn_SLEEP,
                                 Chassis_SLEEP);
            break;
        case 2: //下云台遥控器调试
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_RC,
                                 Shoot_Up_SLEEP,
                                 ((RC_Ctl.rc.ch1 > 1500) ? Shoot_Dn_RC : Shoot_Dn_SLEEP),
                                 Chassis_SLEEP);
            break;
        case 3: //底盘检录模式，底盘运动
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
    Sentry_State.Gimbal_Up_Mode = gimbal_up_mode;
    Sentry_State.Gimbal_Dn_Mode = gimbal_dn_mode;
    Sentry_State.Shoot_Up_Mode = shoot_up_mode;
    Sentry_State.Shoot_Dn_Mode = shoot_dn_mode;
    Sentry_State.Chassis_Mode = chassis_mode;
}
