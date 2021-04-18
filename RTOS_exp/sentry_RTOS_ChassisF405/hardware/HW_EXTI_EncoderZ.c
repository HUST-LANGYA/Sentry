#include "main.h"
#include "HW_EXTI_EncoderZ.h"

//��δ���Ըõײ��Ƿ�ʱ��ȷ��

extern block_disconnect_t block_disconnect;

/**
 * @brief  �ⲿ�ж����ú���(���������㴥��)
 * @param  None
 * @retval None
 */
void EXTI_Config_Encoder(void)
{
    /*
    �����õ����ⲿ�ж��У�
        �������Ļ�е��λ������
        �����г̿��صĴ���
    */
	GPIO_InitTypeDef gpioInit; 
	EXTI_InitTypeDef extiInit;
	NVIC_InitTypeDef nvicInit;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	
	gpioInit.GPIO_Pin = GPIO_Pin_10; 
	gpioInit.GPIO_Mode = GPIO_Mode_OUT;
	gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
    gpioInit.GPIO_OType = GPIO_OType_PP;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &gpioInit); 
  
	// GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource10);  
    extiInit.EXTI_Line = EXTI_Line10; 
    extiInit.EXTI_Mode = EXTI_Mode_Interrupt; 
    extiInit.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش�������ת����е��λʱ�������ϻ�����һ���ߵ�ƽ������ 
    extiInit.EXTI_LineCmd = ENABLE;
    EXTI_Init(&extiInit); 
    EXTI_GenerateSWInterrupt(EXTI_Line10); 
	
	nvicInit.NVIC_IRQChannel = EXTI15_10_IRQn;
	nvicInit.NVIC_IRQChannelPreemptionPriority =  0;
	nvicInit.NVIC_IRQChannelSubPriority = 0;
	nvicInit.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&nvicInit);
}


int32_t last_cnt=10000;//��ʼ������ֹһ��ʼ��TIM5->CNTδ��ʱ��������
int16_t init_journey_flag=1;
int16_t Journey_direction;


//ע���г̿��ز�Ҫ���ⲿ�жϣ�����ּ�ⲻ�������ߵ������������
///**
// * @brief  �ⲿ�ж����ú���(�г̿���)
// * @param  None
// * @retval None
// */
//void EXTI_Configuration_LimitSwitch(void)
//{
//    /*
//    �����õ����ⲿ�ж��У�
//        �������Ļ�е��λ������
//        �����г̿��صĴ���
//    */
//	GPIO_InitTypeDef gpioInit; 
//	EXTI_InitTypeDef extiInit;
//	NVIC_InitTypeDef nvicInit;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
//	
//	gpioInit.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_13; 
//	gpioInit.GPIO_Mode = GPIO_Mode_IPU;
//	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &gpioInit); 
//  
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3|GPIO_PinSource13);  
//  extiInit.EXTI_Line = EXTI_Line3|EXTI_Line13; 
//  extiInit.EXTI_Mode = EXTI_Mode_Interrupt; 
//  extiInit.EXTI_Trigger = EXTI_Trigger_Falling; 
//  extiInit.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&extiInit); 
//  EXTI_GenerateSWInterrupt(EXTI_Line3|EXTI_Line13); 
//	
//	nvicInit.NVIC_IRQChannel = EXTI3_IRQn;
//	nvicInit.NVIC_IRQChannelPreemptionPriority =  0;
//	nvicInit.NVIC_IRQChannelSubPriority = 1;
//	nvicInit.NVIC_IRQChannelCmd = ENABLE; 
//  NVIC_Init(&nvicInit);
//  nvicInit.NVIC_IRQChannel = EXTI15_10_IRQn;
//	nvicInit.NVIC_IRQChannelPreemptionPriority =  0;
//	nvicInit.NVIC_IRQChannelSubPriority = 1;
//	nvicInit.NVIC_IRQChannelCmd = ENABLE; 
//	NVIC_Init(&nvicInit);
//}



int32_t test_exti;
//		block_disconnect_t* temp = Get_block_t();
///**
// * @brief  �ⲿ�ж�(�г̿��ش���)��������PB3�������ⲿ�жϣ�
// * @param  None
// * @retval None
// */
//void EXTI3_IRQHandler(void)
//{
//	if(EXTI_GetITStatus(EXTI_Line3) != RESET) 
//	{
//        test_exti++;
//		block_disconnect.is_left_limit = 1;
//		EXTI_ClearFlag(EXTI_Line3);
//		//EXTI_ClearITPendingBit(EXTI_Line3);	
//	}
//}


/**
 * @brief  �ⲿ�жϣ��������Ĺ��㣩������(PA10������ⲿ�ж�)
 *         ���ų����ⲿ�жϣ��г̿��ش�����������(PB13������ⲿ�ж�)
 * @param  None
 * @retval None
 */
extern int32_t roundNum;//��¼������ת���˼��λ�е��λ
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line10) != RESET) 
	{//�������Ĺ��㴥�� PA10����
        test_exti++;
        roundNum++;
		EXTI_ClearFlag(EXTI_Line10);
		//EXTI_ClearITPendingBit(EXTI_Line10);	//����������Ч����һ����
	}
    
//    if(EXTI_GetITStatus(EXTI_Line13) != RESET) 
//	{//�г̿��ش��� PB13����
//        test_exti++;
//		block_disconnect.is_right_limit = 1;
//		EXTI_ClearFlag(EXTI_Line13);
//		//EXTI_ClearITPendingBit(EXTI_Line13);	//����������Ч����һ����
//	}
}
