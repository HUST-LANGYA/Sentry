#include "main.h"
#include "task_LimitSwRead.h"

LimitSWState_t LimitSw_State;//�г̿���״̬

/**
 *  @brief  ���ڶ�ȡ�г̿���״̬
 *  @param  ��
 *  @retval ��
 */
void task_LimitSwRead(void)
{
    LimitSw_State = Limit_Both_IDLE;
    while (1)
    {
       LimitSw_State = (LimitSw_L == LimitSw_L_Active) ? \
                        (LimitSw_R == LimitSw_R_Active ? Limit_Both_Active : Limit_Left_Active) : \
                        (LimitSw_R == LimitSw_R_Active ? Limit_Right_Active : Limit_Both_IDLE);

        vTaskDelay(10);
    }
}
