#include "main.h"



int16_t is_locked = 0;
//extern _2006_motor_t BodanMotor;
block_disconnect_t block_disconnect;   //�����ʱ����Ҫ������Ϣ���У��ڲ�ͬ��task֮�������ֵ

static void disconnect_count(void);//����ʱ�������
static void disconnect_reset(void);//�������ֵ��λ
/**
  * @brief  ���߼��
  * @param  None
  * @retval None
  */
void task_BlockDisconnect(void)
{    
    disconnect_reset();//���ν������������ʱִ��һ�θ�λ
    //�����ǽ��Ӣ�۵�RTOS��������ĵ�����(�����о�������һ������)
    while(1)
    {
        	
	if(block_disconnect.Bodan_Disconnect_Cnt>100)   
        ;  //ִ�в�������Ĳ���
		
	
	/*���̵��߼��*/
	if(block_disconnect.Chassis_Disconnect_Cnt>150)
		;  //ִ�е��̵�����
	
	/*��̨���߼��*/
	if(block_disconnect.Pitch_Disconnect_Cnt > 100)
		;  //ִ��pitch�������Ĳ���
    
    disconnect_count();//
    //�������
	
    disconnect_count();//�����������һ��
        //IWDG_Feed();//ι��������տ��Ź�������
		 
    vTaskDelay(10); 	 
    }
}

//����ʱ�������
static void disconnect_count(void)
{
    ;
}

//�������ֵ��λ
static void disconnect_reset(void)
{
    block_disconnect.Bodan_Disconnect_Cnt = 0;  //�������
    ;
}

