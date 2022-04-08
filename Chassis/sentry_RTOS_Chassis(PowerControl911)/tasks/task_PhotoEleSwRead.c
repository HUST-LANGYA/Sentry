#include "main.h"
#include "task_PhotoEleSwRead.h"

PhotoEleSWState_t PhotoEle_State;

extern uint8_t set_escape_period;  //�ܻ����Ѳ��������
extern uint8_t cur_escape_period; //��ǰѲ�ߵ������� ��һ���ܻ���������������㣬Ȼ�����cur��set�в��һֱѲ��
//���Ҳ��翪�ش���������

PhotoEleSWState_t Last_PhotoEle_State,Cur_PhotoEle_State;//��������������ĸ���
uint8_t Last_Sw_R_State,Cur_Sw_R_State;//��¼һ��������������
/**
 *  @brief  ���ڶ�ȡ
 *  @param  ��
 *  @retval ��
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
        //ÿ���Ҳ��翪�ش�������Ȧ��+1
        
        vTaskDelay(20);
    }
}
