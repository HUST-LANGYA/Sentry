#include "main.h"
#include "task_PhotoEleSwRead.h"

PhotoEleSWState_t PhotoEle_State;

/**
 *  @brief  定期读取
 *  @param  无
 *  @retval 无
 */
void task_PhotoEleSwRead(void)
{
    extern State_t Sentry_State;
    PhotoEle_State = PhotoEle_Both_IDLE;
    while (1)
    {
        PhotoEle_State = (PhotoEleSw_L == PhotoEleSw_Active) ? \
                         (PhotoEleSw_R == PhotoEleSw_Active ? PhotoEle_Both_Active : PhotoEle_Left_Active) : \
                         (PhotoEleSw_R == PhotoEleSw_Active ? PhotoEle_Right_Active : PhotoEle_Both_IDLE);

        vTaskDelay(5);
    }
}
