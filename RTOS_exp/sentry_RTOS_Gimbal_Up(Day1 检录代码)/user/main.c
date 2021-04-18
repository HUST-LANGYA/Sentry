/**
  ****************************(C) COPYRIGHT 2016 DJI****************************
  * @file       main.c/h
  * @brief      stm32初始化以及开始任务freeRTOS。h文件定义相关全局宏定义以及
  *             typedef 一些常用数据类型
  * @note       
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Dec-26-2018     RM              1. 完成
  *
  @verbatim
  ==============================================================================

  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2016 DJI****************************
  */
#include "main.h"


static void hardware_init(void);

int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//这样中断优先级的管理将非常方便,也是官方强烈建议的(即四位全部用于抢占优先级)。
    //（注意：一旦初始化好 NVIC 的优先级分组后，切不可以在应用中再次更改。）
    //设置 NVIC 的优先级分组为 4 表示支持 0-15 级抢占优先级 （注意， 0-15 级是 16 个级别，包含 0 级）， 不支持子优先级。
    //在这里继续强调下这一点，在 NVIC 分组为 4 的情况下，抢占优先级可配置范围是 0-15，
    //那么数值越小，抢占优先级的级别越高，即 0 代表最高优先级，15 代表最低优先级。 
    
    hardware_init();  //老规矩 上电初始化
    delay_ms(100); 
    start_the_very_first_task();   //这个函数来开启引导任务
    vTaskStartScheduler();   //开启任务调度器（后面vans了就）
    while (1)     
    {
    }
}

static void hardware_init(void)
{
    TIM2_Configuration();  //延时TIM
    delay_ms(1000);  //等陀螺仪初始化完成
    LED_Configuration();//点上云台状态灯
    //delay_ms(100);
    TIM5_Configuration();  //TIM5监测cpu运行时间 
    delay_ms(100);    
    TIM8_Configuration();  //PWM输出给到上摩擦轮   
    //delay_ms(100);
    FrictionWheel_Init();
    USART2_Configuration();  //和PC通信用
    //delay_ms(100);
    USART3_Configuration();  //和遥控器通信用
//    delay_ms(100);
//    UART4_Configuration();  //发送cpu运行状态
//    delay_ms(100);
    CAN_Configuration();
    //delay_ms(100);

    IWDG_Config(IWDG_Prescaler_64,625);
}

/*inline */TickType_t GetSysCnt(void)
{
    return xTaskGetTickCount();
}

//下面这一段看不懂的话就不去管它了
/**
 * 使用了静态分配内存，以下这两个函数是由用户实现，函数在task.c文件中有引用
 * 当且仅当 configSUPPORT_STATIC_ALLOCATION 这个宏定义为 1 的时候才有效
 */
//static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];// 空闲任务任务堆栈
//static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];// 定时器任务堆栈
//static StaticTask_t Idle_Task_TCB;	// 空闲任务控制块
//static StaticTask_t Timer_Task_TCB; // 定时器任务控制块
//    void 
//vApplicationGetTimerTaskMemory(
//    StaticTask_t **ppxTimerTaskTCBBuffer,
//	StackType_t **ppxTimerTaskStackBuffer, 
//	uint32_t *pulTimerTaskStackSize);

//    void 
//vApplicationGetIdleTaskMemory(
//    StaticTask_t **ppxIdleTaskTCBBuffer, 
//    StackType_t **ppxIdleTaskStackBuffer, 
//	uint32_t *pulIdleTaskStackSize);
//    
//    
//    
///**
// * @brief  获取空闲任务的任务堆栈和任务控制块内存
// * ppxTimerTaskTCBBuffer	:任务控制块内存
// * ppxTimerTaskStackBuffer	:任务堆栈内存
// * pulTimerTaskStackSize	:任务堆栈大小
// */ 
//void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
//								   StackType_t **ppxIdleTaskStackBuffer, 
//								   uint32_t *pulIdleTaskStackSize)
//{
//	*ppxIdleTaskTCBBuffer=&Idle_Task_TCB;/* 任务控制块内存 */
//	*ppxIdleTaskStackBuffer=Idle_Task_Stack;/* 任务堆栈内存 */
//	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;/* 任务堆栈大小 */
//}

///**
// * @brief  获取定时器任务的任务堆栈和任务控制块内存
// * ppxTimerTaskTCBBuffer	:任务控制块内存
// * ppxTimerTaskStackBuffer	:任务堆栈内存
// * pulTimerTaskStackSize	:任务堆栈大小
// */ 
//void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, 
//									StackType_t **ppxTimerTaskStackBuffer, 
//									uint32_t *pulTimerTaskStackSize)
//{
//	*ppxTimerTaskTCBBuffer=&Timer_Task_TCB;/* 任务控制块内存 */
//	*ppxTimerTaskStackBuffer=Timer_Task_Stack;/* 任务堆栈内存 */
//	*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;/* 任务堆栈大小 */
//}
//    

