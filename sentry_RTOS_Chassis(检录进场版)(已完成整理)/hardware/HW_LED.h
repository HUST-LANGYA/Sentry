#ifndef __HW_LED_H
#define __HW_LED_H

typedef struct
{
	GPIO_TypeDef* gpioName;
	uint16_t gpioPin;
}LED_struct;

//×¢£º¹²Ñô¼«
#define LED_on(x)       GPIO_ResetBits(x.gpioName,x.gpioPin)
#define LED_off(x)      GPIO_SetBits(x.gpioName,x.gpioPin)
#define LED_all_on()    GPIO_ResetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14);GPIO_ResetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9)
#define LED_all_off()   GPIO_SetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14);GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_9)

void LED_Config(void);
void LED_toggle(LED_struct x);
void LED_all_toggle(void);

#endif //__HW_LED_H

