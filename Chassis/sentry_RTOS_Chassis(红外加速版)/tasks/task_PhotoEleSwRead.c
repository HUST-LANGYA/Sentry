#include "main.h"
#include "task_PhotoEleSwRead.h"

FtirState_t Ftir_State;

extern uint8_t set_escape_period;  //�ܻ����Ѳ��������
extern uint8_t cur_escape_period; //��ǰѲ�ߵ������� ��һ���ܻ���������������㣬Ȼ�����cur��set�в��һֱѲ��

extern float Actual_Value_L;
extern float Actual_Value_R;
//���Ҳ���⿪�ش���������

FtirState_t Last_PhotoEle_State,Cur_PhotoEle_State;//��������������ĸ���
uint8_t Last_Sw_R_State,Cur_Sw_R_State;//��¼һ��������������
uint8_t Ftir_L,Ftir_R;

/**
 *  @brief  ���ڶ�ȡ
 *  @param  ��
 *  @retval ��
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
        //ÿ���Ҳ��翪�ش�������Ȧ��+1
        
        vTaskDelay(20);
    }
}
