#include "main.h"

static void hardware_init(void);

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//即四位全部用于抢占优先级
    //（注意：一旦初始化好 NVIC 的优先级分组后，切不可以在应用中再次更改。）
    //在 NVIC 分组为 4 的情况下，抢占优先级可配置范围是 0-15，
    //数值越小，抢占优先级的级别越高
    
    hardware_init();  //老规矩 上电初始化
    delay_ms(100); 
    start_the_very_first_task();   //这个函数来开启引导任务
    vTaskStartScheduler();   //开启任务调度器（后面vans了就）
    while (1) {;}
}

void hardware_init(void)
{
    TIM2_Configuration();  //延时TIM
    delay_ms(1000);  //等陀螺仪初始化完成
    LED_Configuration();//点上云台状态灯
    //TIM8_Configuration();  //PWM输出给到上摩擦轮   
    TIM5_Configuration();  //TIM5监测cpu运行时间 
    FrictionWheel_Init();
//    USART2_Configuration();  //和PC通信用
    USART3_Configuration();  //和遥控器通信用
    //delay_ms(100);
    UART4_Configuration();  //发送cpu运行状态
    //delay_ms(100);
   CAN_Configuration();
//    IWDG_Config(IWDG_Prescaler_128,3125);
}

///*inline */TickType_t GetSysCnt(void)
//{
//    return xTaskGetTickCount();
//}

