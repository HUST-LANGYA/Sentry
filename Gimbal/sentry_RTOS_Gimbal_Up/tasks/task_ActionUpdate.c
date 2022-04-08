#include "main.h"
#include "task_ActionUpdate.h"

State_t Sentry_State;
block_disconnect_t block_disconnect;
int16_t Gimbal_init_flag,Shoot_init_flag = 0;

static void update_state_with_RCdata(void);
static void sentry_state_reflect(uint8_t gimbal_up_mode,
                                 uint8_t gimbal_dn_mode,
                                 uint8_t shoot_up_mode,
                                 uint8_t shoot_dn_mode,
                                 uint8_t chassis_mode);

/**
 * @brief 状态机本体，执行每次的状态刷新
 * @param 无
 * @retval 无
 */
void task_ActionUpdate(void)
{
    reset_remote_control_msg();
    
    //底盘通信相关（掉电计数、热量控制）
    extern uint32_t chassis_offline_tick;
    extern uint8_t chassis_offline_flag;
    extern uint8_t Heat_ShootAbleFlag_0;
    
	  Sentry_State.Shoot_Up_Mode = Shoot_Up_RC;
    while (1)
    {
        //update_state_with_RCdata(); //从遥控器更新当前状态
        Remote_Can1Send();          //转发遥控器的指令

        chassis_offline_tick++;//掉电计数自加
        if(chassis_offline_tick >= 5000) 
        {
            //标志底盘掉电
            chassis_offline_flag = 1; 
            Heat_ShootAbleFlag_0 = 1;
            //执行底盘掉电时的射击和云台模式调度
            extern uint32_t delayTick_oneShot;
            delayTick_oneShot *=10;  //如果接收不到底盘的热量数据，就把拨弹频率减半，即间隔加倍          
        }
        else chassis_offline_flag = 0; //底盘离线之后，又莫名上线了！

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
        case 1: //比赛上场之后模式
            sentry_state_reflect(Gimbal_Up_PC,
                                 Gimbal_Dn_PC,
                                 Shoot_Up_PC,
                                 Shoot_Dn_PC,
                                 Chassis_Patrol);
            break;
        case 2://底盘缓慢匀速检录模式
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 Shoot_Up_SLEEP,
                                 Shoot_Dn_SLEEP,
                                 Chassis_RC);
            break;
        case 3: //挂在短滑轨上的辅瞄模式
            sentry_state_reflect(Gimbal_Up_PC,
                                 Gimbal_Dn_PC,
                                 ((RC_Ctl.rc.ch1 > 1500) ? Shoot_Up_PC : Shoot_Up_SLEEP),
                                 ((RC_Ctl.rc.ch0 > 1500) ? Shoot_Dn_PC : Shoot_Dn_SLEEP),
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
            sentry_state_reflect(Gimbal_Up_PC,
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
        case 3: //底盘巡逻模式（功率调试）
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
