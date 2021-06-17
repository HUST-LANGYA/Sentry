#include "main.h"
#include "HW_LED.h"

LED_struct LEDs[4] = { 0 };    //Stores LED Pins  ÓÐËÄ¸öLEDµÆ

void LED_Config(void)
{
    GPIO_InitTypeDef  gpioInit;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    //////On Board//////
    gpioInit.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioInit);
    GPIO_SetBits(GPIOC, GPIO_Pin_13 | GPIO_Pin_14);
    LEDs[0].gpioName = GPIOC;
    LEDs[0].gpioPin = GPIO_Pin_13;  //LED_0
    LEDs[1].gpioName = GPIOC;
    LEDs[1].gpioPin = GPIO_Pin_14;  ///LED_1

    /////Not On Board////////
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpioInit.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpioInit);
    GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
    LEDs[2].gpioName = GPIOB;
    LEDs[2].gpioPin = GPIO_Pin_8;    //LED_2
    LEDs[3].gpioName = GPIOB;
    LEDs[3].gpioPin = GPIO_Pin_9;     //LED_3
}

void LED_toggle(LED_struct x)
{
    if (GPIO_ReadOutputDataBit(x.gpioName, x.gpioPin) == Bit_RESET)
        LED_off(x);
    else LED_on(x);
}

void LED_all_toggle(void)
{
    for (uint8_t i = 0; i < 4; i++) LED_toggle(LEDs[i]);
}
