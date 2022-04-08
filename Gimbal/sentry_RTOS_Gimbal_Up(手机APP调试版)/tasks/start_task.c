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

//���߼������
#define task_OffetCheck_PRIO 30
#define task_OffetCheck_SIZE 128
TaskHandle_t task_OffetCheck_Handler;


#define START_TASK_PRIO 1
#define START_STK_SIZE 512
TaskHandle_t StartTask_Handler = NULL;

//�����������������
static void start_task(void *pvParameters)
{
    //BaseType_t xReturn = pdPASS;
    
    taskENTER_CRITICAL();//�ٽ�α���

    //״̬���� Status_Act
    xTaskCreate((TaskFunction_t)task_ActionUpdate,   
                (const char *)"task_ActionUpdate",
                (uint16_t)task_ActionUpdate_SIZE,
                (void *)NULL,
                (UBaseType_t)task_ActionUpdate_PRIO,
                (TaskHandle_t *)&task_ActionUpdate_Handler);

    //��̨����
    xTaskCreate((TaskFunction_t)task_GimbalUp,
                (const char *)"task_GimbalUp",
                (uint16_t)task_GimbalUp_SIZE,
                (void *)NULL,
                (UBaseType_t)task_GimbalUp_PRIO,
                (TaskHandle_t *)&task_GimbalUp_Handler);
                
    //�������
    xTaskCreate((TaskFunction_t)task_ShootUp,
                (const char *)"task_ShootUp",
                (uint16_t)task_ShootUp_SIZE,
                (void *)NULL,
                (UBaseType_t)task_ShootUp_PRIO,
                (TaskHandle_t *)&task_ShootUp_Handler);
                
     // ��TX2ͨ�ŵ�����  (���ڷ���̨�ǶȺ�������)
     xTaskCreate((TaskFunction_t)task_CV_DataSend,
                 (const char*)"task_CV_DataSend",
                 (uint16_t)task_CV_DataSend_SIZE,
                 (void*)NULL,
                 (UBaseType_t)task_CV_DataSend_PRIO,
                 (TaskHandle_t*)&task_CV_DataSend_Handler);                

    //��״̬������
    /*xReturn=*/xTaskCreate((TaskFunction_t)task_StateLED,
                (const char*)"task_StateLED",
                (uint16_t)task_StateLED_SIZE,
                (void*)NULL,
                (UBaseType_t)task_StateLED_PRIO,
                (TaskHandle_t*)&task_StateLED_Handler);

     // if(xReturn != pdPASS)  RED_light_toggle(),BLUE_light_toggle();//RED_light_toggle(),BLUE_light_toggle();
                
    //����������
    xTaskCreate((TaskFunction_t)task_ZeroCheck,
                (const char*)"task_ZeroCheck",
                (uint16_t)task_ZeroCheck_SIZE,
                (void*)NULL,
                (UBaseType_t)task_ZeroCheck_PRIO,
                (TaskHandle_t*)&task_ZeroCheck_Handler);                
    //ͨ�ż������    
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

                
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ��
}

void start_the_very_first_task(void)
{
    //������������
    xTaskCreate((TaskFunction_t)start_task,          //������
                (const char *)"start_task",          //��������
                (uint16_t)START_STK_SIZE,            //�����ջ��С
                (void *)NULL,                        //���ݸ��������Ĳ���
                (UBaseType_t)START_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t *)&StartTask_Handler); //������
}
