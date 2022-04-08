#include "main.h"
#include "task_UsageCPU.h"

/**
 *  @brief  定期从串口反馈CPU使用率
 *  @param  无
 *  @retval 无
 */
uint8_t CPU_RunInfo[MAX_strLENGTH];		//保存任务运行时间信息;
void task_UsageCPU(void)
{
    while(1)
    {  
        //memset(CPU_RunInfo,0,MAX_strLENGTH); //信息缓冲区清零
        vTaskList((char *)&CPU_RunInfo); //获取任务运行时间信息
        printf("%s%-34s%-16s%-18s%-16s%s%s%s%s",                                                \
               "---------------------------------------------------------------------------\r\n",  \
               "Name","State","Priority","Stack","Num","\r\n",                                  \
               CPU_RunInfo,                                                                     \
               "---------------------------------------------------------------------------\r\n");
        vTaskDelay(1000);
        
        memset(CPU_RunInfo,0,MAX_strLENGTH); //信息缓冲区清零
        vTaskGetRunTimeStats((char *)&CPU_RunInfo);
        printf("%s%-36s%-34s%-40s%s%s%s",                                                       \
               "------------------------------------------------------------------\r\n",  \
               "Task","Abs Time","% Time","\r\n",                                               \
               CPU_RunInfo,                                                                     \
               "------------------------------------------------------------------\r\n");
        vTaskDelay(1000);
    }
}
