#include "main.h"
#include "task_UsageCPU.h"

/**
 *  @brief  ���ڴӴ��ڷ���CPUʹ����
 *  @param  ��
 *  @retval ��
 */
uint8_t CPU_RunInfo[MAX_strLENGTH];		//������������ʱ����Ϣ;
void task_UsageCPU(void)
{
    while(1)
    {  
        //memset(CPU_RunInfo,0,MAX_strLENGTH); //��Ϣ����������
        vTaskList((char *)&CPU_RunInfo); //��ȡ��������ʱ����Ϣ
        printf("%s%-34s%-16s%-18s%-16s%s%s%s%s",                                                \
               "---------------------------------------------------------------------------\r\n",  \
               "Name","State","Priority","Stack","Num","\r\n",                                  \
               CPU_RunInfo,                                                                     \
               "---------------------------------------------------------------------------\r\n");
        vTaskDelay(1000);
        
        memset(CPU_RunInfo,0,MAX_strLENGTH); //��Ϣ����������
        vTaskGetRunTimeStats((char *)&CPU_RunInfo);
        printf("%s%-36s%-34s%-40s%s%s%s",                                                       \
               "------------------------------------------------------------------\r\n",  \
               "Task","Abs Time","% Time","\r\n",                                               \
               CPU_RunInfo,                                                                     \
               "------------------------------------------------------------------\r\n");
        vTaskDelay(1000);
    }
}
