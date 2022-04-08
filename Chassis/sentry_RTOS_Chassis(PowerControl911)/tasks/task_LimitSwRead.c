#include "main.h"
#include "task_LimitSwRead.h"

LimitSWState_t LimitSw_State;
uint8_t L_state,R_state;

/**
 *  @brief  定期读取行程开关
 *  @param  无
 *  @retval 无
 */

void task_LimitSwRead(void)
{
    extern State_t Sentry_State;
    LimitSw_State = Limit_Both_IDLE;
    L_state = 0;
    R_state = 0;
    while (1)
    {

       //LimitSw_State = Limit_Both_IDLE;
       LimitSw_State = (LimitSw_L == LimitSw_L_Active) ? \
                        (LimitSw_R == LimitSw_R_Active ? Limit_Both_Active : Limit_Left_Active) : \
                        (LimitSw_R == LimitSw_R_Active ? Limit_Right_Active : Limit_Both_IDLE);
      

        L_state = LimitSw_L;
        R_state = LimitSw_R;

        vTaskDelay(60);
    }
}
