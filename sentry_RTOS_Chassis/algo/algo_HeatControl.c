#include "main.h"
#include "algo_HeatControl.h"
 
extern tGameInfo JudgeReceive;		//����ϵͳ���սṹ�� �������ޣ�������ȴֵ����ǰ�������Ӳ���ϵͳ���գ�����ʱ�жϲ���λ�����ɱ�־λ
//F105_Typedef F105;										//���ϲ��ͨ�� F105.IsShootAble Ϊ���������־λ
//extern ext_power_heat_data_t power_heat;
//extern ext_game_robot_status_t robot_status;
uint8_t IsShootAble=0,HeatUpdateFlag=0,ShootCpltFlag=0;


uint16_t HeatMax17, HeatCool17;
const short BulletHeat17 = 10;

short CurHeat17, LastHeat17, AvailableHeat17; //��ǰ������ ��һ������, ���м�������

uint16_t Shooted17Cnt;	//һ�������Ѵ���ӵ���
uint16_t AvailableBullet17;	//��һ�����������

uint8_t ShootAbleFlag;

/**********************************************************************************************************
*�� �� ��: HeatControl
*����˵��: ��������
*��    ��: ��
*�� �� ֵ: ��
**********************************************************************************************************/


void HeatControl(void)
{
	if(HeatUpdateFlag == 1)	//��������
	{
		Shooted17Cnt = 0;
		AvailableHeat17 = HeatMax17 - CurHeat17 + HeatCool17;
		if(ShootCpltFlag == 1)	//��⵽������Ϊ�������º������ӵ�
		{
			AvailableHeat17 -= BulletHeat17;	
			ShootCpltFlag = 0;	//�Ѵ����걾�η���
		}
		AvailableBullet17 = AvailableHeat17 / BulletHeat17;
		ShootAbleFlag = (AvailableBullet17 < 1)?0:1;		
	}	
	
	else if((ShootCpltFlag == 1) && (HeatUpdateFlag == 0))	//����û�и��£�����⵽����
	{
		ShootCpltFlag = 0;		//�Ѵ����걾�η���
		Shooted17Cnt++;		//������һ���ӵ�
		ShootAbleFlag = (Shooted17Cnt >= AvailableBullet17)?0:1;		
	}
}

/**********************************************************************************************************
*�� �� ��: HeatUpdate
*����˵��: ��������
*��    ��: ��
*�� �� ֵ: ��
**********************************************************************************************************/
const float HeatControlThreshold = 0.8f;   	//�����������Ƶ���ֵ

void HeatUpdate(void)
{
    HeatMax17 = JudgeReceive.HeatMax17;
    HeatCool17 = JudgeReceive.HeatCool17 / 10;
    CurHeat17 = JudgeReceive.shooterHeat17;
	
	if(CurHeat17 != LastHeat17)
	{
		HeatUpdateFlag = 1;
		ShootCpltFlag = 0;			//���������򽫷����־λ����(û�д�����Ĵ�)
	}
	
	if(CurHeat17 < HeatControlThreshold*HeatMax17)
	{
		ShootAbleFlag = 1;
		ShootCpltFlag = 0;
	}
	else
	{
		if((ShootCpltFlag == 1) || (HeatUpdateFlag == 1))
		HeatControl();
	}
	
	HeatUpdateFlag = 0;		//�Ѵ����걾����������
	LastHeat17 = CurHeat17;
	IsShootAble = ShootAbleFlag;
}

