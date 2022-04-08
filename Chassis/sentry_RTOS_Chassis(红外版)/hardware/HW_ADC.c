#include "main.h"
//#include "HW_ADC.h"



uint16_t Adc_Value[Max_Num]={0}; //ADC转化结果接收数组
float Actual_Value_L = 0.0f;
float Actual_Value_R = 0.0f;
float Sum_Value_L;
float Sum_Value_R;
float distance;
short Attack_k=800; //受击阀值,应该是可以用来检测是大弹还是小弹丸的打击
extern uint16_t is_Under_Attack;
uint8_t  Attack_Number=0;
//ADC初始化
void ADC_Config(void)
{
    GPIO_InitTypeDef gpio;
	  DMA_InitTypeDef  dma;
	  ADC_InitTypeDef  adc1;
	  NVIC_InitTypeDef nvicInit;
	
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_AFIO,ENABLE);
	  
	  gpio.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5; //注意这里和光电传感器的引脚可能会冲突,调不出来的时候可以看看
    gpio.GPIO_Mode=GPIO_Mode_AIN; //模拟输入
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
		adc1.ADC_NbrOfChannel=2; //扫描2个通道
		ADC_Init(ADC1,&adc1);
		
		ADC_TempSensorVrefintCmd(ENABLE);//温度传感器使用
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
				    //is_Under_Attack=1;  //初步是想着有四个传感器，所以就建立了一个8长度的DMA接收数组来进行任一判断，只要撞击到了一个传感器就可以认为是子弹打击到了
					  //Attack_Numer++;
				}        				//后期可能不需要这么多
			//				Actual_Value = Actual_Value/2*0.0048828125; //(5/1024 = 0.00488)
				Actual_Value_L = (((Sum_Value_L/2)*5)/4096);//还可以
				Actual_Value_R = (((Sum_Value_R/2)*5)/4096);
				Sum_Value_L = 0;
				Sum_Value_R = 0;
			//	distance  = 12*pow(Actual_Value_L,-1.058);//测不对
		    DMA_Cmd(DMA1_Channel1,ENABLE);
		 }
}


