#include "main.h"
#include "task_PowerReadINA.h"

/**
 *  @brief  定期从INA260上读功率出来
 *  @param  无
 *  @retval 无
 */
uint16_t test_Encoder;
void task_PowerReadINA(void)
{
    extern State_t Sentry_State;
    while (1)
    {
//        if(Sentry_State.Chassis_Mode != Chassis_SLEEP)
//        {   
            INA_READ_Current();
            INA_READ_Vol();
            INA_READ_Power();
            
            test_Encoder = Read_Encoder();
//        }
        vTaskDelay(10);                     
    }
}

