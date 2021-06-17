#include "main.h"
#include "task_StateLED.h"

/**
 *  @brief  ���ݵ��߼��Ľ����״̬��
 *  @param  ��
 *  @retval ��
 */
void task_StateLED(void)
{
    extern LED_struct LEDs[];
    extern State_t Sentry_State;
    while (1)
    {
        if(Sentry_State.Chassis_Mode == Chassis_DEBUG)
            LED_all_toggle(), vTaskDelay(250)/*, IWDG_Feed()*/;  
        else if(Sentry_State.Chassis_Mode == Chassis_RC 
            || Sentry_State.Chassis_Mode == Chassis_Patrol)
        {
           for (uint8_t i = 0; i < 4; i++) {LED_on(LEDs[i]); vTaskDelay(500)/*;IWDG_Feed()*/;}
           for (uint8_t i = 0; i < 4; i++) {LED_off(LEDs[i]);vTaskDelay(500)/*;IWDG_Feed()*/;}
        }
        else if(Sentry_State.Chassis_Mode == Chassis_SLEEP)
            LED_all_toggle(),vTaskDelay(500)/*,IWDG_Feed()*/;
    }
}

