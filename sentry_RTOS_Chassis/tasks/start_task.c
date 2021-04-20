#include "Start_Task.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

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

//#define task_GimbalUp_PRIO 30
//#define task_GimbalUp_SIZE 1024
//TaskHandle_t task_GimbalUp_Handler;
//
//#define task_ShootUp_PRIO 30
//#define task_ShootUp_SIZE 1024
//TaskHandle_t task_ShootUp_Handler;

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

//    //状态更新 Status_Act
//    xTaskCreate((TaskFunction_t)task_ActionUpdate,
//        (const char*)"task_ActionUpdate",
//        (uint16_t)task_ActionUpdate_SIZE,
//        (void*)NULL,
//        (UBaseType_t)task_ActionUpdate_PRIO,
//        (TaskHandle_t*)&task_ActionUpdate_Handler);

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
        
//    //读取光电开关状态的任务
//    xTaskCreate((TaskFunction_t)task_PhotoEleSwRead,
//        (const char*)"task_PhotoEleSwRead",
//        (uint16_t)task_PhotoEleSwRead_SIZE,
//        (void*)NULL,
//        (UBaseType_t)task_PhotoEleSwRead_PRIO,
//        (TaskHandle_t*)&task_PhotoEleSwRead_Handler);
//        
////    //读取行程开关状态的任务
////    xTaskCreate((TaskFunction_t)task_LimitSwRead,
////        (const char*)"task_LimitSwRead",
////        (uint16_t)task_LimitSwRead_SIZE,
////        (void*)NULL,
////        (UBaseType_t)task_LimitSwRead_PRIO,
////        (TaskHandle_t*)&task_LimitSwRead_Handler);
//        
//    //解码裁判系统数据包的任务
//    xTaskCreate((TaskFunction_t)task_JudgeDecode,
//        (const char*)"task_JudgeDecode",
//        (uint16_t)task_JudgeDecode_SIZE,
//        (void*)NULL,
//        (UBaseType_t)task_JudgeDecode_PRIO,
//        (TaskHandle_t*)&task_JudgeDecode_Handler);

//    //    //云台任务
//    //    xTaskCreate((TaskFunction_t)task_GimbalUp,
//    //                (const char *)"task_GimbalUp",
//    //                (uint16_t)task_GimbalUp_SIZE,
//    //                (void *)NULL,
//    //                (UBaseType_t)task_GimbalUp_PRIO,
//    //                (TaskHandle_t *)&task_GimbalUp_Handler);

//        //射击任务
//        //! 可能包括三个子任务： 
//        //!    摩擦轮任务
//        //!    拨弹任务 
//        //!    热量控制任务？
//        //!     
//    //    xTaskCreate((TaskFunction_t)task_ShootUp,
//    //                (const char *)"task_ShootUp",
//    //                (uint16_t)task_ShootUp_SIZE,
//    //                (void *)NULL,
//    //                (UBaseType_t)task_ShootUp_PRIO,
//    //                (TaskHandle_t *)&task_ShootUp_Handler);
//   

    //闪状态灯任务
    /*xReturn=*/xTaskCreate((TaskFunction_t)task_StateLED,
        (const char*)"task_StateLED",
        (uint16_t)task_StateLED_SIZE,
        (void*)NULL,
        (UBaseType_t)task_StateLED_PRIO,
        (TaskHandle_t*)&task_StateLED_Handler);



//    ////过零检测任务
//    //// ！ 我看英雄的代码里面没有把过零检测当作一个单独的任务放出来好像
//    xTaskCreate((TaskFunction_t)task_ZeroCheck,
//                (const char*)"task_ZeroCheck",
//                (uint16_t)task_ZeroCheck_SIZE,
//                (void*)NULL,
//                (UBaseType_t)task_ZeroCheck_PRIO,
//                (TaskHandle_t*)&task_ZeroCheck_Handler);
//    //            
//    ////掉电检测任务
//    //xTaskCreate((TaskFunction_t)task_BlockDisconnect,
//    //            (const char*)"task_BlockDisconnect",
//    //            (uint16_t)task_BlockDisconnect_SIZE,
//    //            (void*)NULL,
//    //            (UBaseType_t)task_BlockDisconnect_PRIO,
//    //            (TaskHandle_t*)&task_BlockDisconnect_Handler);


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
