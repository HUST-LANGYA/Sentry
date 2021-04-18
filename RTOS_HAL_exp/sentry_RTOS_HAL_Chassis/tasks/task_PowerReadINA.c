#include "main.h"
#include "task_PowerReadINA.h"

/**
 *  @brief  ���ڴ�INA260�϶����ʳ���
 *  @param  ��
 *  @retval ��
 */
void task_PowerReadINA(void)
{
   //extern LED_struct LEDs[];
    extern State_t Sentry_State;
    while (1)
    {
        if(Sentry_State.Chassis_Mode != Chassis_SLEEP)
        {   
            INA_READ_Current();
            INA_READ_Vol();
            INA_READ_Power();
        }
        vTaskDelay(10);                     
    }
}

