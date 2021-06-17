#include "main.h"
#include "task_LimitSwRead.h"

LimitSWState_t LimitSw_State;//行程开关状态

/**
 *  @brief  定期读取行程开关状态
 *  @param  无
 *  @retval 无
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
