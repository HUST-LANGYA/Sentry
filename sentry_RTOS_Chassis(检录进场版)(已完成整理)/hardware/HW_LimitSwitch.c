#include "main.h"
#include "HW_LimitSwitch.h"

/**
 * @brief  两侧行程开关的输入引脚配置
 * @param  None
 * @retval None
 */
void GPIO_Config_Limit_Switch(void)
{
	GPIO_InitTypeDef gpioInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpioInit.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_14;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&gpioInit);
}
