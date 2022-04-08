#include "main.h"
#include "task_Chassis.h"


_3508_motor_t motor_chassis;
_2006_motor_t BodanMotor;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;
extern tGameInfo JudgeReceive;
//初始化相关的标志位
uint8_t chassisFlag_Init=0;
int16_t test_init_flag=0;

//光电开关错误检测标志
uint16_t Photo_Error_Flag=0;

//受击策略相关
extern uint16_t last_remainHP,cur_remainHP;
extern uint16_t is_Under_Attack; //如果受击，则将该变量置于某个单稳态的值
extern uint8_t armor_state;
extern uint8_t last_armor_state;

//功率参数
float maxPower = 30.0f;  //先改成20W
float PowerNow = 0.0f;
float PowerLimit_k=0.8f; //功率限制系数
float PowerBack;
float Chassis_Iset_test=0;
uint16_t remainEnergy;
float Balance_Power;
PID_Typedef PowerLimit;
PID_Typedef PowerLimit_1;

//运动状态变量
MovingDir_t movingDir;//当前行进的方向  1为向右 0为向左  
uint16_t waitingTick;//在端头暂停的时间
uint32_t movingTick;//运动中计数器
int16_t last_encoder_abs,cur_encoder_abs;

//位置融合相关变量
extern LimitSWState_t LimitSw_State;//行程开关状态
extern FtirState_t Ftir_State;  //光电开关状态
uint8_t Ftir_Change_Flag = 0;
extern FtirState_t Last_PhotoEle_State,Cur_PhotoEle_State;//这个用来做步数的更新
extern _encoder_t encoder;
int32_t encoder_distance = 0;//用编码器记录到减速段之前的最大距离
int32_t encoder_round = 0; //记录编码器转过的圈数
    
//受击后逃跑相关 （外部链接）
uint8_t set_escape_period = 4;  //受击后的巡逻来回数
uint8_t cur_escape_period = 0; //当前巡逻的来回数 ，一旦受击后，这个变量就清零，然后如果cur和set有差，则一直巡逻
//以右侧光电开关触发来计数

//模糊检测的时停止相关
uint8_t aim_flag=0;

static void Chassis_Patrol_Act(void);
static void Chassis_RC_Act(void);
static void Chassis_SLEEP_Act(void);
static void Chassis_DEBUG_Act(void);
static void Chassis_Patrol_PID_Cal(void);
static void Chassis_RC_PID_Cal(void);
inline static void Chassis_SLEEP_PID_Cal(void);
static void Chassis_DEBUG_PID_Cal(void);
static void PID_Chassis_Init(void);

void task_Chassis(void* parameter)
{
    //进入任务时执行一次复位
    PID_Chassis_Init(); //第一次执行底盘任务时 给底盘的PID参数进行初始化
    
    //编码器计数初始化部分
    TIM_SetCounter(TIM3,22463);
    encoder.angle_abs = 22463;//假装上电时它在轨道的靠右边
	encoder.zerocheck.CountCycle = 65535; //虽然这里初始化了编码器过零检测的结构体，但经实测，2021赛季的轨道不会溢出int16_t长度，所以过零检测是没有起作用的，注意到这里是65535
	encoder.zerocheck.LastValue = encoder.angle_abs;
	encoder.zerocheck.Circle = 0;
    
    //运动策略初始化部分
    cur_escape_period = set_escape_period; //设定初始时已完成逃跑，即静止不动
    
    while (1)
    {
        //枪口热量控制的部分
        //TODO! 可以把这个热量计算的部分分到一个单独的任务里
        HeatUpdate_0();   //计算枪口热量值，这个函数的输出值，会通过CAN发送到上下云台板上去
        HeatUpdate_2();   //17mm枪管的ID号按偶数增长的
        ShootingHeat_CAN2Send();    //发送给上下云台的热量数据 //现在那个线拔下来了(9月11记)
        
        //编码器读位置融合相关
        encoder.angle_abs = Read_Encoder();   // 从TIM的CNT寄存器更新出新的计步值
        last_encoder_abs = cur_encoder_abs;   // 更新上一次计步值（类似现态和次态，做边沿触发用）
        cur_encoder_abs =  encoder.angle_abs; // 更新这一次计步值
        
        //底盘实时功率读取相关
        extern tGameInfo JudgeReceive;
        extern INA260 INA260_1;
			  INA_READ_Power();
        //if(maxPower != JudgeReceive.MaxPower)  maxPower = JudgeReceive.MaxPower; 
        // 如果当前设定的最大功率不是裁判系统给的最大功率，就更新一次
        // 目的是为了避免出现离线调试时和上场之后的返回数据不同的错误
        PowerNow = INA260_1.Power / 1000.0f + 1.3;  //实测裁判系统读到的功率值比INA260的要高约1.9W
        remainEnergy = JudgeReceive.remainEnergy;   //跟随裁判系统来更新剩余能量
         
        
        //位置融合更新（用光电开关和行程开关的触发来矫正编码器的计数值）
        if(LimitSw_State == Limit_Right_Active)  
            TIM_SetCounter(TIM3,22469),encoder.angle_abs = 22469;  //最右端
        if(LimitSw_State == Limit_Left_Active)
            TIM_SetCounter(TIM3,0),encoder.angle_abs = 0;  //最左端
//        if(Last_PhotoEle_State == Ftir_Both_IDLE 
//        && Cur_PhotoEle_State == Ftir_Right_Active)  
//            TIM_SetCounter(TIM3,21327),encoder.angle_abs = 21327;  //右端光电开关触发
//        if(Last_PhotoEle_State == Ftir_Both_IDLE 
//        && Cur_PhotoEle_State == Ftir_Left_Active)
//            TIM_SetCounter(TIM3,781),encoder.angle_abs = 781;   //左端光电开关触发
//        
//				if(armor_state == 0x30)
//				{
//				   aim_flag = 1;
//				}
        //受击状态更新
//        if(cur_remainHP<last_remainHP && JudgeReceive.hurt_type != 0x04) 
//        {//用hunter_type来判定不是因为底盘超功率而造成的掉血
//            is_Under_Attack = 10000;  //受击状态的保持用单稳态递减计数
//            cur_escape_period = 0;    //逃跑回数的计数用递增计数
//        }
//        if(is_Under_Attack) is_Under_Attack--;  //单稳递减
//              
        //Bodan_CAN1Send(0);
        //根据底盘状态值确定进哪个执行函数
        if (Sentry_State.Chassis_Mode == Chassis_Patrol) Chassis_Patrol_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_RC) Chassis_RC_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_SLEEP) Chassis_SLEEP_Act();
        else if (Sentry_State.Chassis_Mode == Chassis_DEBUG) Chassis_DEBUG_Act();

        vTaskDelay(1);
    }
}

/**
  * @brief  PC模式下底盘的运动
  * @param  None
  * @retval None
  */
uint8_t chassisFlag_Patrol_Init=0; //初始化的标志位
//轨道分段变量
static uint16_t  railThreshold_R;  
static uint16_t  railThreshold_L;
static uint8_t railSeg_Flag; //三个状态，标志是在L段、M段还是R段

//中间段变向相关
uint32_t mid_movingTick_RtoL; //用于中段变向的计数器
uint8_t mid_reverse_flag_RtoL;//标志在中间段是变向前还是变向后
uint32_t mid_movingTick_LtoR; //用于中段变向的计数器
uint8_t mid_reverse_flag_LtoR;//标志在中间段是变向前还是变向后
uint8_t mid_return_init = 0;
uint8_t mid_return_init_flag = 0;
int16_t mid_encoder_abs;

//记录变向时间
uint32_t Start_Time;
uint32_t End_Time;
static void Chassis_Patrol_Act(void)
{
//    //轨道分段变量
//    static uint16_t  railThreshold_R;  
//    static uint16_t  railThreshold_L;
//    static uint8_t railSeg_Flag; //三个状态，标志是在L段、M段还是R段

//    //中间段变向相关
//    static uint32_t mid_movingTick_RtoL; //用于中段变向的计数器
//    static uint8_t mid_reverse_flag_RtoL;//标志在中间段是变向前还是变向后
//    static uint32_t mid_movingTick_LtoR; //用于中段变向的计数器
//    static uint8_t mid_reverse_flag_LtoR;//标志在中间段是变向前还是变向后


    if(chassisFlag_Patrol_Init == 0)
    {    
        railThreshold_L = 4000;
        railThreshold_R = 4000;
        srand(25);  //这个随机数种子初始化和底盘的运动变向有关
        chassisFlag_Patrol_Init=1; 
    }
        
		 

    
    //根据运动状态，做计数器计数
    if(LimitSw_State == Limit_Both_IDLE) 
    {//两边都未触发
//        if(motor_chassis.pid.SetPoint)
//        {//若是正常运动，waitingTick清零，movingTick++
            waitingTick = 0,movingTick++;//}
			      Ftir_Change_Flag = 0;
//        else {//若是人为暂停，则同时清零waitingTick和movingTick
//           waitingTick = 0,movingTick = 0;//}
    }
		else if(Ftir_State == Ftir_Right_Active || Ftir_State == Ftir_Left_Active ) //红外传感触发,负责开始加速的过程
    {
			  if(LimitSw_State != Limit_Both_IDLE)
        waitingTick++,movingTick = 0;
				Ftir_Change_Flag = 1;
    }
		else if(Ftir_State == Ftir_Both_Active ) 
    {//双边触发，考虑为人为暂停情况
        waitingTick = 1,movingTick = 0;//报错！！！！
    }
		
    //行程开关部分
    if(LimitSw_State == Limit_Left_Active || LimitSw_State == Limit_Right_Active )  //行程开关触发
    {
			// Start_Time = xTaskGetTickCount();
			  movingDir = (LimitSw_State == Limit_Left_Active)? (MovingDir_R) : (MovingDir_L);
			  if(Ftir_Change_Flag == 0)     //行程开关触发,但是红外传感没有触发的时候速度设为0
            waitingTick=49;
			  movingTick = 0;
    }
    else if(LimitSw_State == Limit_Both_Active)
		{
		   waitingTick = 1, movingTick = 0;      //报错!!!
		}

//    //更新底盘在轨道上的哪一段
//    if(encoder.angle_abs < railThreshold_L)  railSeg_Flag = RailSeg_L;
//    else if(encoder.angle_abs >= railThreshold_L && encoder.angle_abs <= railThreshold_R)  railSeg_Flag = RailSeg_M;
//    else if(encoder.angle_abs > railThreshold_R)  railSeg_Flag = RailSeg_R;
		
		//错误状态判断
		 Photo_Error_Flag=((Ftir_State==Ftir_Right_Active)&&(railSeg_Flag==RailSeg_M));
    
    //根据各种计数变量的值，改变运动状态
    if(waitingTick<50 && movingTick == 0) 
        motor_chassis.pid.SetPoint = 0;//暂停状态0速
    else if(waitingTick>=50 && movingTick == 0)
        motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);  //从零启动的1速状态 
    else{//阶梯加速，防止单阶跃功率过大导致底盘超功率挂掉
        if(waitingTick == 0 && movingTick > 10)
            motor_chassis.pid.SetPoint = (movingDir)?(-3500):(3500);  //从1速开始再加一次速的2速状态 
        if(waitingTick == 0 && movingTick > 20)
            motor_chassis.pid.SetPoint = (movingDir)?(-4000):(4000);  //从1速开始再加一次速的2速状态
        if(waitingTick == 0 && movingTick > 30)
            motor_chassis.pid.SetPoint = (movingDir)?(-4500):(4500);  //从2速开始再加一次速的3速状态  
        if(waitingTick == 0 && movingTick > 50)
            motor_chassis.pid.SetPoint = (movingDir)?(-5000):(5000);  //从3速开始再加一次速的4速状态 
        if(waitingTick == 0 && movingTick > 60)
            motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);    
        if(waitingTick == 0 && movingTick > 70 /*&& movingTick < 100*/)  
            motor_chassis.pid.SetPoint = (movingDir)?(-5500):(5500);  
        if(waitingTick == 0&& movingTick>100 && movingTick<150)
				{
					motor_chassis.pid.SetPoint = (movingDir)?(-5800):(5800);					
				//	End_Time = xTaskGetTickCount();
				//	End_Time = End_Time - Start_Time;
				}
        //810ms加速段结束后就把这个速度的控制权交给编码器了
    }
    
		//中间段随机变向 
				
//		 if(armor_state == 0x30 /*|| cur_remainHP<300*/)
//   {
//			 mid_return_init_flag = ((last_armor_state == 0x30 && armor_state == 0x30) || (cur_remainHP<300 && last_remainHP < 300))? (1) : (0);
//    
//			   if(ABS(cur_encoder_abs-12000) < 50)
//					 mid_return_init = 1;
//				 if(mid_return_init)
//			   {
//				    mid_encoder_abs = cur_encoder_abs;
//					  railThreshold_R = mid_encoder_abs + 4000;
//					  railThreshold_L = mid_encoder_abs -4000;
//					  mid_return_init = 0;
//					  mid_return_init_flag = 1;
//				 }
//				 
//			 if(mid_return_init_flag)
//		   {
//          if((last_encoder_abs < railThreshold_R)
//          && (cur_encoder_abs >= railThreshold_R)
//          && movingDir == MovingDir_R
//          && !mid_reverse_flag_RtoL)//且在向右运动，没有经过右边的变向点
//        {   
//            mid_movingTick_RtoL=0;  //清空中间段计数器
//					  mid_movingTick_LtoR = 0;
//					  mid_reverse_flag_LtoR = 0;
//            motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000); //先减一段速        
//            mid_reverse_flag_RtoL = 1; //中间段触发置位
//        }
//        if(mid_reverse_flag_RtoL)
//        {
//					 //中间段反向后，阶梯加速
//            mid_movingTick_RtoL++;
//            
//            if(mid_movingTick_RtoL >10 )   { motor_chassis.pid.SetPoint = 0;movingDir = MovingDir_L; }//先变向，再加速
//						if(mid_movingTick_RtoL > 30)  motor_chassis.pid.SetPoint = (movingDir)? (-3000):(3000);
//            if(mid_movingTick_RtoL > 60)  motor_chassis.pid.SetPoint = (movingDir)?(-4200):(4200);      
//						if(mid_movingTick_RtoL > 90)  motor_chassis.pid.SetPoint = (movingDir)?(-5800):(5800);
//        }
//        if(mid_movingTick_RtoL>600) mid_reverse_flag_RtoL = 0,mid_movingTick_RtoL = 0;//中间段变向标志的超时复位
//   
//		
//		 // 变向的实现
//			  if((last_encoder_abs >= railThreshold_L) //这里设置成和14200相近主要是为了能够在躲避打击的时候短距离内实现多个变向
//          && (cur_encoder_abs < railThreshold_L)
//          && movingDir == MovingDir_L
//          && !mid_reverse_flag_LtoR)//且在向左运动
//        {   
//            mid_movingTick_LtoR=0;  //清空中间段计数器
//					  mid_movingTick_RtoL = 0;
//					  mid_reverse_flag_RtoL = 0;
//            motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000); //先减一段速        
//            mid_reverse_flag_LtoR = 1; //中间段触发置位
//        }
//        if(mid_reverse_flag_LtoR)
//        {
//					 //中间段反向后,阶梯加速
//            mid_movingTick_LtoR++;
//            if(mid_movingTick_LtoR >10 )   { motor_chassis.pid.SetPoint = 0;movingDir = MovingDir_R; }//先变向，再加速
//            if(mid_movingTick_LtoR > 30)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);
//            if(mid_movingTick_LtoR > 60)  motor_chassis.pid.SetPoint = (movingDir)?(-4200):(4200);
//            if(mid_movingTick_LtoR > 90)  motor_chassis.pid.SetPoint = (movingDir)?(-5800):(5800)						;
//        }
//        if(mid_movingTick_LtoR>600) mid_reverse_flag_LtoR = 0,mid_movingTick_LtoR = 0;//中间段变向标志的超时复位
//      
//			 }
//		
//  	}
		
    //左右两端靠近立柱段的减速（先用编码器值减速，在用光电开关的触发来反向）
    //if(movingDir == MovingDir_L && railSeg_Flag == RailSeg_L)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000); 
    //if(movingDir == MovingDir_R && railSeg_Flag == RailSeg_R)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000); 
		
    //瞄准的时候停下来
    //if(armor_state == 0x30)   motor_chassis.pid.SetPoint = 0;
//    Chassis_SLEEP_PID_Cal();
    if(is_Under_Attack /*|| cur_escape_period < set_escape_period*/) Chassis_Patrol_PID_Cal(); // is_Under_Attack现在是人为给定的
    else movingTick = 0,Chassis_SLEEP_PID_Cal();
}


/**
  * @brief  遥控模式下底盘的运动(改成检录模式的底盘运动，运动很慢 只考虑光电开关和行程开关的 读取)
  * @param  None
  * @retval None
  */
static void Chassis_RC_Act(void)
{
    static float kChassis = 10.0f;   //摇杆缩放系数
	  
	  RC_Ctl_t RC_Ctl = getRCData();
    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=0;
        chassisFlag_Init=1; 
			  railThreshold_L = 4000;
        railThreshold_R = 18469;
    }
    
		//根据运动状态，做计数器计数
    if(Ftir_State == Ftir_Both_IDLE || LimitSw_State == Limit_Both_IDLE) 
    {//两边都未触发
//        if(motor_chassis.pid.SetPoint)
//        {//若是正常运动，waitingTick清零，movingTick++
            waitingTick = 0,movingTick++;//}
//        else {//若是人为暂停，则同时清零waitingTick和movingTick
//           waitingTick = 0,movingTick = 0;//}
    }
		else if(Ftir_State == Ftir_Right_Active || Ftir_State == Ftir_Left_Active ) //红外传感触发,负责开始加速的过程
    {
			  
        waitingTick++,movingTick = 0;
    }
		else if(Ftir_State == Ftir_Both_Active ) 
    {//双边触发，考虑为人为暂停情况
        waitingTick = 1,movingTick = 0;//报错！！！！
    }
		
    //行程开关部分
    if(LimitSw_State == Limit_Left_Active || LimitSw_State == Limit_Right_Active )  //行程开关触发
    {
			  movingDir = (LimitSw_State == Limit_Left_Active)? (MovingDir_R) : (MovingDir_L);
			  if(Ftir_State == Ftir_Both_IDLE)     //行程开关触发,但是红外传感没有触发的时候速度设为0
            waitingTick=0;
			  movingTick = 0;
    }
    else if(LimitSw_State == Limit_Both_Active)
		{
		   waitingTick = 1, movingTick = 0;      //报错!!!
		}

    
		

    //更新底盘在轨道上的哪一段
    if(encoder.angle_abs < railThreshold_L)  railSeg_Flag = RailSeg_L;
    else if(encoder.angle_abs >= railThreshold_L && encoder.angle_abs <= railThreshold_R)  railSeg_Flag = RailSeg_M;
    else if(encoder.angle_abs > railThreshold_R)  railSeg_Flag = RailSeg_R;
		
		//错误状态判断,这是因为在长时间的运动中会出现有时候碰壁的情况
		 Photo_Error_Flag=((Ftir_State==Ftir_Right_Active)&&(railSeg_Flag==RailSeg_M));
		
    //中间地带直接由遥控器来设定速度的目标值
		//motor_chassis.pid.SetPoint=kChassis*(RC_Ctl.rc.ch0-1024);//ch0控制速度值 
//		 motor_chassis.pid.SetPoint =  (movingDir)?(-1500):(1500);
//		 if(Ftir_State == Ftir_Both_Active ) 
//		 {
//		    motor_chassis.pid.SetPoint =  0;
//		 }
		 
		 //根据各种计数变量的值，改变运动状态
    if(waitingTick<50 && movingTick == 0) 
        motor_chassis.pid.SetPoint = 0;//暂停状态0速
    else if(waitingTick>=50 && movingTick == 0)
        motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500);  //从零启动的1速状态 
    else{//阶梯加速，防止单阶跃功率过大导致底盘超功率挂掉
        if(waitingTick == 0 && movingTick > 10)
            motor_chassis.pid.SetPoint = (movingDir)?(-2000):(2000);  //从1速开始再加一次速的2速状态 
        if(waitingTick == 0 && movingTick > 20)
            motor_chassis.pid.SetPoint = (movingDir)?(-2500):(2500);  //从1速开始再加一次速的2速状态
        if(waitingTick == 0 && movingTick > 30)
            motor_chassis.pid.SetPoint = (movingDir)?(-3200):(3200);  //从2速开始再加一次速的3速状态  
        if(waitingTick == 0 && movingTick > 50)
            motor_chassis.pid.SetPoint = (movingDir)?(-4000):(4000);  //从3速开始再加一次速的4速状态 
        if(waitingTick == 0 && movingTick > 60)
            motor_chassis.pid.SetPoint = (movingDir)?(-4300):(4300);    
        if(waitingTick == 0 && movingTick > 70 /*&& movingTick < 100*/)  
            motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);  
        if(waitingTick == 0&& movingTick>100 && movingTick<150)
					motor_chassis.pid.SetPoint = (movingDir)?(-5800):(5800);					
        //810ms加速段结束后就把这个速度的控制权交给编码器了
    }
    
		 
		//进入两边的区域速度设定值直接设置成3000
		//if(movingDir == MovingDir_L && railSeg_Flag == RailSeg_L)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
    //if(movingDir == MovingDir_R && railSeg_Flag == RailSeg_R)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
		
    //根据运动状态，做计数器计数
//    if(PhotoEle_State == PhotoEle_Both_IDLE) 
//        waitingTick = 0,movingTick++;
//    else if((PhotoEle_State == PhotoEle_Left_Active) || (LimitSw_State == Limit_Left_Active) )  
//        waitingTick++,movingTick = 0,movingDir = MovingDir_R;
//    else if( (PhotoEle_State == PhotoEle_Right_Active) || (LimitSw_State == Limit_Right_Active ) )
//        waitingTick++,movingTick = 0,movingDir = MovingDir_L;
//    else if(PhotoEle_State == PhotoEle_Both_Active ) 
//        waitingTick = 1, movingTick = 0;//报错！！！！


//    //根据计数值，改变运动状态
//    if(waitingTick>0 && waitingTick<100 && movingTick == 0) 
//        motor_chassis.pid.SetPoint = 0;
//    else if(waitingTick>100 && movingTick == 0)
//        motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500);    
//     
//    if(movingDir == MovingDir_L)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
//    if(movingDir == MovingDir_R)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
    
    /*if(is_Under_Attack || cur_escape_period < set_escape_period)*/ Chassis_RC_PID_Cal();
    //else movingTick = 0,Chassis_SLEEP_PID_Cal();
}


inline static void Chassis_SLEEP_Act(void)
{
    Chassis_SLEEP_PID_Cal();
}

int16_t debug_SetPoint = 0;
static void Chassis_DEBUG_Act(void)
{     
	  float Debug_Chassis_k=5.0f;
	  RC_Ctl_t RC_Ctl = getRCData();
	  debug_SetPoint=Debug_Chassis_k*(RC_Ctl.rc.ch0-1024);
    motor_chassis.pid.SetPoint = debug_SetPoint;
    Chassis_DEBUG_PID_Cal();
}

static float EnergyDanger=20;   //缓冲能量危险值
static float Excess_Power=80;  //设置的超限功率//改小了主要是怕限不住啊因为正常的时间太长了
//float Chassis_Iset;
float Chassis_V_Rc_Cal=0.00168f;
float Power_Rc_Cal=0.0f;
static void Chassis_RC_PID_Cal(void)
{
    float Chassis_Iset;
	  extern INA260 INA260_1;
	   
	  motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint,ChassisCurrentLimit , -ChassisCurrentLimit);
    Chassis_Iset = LIMIT_MAX_MIN(PID_Calc( &motor_chassis.pid, motor_chassis.real_speed), ChassisCurrentLimit, -ChassisCurrentLimit);
			
	   //Power_Rc_Cal=Chassis_Iset*Chassis_V_Rc_Cal;
//	  if(JudgeReceive.remainEnergy<20)
//		PowerLimit.SetPoint=JudgeReceive.MaxPower;
//    else
//    PowerLimit.SetPoint=Excess_Power*(JudgeReceive.remainEnergy-EnergyDanger)/(200-EnergyDanger);			
//		
		 
//		while(PowerNow>PowerLimit.SetPoint)  //利用功率环实现功率衰减
//		{
//		     PowerLimit_k+= PID_Calc(&PowerLimit,PowerNow) / PowerLimit.SetPoint;
//			   PowerLimit_k = LIMIT_MAX_MIN(PowerLimit_k,1,0);
//			   Chassis_Iset=LIMIT_MAX_MIN(Chassis_Iset,ChassisCurrentLimit,-ChassisCurrentLimit);
//			   Chassis_Iset=PowerLimit_k*Chassis_Iset;		
//		}
	 
    //底盘3508 限幅+赋值
    //motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint,ChassisCurrentLimit , -ChassisCurrentLimit);
    //Chassis_Iset = LIMIT_MAX_MIN(PID_Calc( &motor_chassis.pid, motor_chassis.real_speed), ChassisCurrentLimit, -ChassisCurrentLimit);

    Chassis_CAN1Send(Chassis_Iset);
}
//uint16_t Power_De_Time=0;
float Chassis_V=0.024f;
float Chassis_V_Cal=0.0168f;
uint16_t Deline_Flag=0;
short Power_Time=0;
float Power_Cal=0.0f;
float Debug_Iset=0.0f;
static void Chassis_Patrol_PID_Cal(void)
{ 
	       float Chassis_Iset;
	       //short Power_De_Time=0;  //内部变量
	  extern INA260 INA260_1;
	  //motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint,ChassisCurrentLimit , -ChassisCurrentLimit);
    Chassis_Iset = LIMIT_MAX_MIN(PID_Calc( &motor_chassis.pid, motor_chassis.real_speed), ChassisCurrentLimit, -ChassisCurrentLimit);
	
	  //if(JudgeReceive.remainEnergy<20)
	  if(remainEnergy<20) //功率上限值选择
		{
		PowerLimit.SetPoint=18.0f;
		}
		
	  //else if(movingTick<=100||mid_movingTick_RtoL<=150||mid_movingTick_LtoR<=150)  //这些需要根据缓冲能量来进一步确定看加速度确定为多少合适
		else if(motor_chassis.real_speed<6000&&motor_chassis.real_speed>-6000)
		{
		PowerLimit.SetPoint=Excess_Power*(remainEnergy-EnergyDanger)/(200-EnergyDanger);
		}
		else if(motor_chassis.real_speed>6000||motor_chassis.real_speed<-6000)
    {			
		PowerLimit.SetPoint=(maxPower*0.9f); //匀速段的功率要求
		}
	  Chassis_Iset_test = PowerLimit.SetPoint/(Chassis_V); //有个问题就是实际的电流并不等于速度环的输出值
    //else
    //PowerLimit.SetPoint=Excess_Power*(JudgeReceive.remainEnergy-EnergyDanger)/(200-EnergyDanger);			
	   //if(movingTick>100||mid_movingTick_RtoL>150||mid_movingTick_LtoR>150)  //测试一下匀速段的功率情况
//		if(motor_chassis.real_speed>5200||motor_chassis.real_speed<-5200) //这个主要是为了观察匀速段的功率使用情况
//			 Balance_Power=PowerNow;                                             
//		 else
//			 Balance_Power=30.0f;   //0.9*30.0
		 
		  //Power_Cal=Chassis_Iset*Chassis_V_Cal;   //自行计算功率值
		//while(PowerNow>PowerLimit_1.SetPoint&&Power_De_Time<8)  //利用功率环实现功率衰减,之前是while
	  if(PowerNow>PowerLimit.SetPoint&&Power_Time<8)
	 {       
		     Chassis_Iset=PowerLimit_k*Chassis_Iset;
			   Chassis_Iset=LIMIT_MAX_MIN(Chassis_Iset,ChassisCurrentLimit,-ChassisCurrentLimit);
		     INA_READ_Power();
		     PowerNow=INA260_1.Power; //读取实时功率值
		     //Power_Cal=Chassis_Iset*Chassis_V_Cal;
			   PowerBack=PID_Calc(&PowerLimit,PowerNow); 
		     PowerLimit_k+=(PowerBack/PowerLimit.SetPoint);
			   PowerLimit_k = LIMIT_MAX_MIN(PowerLimit_k,1,0); // 之前没给限幅,主要是感觉会出现停发下来的情况
		     //Power_Cal=Chassis_Iset*Chassis_V;
			   Power_Time++;	
         Deline_Flag = !Deline_Flag;	 //取反做衰减的标志		
		}
//		 PowerLimit_k=0.8f; //衰减系数复原
//		 Power_Time=0;  //衰减清零
		 
		 else if(PowerNow>PowerLimit.SetPoint && Power_Time == 8) //到衰减次数但是还是没有降低到允许功率以下
		{
		   Chassis_Iset=Chassis_Iset*(PowerLimit.SetPoint/PowerNow);
			 Chassis_Iset=LIMIT_MAX_MIN(Chassis_Iset,ChassisCurrentLimit,-ChassisCurrentLimit);
		}
		 else if(PowerNow < PowerLimit.SetPoint)
		{
		   Power_Time = 0;  
		   PowerLimit_k = 0.8f;
		}
		
		  //Chassis_V=PowerNow/Chassis_Iset; //实际上这个值并不是常数
//    //功率控制相关 计算当前的缩放系数
//    PowerLimit_k = LIMIT_MAX_MIN(maxPower / PowerNow, 2.7f, 0.0f);  //用当前功率和实际功率算出的一个功率系数
//    if(remainEnergy <= 50) PowerLimit_k *=0.33f;  //这个0.4得多试试
//    else if(remainEnergy >=150 && PowerLimit_k > 1.0f) PowerLimit_k *= PowerLimit_k;    //根据剩余能量，给功率系数做进一步修饰
//    else if(remainEnergy <100 && remainEnergy >50 && PowerLimit_k >= 1.0f) PowerLimit_k = 1.0f; 
//    
//    //电流环PID相关
//    float Chassis_Iset;
//    Chassis_Iset = PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
//    Chassis_Iset = LIMIT_MAX_MIN(PowerLimit_k * Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    Debug_Iset=Chassis_Iset;
    Chassis_CAN1Send(Chassis_Iset);
}

inline static void Chassis_SLEEP_PID_Cal(void)
{
    Chassis_CAN1Send(0);
	  //Bodan_CAN1Send(0);
}

int32_t debug_Chassis_Iset;
static void Chassis_DEBUG_PID_Cal(void)
{
    extern INA260 INA260_1;
    debug_Chassis_Iset = (int16_t)PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
    debug_Chassis_Iset = LIMIT_MAX_MIN(debug_Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    
    Chassis_CAN1Send(debug_Chassis_Iset);
}

/**
  * @brief  底盘PID初始化
  * @param  None
  * @retval None
  */
static void PID_Chassis_Init(void)
{ 
  motor_chassis.pid.P = 25.0f;//52.0f;//50.0f;//0.8f;
	motor_chassis.pid.I = 0.6f;//0;//0.6f;
	motor_chassis.pid.D = 0.2f;//0;//0.7f;
	motor_chassis.pid.IMax = 10.0f;
    motor_chassis.pid.OutMax = 10000.0f;//5000.0f;
	motor_chassis.zerocheck.CountCycle = 8191;
	motor_chassis.zerocheck.LastValue = motor_chassis.angle_abs;
	motor_chassis.zerocheck.Circle = 0;
	
	//功率环参数
	PowerLimit.SetPoint=30.0f;  //现在是30W的时候基本上不用到缓冲能量
	PowerLimit.P= 0.001f;//0.001f;
	PowerLimit.I=0.01f; //0.0005f;
	PowerLimit.D= 0.001f;
	PowerLimit.IMax=0.5f;
	PowerLimit.OutMax=15.0f;  //输出限制
	
	PowerLimit_1.SetPoint=20.0f;
	PowerLimit_1.P=0.3f; //0.004f;
	PowerLimit_1.I=0.02f;
	PowerLimit_1.D=0;//0.009f;
	PowerLimit_1.IMax=0.5f;
	PowerLimit_1.OutMax=10.0f;
}

void PID_Chassis_ClearError(void)
{  //按理说是可以不用这个函数的，但目前加上一次清零以防万一
    motor_chassis.pid.dError = 0.0f;
    motor_chassis.pid.PreError = 0.0f;
    motor_chassis.pid.SumError = 0.0f;
}

//void ReadINA(void)
//{
//     INA_READ_Current();
//     INA_READ_Vol();
//     INA_READ_Power();

//}

