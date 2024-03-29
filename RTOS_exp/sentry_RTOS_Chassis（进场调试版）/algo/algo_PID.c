#include "main.h"
#include "algo_PID.h"

/**
  * @brief  位置式PID反馈算法
  * @param  PID_Struct *P  PID参数结构体
  *         ActualValue    PID计算反馈量（当前真实检测值）
  * @retval PID反馈计算输出值
  */
float PID_Calc(PID_Typedef *P, float ActualValue)
{
		P->PreError = P->SetPoint - ActualValue;
		P->dError = P->PreError - P->LastError;
	
		P->SumError += P->PreError;
		P->LastError = P->PreError;
		
		if(P->SumError >= P->IMax)
			P->SumError = P->IMax;
		else if(P->SumError <= -P->IMax)
			P->SumError = -P->IMax;
		
		P->POut = P->P * P->PreError;
		P->IOut = P->I * P->SumError;
		P->DOut = P->D * P->dError;
		
		return LIMIT_MAX_MIN( P->POut+P->IOut+P->DOut,P->OutMax,-P->OutMax);
}

