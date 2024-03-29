#include "main.h"
#include "algo_HeatControl.h"

//变量定义
//extern JudgeReceive_t JudgeReceive;		//裁判系统接收结构体 热量上限，热量冷却值，当前热量均从裁判系统接收；接收时判断并置位射击完成标志位
//F105_Typedef F105;										//与上层板通信 F105.IsShootAble 为允许射击标志位
extern ext_power_heat_data_t power_heat;
extern ext_game_robot_status_t robot_status;
uint8_t IsShootAble=0,HeatUpdateFlag=0,ShootCpltFlag=0;


uint16_t HeatMax17, HeatCool17;
const short BulletHeat17 = 10;

short CurHeat17, LastHeat17, AvailableHeat17; //当前热量， 上一次热量, 自行计算热量

uint16_t Shooted17Cnt;	//一周期内已打出子弹数
uint16_t AvailableBullet17;	//下一周期允许打弹数

uint8_t ShootAbleFlag;

/**********************************************************************************************************
*函 数 名: HeatControl
*功能说明: 热量控制
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/


void HeatControl(void)
{
	if(HeatUpdateFlag == 1)	//热量更新
	{
		Shooted17Cnt = 0;
		AvailableHeat17 = HeatMax17 - CurHeat17 + HeatCool17;
		if(ShootCpltFlag == 1)	//检测到发弹。为热量更新后打出的子弹
		{
			AvailableHeat17 -= BulletHeat17;	
			ShootCpltFlag = 0;	//已处理完本次发弹
		}
		AvailableBullet17 = AvailableHeat17 / BulletHeat17;
		ShootAbleFlag = (AvailableBullet17 < 1)?0:1;		
	}	
	
	else if((ShootCpltFlag == 1) && (HeatUpdateFlag == 0))	//热量没有更新，但检测到发弹
	{
		ShootCpltFlag = 0;		//已处理完本次发弹
		Shooted17Cnt++;		//发射了一发子弹
		ShootAbleFlag = (Shooted17Cnt >= AvailableBullet17)?0:1;		
	}
}

/**********************************************************************************************************
*函 数 名: HeatUpdate
*功能说明: 热量更新
*形    参: 无
*返 回 值: 无
**********************************************************************************************************/
const float HeatControlThreshold = 0.8f;   	//开启热量控制的阈值

void HeatUpdate(void)
{
	HeatMax17 = robot_status.shooter_id1_17mm_cooling_limit;
	HeatCool17 = robot_status.shooter_id1_17mm_cooling_rate/10;
	CurHeat17 = power_heat.shooter_id1_17mm_cooling_heat;
	
	if(CurHeat17 != LastHeat17)
	{
		HeatUpdateFlag = 1;
		ShootCpltFlag = 0;			//热量更新则将发射标志位清零(没有待处理的打弹)
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
	
	HeatUpdateFlag = 0;		//已处理完本次热量更新
	LastHeat17 = CurHeat17;
	IsShootAble = ShootAbleFlag;
}

