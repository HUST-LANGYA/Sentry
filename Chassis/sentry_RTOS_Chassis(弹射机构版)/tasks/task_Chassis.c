#include "main.h"
#include "task_Chassis.h"


_3508_motor_t motor_chassis;
_2006_motor_t motor_gearbox;
extern State_t Sentry_State;
extern block_disconnect_t block_disconnect;

//初始化相关的标志位
uint8_t chassisFlag_Init=0;
int16_t test_init_flag=0;
uint8_t gearbox_angleinit_flag = 0;

//受击策略相关
extern uint16_t last_remainHP,cur_remainHP;
extern uint16_t is_Under_Attack; //如果受击，则将该变量置于某个单稳态的值

//减速轮相关参数
int16_t   debug_SetPoint = 1500;
int16_t   rc_debug_SetPoint;
float  motor_gearbox_vertic;
float  motor_gearbox_Init;
float lamda = 10.0f; //自锁需要的固定角度值,这个会与最大速度有关但是我觉得不应该以最大速度时对应的角度来看因为只需要起到减速的效果即可
float lamda_trans = 0.0f; //从垂直状态到锁住状态所需要的变化角度
float GearBox_Actual = 0; //2006的实际位置
uint8_t Hide_Cmd_R = 0;  //弹射指令
uint8_t Hide_Cmd_L = 0;
float Gearbox_AngleInit; //2006上电后垂直向下状态的编码器角度，先看看需不需要过零检测吧

//功率参数
float maxPower = 30.0f;
float PowerNow = 0.0f;
float PowerLimit_k=0.8f;
uint16_t remainEnergy;


//运动状态变量
MovingDir_t movingDir;//当前行进的方向  1为向右 0为向左  
uint16_t waitingTick;//在端头暂停的时间
uint32_t movingTick;//运动中计数器
int16_t last_encoder_abs,cur_encoder_abs;

//位置融合相关变量
extern LimitSWState_t LimitSw_State;//行程开关状态
extern PhotoEleSWState_t PhotoEle_State;  //光电开关状态
extern PhotoEleSWState_t Last_PhotoEle_State,Cur_PhotoEle_State;//这个用来做步数的更新
extern _encoder_t encoder;
int32_t encoder_distance = 0;//用编码器记录到减速段之前的最大距离
int32_t encoder_round = 0; //记录编码器转过的圈数
    
//受击后逃跑相关 （外部链接）
uint8_t set_escape_period = 4;  //受击后的巡逻来回数
uint8_t cur_escape_period = 0; //当前巡逻的来回数 ，一旦受击后，这个变量就清零，然后如果cur和set有差，则一直巡逻
//以右侧光电开关触发来计数


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
	encoder.zerocheck.CountCycle = 65535; //虽然这里初始化了编码器过零检测的结构体，但经实测，2021赛季的轨道不会溢出int16_t长度
	encoder.zerocheck.LastValue = encoder.angle_abs;
	encoder.zerocheck.Circle = 0;
    
    //运动策略初始化部分
    cur_escape_period = set_escape_period; //设定初始时已完成逃跑，即静止不动
	 //初始化移动角度
    lamda_trans = (lamda / 360.0f) * 8191*97;
    while (1)
    {
        //枪口热量控制的部分
        //TODO! 可以把这个热量计算的部分分到一个单独的任务里
        HeatUpdate_0();   //计算枪口热量值，这个函数的输出值，会通过CAN发送到上下云台板上去
        HeatUpdate_2();   //17mm枪管的ID号按偶数增长的
        ShootingHeat_CAN2Send();    //发送给上下云台的热量数据
        
        //编码器读位置融合相关
        encoder.angle_abs = Read_Encoder();   // 从TIM的CNT寄存器更新出新的计步值
        last_encoder_abs = cur_encoder_abs;   // 更新上一次计步值（类似现态和次态，做边沿触发用）
        cur_encoder_abs =  encoder.angle_abs; // 更新这一次计步值
        
        //底盘实时功率读取相关
        extern tGameInfo JudgeReceive;
        extern INA260 INA260_1;
        if(maxPower != JudgeReceive.MaxPower)  maxPower = JudgeReceive.MaxPower; 
        // 如果当前设定的最大功率不是裁判系统给的最大功率，就更新一次
        // 目的是为了避免出现离线调试时和上场之后的返回数据不同的错误
        PowerNow = INA260_1.Power / 1000.0f + 1.9;  //实测裁判系统读到的功率值比INA260的要高约1.9W
        remainEnergy = JudgeReceive.remainEnergy;   //跟随裁判系统来更新剩余能量
             
        
        //位置融合更新（用光电开关和行程开关的触发来矫正编码器的计数值）
        if(LimitSw_State == Limit_Right_Active)  
            TIM_SetCounter(TIM3,22469),encoder.angle_abs = 22469;  //最右端
        if(LimitSw_State == Limit_Left_Active)
            TIM_SetCounter(TIM3,0),encoder.angle_abs = 0;  //最左端
        if(Last_PhotoEle_State == PhotoEle_Both_IDLE 
        && Cur_PhotoEle_State == PhotoEle_Right_Active)  
            TIM_SetCounter(TIM3,21327),encoder.angle_abs = 21327;  //右端光电开关触发
        if(Last_PhotoEle_State == PhotoEle_Both_IDLE 
        && Cur_PhotoEle_State == PhotoEle_Left_Active)
            TIM_SetCounter(TIM3,781),encoder.angle_abs = 781;   //左端光电开关触发
        
        //受击状态更新
        if(cur_remainHP<last_remainHP && JudgeReceive.hurt_type != 0x04) 
        {//用hunter_type来判定不是因为底盘超功率而造成的掉血
            is_Under_Attack = 10000;  //受击状态的保持用单稳态递减计数
            cur_escape_period = 0;    //逃跑回数的计数用递增计数
        }
        if(is_Under_Attack) is_Under_Attack--;  //单稳递减
        //      
        GearBox_Actual = GearBoxAngle_Output();
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
static void Chassis_Patrol_Act(void)
{
    //轨道分段变量
    static uint16_t  railThreshold_R;  
    static uint16_t  railThreshold_L;
    static uint8_t railSeg_Flag; //三个状态，标志是在L段、M段还是R段

    //中间段变向相关
    static uint32_t mid_movingTick_RtoL; //用于中段变向的计数器
    static uint8_t mid_reverse_flag_RtoL;//标志在中间段是变向前还是变向后
    static uint32_t mid_movingTick_LtoR; //用于中段变向的计数器
    static uint8_t mid_reverse_flag_LtoR;//标志在中间段是变向前还是变向后


    if(chassisFlag_Patrol_Init == 0)
    {    
        railThreshold_L = 4000;
        railThreshold_R = 18469;
        srand(25);  //这个随机数种子初始化和底盘的运动变向有关
        chassisFlag_Patrol_Init=1; 
    }
        

    
    //根据运动状态，做计数器计数
    if(PhotoEle_State == PhotoEle_Both_IDLE) 
    {//两边都未触发
//        if(motor_chassis.pid.SetPoint)
//        {//若是正常运动，waitingTick清零，movingTick++
            waitingTick = 0,movingTick++;//}
//        else {//若是人为暂停，则同时清零waitingTick和movingTick
//           waitingTick = 0,movingTick = 0;//}
    }
    else if((PhotoEle_State == PhotoEle_Left_Active) || (LimitSw_State == Limit_Left_Active) )  
    {//左侧触发（￣︶￣）↗　，运动暂停，movingTick清零，waitingTick++, 运动变向
        waitingTick++,movingTick = 0,movingDir = MovingDir_R;
    }
    else if( (PhotoEle_State == PhotoEle_Right_Active) || (LimitSw_State == Limit_Right_Active ) )
    {//右侧触发（￣︶￣）↗　，运动暂停，movingTick清零，waitingTick++, 运动变向
        waitingTick++,movingTick = 0,movingDir = MovingDir_L;
    }
    else if(PhotoEle_State == PhotoEle_Both_Active ) 
    {//双边触发，考虑为人为暂停情况
        waitingTick = 1,movingTick = 0;//报错！！！！
    }
    

    //更新底盘在轨道上的哪一段
    if(encoder.angle_abs < railThreshold_L)  railSeg_Flag = RailSeg_L;
    else if(encoder.angle_abs >= railThreshold_L && encoder.angle_abs <= railThreshold_R)  railSeg_Flag = RailSeg_M;
    else if(encoder.angle_abs > railThreshold_R)  railSeg_Flag = RailSeg_R;
    
    //根据各种计数变量的值，改变运动状态
    if(waitingTick>0 && waitingTick<100 && movingTick == 0) 
        motor_chassis.pid.SetPoint = 0;//暂停状态0速
    else if(waitingTick>100 && movingTick == 0)
        motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500);  //从零启动的1速状态 
    else{//阶梯加速，防止单阶跃功率过大导致底盘超功率挂掉
        if(waitingTick == 0 && movingTick > 60)
            motor_chassis.pid.SetPoint = (movingDir)?(-2000):(2000);  //从1速开始再加一次速的2速状态 
        if(waitingTick == 0 && movingTick > 100)
            motor_chassis.pid.SetPoint = (movingDir)?(-2500):(2500);  //从1速开始再加一次速的2速状态
        if(waitingTick == 0 && movingTick > 200)
            motor_chassis.pid.SetPoint = (movingDir)?(-3200):(3200);  //从2速开始再加一次速的3速状态  
        if(waitingTick == 0 && movingTick > 300)
            motor_chassis.pid.SetPoint = (movingDir)?(-4000):(4000);  //从3速开始再加一次速的4速状态 
        if(waitingTick == 0 && movingTick > 350)
            motor_chassis.pid.SetPoint = (movingDir)?(-4300):(4300);    
        if(waitingTick == 0 && movingTick > 480 && movingTick < 810)  
            motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);    
        //810ms加速段结束后就把这个速度的控制权交给编码器了
    }
    
    
    //中间段随机变向 ///嘿嘿嘿(●ˇ?ˇ●)
    if(!(rand() % 2))
    {
        if((last_encoder_abs < 14200)
          && (cur_encoder_abs >= 14200)
          && movingDir == MovingDir_R
          && !mid_reverse_flag_RtoL)//且在向右运动
        {   
            mid_movingTick_RtoL=0;  //清空中间段计数器
            motor_chassis.pid.SetPoint = (movingDir)?(-1800):(1800); //先减一段速        
            mid_reverse_flag_RtoL = 1; //中间段触发置位
        }
        if(mid_reverse_flag_RtoL)
        {//中间段反向后，阶梯加速
            mid_movingTick_RtoL++;
            
            if(mid_movingTick_RtoL >50 )   { motor_chassis.pid.SetPoint = 0;movingDir = MovingDir_L; }//先变向，再加速
            if(mid_movingTick_RtoL > 100)  motor_chassis.pid.SetPoint = (movingDir)?(-1800):(1800);
            if(mid_movingTick_RtoL > 200)  motor_chassis.pid.SetPoint = (movingDir)?(-2400):(2400);
            if(mid_movingTick_RtoL > 300)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);
            if(mid_movingTick_RtoL > 400)  motor_chassis.pid.SetPoint = (movingDir)?(-3600):(3600);
            if(mid_movingTick_RtoL > 500)  motor_chassis.pid.SetPoint = (movingDir)?(-4200):(4200);
            if(mid_movingTick_RtoL > 600)  motor_chassis.pid.SetPoint = (movingDir)?(-4800):(4800);
            if(mid_movingTick_RtoL > 700)  motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);       
        }
        if(mid_movingTick_RtoL>1000) mid_reverse_flag_RtoL = 0,mid_movingTick_RtoL = 0;//中间段变向标志的超时复位
    }
    if(!rand() % 2)
    {
        if((last_encoder_abs >= 9000)
          && (cur_encoder_abs < 9000)
          && movingDir == MovingDir_L
          && !mid_reverse_flag_LtoR)//且在向左运动
        {   
            mid_movingTick_LtoR=0;  //清空中间段计数器
            motor_chassis.pid.SetPoint = (movingDir)?(-1800):(1800); //先减一段速        
            mid_reverse_flag_LtoR = 1; //中间段触发置位
        }
        if(mid_reverse_flag_LtoR)
        {//中间段反向后,阶梯加速
            mid_movingTick_LtoR++;
            if(mid_movingTick_LtoR >50 )   { motor_chassis.pid.SetPoint = 0;movingDir = MovingDir_R; }//先变向，再加速
            if(mid_movingTick_LtoR > 100)  motor_chassis.pid.SetPoint = (movingDir)?(-1800):(1800);
            if(mid_movingTick_LtoR > 200)  motor_chassis.pid.SetPoint = (movingDir)?(-2400):(2400);
            if(mid_movingTick_LtoR > 300)  motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);
            if(mid_movingTick_LtoR > 400)  motor_chassis.pid.SetPoint = (movingDir)?(-3600):(3600);
            if(mid_movingTick_LtoR > 500)  motor_chassis.pid.SetPoint = (movingDir)?(-4200):(4200);
            if(mid_movingTick_LtoR > 600)  motor_chassis.pid.SetPoint = (movingDir)?(-4800):(4800);
            if(mid_movingTick_LtoR > 700)  motor_chassis.pid.SetPoint = (movingDir)?(-5200):(5200);       
        }
        if(mid_movingTick_LtoR>1000) mid_reverse_flag_LtoR = 0,mid_movingTick_LtoR = 0;//中间段变向标志的超时复位
    }   
    
    //左右两端靠近立柱段的减速（先用编码器值减速，在用光电开关的触发来反向）
    if(movingDir == MovingDir_L && railSeg_Flag == RailSeg_L)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
    if(movingDir == MovingDir_R && railSeg_Flag == RailSeg_R)  motor_chassis.pid.SetPoint = (movingDir)?(-1500):(1500); 
    
    
//    Chassis_SLEEP_PID_Cal();
    if(is_Under_Attack || cur_escape_period < set_escape_period) Chassis_Patrol_PID_Cal();
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
	  extern RC_Ctl_t RC_Ctl;
    if(chassisFlag_Init == 0)
    {
        motor_chassis.pid.SetPoint=0;
        chassisFlag_Init=1; 
    }
 
    //需要注意在求角度的时候考虑2006本身的减速比为96:1
		//rc_debug_SetPoint = 0;
		//rc_debug_SetPoint = kChassis*(ABS(RC_Ctl.rc.ch0-1024));
	 // motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);/*kChassis*(RC_Ctl.rc.ch0-1024);*/
		
		//movingDir = (RC_Ctl.rc.ch0>1024)? (MovingDir_L) : (MovingDir_R); //这个还需要调试才知道
		//movingDir = MovingDir_L;
		//lamda_trans = (lamda / 360.0f) * 8191*97; //把需要移动的角度转化成编码器所需要变化的值,这个应该是给定的锁定值
		//Gearbox_AngleInit =  motor_gearbox_vertic/ 8192.0f * 360.0f/97;//上电后2006垂直方向上对应的角度,97为2006的减速比
		Hide_Cmd_R = (RC_Ctl.rc.ch3>1500); //把ch3拨杆向上拨的时候触发弹射指令,分两边控制,可以理解为光电开关的替代品
		Hide_Cmd_L = (RC_Ctl.rc.ch1>1500);
		//motor_gearbox.PidPos.SetPoint = motor_gearbox_vertic;
		if(Hide_Cmd_R)//正常情况下Hide_Cmd会占用几个底盘任务处理的时间吧
		{ 
			//减速过程
			waitingTick++,movingTick=0;
			movingDir = MovingDir_L;
//			//motor_chassis.pid.SetPoint = 0;//变向的时候速度值直接给到零
//		  motor_gearbox.PidPos.SetPoint = (movingDir)? (motor_gearbox_vertic + lamda_trans) : (motor_gearbox_vertic - lamda_trans); //其实这些都可以测得
//			//反向过程
//			if(ABS(motor_chassis.real_speed-0) < 10.0f) //减速到接近0之后
//			{
//			   motor_gearbox.PidPos.SetPoint = motor_gearbox_vertic ;//恢复到垂直杠对应的编码器值,但是应该还是掉不下来(理想?)
//				 movingDir = !movingDir; //减小到零之后变向,应该要保持motor_gearbox和方向变化的同时性
//			}
		}
		else if(Hide_Cmd_L)
		{
		  waitingTick++,movingTick=0;
			movingDir = MovingDir_R;
		}
		else
		{
		  movingTick++,waitingTick=0;
			motor_gearbox.PidPos.SetPoint = motor_gearbox_vertic;
			
		}
		if(movingTick==0&&waitingTick<500&&waitingTick>0)
		{	
			motor_chassis.pid.SetPoint = 0;
			motor_gearbox.PidPos.SetPoint = (movingDir)? (motor_gearbox_vertic - lamda_trans) : (motor_gearbox_vertic + lamda_trans); //其实这些都可以测得,这里应该是方向改变的减速所需要变化一下
			//反向过程
//			if(ABS(motor_chassis.real_speed-0) < 10.0f) //减速到接近0之后,问题是检测到了向前是因为速度没有给到零还是
//			{
//			   motor_gearbox.PidPos.SetPoint = motor_gearbox_vertic ;//恢复到垂直杠对应的编码器值,但是应该还是掉不下来(理想?)
//				 //movingDir = !movingDir; //减小到零之后变向,应该要保持motor_gearbox和方向变化的同时性
//			}
		}
		else if(waitingTick==0&&movingTick>50) motor_chassis.pid.SetPoint = (movingDir)?(-3000):(3000);
		
    /*if(is_Under_Attack || cur_escape_period < set_escape_period)*/ Chassis_RC_PID_Cal();
    //else movingTick = 0,Chassis_SLEEP_PID_Cal();
}


inline static void Chassis_SLEEP_Act(void)
{
    Chassis_SLEEP_PID_Cal();
}

////减速轮相关参数
//int16_t debug_SetPoint = 1500;
//uint16_t motor_gearbox_Init = 0;
//float lamda = 10.0f; //自锁需要的固定角度值
//float lamda_trans = 0.0f; //从垂直状态到锁住状态所需要的变化角度
//float GearBox_Actual = 0; //2006的实际位置
//short Hide_Cmd = 0;  //弹射指令
int16_t debug_gearbox_setpoint;
static void Chassis_DEBUG_Act(void)
{   
	  extern RC_Ctl_t RC_Ctl;
	  if(chassisFlag_Init == 0)
		{
		  //首次进来的时候应该是先根据垂直的时候对应的编码器值设定为初值,然后两边自锁时移动相同的编码器角度
			//motor_gearbox_Init = motor_gearbox.angle_inc; // 这个可以是手动测量或者自测
			
			chassisFlag_Init = 1;
		}
		
		  //根据运动状态，做计数器计数
    if(PhotoEle_State == PhotoEle_Both_IDLE) 
    {//两边都未触发
//        if(motor_chassis.pid.SetPoint)
//        {//若是正常运动，waitingTick清零，movingTick++
            waitingTick = 0,movingTick++;//}
//        else {//若是人为暂停，则同时清零waitingTick和movingTick
//           waitingTick = 0,movingTick = 0;//}
    }
    else if((PhotoEle_State == PhotoEle_Left_Active) || (LimitSw_State == Limit_Left_Active) )  
    {//左侧触发（￣︶￣）↗　，运动暂停，movingTick清零，waitingTick++, 运动变向
        waitingTick++,movingTick = 0,movingDir = MovingDir_R;
    }
    else if( (PhotoEle_State == PhotoEle_Right_Active) || (LimitSw_State == Limit_Right_Active ) )
    {//右侧触发（￣︶￣）↗　，运动暂停，movingTick清零，waitingTick++, 运动变向
        waitingTick++,movingTick = 0,movingDir = MovingDir_L;
    }
    else if(PhotoEle_State == PhotoEle_Both_Active ) 
    {//双边触发，考虑为人为暂停情况
        waitingTick = 1,movingTick = 0;//报错！！！！
    }
		
		
//		lamda_trans = (lamda / 360.0f) * 8191; //把需要移动的角度转化成编码器所需要变化的值
//		motor_gearbox_Init = Gearbox_AngleInit / 8192.0f * 360.0f;
//		//Hide_Cmd = (RC_Ctl.rc.ch1>1500) && (RC_Ctl.rc.ch3>1500); //把两个拨杆向上拨的时候触发弹射指令
//		if(Hide_Cmd)
//		{ 
//			//减速过程
//		  motor_gearbox.PidPos.SetPoint = (movingDir)? (motor_gearbox_Init + lamda_trans) : (motor_gearbox_Init - lamda_trans); //其实这些都可以测得
//			//反向过程
//			if((motor_chassis.real_speed-0) < 10.0f) //减速到接近0之后
//			{
//			   motor_gearbox.PidPos.SetPoint = motor_gearbox_Init;//给到垂直杠对应的编码器值,但是应该还是掉不下来(理想?)
//			}
//			
	//	}
		
		//完成弹射过程之后再开始正常的加速吧或者可以直接借助势能一起
		 motor_gearbox.PidPos.SetPoint = debug_gearbox_setpoint;
    motor_chassis.pid.SetPoint = debug_SetPoint;//(movingDir)?(-debug_SetPoint): (debug_SetPoint); //先给为0或者调试的时候改变,按道理
    Chassis_DEBUG_PID_Cal(); 
}


static void Chassis_RC_PID_Cal(void)
{
    float Chassis_Iset;
    float Gearbox_Iset;    
    //底盘2006 限幅+赋值
    motor_chassis.pid.SetPoint = LIMIT_MAX_MIN(motor_chassis.pid.SetPoint,ChassisCurrentLimit , -ChassisCurrentLimit);
	
	  motor_gearbox.PidSpeed.SetPoint = PID_Calc(&motor_gearbox.PidPos,motor_gearbox.angle_inc);   
	  
    Chassis_Iset = LIMIT_MAX_MIN(PID_Calc( &motor_chassis.pid, motor_chassis.real_speed), ChassisCurrentLimit, -ChassisCurrentLimit);
	 // Deline_Iset = LIMIT_MAX_MIN(PID_Calc(&motor_gearbox.PidSpeed,motor_gearbox.real_speed),motor_gearbox_CurrentLimit,-motor_gearbox_CurrentLimit);
	
	  motor_gearbox.PidSpeed.SetPoint = PID_Calc(&motor_gearbox.PidPos,motor_gearbox.angle_inc); //计算得到速度环的输入值
	  Gearbox_Iset = PID_Calc(&motor_gearbox.PidSpeed,motor_gearbox.real_speed);  //
	 // debug_I = Gearbox_Iset;
	  Gearbox_Iset = LIMIT_MAX_MIN(Gearbox_Iset, motor_gearbox_CurrentLimit,-motor_gearbox_CurrentLimit);
   // aaa = Gearbox_Iset;
	
    Chassis_CAN2Send(Chassis_Iset); //这个是3508的
	  Deline_CAN2Send(Gearbox_Iset);   //这个是2006的
}

static void Chassis_Patrol_PID_Cal(void)
{
    //功率控制相关 计算当前的缩放系数
    PowerLimit_k = LIMIT_MAX_MIN(maxPower / PowerNow, 2.7f, 0.0f);  //用当前功率和实际功率算出的一个功率系数
    if(remainEnergy <= 50) PowerLimit_k *=0.33f;  //这个0.4得多试试
    else if(remainEnergy >=150 && PowerLimit_k > 1.0f) PowerLimit_k *= PowerLimit_k;    //根据剩余能量，给功率系数做进一步修饰
    else if(remainEnergy <100 && remainEnergy >50 && PowerLimit_k >= 1.0f) PowerLimit_k = 1.0f; 
    
//    //电流环PID相关
//    float Chassis_Iset;
//    Chassis_Iset = PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
//    Chassis_Iset = LIMIT_MAX_MIN(PowerLimit_k * Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    
  //  Chassis_CAN1Send(0);
}

inline static void Chassis_SLEEP_PID_Cal(void)
{
    Chassis_CAN2Send(0);
	  Deline_CAN2Send(0);
}

int32_t debug_Chassis_Iset;
float debug_I;
float aaa;
static void Chassis_DEBUG_PID_Cal(void)
{
	  float  Gearbox_Iset;
    extern INA260 INA260_1;
    debug_Chassis_Iset = (int16_t)PID_Calc(&motor_chassis.pid, motor_chassis.real_speed);
    debug_Chassis_Iset = LIMIT_MAX_MIN(debug_Chassis_Iset, ChassisCurrentLimit, -ChassisCurrentLimit);
    
	  motor_gearbox.PidSpeed.SetPoint = PID_Calc(&motor_gearbox.PidPos,motor_gearbox.angle_inc); //计算得到速度环的输入值
	  Gearbox_Iset = PID_Calc(&motor_gearbox.PidSpeed,motor_gearbox.real_speed);  //
	  debug_I = Gearbox_Iset;
	  Gearbox_Iset = LIMIT_MAX_MIN(Gearbox_Iset, motor_gearbox_CurrentLimit,-motor_gearbox_CurrentLimit);
     aaa = Gearbox_Iset;
    Chassis_CAN2Send(0);
	  Deline_CAN2Send(Gearbox_Iset);
	  
}

/**
  * @brief  底盘PID初始化
  * @param  None
  * @retval None
  */
static void PID_Chassis_Init(void)
{ 
    motor_chassis.pid.P = 18.0f;//50.0f;//0.8f;
	motor_chassis.pid.I = 0;//0.6f;
	motor_chassis.pid.D = 0;//0.7f;
	motor_chassis.pid.IMax = 10.0f;
  motor_chassis.pid.OutMax = 6000.0f;
	motor_chassis.zerocheck.CountCycle = 8191;
	motor_chassis.zerocheck.LastValue = motor_chassis.angle_abs;
	motor_chassis.zerocheck.Circle = 0;
	
	
	// motor_gearbox//2006减速轮的PID初始化
	
	motor_gearbox.PidPos.P = 0.25f;//0.20f;
	motor_gearbox.PidPos.I = 0.02f;//0.020f;
	motor_gearbox.PidPos.D = 0.0f;
	motor_gearbox.PidPos.IMax = 500.0f;//500
	motor_gearbox.PidPos.OutMax = 10000.0f;
	
	motor_gearbox.PidSpeed.P = 0.5f;//15
	motor_gearbox.PidSpeed.I = 0.0f;
	motor_gearbox.PidSpeed.D = 0.0f;
	motor_gearbox.PidSpeed.IMax = 0.0f;
	motor_gearbox.PidSpeed.OutMax = 10000.0f;
	//zero check
	motor_gearbox.zerocheck.CountCycle = 8191;
	motor_gearbox.zerocheck.LastValue = motor_chassis.angle_abs;
	motor_gearbox.zerocheck.Circle = 0;
}

void PID_Chassis_ClearError(void)
{  //按理说是可以不用这个函数的，但目前加上一次清零以防万一
    motor_chassis.pid.dError = 0.0f;
    motor_chassis.pid.PreError = 0.0f;
    motor_chassis.pid.SumError = 0.0f;
}

//float Gearbox_AngleInit;
inline float GearBoxAngle_Output(void)
{
   float gearbox_angle; 
	 if(gearbox_angleinit_flag == 0)
	 {
	   gearbox_angleinit_flag = 1;
		 motor_gearbox_Init = motor_gearbox.angle_inc;
		 motor_gearbox_vertic = motor_gearbox.angle_inc;
		// Gearbox_AngleInit =  (motor_gearbox_vertic-motor_gearbox_Init)/ 8192.0f * 360.0f/97;
	 }
   gearbox_angle = ((motor_gearbox.angle_inc - motor_gearbox_Init) / 8192.0f)*360.0f/97; //97为2006的减速比
	 return gearbox_angle;
}
