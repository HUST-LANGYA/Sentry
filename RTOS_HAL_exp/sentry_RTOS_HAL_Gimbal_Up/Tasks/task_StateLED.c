#include "main.h"

/**
 *  @brief  ���ݵ��߼��Ľ����״̬��
 *  @param  ��
 *  @retval ��
 */
//uint32_t blinkTick;//���Ƽ������� 
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

