#include "main.h"
#include "task_BlockDisconnect.h"

//TODO! 这个掉电检测考虑不做成独立的task了
//TODO! 在这个block_disconnect 结构体里面把
//TODO! 需要做掉电检测的的tick和flag放进去，
//TODO! 在那些需要计数的地方，自己加自己的就行了

extern block_disconnect_t block_disconnect;

//static void disconnect_count(void);//掉电时间计数器
//static void disconnect_reset(void);//掉电计数值复位
/**
  * @brief  断线检测
  * @param  None
  * @retval None
  */
void task_BlockDisconnect(void)
{

}

//掉电时间计数器
//static void disconnect_count(void)
//{
//    ;
//}

////掉电计数值复位
//static void disconnect_reset(void)
//{
////    block_disconnect.Bodan_Disconnect_Cnt = 0;  //诸如此类
////    ;
//}

