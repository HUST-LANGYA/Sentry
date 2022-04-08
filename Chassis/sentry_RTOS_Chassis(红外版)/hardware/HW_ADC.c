#include "main.h"
//#include "HW_ADC.h"



uint16_t Adc_Value[Max_Num]={0}; //ADCת�������������
float Actual_Value_L = 0.0f;
float Actual_Value_R = 0.0f;
float Sum_Value_L;
float Sum_Value_R;
float distance;
short Attack_k=800; //�ܻ���ֵ,Ӧ���ǿ�����������Ǵ󵯻���С����Ĵ��
extern uint16_t is_Under_Attack;
uint8_t  Attack_Number=0;
//ADC��ʼ��
void ADC_Config(void)
{
    GPIO_InitTypeDef gpio;
	  DMA_InitTypeDef  dma;
	  ADC_InitTypeDef  adc1;
	  NVIC_InitTypeDef nvicInit;
	
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO,ENABLE);
	  
	  gpio.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5; //ע������͹�紫���������ſ��ܻ��ͻ,����������ʱ����Կ���
    gpio.GPIO_Mode=GPIO_Mode_AIN; //ģ������
    gpio.GPIO_Speed=GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA,&gpio);
	  
	  dma.DMA_PeripheralBaseAddr= ADC1_DR_Address;
	  dma.DMA_MemoryBaseAddr=(uint32_t)&Adc_Value;
	  dma.DMA_DIR=DMA_DIR_PeripheralSRC;
	  dma.DMA_BufferSize=Max_Num;
	  dma.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	  dma.DMA_MemoryInc=DMA_MemoryInc_Enable;
	  dma.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	  dma.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	  dma.DMA_Mode=DMA_Mode_Circular;
	  dma.DMA_M2M=DMA_M2M_Disable;
		dma.DMA_Priority=DMA_Priority_High;
		DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
		DMA_Init(DMA1_Channel1,&dma);
		DMA_Cmd(DMA1_Channel1,ENABLE);
		

		nvicInit.NVIC_IRQChannel = DMA1_Channel1_IRQn;
		nvicInit.NVIC_IRQChannelPreemptionPriority = 2;
		nvicInit.NVIC_IRQChannelSubPriority = 0;
		nvicInit.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicInit);

		
		
	  ADC_DeInit(ADC1);
		adc1.ADC_Mode=ADC_Mode_Independent;
		adc1.ADC_ScanConvMode=ENABLE;
		adc1.ADC_ContinuousConvMode=ENABLE;
		adc1.ADC_DataAlign=ADC_DataAlign_Right;
		adc1.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
		adc1.ADC_NbrOfChannel=2; //ɨ��2��ͨ��
		ADC_Init(ADC1,&adc1);
		
		ADC_TempSensorVrefintCmd(ENABLE);//�¶ȴ�����ʹ��
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);	//u_in
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_239Cycles5);	//u_in
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 3, ADC_SampleTime_239Cycles5);	//u_in
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_239Cycles5);
	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		
	  
}


void DMA1_Channel1_IRQHandler(void)
{
     if(DMA_GetFlagStatus(DMA1_IT_TC1)!=RESET)
		 {
		    DMA_Cmd(DMA1_Channel1,DISABLE);
			  DMA_ClearFlag(DMA1_FLAG_TC1);
			  DMA_ClearITPendingBit(DMA_IT_TC);
			// Sum_Value = 0;
			  for(int i=0;i<3;i=i+2)
			  {
					Sum_Value_L += Adc_Value[i];
					Sum_Value_R += Adc_Value[i+1];
				   //if(Adc_Value[i] <= Attack_k)
						// is_Under_Attack = 1;
				    //is_Under_Attack=1;  //�������������ĸ������������Ծͽ�����һ��8���ȵ�DMA����������������һ�жϣ�ֻҪײ������һ���������Ϳ�����Ϊ���ӵ��������
					  //Attack_Numer++;
				}        				//���ڿ��ܲ���Ҫ��ô��
			//				Actual_Value = Actual_Value/2*0.0048828125; //(5/1024 = 0.00488)
				Actual_Value_L = (((Sum_Value_L/2)*5)/4096);//������
				Actual_Value_R = (((Sum_Value_R/2)*5)/4096);
				Sum_Value_L = 0;
				Sum_Value_R = 0;
			//	distance  = 12*pow(Actual_Value_L,-1.058);//�ⲻ��
		    DMA_Cmd(DMA1_Channel1,ENABLE);
		 }
}


