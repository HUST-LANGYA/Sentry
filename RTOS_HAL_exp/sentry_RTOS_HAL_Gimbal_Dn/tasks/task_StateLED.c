#include "main.h"

/**
 *  @brief  ���ݵ��߼��Ľ����״̬��
 *  @param  ��
 *  @retval ��
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

