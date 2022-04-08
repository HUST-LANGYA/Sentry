#include "main.h"
#include "task_ActionUpdate.h"

State_t Sentry_State;
extern block_disconnect_t block_disconnect;

static void update_state_with_RCdata(void);
static void sentry_state_reflect(uint8_t gimbal_up_mode,uint8_t gimbal_dn_mode,
                                 uint8_t shoot_up_mode,uint8_t shoot_dn_mode,
                                 uint8_t chassis_mode); //reflect������һ��ӳ�����˼

/**
 * @brief ״̬�����壬ִ��ÿ�ε�״̬ˢ��
 * @param ��
 * @retval ��
 */
void task_ActionUpdate(void)
{
    reset_remote_control_msg(); //�ս�����ʱ��ִ��һ������
    //Sentry_State.Chassis_Mode = Chassis_SLEEP;
    while (1)
    {
        update_state_with_RCdata(); //��ң�������µ�ǰ״̬
        vTaskDelay(1);
    }
}

/**
  * @brief  ��ң������Ϣ���¹���״̬
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
        case 1: //��ʽ�Ϲ�����ģʽ��������
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
        case 3: //���ڶ̻����ϲ⸨���ģʽ
            sentry_state_reflect(Gimbal_Up_PC,
                                 Gimbal_Dn_PC,
                                 ((RC_Ctl.rc.ch1 > 1500) ? Shoot_Up_PC : Shoot_Up_SLEEP),
                                 ((RC_Ctl.rc.ch0 > 1500) ? Shoot_Dn_PC : Shoot_Dn_SLEEP), //�ǵø�����̨������ط���������
                                 Chassis_SLEEP);
            break;
        default:
            break;
        }
        break;
    case 2:
        switch (RC_Ctl.rc.s2)
        {
        case 1: //����̨��¼���ӵ�
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 ((RC_Ctl.rc.ch1 > 1500) ? Shoot_Up_RC : Shoot_Up_SLEEP),
                                 Shoot_Dn_SLEEP,
                                 Chassis_SLEEP);
            break;
        case 2: //����ģʽ
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_SLEEP,
                                 Shoot_Up_SLEEP,
                                 Shoot_Dn_SLEEP,
                                 Chassis_SLEEP);
            break;
        case 3: //����̨��¼���ӵ�
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
        case 1: //����̨ң��������
            sentry_state_reflect(Gimbal_Up_RC,
                                 Gimbal_Dn_SLEEP,
                                 ((RC_Ctl.rc.ch1 > 1500) ? Shoot_Up_RC : Shoot_Up_SLEEP),
                                 Shoot_Dn_SLEEP,
                                 Chassis_SLEEP);
            break;
        case 2: //����̨ң��������
            sentry_state_reflect(Gimbal_Up_SLEEP,
                                 Gimbal_Dn_RC,
                                 Shoot_Up_SLEEP,
                                 ((RC_Ctl.rc.ch1 > 1500) ? Shoot_Dn_RC : Shoot_Dn_SLEEP),
                                 Chassis_SLEEP);
            break;
        case 3: //���̼�¼ģʽ�������˶�
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
