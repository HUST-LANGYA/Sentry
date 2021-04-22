#include "main.h"

RCC_ClocksTypeDef rcc;

Robot Sentry;

static void hardware_init(void);

//������
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //�����ж����ȼ��Ĺ����ǳ�����,Ҳ�ǹٷ�ǿ�ҽ����(����λȫ��������ռ���ȼ�)��
    //��ע�⣺һ����ʼ���� NVIC �����ȼ�������в�������Ӧ�����ٴθ��ġ���
    //���� NVIC �����ȼ�����Ϊ 4 ��ʾ֧�� 0-15 ����ռ���ȼ� ��ע�⣬ 0-15 ���� 16 �����𣬰��� 0 ������ ��֧�������ȼ���
    //���������ǿ������һ�㣬�� NVIC ����Ϊ 4 ������£���ռ���ȼ������÷�Χ�� 0-15��
    //��ô��ֵԽС����ռ���ȼ��ļ���Խ�ߣ��� 0 ����������ȼ���15 ����������ȼ���

    hardware_init();             //�Ϲ�� �ϵ��ʼ��
    start_the_very_first_task(); //���������������������
    vTaskStartScheduler();       //�������������������vans�˾ͣ�
    while (1)
    {
    }
}

/**
 *  @brief  ��Ƭ���ϵ�ʱ���е�Ӳ����ʼ��
 *  @param  ��
 *  @retval ��
 */
static void hardware_init(void)
{
    RCC_GetClocksFreq(&rcc); //��ȡʱ���Ƿ�׼ȷ��F105Ϊ72M

    //TIM2_Config();//��ʱTIM

    CAN1_Configuration();
    CAN2_Configuration();
    LED_Config();
    TIM3_Config_Encoder();              //AB���ȡ
    TIM5_Configuration();               //CPU���
    USART1_Configuration();             // ����Է���CPU״̬����  (Ŀǰ������������Ͳ���ϵͳͨ�ŵĽӿڣ�������F1û�йٷ�����˫����ӿڣ�����ʱû�и�DMA���ò���)
                                        //  USART2_Configuration(); // �Ͳ���ϵͳͨ�Ų⴮��(����û��������
    TIM1_Config_EncoderZ();             // Z�ഥ��
    GPIO_Config_Phtotelectric_Switch(); //��翪������
                                        //    GPIO_Config_Limit_Switch();//�г̿�������
    TIM2_Configuration_Timer();
    i2c_init(); //ģ��IIC��INA260ͨ��

    //   IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s ��ʱ���
}

TickType_t GetSysCnt()
{
    return xTaskGetTickCount();
}
