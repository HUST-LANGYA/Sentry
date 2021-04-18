#include "main.h"


int32_t movingTick = 0;//记录半程的时间
uint8_t reverseFlag = 0;//标志是否已反向
uint8_t destinationFlag = 0;//标志单程完成
uint8_t startFlag = 0;//标志开始跑动
void photoelectricSwitch_detect(void)		
{
    // 光电开关的空闲电平是1，触发状态的电平才是0
    
	static uint8_t flag = 0;
    //这个if是一个测试分支
//    if(PHOTOELECTRIC_SWITCH_R == 0 /*|| PHOTOELECTRIC_SWITCH_L == 0*/)
//    { 
//        int a=0;
//    }
    if(PHOTOELECTRIC_SWITCH_R == 1 /*|| PHOTOELECTRIC_SWITCH_L == 0*/)
    { 
        int photoEleSw_a=0;
    }
    

	if(PHOTOELECTRIC_SWITCH_L == 0 && Sentry.Journey <= -(END_POS-2000) && flag == 0)
	{
		Sentry.Journey = -(END_POS-1000);
        
		flag = 1;
	}
	else if(PHOTOELECTRIC_SWITCH_L == 0 && Sentry.Journey >= (END_POS-2000) && flag == 0)
	{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
		Sentry.Journey = (END_POS-1000);
		flag = 1;
	}
    

	if(PHOTOELECTRIC_SWITCH_L == 0 && Sentry.MovingDir == FORWARD && reverseFlag == 0)
    {//反向
        Sentry.MovingDir = BACKWARD;
        reverseFlag = 1;
    }
//    else if(PHOTOELECTRIC_SWITCH_L == 0 && Sentry.MovingDir == BACKWARD && reverseFlag == 0)
//    {//反向
//        Sentry.MovingDir = FORWARD;
//        reverseFlag = 1;
//    }
    
	if(PHOTOELECTRIC_SWITCH_L == 1 /*&& PHOTOELECTRIC_SWITCH_R == 1*/) //空闲
		flag = 0;
}
