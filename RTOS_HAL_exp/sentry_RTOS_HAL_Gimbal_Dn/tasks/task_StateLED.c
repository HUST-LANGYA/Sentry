#include "main.h"

/**
 *  @brief  根据掉线检测的结果闪状态灯
 *  @param  无
 *  @retval 无
 */
void task_StateLED(void)
{
    while(1)
    {  
        BLUE_light_toggle();
        vTaskDelay(500);
        //delay_ms(1000);    
        RED_light_toggle();
        //delay_ms(1000);
        vTaskDelay(500);
    }
}

