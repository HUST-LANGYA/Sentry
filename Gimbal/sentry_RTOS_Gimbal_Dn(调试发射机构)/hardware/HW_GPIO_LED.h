#ifndef __HW_GPIO_LED_H
#define __HW_GPIO_LED_H



//注：拉高熄灯、拉低开灯
#define RED_light_on()    GPIO_ResetBits(GPIOC,GPIO_Pin_14)
#define BLUE_light_on()   GPIO_ResetBits(GPIOC,GPIO_Pin_13)
#define RED_light_off()   GPIO_SetBits(GPIOC,GPIO_Pin_14)
#define BLUE_light_off()  GPIO_SetBits(GPIOC,GPIO_Pin_13)

#define light_on()        RED_light_on()  , BLUE_light_on()
#define light_off()       RED_light_off() , BLUE_light_off()

////注：拉高熄灯、拉低开灯
//#define RED_light_on()    GPIO_SetBits(GPIOH,GPIO_Pin_12)
//#define BLUE_light_on()   GPIO_SetBits(GPIOH,GPIO_Pin_10)
//#define GREEN_light_on()   GPIO_SetBits(GPIOH,GPIO_Pin_11)
//#define RED_light_off()   GPIO_ResetBits(GPIOH,GPIO_Pin_12)
//#define BLUE_light_off()  GPIO_ResetBits(GPIOH,GPIO_Pin_10)
//#define GREEN_light_off()   GPIO_ResetBits(GPIOH,GPIO_Pin_11)

//#define light_on()        RED_light_on()  , BLUE_light_on() ,GREEN_light_on()
//#define light_off()       RED_light_off() , BLUE_light_off() ,GREEN_light_off()


void LED_Configuration(void);
void light_toggle(void);
void RED_light_toggle(void);
void BLUE_light_toggle(void);
//void GREEN_light_toggle(void);

#endif //__HW_GPIO_LED_H

