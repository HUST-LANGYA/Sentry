#include "main.h"
#include "HW_TIM2_delay.h"

void TIM2_Configuration(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);      //ʹ��TIM2ʱ��
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    
    //ʱ�����ã�
        //ÿ32���������32��period�󣩲���һ��UP�жϣ�
        //�ڸ����ж����棬����һ�ε�DSHOT��16�������ú�
        //f(HSE)=168MHz,����Ԥ��Ƶ��PSC�󣬵õ�f(CNT)=24MHz�ļ�ʱ��Ԫ��
        //Ȼ����1200����ʱ��ԪΪһ�����ڣ�������T=1/f(CNT)*1200=50.0us,
        //������̳������˵��500Hz���Լ���C615������Ҳ����500Hz��һ����
	TIM_TimeBaseInitStruct.TIM_Prescaler=7-1;                               //ʱ���ṹ���Ԥ��Ƶ��24Mhz(168M/7=24M)
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;              //����ģʽ������������
	TIM_TimeBaseInitStruct.TIM_Period=24000-1;                              //f=24MHz/24=1MHz ~ T=1.0us��д��ARR��ȥ��
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;                  //ʱ�ӷ�Ƶ��������Ƶ��
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;                         //�ظ���������д��0��RCR��ȥ�������ظ�����˼��
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);

    TIM_ClearFlag(TIM2,TIM_FLAG_Update);        //����жϱ�־λ
}

/**
  * @brief  Millisecond delay (Ӳ��ʱ)
  * @param  None
  * @retval None
  */
void delay_ms(uint32_t t)
{
    //TIM_SetCounter(TIM5, 0);
    TIM_Cmd(TIM2,ENABLE); //ʹ��TIM2
    while(t--)
    {
        while(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update)!=SET) ;
        TIM_ClearFlag(TIM2,TIM_FLAG_Update);
    }
    TIM_Cmd(TIM2,DISABLE);
}
