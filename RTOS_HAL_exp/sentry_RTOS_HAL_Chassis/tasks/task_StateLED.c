#include "main.h"

/**
 *  @brief  根据掉线检测的结果闪状态灯
 *  @param  无
 *  @retval 无
 */
void task_StateLED(void)
{
    extern LED_struct LEDs[];
    extern State_t Sentry_State;
    while (1)
    {
        if(Sentry_State.Chassis_Mode == Chassis_DEBUG)
        {   
            LED_all_on();
            vTaskDelay(500);
            LED_all_off();
            vTaskDelay(500);
        }
        else if(Sentry_State.Chassis_Mode == Chassis_RC || Sentry_State.Chassis_Mode == Chassis_PC)
        {
           for (uint8_t i = 0; i < 4; i++) {LED_on(LEDs[i]); vTaskDelay(500);}
           for (uint8_t i = 0; i < 4; i++) {LED_off(LEDs[i]);vTaskDelay(500);}
        }
        else if(Sentry_State.Chassis_Mode == Chassis_SLEEP)
           LED_all_on(),vTaskDelay(500);                     
    }
}

