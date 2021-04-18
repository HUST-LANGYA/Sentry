#include "main.h"
#include "task_PhotoEleSwRead.h"

PhotoEleState_t  PhotoEle_State; 

/**
 *  @brief  定期读取
 *  @param  无
 *  @retval 无
 */
void task_PhotoEleSwRead(void)
{
    extern State_t Sentry_State;
    PhotoEle_State = Both_IDLE;
    while (1)
    {
        if(Sentry_State.Chassis_Mode != Chassis_SLEEP)
        {   
            PhotoEle_State = (PhotoEleSw_L==PhotoEleSw_Active)?                         \
                             (PhotoEleSw_R==PhotoEleSw_Active?Both_Active:Left_Active): \
                             (PhotoEleSw_R==PhotoEleSw_Active?Right_Active:Both_IDLE)   ;
        }
        vTaskDelay(5);                     
    }
}

