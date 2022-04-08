/**************************************************
* 文件内容:掉线检测任务
* 时    间: 2021年10月10日
* 作    者: Hellocoded
**************************************************/

#include "main.h"
#include "task_OffetCheck.h"


////外部变量
//extern uint8_t Offet_Check_Flag;
//extern uint8_t is_game_start;
//extern uint8_t aim_flag;
////内部变量
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


