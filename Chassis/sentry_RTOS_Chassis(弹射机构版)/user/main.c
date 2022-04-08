#include "main.h"

RCC_ClocksTypeDef rcc;

Robot Sentry;

static void hardware_init(void);

//������
int main(void)
{	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����λȫ��������ռ���ȼ�
    //!��ע�⣺һ����ʼ���� NVIC �����ȼ�������в�������Ӧ�����ٴθ��ġ���
    //�� NVIC ����Ϊ 4 ������£���ռ���ȼ������÷�Χ�� 0-15��
    //��ֵԽС����ռ���ȼ��ļ���Խ��
    
    hardware_init();  //�ϵ��ʼ��
    start_the_very_first_task();   //����������
    vTaskStartScheduler();   //�������������
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

    CAN1_Configuration();
    CAN2_Configuration();
    LED_Config();
	  TIM3_Config_Encoder(); //AB���ȡ
    TIM5_Configuration();  //CPU���
    USART1_Configuration(); // ����Է���CPU״̬����  (Ŀǰ������������Ͳ���ϵͳͨ�ŵĽӿڣ�������F1û�йٷ�����˫����ӿڣ�����ʱû�и�DMA)
    //USART2_Configuration(); // �Ͳ���ϵͳͨ�Ų⴮��
	  USART3_Init();
    GPIO_Config_Phtotelectric_Switch();//��翪������
    GPIO_Config_Limit_Switch();//�г̿�������
    TIM2_Configuration_Timer();//us����ʱTIM
	  //i2c_init();  //ģ��IIC��INA260ͨ��
    ADC_Config();
 //   IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s ��ʱ���
}

TickType_t GetSysCnt()
{
    return xTaskGetTickCount();
}
