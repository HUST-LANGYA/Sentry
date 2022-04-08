#ifndef __TASK_CV_DATA_SEND_H__
#define __TASK_CV_DATA_SEND_H__

#define Tx2_Off 0xff        //关机模式
#define Tx2_Small_Buff 0x10 //小符模式
#define Tx2_Big_Buff 0x20   //大符模式
#define Tx2_Armor 0x30      //辅瞄模式

void task_CV_DataSend(void *pvParameters);

#endif //__TASK_CV_DATA_SEND_H__
