#include "main.h"

typedef struct 
{
	GPIO_TypeDef * gpioName;
	uint16_t gpioPin;
}LED_struct;

///////////////////LED////////
static LED_struct LEDarray[4] = {0};    //Stores LED Pins

void LED_Init(void)
{
	//////////LED//////////////////////
	GPIO_InitTypeDef  gpioInit;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
	//////On Board//////
	gpioInit.GPIO_Pin = GPIO_Pin_13;
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&gpioInit);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
	LEDarray[0].gpioName = GPIOC;
	LEDarray[0].gpioPin = GPIO_Pin_13;  ///////////////LED PIN1

	
	gpioInit.GPIO_Pin = GPIO_Pin_14;
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&gpioInit);
	GPIO_SetBits(GPIOC,GPIO_Pin_14);
	LEDarray[1].gpioName = GPIOC;
	LEDarray[1].gpioPin = GPIO_Pin_14;  ///////////////LED PIN2
	     /////////////////////
			 
	/////Not On Board////////
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	gpioInit.GPIO_Pin = GPIO_Pin_8;
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&gpioInit);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	LEDarray[2].gpioName = GPIOB;
	LEDarray[2].gpioPin = GPIO_Pin_8;    ///////////////LED PIN3
	
	gpioInit.GPIO_Pin = GPIO_Pin_9;
	gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&gpioInit);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	LEDarray[3].gpioName = GPIOB;
	LEDarray[3].gpioPin = GPIO_Pin_9;     ///////////////LED PIN4
			////////////////////////

}
//int16_t divider = 0;
extern int32_t syscnt;
int32_t lastsyscnt;
int16_t Blink_Counter = 0;
int16_t now_light_judge = 0;
#define SHORT_DIVIDER  500
#define LONG_DIVIDER  5000

void Blink(int16_t BlinkTimes, int16_t LED_ID)
{
	
	int16_t light_judge = 1;

	if(lastsyscnt!=syscnt)
	{
		if(syscnt%SHORT_DIVIDER==0)
		{
			now_light_judge = !now_light_judge;
			if(now_light_judge == 1)
				Blink_Counter++;
		}
		lastsyscnt = syscnt;
	}

	light_judge = now_light_judge;

	if(Blink_Counter > BlinkTimes)
		light_judge = 0;

	if(light_judge == 1)
	{
		 GPIO_ResetBits(LEDarray[LED_ID].gpioName,	LEDarray[LED_ID].gpioPin);
	}
	else
	{
		 GPIO_SetBits(LEDarray[LED_ID].gpioName,	LEDarray[LED_ID].gpioPin);
	}
	if(syscnt % LONG_DIVIDER==0)
		Blink_Counter = 0;
}
