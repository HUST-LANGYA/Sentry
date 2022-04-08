#include "main.h"

static void hardware_init(void);

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����λȫ��������ռ���ȼ�
    //��ע�⣺һ����ʼ���� NVIC �����ȼ�������в�������Ӧ�����ٴθ��ġ���
    //�� NVIC ����Ϊ 4 ������£���ռ���ȼ������÷�Χ�� 0-15��
    //��ֵԽС����ռ���ȼ��ļ���Խ��
    
    hardware_init();  //�Ϲ�� �ϵ��ʼ��
    delay_ms(100); 
    start_the_very_first_task();   //���������������������
    vTaskStartScheduler();   //�������������������vans�˾ͣ�
    while (1) {;}
}

void hardware_init(void)
{
    TIM2_Configuration();  //��ʱTIM
    delay_ms(1000);  //�������ǳ�ʼ�����
    LED_Configuration();//������̨״̬��
    //TIM8_Configuration();  //PWM���������Ħ����   
    TIM5_Configuration();  //TIM5���cpu����ʱ�� 
    FrictionWheel_Init();
//    USART2_Configuration();  //��PCͨ����
    USART3_Configuration();  //��ң����ͨ����
    //delay_ms(100);
    UART4_Configuration();  //����cpu����״̬
    //delay_ms(100);
   CAN_Configuration();
//    IWDG_Config(IWDG_Prescaler_128,3125);
}

///*inline */TickType_t GetSysCnt(void)
//{
//    return xTaskGetTickCount();
//}

