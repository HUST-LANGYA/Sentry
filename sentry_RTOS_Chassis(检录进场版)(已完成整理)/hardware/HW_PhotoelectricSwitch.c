#include "main.h"
#include "HW_PhotoelectricSwitch.h"

/**
 * @brief  两侧光电开关的输入引脚配置
 * @param  None
 * @retval None
 */
void GPIO_Config_Phtotelectric_Switch(void)
{
	GPIO_InitTypeDef  gpioInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	gpioInit.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_11;
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&gpioInit);			
}
