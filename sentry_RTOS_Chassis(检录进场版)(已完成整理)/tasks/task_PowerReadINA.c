#include "main.h"
#include "task_PowerReadINA.h"

/**
 *  @brief  ���ڴ�INA260�϶����ʳ���
 *  @param  ��
 *  @retval ��
 */
void task_PowerReadINA(void)
{
    while (1)
    {
        INA_READ_Current();
        INA_READ_Vol();
        INA_READ_Power();

        vTaskDelay(10);                     
    }
}

