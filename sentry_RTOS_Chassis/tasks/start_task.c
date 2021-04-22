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


//�����������������
static void start_task(void* pvParameters)
{
    //BaseType_t xReturn = pdPASS;

    taskENTER_CRITICAL();//�ٽ�α���

//    //״̬���� Status_Act
//    xTaskCreate((TaskFunction_t)task_ActionUpdate,
//        (const char*)"task_ActionUpdate",
//        (uint16_t)task_ActionUpdate_SIZE,
//        (void*)NULL,
//        (UBaseType_t)task_ActionUpdate_PRIO,
//        (TaskHandle_t*)&task_ActionUpdate_Handler);

    //���̶�������
    xTaskCreate((TaskFunction_t)task_Chassis,
        (const char*)"task_Chassis",
        (uint16_t)task_Chassis_SIZE,
        (void*)NULL,
        (UBaseType_t)task_Chassis_PRIO,
        (TaskHandle_t*)&task_Chassis_Handler);

//    //��ȡ���̹�������
//    xTaskCreate((TaskFunction_t)task_PowerReadINA,
//        (const char*)"task_INAReadINA",
//        (uint16_t)task_PowerReadINA_SIZE,
//        (void*)NULL,
//        (UBaseType_t)task_PowerReadINA_PRIO,
//        (TaskHandle_t*)&task_PowerReadINA_Handler);
        
//    //��ȡ��翪��״̬������
//    xTaskCreate((TaskFunction_t)task_PhotoEleSwRead,
//        (const char*)"task_PhotoEleSwRead",
//        (uint16_t)task_PhotoEleSwRead_SIZE,
//        (void*)NULL,
//        (UBaseType_t)task_PhotoEleSwRead_PRIO,
//        (TaskHandle_t*)&task_PhotoEleSwRead_Handler);
//        
////    //��ȡ�г̿���״̬������
////    xTaskCreate((TaskFunction_t)task_LimitSwRead,
////        (const char*)"task_LimitSwRead",
////        (uint16_t)task_LimitSwRead_SIZE,
////        (void*)NULL,
////        (UBaseType_t)task_LimitSwRead_PRIO,
////        (TaskHandle_t*)&task_LimitSwRead_Handler);
//        
//    //�������ϵͳ���ݰ�������
//    xTaskCreate((TaskFunction_t)task_JudgeDecode,
//        (const char*)"task_JudgeDecode",
//        (uint16_t)task_JudgeDecode_SIZE,
//        (void*)NULL,
//        (UBaseType_t)task_JudgeDecode_PRIO,
//        (TaskHandle_t*)&task_JudgeDecode_Handler);

//    //    //��̨����
//    //    xTaskCreate((TaskFunction_t)task_GimbalUp,
//    //                (const char *)"task_GimbalUp",
//    //                (uint16_t)task_GimbalUp_SIZE,
//    //                (void *)NULL,
//    //                (UBaseType_t)task_GimbalUp_PRIO,
//    //                (TaskHandle_t *)&task_GimbalUp_Handler);

//        //�������
//        //! ���ܰ������������� 
//        //!    Ħ��������
//        //!    �������� 
//        //!    ������������
//        //!     
//    //    xTaskCreate((TaskFunction_t)task_ShootUp,
//    //                (const char *)"task_ShootUp",
//    //                (uint16_t)task_ShootUp_SIZE,
//    //                (void *)NULL,
//    //                (UBaseType_t)task_ShootUp_PRIO,
//    //                (TaskHandle_t *)&task_ShootUp_Handler);
//   

    //��״̬������
    /*xReturn=*/xTaskCreate((TaskFunction_t)task_StateLED,
        (const char*)"task_StateLED",
        (uint16_t)task_StateLED_SIZE,
        (void*)NULL,
        (UBaseType_t)task_StateLED_PRIO,
        (TaskHandle_t*)&task_StateLED_Handler);



//    ////����������
//    //// �� �ҿ�Ӣ�۵Ĵ�������û�аѹ����⵱��һ������������ų�������
//    xTaskCreate((TaskFunction_t)task_ZeroCheck,
//                (const char*)"task_ZeroCheck",
//                (uint16_t)task_ZeroCheck_SIZE,
//                (void*)NULL,
//                (UBaseType_t)task_ZeroCheck_PRIO,
//                (TaskHandle_t*)&task_ZeroCheck_Handler);
//    //            
//    ////����������
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


    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ��
}

void start_the_very_first_task(void)
{
    //������������
    xTaskCreate((TaskFunction_t)start_task,          //������
        (const char*)"start_task",          //��������
        (uint16_t)START_STK_SIZE,            //�����ջ��С
        (void*)NULL,                        //���ݸ��������Ĳ���
        (UBaseType_t)START_TASK_PRIO,        //�������ȼ�
        (TaskHandle_t*)&StartTask_Handler); //������
}
