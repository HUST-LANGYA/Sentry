#include "main.h"

/**
 * @brief  �����г̿��ص�������������
 * @param  None
 * @retval None
 */

void GPIO_Configuration_Limit_Switch(void)
{
	GPIO_InitTypeDef gpioInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	gpioInit.GPIO_Mode = GPIO_Mode_IPU;//������������Ϊ������ⲿ�����ڿ���ʱ�Ǹߵ�ƽ��
    gpioInit.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_13;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&gpioInit);
}

