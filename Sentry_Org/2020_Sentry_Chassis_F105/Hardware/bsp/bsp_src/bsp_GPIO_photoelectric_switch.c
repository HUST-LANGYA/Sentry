#include "main.h"

/**
 * @brief  两侧行程开关的输入引脚配置
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

//	GPIO_SetBits(GPIOC,GPIO_Pin_4);  //PC4 拉高//一个不知道为什么要拉高的东西 //???你不对劲
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

