/**************************************************
* �ļ�����:���߼������
* ʱ    ��: 2021��10��10��
* ��    ��: Hellocoded
**************************************************/

#include "main.h"
#include "task_OffetCheck.h"


////�ⲿ����
//extern uint8_t Offet_Check_Flag;
//extern uint8_t is_game_start;
//extern uint8_t aim_flag;
////�ڲ�����
//uint8_t Offet_ComSend[NUMBER_SEND];
//Com_Offet_t Com_Offet;

void Offet_Task(void*pvParameters)
{
   Offet_Reset();

   while(1)
	 {
	   Offet_Judge();
//	   Offet_Send();
		 
		 vTaskDelay(1000);
	 }


}	


