#include "main.h"

//TODO! 这个掉电检测考虑不做成独立的task了
//TODO! 在这个block_disconnect 结构体里面把
//TODO! 需要做掉电检测的的tick和flag放进去，
//TODO! 在那些需要计数的地方，自己加自己的就行了

//extern _2006_motor_t BodanMotor;
//extern block_disconnect_t block_disconnect;

static void disconnect_count(void);//掉电时间计数器
static void disconnect_reset(void);//掉电计数值复位
/**
  * @brief  断线检测
  * @param  None
  * @retval None
  */
void task_BlockDisconnect(void)
{    
    disconnect_reset();//初次进入掉电检测任务时执行一次复位
    //下面是借鉴英雄的RTOS代码里面的掉电检测(不过感觉好像有一点问题)
    while(1)
    {
        	
	if(block_disconnect.Bodan_Disconnect_Cnt>100)   
        ;  //执行拨弹掉电的操作
		
	
	/*底盘掉线检测*/
	if(block_disconnect.Chassis_Disconnect_Cnt>150)
		;  //执行底盘掉电检测
	
	/*云台掉线检测*/
	if(block_disconnect.Pitch_Disconnect_Cnt > 100)
		;  //执行pitch电机掉电的操作
    
    disconnect_count();//
    //诸如此类
	
    disconnect_count();//掉电计数增加一次
        //IWDG_Feed();//喂狗，即清空看门狗计数器
		 
    vTaskDelay(10); 	 
    }
}

//掉电时间计数器
static void disconnect_count(void)
{
    ;
}

//掉电计数值复位
static void disconnect_reset(void)
{
    block_disconnect.Bodan_Disconnect_Cnt = 0;  //诸如此类
    ;
}

