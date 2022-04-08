#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#include "Start_Task.h"

#define task_ActionUpdate_PRIO 31
#define task_ActionUpdate_SIZE 512
TaskHandle_t task_ActionUpdate_Handler;

#define task_GimbalUp_PRIO 30
#define task_GimbalUp_SIZE 1024
TaskHandle_t task_GimbalUp_Handler;

#define task_ShootUp_PRIO 30
#define task_ShootUp_SIZE 1024
TaskHandle_t task_ShootUp_Handler;

#define task_CV_DataSend_PRIO 29  //30
#define task_CV_DataSend_SIZE 512
TaskHandle_t task_CV_DataSend_Handler;

#define task_StateLED_PRIO 29
#define task_StateLED_SIZE 128
TaskHandle_t task_StateLED_Handler = NULL;

#define task_ZeroCheck_PRIO 31
#define task_ZeroCheck_SIZE 256
TaskHandle_t task_ZeroCheck_Handler;

#define task_UsageCPU_PRIO 29
#define task_UsageCPU_SIZE 128
TaskHandle_t task_UsageCPU_Handler;

//掉线检测任务
#define task_OffetCheck_PRIO 30
#define task_OffetCheck_SIZE 128
TaskHandle_t task_OffetCheck_Handler;


#define START_TASK_PRIO 1
#define START_STK_SIZE 512
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
    xTaskCreate((TaskFunction_t)task_GimbalUp,
                (const char *)"task_GimbalUp",
                (uint16_t)task_GimbalUp_SIZE,
                (void *)NULL,
                (UBaseType_t)task_GimbalUp_PRIO,
                (TaskHandle_t *)&task_GimbalUp_Handler);
                
    //射击任务
    xTaskCreate((TaskFunction_t)task_ShootUp,
                (const char *)"task_ShootUp",
                (uint16_t)task_ShootUp_SIZE,
                (void *)NULL,
                (UBaseType_t)task_ShootUp_PRIO,
                (TaskHandle_t *)&task_ShootUp_Handler);
                
     // 和TX2通信的任务  (定期发云台角度和正脉冲)
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
    //通信检测任务    
    xTaskCreate(
								 (TaskFunction_t) Offet_Task,
								 (const char*)" Offet_Task",
								 (uint16_t)task_OffetCheck_SIZE,
								  (void*)NULL,
									(UBaseType_t)task_OffetCheck_PRIO,
									(TaskHandle_t*)&task_OffetCheck_Handler	
								
								);
								
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
