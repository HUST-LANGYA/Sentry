#include "main.h"

void LED_Configuration(void)
{
    //˫ɫLED �ֱ��� PC13 PC14
    GPIO_InitTypeDef       gpioInit;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    gpioInit.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
    gpioInit.GPIO_Mode = GPIO_Mode_OUT;
	gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
    gpioInit.GPIO_OType = GPIO_OType_PP;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &gpioInit);
    GPIO_ResetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14);  
    
    //13 14 ������ ----  ����   
    //13    ����   ----  �����
    //   14 ����   ----  ������
    //13 14 ������ ----  ����˫��
}

