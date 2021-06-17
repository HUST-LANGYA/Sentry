#include "main.h"
#include "task_PhotoEleSwRead.h"

//��翪��״̬
PhotoEleSWState_t PhotoEle_State;
PhotoEleSWState_t Last_PhotoEle_State,Cur_PhotoEle_State;//��翪�ص�״̬

//���ܲ������
extern uint8_t set_escape_period; //�ܻ����Ѳ��������
extern uint8_t cur_escape_period; //��ǰѲ�ߵ������� 
uint8_t Last_Sw_R_State,Cur_Sw_R_State;//�Ҳ��翪�ص�״̬

/**
 *  @brief  ���ڶ�ȡ��翪�ص�״̬
 *  @param  ��
 *  @retval ��
 */
void task_PhotoEleSwRead(void)
{
    PhotoEle_State = PhotoEle_Both_IDLE;
    while (1)
    {
        //���¹�翪��״̬
        Last_PhotoEle_State = Cur_PhotoEle_State;
        PhotoEle_State = (PhotoEleSw_L == PhotoEleSw_Active) ? \
                         (PhotoEleSw_R == PhotoEleSw_Active ? PhotoEle_Both_Active : PhotoEle_Left_Active) : \
                         (PhotoEleSw_R == PhotoEleSw_Active ? PhotoEle_Right_Active : PhotoEle_Both_IDLE);
        Cur_PhotoEle_State = PhotoEle_State;        
        
        //�����Ҳ��翪�ص�״̬�����ܼ����ã�
        Last_Sw_R_State = Cur_Sw_R_State,Cur_Sw_R_State = PhotoEleSw_R;
        if(Last_Sw_R_State==PhotoEleSw_Idle && Cur_Sw_R_State==PhotoEleSw_Active) //�Ҳ��翪�صı��ش���
            cur_escape_period = (cur_escape_period >= set_escape_period)?
                                (set_escape_period):(cur_escape_period+1);//Ȧ��+1
 
        vTaskDelay(20);
    }
}
