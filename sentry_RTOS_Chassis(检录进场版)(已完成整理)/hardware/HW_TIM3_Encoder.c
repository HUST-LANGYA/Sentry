#include "main.h"
#include "HW_TIM3_Encoder.h"

_encoder_t encoder;//������

void TIM3_Config_Encoder(void) //A��B��
{
    GPIO_InitTypeDef gpioInit;
    TIM_TimeBaseInitTypeDef timTimeBase;
    TIM_ICInitTypeDef timICInit;
    NVIC_InitTypeDef nvicInit;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    timTimeBase.TIM_Prescaler = 0x0;
    timTimeBase.TIM_Period = 65535;
    timTimeBase.TIM_ClockDivision = TIM_CKD_DIV1;
    timTimeBase.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &timTimeBase);
    
    GPIO_StructInit(&gpioInit);
    gpioInit.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //�ֱ�ΪA��B��
    gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPD;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpioInit);

    TIM_DeInit(TIM3);

    TIM_ICStructInit(&timICInit);
    timICInit.TIM_Channel = TIM_Channel_1 | TIM_Channel_2; // �������벶��ͨ��12
    timICInit.TIM_ICFilter = 0x00;                         // IC1F=0011",IC2F="0011" ���벶��1��2���˲���?
    timICInit.TIM_ICPolarity = TIM_ICPolarity_Rising;      // �����ش���
    timICInit.TIM_ICSelection = TIM_ICSelection_DirectTI;  // CC1S="01",CC2S="02",IC1FP1ӳ�䵽TI1,IC2FP2=TI2
    timICInit.TIM_ICPrescaler = TIM_ICPSC_DIV1;            //��Ԥ��Ƶ��������������ϼ�⵽��ÿһ�����ض�����һ�β���
    TIM_ICInit(TIM3, &timICInit);
    
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI2, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    //��һ����������3�����飨д��CCMR1��CC1S��CC2Sλ��д��SMCR��SMSλ��д��CCER��CC1E,CC1P,CC2E,CC2Pλ����
    //CC1ͨ��������Ϊ���룬IC1ӳ����TI1�ϣ�
    //CC2ͨ��������Ϊ���룬IC2ӳ����TI2�ϣ�
    //������������ͨ���������࣬�������ز���
    //ʹ�ñ�����ģʽ3 �C ������һ���źŵ������ƽ����������TI1FP1��TI2FP2�ı�������/�¼����� 

    TIM_ClearFlag(TIM3, TIM_FLAG_Update);      //����������жϡ����������жϡ��ı�־λ
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //ʹ�� �����жϡ������ж�

    nvicInit.NVIC_IRQChannel = TIM3_IRQn;
    nvicInit.NVIC_IRQChannelPreemptionPriority = 3;
    nvicInit.NVIC_IRQChannelSubPriority = 0;
    nvicInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicInit);

    TIM_SetCounter(TIM3, 0x00); //��λTIM3�ļ�������CNT�Ĵ�����
    TIM_Cmd(TIM3, ENABLE);
}

/**
 * @brief  ��λʱ���ȡ����������
 * @param  None
 * @retval ����������
 */

/*inline*/ uint16_t Read_Encoder(void)
{
   // int16_t encoderVal;
   // encoderVal = TIM_GetCounter(TIM3); //��ȡ��������ֵ
   //// TIM_SetCounter(TIM3, 0);           //��������ֵ����
   // return encoderVal;
   
    uint16_t encoderVal = TIM_GetCounter(TIM3);  //������������ֵ
    return encoderVal;
}

void TIM3_IRQHandler(void)
{ 
    if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
    {
        //��һ��������ж����绹�����磬Ȼ���¼period��Ȧ��
        //ע�⣺���Ȧ�����Ǳ�����ת����Ȧ��
        TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
    }
}
