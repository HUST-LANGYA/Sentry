#include "main.h"

/**
 * @brief  �����г̿��ص�������������
 * @param  None
 * @retval None
 */

void GPIO_Configuration_Phtotelectric_Switch(void)
{
	GPIO_InitTypeDef  gpioInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

//	gpioInit.GPIO_Pin = GPIO_Pin_4;			
//	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
//	gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC,&gpioInit);

//	GPIO_SetBits(GPIOC,GPIO_Pin_4);  //PC4 ����//һ����֪��ΪʲôҪ���ߵĶ��� //???�㲻�Ծ�
//	
	gpioInit.GPIO_Pin = GPIO_Pin_5;
	gpioInit.GPIO_Mode = /*GPIO_Mode_Out_PP;*/GPIO_Mode_IN_FLOATING;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&gpioInit);			

	gpioInit.GPIO_Pin = GPIO_Pin_11;
	gpioInit.GPIO_Mode = /*GPIO_Mode_Out_PP;*/GPIO_Mode_IN_FLOATING;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&gpioInit);
    
    GPIO_SetBits(GPIOC,GPIO_Pin_5|GPIO_Pin_11);
}

