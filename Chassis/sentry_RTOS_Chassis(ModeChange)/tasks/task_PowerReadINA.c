#include "main.h"
#include "task_PowerReadINA.h"

/**
 *  @brief  定期从INA260上读功率出来
 *  @param  无
 *  @retval 无
 */
void task_PowerReadINA(void)
{
    extern State_t Sentry_State;
    while (1)
    {
  
            INA_READ_Current();
            INA_READ_Vol();
            INA_READ_Power();

        vTaskDelay(10);                     
    }
}

