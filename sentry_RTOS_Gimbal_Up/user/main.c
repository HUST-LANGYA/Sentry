/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       main.c/h
  * @brief      stm32��ʼ���Լ���ʼ����freeRTOS��h�ļ��������ȫ�ֺ궨���Լ�
  *             typedef һЩ������������
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
#include "main.h"


static void hardware_init(void);

int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//�����ж����ȼ��Ĺ����ǳ�����,Ҳ�ǹٷ�ǿ�ҽ����(����λȫ��������ռ���ȼ�)��
    //��ע�⣺һ����ʼ���� NVIC �����ȼ�������в�������Ӧ�����ٴθ��ġ���
    //���� NVIC �����ȼ�����Ϊ 4 ��ʾ֧�� 0-15 ����ռ���ȼ� ��ע�⣬ 0-15 ���� 16 �����𣬰��� 0 ������ ��֧�������ȼ���
    //���������ǿ������һ�㣬�� NVIC ����Ϊ 4 ������£���ռ���ȼ������÷�Χ�� 0-15��
    //��ô��ֵԽС����ռ���ȼ��ļ���Խ�ߣ��� 0 ����������ȼ���15 ����������ȼ��� 
    
    hardware_init();  //�Ϲ�� �ϵ��ʼ��
    delay_ms(100); 
    start_the_very_first_task();   //���������������������
    vTaskStartScheduler();   //�������������������vans�˾ͣ�
    while (1)     
    {
    }
}

static void hardware_init(void)
{
    TIM2_Configuration();  //��ʱTIM
    delay_ms(1000);  //�������ǳ�ʼ�����
    LED_Configuration();//������̨״̬��
    //delay_ms(100);
    TIM5_Configuration();  //TIM5���cpu����ʱ�� 
    delay_ms(100);    
    TIM8_Configuration();  //PWM���������Ħ����   
    //delay_ms(100);
    FrictionWheel_Init();
    USART2_Configuration();  //��PCͨ����
    //delay_ms(100);
    USART3_Configuration();  //��ң����ͨ����
//    delay_ms(100);
//    UART4_Configuration();  //����cpu����״̬
//    delay_ms(100);
    CAN_Configuration();
    //delay_ms(100);

    IWDG_Config(IWDG_Prescaler_64,625);
}

/*inline */TickType_t GetSysCnt(void)
{
    return xTaskGetTickCount();
}

//������һ�ο������Ļ��Ͳ�ȥ������
/**
 * ʹ���˾�̬�����ڴ棬�������������������û�ʵ�֣�������task.c�ļ���������
 * ���ҽ��� configSUPPORT_STATIC_ALLOCATION ����궨��Ϊ 1 ��ʱ�����Ч
 */
//static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];// �������������ջ
//static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];// ��ʱ�������ջ
//static StaticTask_t Idle_Task_TCB;	// ����������ƿ�
//static StaticTask_t Timer_Task_TCB; // ��ʱ��������ƿ�
//    void 
//vApplicationGetTimerTaskMemory(
//    StaticTask_t **ppxTimerTaskTCBBuffer,
//	StackType_t **ppxTimerTaskStackBuffer, 
//	uint32_t *pulTimerTaskStackSize);

//    void 
//vApplicationGetIdleTaskMemory(
//    StaticTask_t **ppxIdleTaskTCBBuffer, 
//    StackType_t **ppxIdleTaskStackBuffer, 
//	uint32_t *pulIdleTaskStackSize);
//    
//    
//    
///**
// * @brief  ��ȡ��������������ջ��������ƿ��ڴ�
// * ppxTimerTaskTCBBuffer	:������ƿ��ڴ�
// * ppxTimerTaskStackBuffer	:�����ջ�ڴ�
// * pulTimerTaskStackSize	:�����ջ��С
// */ 
//void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
//								   StackType_t **ppxIdleTaskStackBuffer, 
//								   uint32_t *pulIdleTaskStackSize)
//{
//	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* ������ƿ��ڴ� */
//	*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* �����ջ�ڴ� */
//	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* �����ջ��С */
//}

///**
// * @brief  ��ȡ��ʱ������������ջ��������ƿ��ڴ�
// * ppxTimerTaskTCBBuffer	:������ƿ��ڴ�
// * ppxTimerTaskStackBuffer	:�����ջ�ڴ�
// * pulTimerTaskStackSize	:�����ջ��С
// */ 
//void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
//									StackType_t **ppxTimerTaskStackBuffer, 
//									uint32_t *pulTimerTaskStackSize)
//{
//	*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* ������ƿ��ڴ� */
//	*ppxTimerTaskStackBuffer=Timer_Task_Stack;/* �����ջ�ڴ� */
//	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* �����ջ��С */
//}
//    

