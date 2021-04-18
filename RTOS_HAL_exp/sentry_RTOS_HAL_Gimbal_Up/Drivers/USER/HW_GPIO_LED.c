#include "main.h"
#include "HW_GPIO_LED.h"

/**
 *  @brief  ·ÛµÆ±ä×´Ì¬£¨ÁÁ¸ÄÃð¡¢Ãð¸ÄÁÁ£©
 *  @param  ÎÞ
 *  @retval ÎÞ
 */
void light_toggle(void)
{
    if
     (
       HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_14) == GPIO_PIN_RESET
       && 
       HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_13) == GPIO_PIN_RESET
     ) //·ÛµÆÁÁ
        light_off();
    else if
     (
       HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_14) == GPIO_PIN_RESET
       && 
       HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_13) == GPIO_PIN_RESET
     ) //·ÛµÆÃð
        light_on();
}

/**
 *  @brief  ºìµÆ±ä×´Ì¬£¨ÁÁ¸ÄÃð¡¢Ãð¸ÄÁÁ£©
 *  @param  ÎÞ
 *  @retval ÎÞ
 */
void RED_light_toggle(void)
{
    if(HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_14) == GPIO_PIN_RESET) //ºìµÆÁÁ
        RED_light_off();
    else
        RED_light_on();
}

/**
 *  @brief  À¶µÆ±ä×´Ì¬£¨ÁÁ¸ÄÃð¡¢Ãð¸ÄÁÁ£©
 *  @param  ÎÞ
 *  @retval ÎÞ
 */
void BLUE_light_toggle(void)
{
    if(HAL_GPIO_ReadPin(GPIOC,GPIO_Pin_13) == GPIO_PIN_RESET) //À¶µÆÁÁ
        BLUE_light_off();
    else
        BLUE_light_on();
}



