#include "main.h"

/**
 *  @brief  根据掉线检测的结果闪状态灯
 *  @param  无
 *  @retval 无
 */
//uint32_t blinkTick;//闪灯计数变量 
void task_StateLED(void)
{
    while(1)
    {  
        //if(GetSysCnt() % 1000 ==0 )
          
    //    if(blinkTick % 2000 == 0)
    //        ;
    //    if(blinkTick % 3000 == 0)
    //        ;
    //    if(blinkTick % 500  == 0)
    //        ;
            BLUE_light_toggle();
           vTaskDelay(500);
            //delay_ms(1000);
            
              RED_light_toggle();
            //delay_ms(1000);
            vTaskDelay(500);
            
    }
}

