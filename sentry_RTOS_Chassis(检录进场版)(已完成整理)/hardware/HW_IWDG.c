/**********************************************************************************************************
 * @文件     iwdg.c
 * @说明     看门狗初始化
 * @版本  	 V1.0
 * @作者     黄志雄
 * @日期     2020.1
**********************************************************************************************************/
#include "main.h"
#include "HW_IWDG.h"
/*
 *        独立看门狗使用LSI作为时钟。
 *        LSI 的频率一般在 30~60KHZ 之间，根据温度和工作场合会有一定的漂移，我
 *        们一般取 40KHZ，所以独立看门狗的定时时间不一定非常精确，只适用于对时间精度
 *        要求比较低的场合。
 *
 * rlv:重装载寄存器的值，取值范围为：0x0~0xFFF
 * 函数调用举例：
 * IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s 超时溢出
 *                        （64/40）*625 = 1s
 */
/**
 * 函 数 名: IWDG_Config
 * 功能说明: 看门狗初始化
 * 形    参: prv  rlv
 * 返 回 值: 无
 */
void IWDG_Config(uint8_t prv,uint16_t rlv)
{
    IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );  // 使能 预分频寄存器PR和重装载寄存器RLR可写 
    IWDG_SetPrescaler( prv );  // 设置预分频器值
    IWDG_SetReload( rlv );  // 设置重装载寄存器值
    IWDG_ReloadCounter();  // 把重装载寄存器的值放到计数器中
    
    IWDG_Enable();  // 使能 IWDG
}

/**
 * 函 数 名: IWDG_Feed
 * 功能说明: 喂狗
 * 形    参: 无
 * 返 回 值: 无
 */
void IWDG_Feed(void)
{
    // 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
    // 当计数器的值减到0的时候会产生系统复位
    IWDG_ReloadCounter();
}

