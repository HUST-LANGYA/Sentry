#include "main.h"

RCC_ClocksTypeDef rcc;

Robot Sentry;

static void hardware_init(void);

//主函数
int main(void)
{	
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//这样中断优先级的管理将非常方便,也是官方强烈建议的(即四位全部用于抢占优先级)。
    //（注意：一旦初始化好 NVIC 的优先级分组后，切不可以在应用中再次更改。）
    //设置 NVIC 的优先级分组为 4 表示支持 0-15 级抢占优先级 （注意， 0-15 级是 16 个级别，包含 0 级）， 不支持子优先级。
    //在这里继续强调下这一点，在 NVIC 分组为 4 的情况下，抢占优先级可配置范围是 0-15，
    //那么数值越小，抢占优先级的级别越高，即 0 代表最高优先级，15 代表最低优先级。 
    
    hardware_init();  //老规矩 上电初始化
    start_the_very_first_task();   //这个函数来开启引导任务
    vTaskStartScheduler();   //开启任务调度器（后面vans了就）
    while (1) {}
}

/**
 *  @brief  单片机上电时进行的硬件初始化
 *  @param  无
 *  @retval 无
 */
static void hardware_init(void)
{
	RCC_GetClocksFreq(&rcc);//读取时钟是否准确，F105为72M
    
    //TIM2_Config();//延时TIM

    CAN1_Configuration();
    CAN2_Configuration();
    LED_Config();
	TIM3_Config_Encoder(); //AB相读取
    TIM5_Configuration();  //CPU监测
    USART1_Configuration(); // 向电脑发送CPU状态数据  (目前考虑拿这个做和裁判系统通信的接口，但由于F1没有官方给的双缓冲接口，故暂时没有改DMA，用不了)
//  USART2_Configuration(); // 和裁判系统通信测串口
    //EXTI_Config_Encoder(); // Z相触发
    GPIO_Config_Phtotelectric_Switch();//光电开关配置
//    GPIO_Config_Limit_Switch();//行程开关配置
    TIM2_Configuration_Timer();
	i2c_init();  //模拟IIC和INA260通信

 //   IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s 超时溢出
}

TickType_t GetSysCnt()
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
