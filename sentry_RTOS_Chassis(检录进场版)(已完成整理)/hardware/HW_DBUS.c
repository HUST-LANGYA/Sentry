#include "main.h"

RC_Ctl_t RC_Ctl;

/**
  * @brief  初始化从遥控器传来的参数
  * @param  None
  * @retval None
  */
void reset_remote_control_msg(void)
{
	RC_Ctl.rc.s1 = 2;
	RC_Ctl.rc.s2 = 2;
    
	RC_Ctl.rc.ch0 = 1024;
	RC_Ctl.rc.ch1 = 1024;
	RC_Ctl.rc.ch2 = 1024;
	RC_Ctl.rc.ch3 = 1024;
	
	//  RC_Ctl.mouse.x = 0;
	//  RC_Ctl.mouse.y = 0;
	//  RC_Ctl.mouse.z = 0;
	//  RC_Ctl.mouse.press_l = 0;
	//  RC_Ctl.mouse.press_r = 0;

    //	RC_Ctl.key.w_ = 0;
    //	RC_Ctl.key.s_ = 0;                            
    //	RC_Ctl.key.a_ = 0;
    //	RC_Ctl.key.d_ = 0;
    //	RC_Ctl.key.q_ = 0;
    //	RC_Ctl.key.e_ = 0;
    //	RC_Ctl.key.r_ = 0;
    //	RC_Ctl.key.f_ = 0;
    //	RC_Ctl.key.shift_ = 0;
    //	RC_Ctl.key.ctrl_ = 0;
}

/**
  * @brief  获取遥控器值
  * @param  None
  * @retval 遥控器结构体
  */
/*inline*/ RC_Ctl_t getRCData(void)
{
	return RC_Ctl;
}
