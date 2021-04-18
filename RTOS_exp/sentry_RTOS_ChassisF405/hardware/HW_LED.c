#include "main.h"
#include "HW_LED.h"


LED_struct LEDs[4] = { 0 };    //Stores LED Pins  ÓÐËÄ¸öLEDµÆ

void LED_Config(void)
{
    GPIO_InitTypeDef  gpioInit;

    //////On Board//////
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    gpioInit.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
    gpioInit.GPIO_Mode = GPIO_Mode_OUT;
	gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
    gpioInit.GPIO_OType = GPIO_OType_PP;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &gpioInit);
    GPIO_ResetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14);  
    LEDs[0].gpioName = GPIOC;
    LEDs[0].gpioPin = GPIO_Pin_13;  //LED_0

    LEDs[1].gpioName = GPIOC;
    LEDs[1].gpioPin = GPIO_Pin_14;  ///LED_1


    /////Not On Board////////
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    gpioInit.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    gpioInit.GPIO_Mode = GPIO_Mode_OUT;
	gpioInit.GPIO_Speed = GPIO_Speed_100MHz;
    gpioInit.GPIO_OType = GPIO_OType_PP;
	gpioInit.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &gpioInit);
    GPIO_ResetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
    LEDs[2].gpioName = GPIOB;
    LEDs[2].gpioPin = GPIO_Pin_8;    //LED_2

    LEDs[3].gpioName = GPIOB;
    LEDs[3].gpioPin = GPIO_Pin_9;     //LED_3

}


/*inline*/ void LED_toggle(LED_struct x)
{
    if (GPIO_ReadOutputDataBit(x.gpioName, x.gpioPin) == Bit_RESET)
        LED_off(x);
    else LED_on(x);
}

/*inline*/ void LED_all_toggle(void)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        LED_toggle(LEDs[i]);
    }

}

////int16_t divider = 0;
//extern int32_t syscnt;
//int32_t lastsyscnt;
//int16_t Blink_Counter = 0;
//int16_t now_light_judge = 0;
//
//
//void Blink(int16_t BlinkTimes, int16_t LED_ID)
//{
//	
//	int16_t light_judge = 1;
//
//	if(lastsyscnt!=syscnt)
//	{
//		if(syscnt%SHORT_DIVIDER==0)
//		{
//			now_light_judge = !now_light_judge;
//			if(now_light_judge == 1)
//				Blink_Counter++;
//		}
//		lastsyscnt = syscnt;
//	}
//
//	light_judge = now_light_judge;
//
//	if(Blink_Counter > BlinkTimes)
//		light_judge = 0;
//
//	if(light_judge == 1)
//	{
//		 GPIO_ResetBits(LEDarray[LED_ID].gpioName,	LEDarray[LED_ID].gpioPin);
//	}
//	else
//	{
//		 GPIO_SetBits(LEDarray[LED_ID].gpioName,	LEDarray[LED_ID].gpioPin);
//	}
//	if(syscnt % LONG_DIVIDER==0)
//		Blink_Counter = 0;
//}
