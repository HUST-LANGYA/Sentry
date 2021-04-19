#include "main.h"

/**
 *  @brief  LED灯初始化配置
 *  @param  无
 *  @retval 无
 */
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

/**
 *  @brief  粉灯变状态（亮改灭、灭改亮）
 *  @param  无
 *  @retval 无
 */
void light_toggle(void)
{
    if
     (
       GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_14) == Bit_RESET
       && 
       GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13) == Bit_RESET
     ) //粉灯亮
        light_off();
    else if
     (
       GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_14) == Bit_SET
       && 
       GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13) == Bit_SET
     ) //粉灯灭
        light_on();
}

/**
 *  @brief  红灯变状态（亮改灭、灭改亮）
 *  @param  无
 *  @retval 无
 */
void RED_light_toggle(void)
{
    if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_14) == Bit_RESET) //红灯亮
        RED_light_off();
    else
        RED_light_on();
}

/**
 *  @brief  蓝灯变状态（亮改灭、灭改亮）
 *  @param  无
 *  @retval 无
 */
void BLUE_light_toggle(void)
{
    if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_13) == Bit_RESET) //蓝灯亮
        BLUE_light_off();
    else
        BLUE_light_on();
}



