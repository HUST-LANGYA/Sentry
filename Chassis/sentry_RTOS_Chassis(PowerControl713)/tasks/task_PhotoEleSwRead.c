#include "main.h"
#include "task_PhotoEleSwRead.h"

PhotoEleSWState_t PhotoEle_State;

extern uint8_t set_escape_period;  //受击后的巡逻来回数
extern uint8_t cur_escape_period; //当前巡逻的来回数 ，一旦受击后，这个变量就清零，然后如果cur和set有差，则一直巡逻
//以右侧光电开关触发来计数

PhotoEleSWState_t Last_PhotoEle_State,Cur_PhotoEle_State;//这个用来做步数的更新
uint8_t Last_Sw_R_State,Cur_Sw_R_State;//记录一个上升沿来计数
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
        Last_PhotoEle_State = Cur_PhotoEle_State;
        PhotoEle_State = (PhotoEleSw_L == PhotoEleSw_Active) ? \
                         (PhotoEleSw_R == PhotoEleSw_Active ? PhotoEle_Both_Active : PhotoEle_Left_Active) : \
                         (PhotoEleSw_R == PhotoEleSw_Active ? PhotoEle_Right_Active : PhotoEle_Both_IDLE);
        Cur_PhotoEle_State = PhotoEle_State;        
        
        Last_Sw_R_State = Cur_Sw_R_State;
        Cur_Sw_R_State = PhotoEleSw_R;
        if(Last_Sw_R_State==PhotoEleSw_Idle && Cur_Sw_R_State==PhotoEleSw_Active) cur_escape_period = (cur_escape_period >= set_escape_period?set_escape_period:cur_escape_period+1);
        //每次右侧光电开关触发，则圈数+1
        
        vTaskDelay(20);
    }
}
