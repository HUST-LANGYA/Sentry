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
        //IWDG_Feed();
        RED_light_toggle();
        vTaskDelay(500);
        //IWDG_Feed();
    }
}

