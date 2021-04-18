/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       start_task.c/h
  * @brief      �������񣬽�һ������������������Դ�������������ȼ�,
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. ���
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */

#include "Start_Task.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

#define task_ActionUpdate_PRIO 30
#define task_ActionUpdate_SIZE 256
TaskHandle_t task_ActionUpdate_Handler;

#define task_GimbalDn_PRIO 30
#define task_GimbalDn_SIZE 512
TaskHandle_t task_GimbalDn_Handler;

#define task_ShootDn_PRIO 30
#define task_ShootDn_SIZE 512
TaskHandle_t task_ShootDn_Handler;

#define task_cVisual_DataSend_PRIO 30
#define task_cVisual_DataSend_SIZE 512
TaskHandle_t task_PcVisual_DataSend_Handler;

#define task_StateLED_PRIO 29
#define task_StateLED_SIZE 128
TaskHandle_t task_StateLED_Handler = NULL;

#define task_ZeroCheck_PRIO 30
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
    xTaskCreate((TaskFunction_t)task_GimbalDn,
                (const char *)"task_GimbalDn",
                (uint16_t)task_GimbalDn_SIZE,
                (void *)NULL,
                (UBaseType_t)task_GimbalDn_PRIO,
                (TaskHandle_t *)&task_GimbalDn_Handler);
                
    //�������
    //! ���ܰ������������� 
    //!    Ħ��������
    //!    �������� 
    //!    ������������
    //!     
    xTaskCreate((TaskFunction_t)task_ShootDn,
                (const char *)"task_ShootDn",
                (uint16_t)task_ShootDn_SIZE,
                (void *)NULL,
                (UBaseType_t)task_ShootDn_PRIO,
                (TaskHandle_t *)&task_ShootDn_Handler);
                
                           
    //������������
    // ! ��û��������� ��ͬ�ķ����������� ����Ҫ�ֳɲ�ͬ��task �Ͼ�Ƶ�ʲ�̫һ��           
    // xTaskCreate((TaskFunction_t)task_DataSend,
    //             (const char*)"task_DataSend",
    //             (uint16_t)task_DataSend_SIZE,
    //             (void*)NULL,
    //             (UBaseType_t)task_DataSend_PRIO,
    //             (TaskHandle_t*)&task_DataSend_Handler);     
                
    // ? ��TX2ͨ�ŵ�����
    // xTaskCreate((TaskFunction_t)task_cVisual_DataSend,
    //             (const char*)"task_cVisual_DataSend",
    //             (uint16_t)task_cVisual_DataSend_SIZE,
    //             (void*)NULL,
    //             (UBaseType_t)task_cVisual_DataSend_PRIO,
    //             (TaskHandle_t*)&task_cVisual_DataSend_Handler);                

    //��״̬������
    /*xReturn=*/xTaskCreate((TaskFunction_t)task_StateLED,
                (const char*)"task_StateLED",
                (uint16_t)task_StateLED_SIZE,
                (void*)NULL,
                (UBaseType_t)task_StateLED_PRIO,
                (TaskHandle_t*)&task_StateLED_Handler);

     // if(xReturn != pdPASS)  RED_light_toggle(),BLUE_light_toggle();//RED_light_toggle(),BLUE_light_toggle();
                
    //����������
    // �� �ҿ�Ӣ�۵Ĵ�������û�аѹ����⵱��һ������������ų�������
    xTaskCreate((TaskFunction_t)task_ZeroCheck,
                (const char*)"task_ZeroCheck",
                (uint16_t)task_ZeroCheck_SIZE,
                (void*)NULL,
                (UBaseType_t)task_ZeroCheck_PRIO,
                (TaskHandle_t*)&task_ZeroCheck_Handler);
                
    //����������
//    xTaskCreate((TaskFunction_t)task_BlockDisconnect,
//                (const char*)"task_BlockDisconnect",
//                (uint16_t)task_BlockDisconnect_SIZE,
//                (void*)NULL,
//                (UBaseType_t)task_BlockDisconnect_PRIO,
//                (TaskHandle_t*)&task_BlockDisconnect_Handler);
                
                
     xTaskCreate((TaskFunction_t)task_UsageCPU,        
                 (const char *)"task_UsageCPU",          
                 (uint16_t)task_UsageCPU_SIZE,            
                 (void *)NULL,                    
                 (UBaseType_t)task_UsageCPU_PRIO,        
                 (TaskHandle_t *)&task_UsageCPU_Handler); 

                
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ��
}

void start_the_very_first_task(void)
{
    //����״̬��ʼ�� ��ԭsystem_Init()��
    reset_remote_control_msg();

    //Sentry_Status.GimbalMode=Gimbal_Up_PC;
    //Sentry_Status.ShootMode=Shoot_Up_PC;


    //������������
    xTaskCreate((TaskFunction_t)start_task,          //������
                (const char *)"start_task",          //��������
                (uint16_t)START_STK_SIZE,            //�����ջ��С
                (void *)NULL,                        //���ݸ��������Ĳ���
                (UBaseType_t)START_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t *)&StartTask_Handler); //������
}
