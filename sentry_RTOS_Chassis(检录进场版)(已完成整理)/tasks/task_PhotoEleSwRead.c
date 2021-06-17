#include "main.h"
#include "task_PhotoEleSwRead.h"

//光电开关状态
PhotoEleSWState_t PhotoEle_State;
PhotoEleSWState_t Last_PhotoEle_State,Cur_PhotoEle_State;//光电开关的状态

//逃跑策略相关
extern uint8_t set_escape_period; //受击后的巡逻来回数
extern uint8_t cur_escape_period; //当前巡逻的来回数 
uint8_t Last_Sw_R_State,Cur_Sw_R_State;//右侧光电开关的状态

/**
 *  @brief  定期读取光电开关的状态
 *  @param  无
 *  @retval 无
 */
void task_PhotoEleSwRead(void)
{
    PhotoEle_State = PhotoEle_Both_IDLE;
    while (1)
    {
        //更新光电开关状态
        Last_PhotoEle_State = Cur_PhotoEle_State;
        PhotoEle_State = (PhotoEleSw_L == PhotoEleSw_Active) ? \
                         (PhotoEleSw_R == PhotoEleSw_Active ? PhotoEle_Both_Active : PhotoEle_Left_Active) : \
                         (PhotoEleSw_R == PhotoEleSw_Active ? PhotoEle_Right_Active : PhotoEle_Both_IDLE);
        Cur_PhotoEle_State = PhotoEle_State;        
        
        //更新右侧光电开关的状态（逃跑计数用）
        Last_Sw_R_State = Cur_Sw_R_State,Cur_Sw_R_State = PhotoEleSw_R;
        if(Last_Sw_R_State==PhotoEleSw_Idle && Cur_Sw_R_State==PhotoEleSw_Active) //右侧光电开关的边沿触发
            cur_escape_period = (cur_escape_period >= set_escape_period)?
                                (set_escape_period):(cur_escape_period+1);//圈数+1
 
        vTaskDelay(20);
    }
}
