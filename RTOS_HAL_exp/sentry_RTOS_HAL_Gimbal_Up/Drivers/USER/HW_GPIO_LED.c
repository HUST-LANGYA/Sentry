#include "main.h"
#include "HW_GPIO_LED.h"

/**
 *  @brief  �۵Ʊ�״̬���������������
 *  @param  ��
 *  @retval ��
 */
void light_toggle(void)
{
    if
     (
       HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_14) == GPIO_PIN_RESET
       && 
       HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_13) == GPIO_PIN_RESET
     ) //�۵���
        light_off();
    else if
     (
       HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_14) == GPIO_PIN_RESET
       && 
       HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_13) == GPIO_PIN_RESET
     ) //�۵���
        light_on();
}

/**
 *  @brief  ��Ʊ�״̬���������������
 *  @param  ��
 *  @retval ��
 */
void RED_light_toggle(void)
{
    if(HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_14) == GPIO_PIN_RESET) //�����
        RED_light_off();
    else
        RED_light_on();
}

/**
 *  @brief  ���Ʊ�״̬���������������
 *  @param  ��
 *  @retval ��
 */
void BLUE_light_toggle(void)
{
    if(HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_13) == GPIO_PIN_RESET) //������
        BLUE_light_off();
    else
        BLUE_light_on();
}



