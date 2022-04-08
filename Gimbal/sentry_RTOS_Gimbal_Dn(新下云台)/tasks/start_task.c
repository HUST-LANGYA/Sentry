#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Start_Task.h"

#define task_ActionUpdate_PRIO 30
#define task_ActionUpdate_SIZE 256
TaskHandle_t task_ActionUpdate_Handler;

#define task_GimbalDn_PRIO 31
#define task_GimbalDn_SIZE 1024
TaskHandle_t task_GimbalDn_Handler;

#define task_ShootDn_PRIO 30
#define task_ShootDn_SIZE 512
TaskHandle_t task_ShootDn_Handler;

#define task_CV_DataSend_PRIO 30
#define task_CV_DataSend_SIZE 512
TaskHandle_t task_CV_DataSend_Handler;

#define task_StateLED_PRIO 29
#define task_StateLED_SIZE 128
TaskHandle_t task_StateLED_Handler = NULL;

#define task_ZeroCheck_PRIO 31
#define task_ZeroCheck_SIZE 1024
TaskHandle_t task_ZeroCheck_Handler;

#define task_BlockDisconnect_PRIO 30
#define task_BlockDisconnect_SIZE 256
TaskHandle_t task_BlockDisconnect_Handler;

#define task_UsageCPU_PRIO 29
#define task_UsageCPU_SIZE 256
TaskHandle_t task_UsageCPU_Handler;


#define START_TASK_PRIO 1
#define START_STK_SIZE 256
TaskHandle_t StartTask_Handler = NULL;


//这个就是引导任务了
static void start_task(void *pvParameters)
{
    //BaseType_t xReturn = pdPASS;
    
    taskENTER_CRITICAL();//临界段保护

    //状态更新 Status_Act
    xTaskCreate((TaskFunction_t)task_ActionUpdate,   
                (const char *)"task_ActionUpdate",
                (uint16_t)task_ActionUpdate_SIZE,
                (void *)NULL,
                (UBaseType_t)task_ActionUpdate_PRIO,
                (TaskHandle_t *)&task_ActionUpdate_Handler);

    //云台任务
    xTaskCreate((TaskFunction_t)task_GimbalDn,
                (const char *)"task_GimbalDn",
                (uint16_t)task_GimbalDn_SIZE,
                (void *)NULL,
                (UBaseType_t)task_GimbalDn_PRIO,
                (TaskHandle_t *)&task_GimbalDn_Handler);
                
    //射击任务
    xTaskCreate((TaskFunction_t)task_ShootDn,
                (const char *)"task_ShootDn",
                (uint16_t)task_ShootDn_SIZE,
                (void *)NULL,
                (UBaseType_t)task_ShootDn_PRIO,
                (TaskHandle_t *)&task_ShootDn_Handler);
                
                           
    //发送数据任务
    // ! 还没有想好这里 不同的发送数据任务 可能要分成不同的task 毕竟频率不太一样           
    // xTaskCreate((TaskFunction_t)task_DataSend,
    //             (const char*)"task_DataSend",
    //             (uint16_t)task_DataSend_SIZE,
    //             (void*)NULL,
    //             (UBaseType_t)task_DataSend_PRIO,
    //             (TaskHandle_t*)&task_DataSend_Handler);     
                
     // 和TX2通信的任务  (发云台角度和正脉冲)
     xTaskCreate((TaskFunction_t)task_CV_DataSend,
                 (const char*)"task_CV_DataSend",
                 (uint16_t)task_CV_DataSend_SIZE,
                 (void*)NULL,
                 (UBaseType_t)task_CV_DataSend_PRIO,
                 (TaskHandle_t*)&task_CV_DataSend_Handler);   

    //闪状态灯任务
    /*xReturn=*/xTaskCreate((TaskFunction_t)task_StateLED,
                (const char*)"task_StateLED",
                (uint16_t)task_StateLED_SIZE,
                (void*)NULL,
                (UBaseType_t)task_StateLED_PRIO,
                (TaskHandle_t*)&task_StateLED_Handler);

     // if(xReturn != pdPASS)  RED_light_toggle(),BLUE_light_toggle();//RED_light_toggle(),BLUE_light_toggle();
                
    //过零检测任务
    xTaskCreate((TaskFunction_t)task_ZeroCheck,
                (const char*)"task_ZeroCheck",
                (uint16_t)task_ZeroCheck_SIZE,
                (void*)NULL,
                (UBaseType_t)task_ZeroCheck_PRIO,
                (TaskHandle_t*)&task_ZeroCheck_Handler);
                
    //掉电检测任务
//    xTaskCreate((TaskFunction_t)task_BlockDisconnect,
//                (const char*)"task_BlockDisconnect",
//                (uint16_t)task_BlockDisconnect_SIZE,
//                (void*)NULL,
//                (UBaseType_t)task_BlockDisconnect_PRIO,
//                (TaskHandle_t*)&task_BlockDisconnect_Handler);
                
                
//     xTaskCreate((TaskFunction_t)task_UsageCPU,        
//                 (const char *)"task_UsageCPU",          
//                 (uint16_t)task_UsageCPU_SIZE,            
//                 (void *)NULL,                    
//                 (UBaseType_t)task_UsageCPU_PRIO,        
//                 (TaskHandle_t *)&task_UsageCPU_Handler); 

                
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界段
}

void start_the_very_first_task(void)
{
    //开启引导任务
    xTaskCreate((TaskFunction_t)start_task,          //任务函数
                (const char *)"start_task",          //任务名称
                (uint16_t)START_STK_SIZE,            //任务堆栈大小
                (void *)NULL,                        //传递给任务函数的参数
                (UBaseType_t)START_TASK_PRIO,        //任务优先级
                (TaskHandle_t *)&StartTask_Handler); //任务句柄
}
