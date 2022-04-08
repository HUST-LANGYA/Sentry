#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Start_Task.h"

#define task_ActionUpdate_PRIO 31
#define task_ActionUpdate_SIZE 512
TaskHandle_t task_ActionUpdate_Handler;

#define task_Chassis_PRIO 30
#define task_Chassis_SIZE 1024
TaskHandle_t task_Chassis_Handler;

#define task_PhotoEleSwRead_PRIO 30
#define task_PhotoEleSwRead_SIZE 512
TaskHandle_t task_PhotoEleSwRead_Handler;

#define task_LimitSwRead_PRIO 30
#define task_LimitSwRead_SIZE 512
TaskHandle_t task_LimitSwRead_Handler;

#define task_JudgeDecode_PRIO 30
#define task_JudgeDecode_SIZE 1024
TaskHandle_t task_JudgeDecode_Handler;

#define task_StateLED_PRIO 29
#define task_StateLED_SIZE 256
TaskHandle_t task_StateLED_Handler = NULL;

#define task_ZeroCheck_PRIO 30
#define task_ZeroCheck_SIZE 256
TaskHandle_t task_ZeroCheck_Handler;

#define task_BlockDisconnect_PRIO 30
#define task_BlockDisconnect_SIZE 1024
TaskHandle_t task_BlockDisconnect_Handler;

#define task_PowerReadINA_PRIO 29
#define task_PowerReadINA_SIZE 256
TaskHandle_t task_PowerReadINA_Handler;

#define task_UsageCPU_PRIO 29
#define task_UsageCPU_SIZE 512
TaskHandle_t task_UsageCPU_Handler;


#define START_TASK_PRIO 1
#define START_STK_SIZE 512
TaskHandle_t StartTask_Handler = NULL;


//这个就是引导任务了
static void start_task(void* pvParameters)
{
    //BaseType_t xReturn = pdPASS;

    taskENTER_CRITICAL();//临界段保护

    //状态更新 Status_Act
    xTaskCreate((TaskFunction_t)task_ActionUpdate,
        (const char*)"task_ActionUpdate",
        (uint16_t)task_ActionUpdate_SIZE,
        (void*)NULL,
        (UBaseType_t)task_ActionUpdate_PRIO,
        (TaskHandle_t*)&task_ActionUpdate_Handler);

    //底盘动作任务
    xTaskCreate((TaskFunction_t)task_Chassis,
        (const char*)"task_Chassis",
        (uint16_t)task_Chassis_SIZE,
        (void*)NULL,
        (UBaseType_t)task_Chassis_PRIO,
        (TaskHandle_t*)&task_Chassis_Handler);

//    //读取底盘功率任务
//    xTaskCreate((TaskFunction_t)task_PowerReadINA,
//        (const char*)"task_INAReadINA",
//        (uint16_t)task_PowerReadINA_SIZE,
//        (void*)NULL,
//        (UBaseType_t)task_PowerReadINA_PRIO,
//        (TaskHandle_t*)&task_PowerReadINA_Handler);
//        
//    //读取光电开关状态的任务
//    xTaskCreate((TaskFunction_t)task_PhotoEleSwRead,
//        (const char*)"task_PhotoEleSwRead",
//        (uint16_t)task_PhotoEleSwRead_SIZE,
//        (void*)NULL,
//        (UBaseType_t)task_PhotoEleSwRead_PRIO,
//        (TaskHandle_t*)&task_PhotoEleSwRead_Handler);
//        
//    //读取行程开关状态的任务
//    xTaskCreate((TaskFunction_t)task_LimitSwRead,
//        (const char*)"task_LimitSwRead",
//        (uint16_t)task_LimitSwRead_SIZE,
//        (void*)NULL,
//        (UBaseType_t)task_LimitSwRead_PRIO,
//        (TaskHandle_t*)&task_LimitSwRead_Handler);   

    //闪状态灯任务
    /*xReturn=*/xTaskCreate((TaskFunction_t)task_StateLED,
        (const char*)"task_StateLED",
        (uint16_t)task_StateLED_SIZE,
        (void*)NULL,
        (UBaseType_t)task_StateLED_PRIO,
        (TaskHandle_t*)&task_StateLED_Handler);

    //过零检测任务
    xTaskCreate((TaskFunction_t)task_ZeroCheck,
                (const char*)"task_ZeroCheck",
                (uint16_t)task_ZeroCheck_SIZE,
                (void*)NULL,
                (UBaseType_t)task_ZeroCheck_PRIO,
                (TaskHandle_t*)&task_ZeroCheck_Handler);

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
        (const char*)"start_task",          //任务名称
        (uint16_t)START_STK_SIZE,            //任务堆栈大小
        (void*)NULL,                        //传递给任务函数的参数
        (UBaseType_t)START_TASK_PRIO,        //任务优先级
        (TaskHandle_t*)&StartTask_Handler); //任务句柄
}
