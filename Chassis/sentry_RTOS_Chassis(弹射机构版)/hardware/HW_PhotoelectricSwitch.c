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
	gpioInit.GPIO_Mode = /*GPIO_Mode_IPU;*/    GPIO_Mode_IN_FLOATING;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&gpioInit);			
}


uint8_t reverseFlag = 0;//标志是否已反向
uint8_t destinationFlag = 0;//标志单程完成
uint8_t startFlag = 0;//标志开始跑动
//void photoelectricSwitch_detect(void)		
//{
//    // 光电开关的空闲电平是1，触发状态的电平才是0
////    extern Robot Sentry;
////    static uint8_t flag = 0;
////    //这个if是一个测试分支
//////    if(PhotoEleSw_R == 0 /*|| PhotoEleSw_L == 0*/)
//////    { 
//////        int a=0;
//////    }
////    if(PhotoEleSw_R == 1 /*|| PhotoEleSw_L == 0*/)
////    { 
//////        int photoEleSw_a=0;
////    }
////    

////    if(PhotoEleSw_L == 0 && Sentry.Journey <= -(END_POS-2000) && flag == 0)
////    {
////        Sentry.Journey = -(END_POS-1000);
////        
////        flag = 1;
////    }
////    else if(PhotoEleSw_L == 0 && Sentry.Journey >= (END_POS-2000) && flag == 0)
////    {                                      
////        Sentry.Journey = (END_POS-1000);
////        flag = 1;
////    }
////    

////    if(PhotoEleSw_L == 0 && Sentry.MovingDir == FORWARD && reverseFlag == 0)
////    {//反向
////        Sentry.MovingDir = BACKWARD;
////        reverseFlag = 1;
////    }
//////    else if(PhotoEleSw_L == 0 && Sentry.MovingDir == BACKWARD && reverseFlag == 0)
//////    {//反向
//////        Sentry.MovingDir = FORWARD;
//////        reverseFlag = 1;
//////    }
////    
////    if(PhotoEleSw_L == 1 /*&& PhotoEleSw_R == 1*/) //空闲
////        flag = 0;
//}
