#include "main.h"

RCC_ClocksTypeDef rcc;

Robot Sentry;

static void hardware_init(void);

//������
int main(void)
{	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//�����ж����ȼ��Ĺ����ǳ�����,Ҳ�ǹٷ�ǿ�ҽ����(����λȫ��������ռ���ȼ�)��
    //��ע�⣺һ����ʼ���� NVIC �����ȼ�������в�������Ӧ�����ٴθ��ġ���
    //���� NVIC �����ȼ�����Ϊ 4 ��ʾ֧�� 0-15 ����ռ���ȼ� ��ע�⣬ 0-15 ���� 16 �����𣬰��� 0 ������ ��֧�������ȼ���
    //���������ǿ������һ�㣬�� NVIC ����Ϊ 4 ������£���ռ���ȼ������÷�Χ�� 0-15��
    //��ô��ֵԽС����ռ���ȼ��ļ���Խ�ߣ��� 0 ����������ȼ���15 ����������ȼ��� 
    
    hardware_init();  //�Ϲ�� �ϵ��ʼ��

    start_the_very_first_task();   //���������������������
    vTaskStartScheduler();   //�������������������vans�˾ͣ�
    while (1) {}
}

/**
 *  @brief  ��Ƭ���ϵ�ʱ���е�Ӳ����ʼ��
 *  @param  ��
 *  @retval ��
 */
static void hardware_init(void)
{
	RCC_GetClocksFreq(&rcc);//��ȡʱ���Ƿ�׼ȷ��F105Ϊ72M

//    delay_ms(100);
//    CAN1_Configuration();
//    delay_ms(100);
//    CAN2_Configuration();
//    delay_ms(100);
    LED_Config();
//    delay_ms(100);
	TIM3_Config_Encoder(); //AB���ȡ
//    delay_ms(100);
    TIM5_Configuration();  //CPU���
//    delay_ms(100);
//    USART1_Configuration(); // ����Է���CPU״̬����(�����Ƿ�������ϵͳ����)
//    delay_ms(100);
//  USART2_Configuration(); // �Ͳ���ϵͳͨ�Ų⴮��
//    delay_ms(100);
    //EXTI_Config_Encoder(); // Z�ഥ��
//    delay_ms(100);
//    GPIO_Config_Phtotelectric_Switch();//��翪������
//    delay_ms(100);
//    //GPIO_Configuration_Limit_Switch();//�г̿�������
    TIM2_Config();//��ʱTIM
    i2c_init();  //ģ��IIC��INA260ͨ��
//    delay_ms(100);
    IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s ��ʱ���

}

TickType_t GetSysCnt()
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
 
