#include "main.h"


int32_t movingTick = 0;//��¼��̵�ʱ��
uint8_t reverseFlag = 0;//��־�Ƿ��ѷ���
uint8_t destinationFlag = 0;//��־�������
uint8_t startFlag = 0;//��־��ʼ�ܶ�
void photoelectricSwitch_detect(void)		
{
    // ��翪�صĿ��е�ƽ��1������״̬�ĵ�ƽ����0
    
	static uint8_t flag = 0;
    //���if��һ�����Է�֧
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
    {//����
        Sentry.MovingDir = BACKWARD;
        reverseFlag = 1;
    }
//    else if(PHOTOELECTRIC_SWITCH_L == 0 && Sentry.MovingDir == BACKWARD && reverseFlag == 0)
//    {//����
//        Sentry.MovingDir = FORWARD;
//        reverseFlag = 1;
//    }
    
	if(PHOTOELECTRIC_SWITCH_L == 1 /*&& PHOTOELECTRIC_SWITCH_R == 1*/) //����
		flag = 0;
}
