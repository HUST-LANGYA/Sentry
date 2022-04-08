#include "main.h"
#include "task_PhotoEleSwRead.h"

FtirState_t Ftir_State;

extern uint8_t set_escape_period;  //受击后的巡逻来回数
extern uint8_t cur_escape_period; //当前巡逻的来回数 ，一旦受击后，这个变量就清零，然后如果cur和set有差，则一直巡逻

extern float Actual_Value_L;
extern float Actual_Value_R;
//以右侧红外开关触发来计数

FtirState_t Last_PhotoEle_State,Cur_PhotoEle_State;//这个用来做步数的更新
uint8_t Last_Sw_R_State,Cur_Sw_R_State;//记录一个上升沿来计数
uint8_t Ftir_L,Ftir_R;

/**
 *  @brief  定期读取
 *  @param  无
 *  @retval 无
 */
void task_FtirRead(void)
{
    extern State_t Sentry_State;
    Ftir_State = Ftir_Both_IDLE;
    while (1)
    {
			  Ftir_L = (Actual_Value_L > K_ACCELERATE);
			  Ftir_R = (Actual_Value_R > K_ACCELERATE);
			
        Last_PhotoEle_State = Cur_PhotoEle_State;
        Ftir_State = (Ftir_L == Ftir_Active) ? \
                         (Ftir_R == Ftir_Active ? Ftir_Both_Active : Ftir_Left_Active) : \
                         (Ftir_R == Ftir_Active ? Ftir_Right_Active : Ftir_Both_IDLE);
        Cur_PhotoEle_State = Ftir_State;        
        
        Last_Sw_R_State = Cur_Sw_R_State;
        Cur_Sw_R_State = Ftir_R;
        //if(Last_Sw_R_State==Ftir_Idle && Cur_Sw_R_State==Ftir_Active) cur_escape_period = (cur_escape_period >= set_escape_period?set_escape_period:cur_escape_period+1);
        //每次右侧光电开关触发，则圈数+1
        
        vTaskDelay(20);
    }
}
