#include "main.h"

void LED_Configuration(void)
{
    //双色LED 分别是 PC13 PC14
    GPIO_InitTypeDef       gpioInit;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
    gpioInit.GPIO_Mode = GPIO_Mode_OUT;
	gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
    gpioInit.GPIO_OType = GPIO_OType_PP;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &gpioInit);
    GPIO_ResetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14);  
    
    //13 14 都拉高 ----  不亮   
    //13    拉高   ----  红灯亮
    //   14 拉高   ----  蓝灯亮
    //13 14 不拉高 ----  红蓝双亮
}

