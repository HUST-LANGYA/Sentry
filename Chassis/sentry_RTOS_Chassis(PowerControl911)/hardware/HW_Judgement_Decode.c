#include "main.h"
#include "HW_Judgement_Decode.h"




/*extern*/ judge_header_t judge_head;
/*extern*/ judge_control_t judge_control;

///*extern*/ ext_game_status_t game_status;
///*extern*/ ext_game_result_t game_result;
///*extern*/ ext_game_robot_HP_t robot_HP;
///*extern*/ ext_dart_status_t dart_status;
///*extern*/ ext_ICRA_buff_debuff_zone_status_t ICRA_buff_debuff_zone_status;
///*extern*/ ext_event_data_t event_data;
///*extern*/ ext_supply_projectile_action_t supply_projectile_action;
/////*extern*/ ext_supply_projectile_booking_t supply_projectile_booking; //未开放的接口
///*extern*/ ext_referee_warning_t referee_warning;
///*extern*/ ext_dart_remaining_time_t dart_remaining_time;
/*extern*/ ext_game_robot_status_t robot_status;
/*extern*/ ext_power_heat_data_t power_heat;
///*extern*/ ext_game_robot_pos_t robot_pos;
///*extern*/ ext_buff_t buff;
///*extern*/ aerial_robot_energy_t aerial_robot_energy;
///*extern*/ ext_robot_hurt_t robot_hurt;
/*extern*/ ext_shoot_data_t shoot_data;
///*extern*/ ext_bullet_remaining_t bullet_remaining;
///*extern*/ ext_rfid_status_t rfid_status;
///*extern*/ ext_dart_client_cmd_t dart_client_cmd;

///*extern*/ ext_student_interactive_header_data_t stu_interact_head;
///*extern*/ robot_interactive_data_t robot_interact;


//** 包头相关
static judge_BOOL_t Check_SOF(judge_header_t frame_header);  //帧起始码检验
static uint16_t get_frame_dataLength(uint8_t RxBuffer[]);  //获得数据段长度
static uint8_t get_frame_seq(uint8_t RxBuffer[]); //获得包序号seq
static uint8_t get_frame_header_CRC8(uint8_t RxBuffer[]); //获得包头CRC8校验位
static void get_frame_header(uint8_t RxBuffer[], judge_header_t* judge_head);  //解出包头
static judge_BOOL_t Check_frame_header_CRC(judge_header_t judgeRx_header);  //包头CRC校验
static uint16_t get_frame_cmdID(uint8_t RxBuffer[]);   //获得命令码ID

//** 包尾相关
static uint16_t get_frame_tail_CRC16(uint8_t RxBuffer[], uint16_t data_length);// 获得包尾(即校验码)(但获取这个效验码可能没啥卵用)
static judge_BOOL_t Check_wholeframe_CRC(uint8_t RxBuffer[], uint16_t data_length);// 全包CRC校验，需要知道包尾的位置

//** 数据段相关
static void get_frame_data(uint8_t RxBuffer[], uint16_t data_length, uint8_t data_buffer[]);   //获得数据段镜像(需要知道数据段长度)
//static void get_game_status(uint8_t DataBuffer[], uint16_t data_length, ext_game_status_t* game_status); //1.比赛状态数据：0x0001。发送频率：1Hz 
//static void get_game_result(uint8_t DataBuffer[], uint16_t data_length, ext_game_result_t* game_result); //2.比赛结果数据：0x0002。发送频率：比赛结束后发送 
//static void get_robot_HP(uint8_t DataBuffer[], uint16_t data_length, ext_game_robot_HP_t* robot_HP); //3.机器人血量数据：0x0003。发送频率：1Hz 
//static void get_dart_status(uint8_t DataBuffer[], uint16_t data_length, ext_dart_status_t* dart_status);//4. 飞镖发射状态：0x0004。发送频率：飞镖发射后发送，发送范围：所有机器人。 
//static void get_ICRA_buff_debuff_zone_status(uint8_t DataBuffer[], uint16_t data_length, ext_ICRA_buff_debuff_zone_status_t* ICRA_buff_debuff_zone_status);//5. 人工智能挑战赛加成与惩罚区状态：0x0005。发送频率：1Hz 周期发送，发送范围：所有机器人。 
//static void get_event_data(uint8_t DataBuffer[], uint16_t data_length, ext_event_data_t* event_data);//4. 场地事件数据：0x0101。发送频率：事件改变后发送 
//static void get_supply_projectile_action(uint8_t DataBuffer[], uint16_t data_length, ext_supply_projectile_action_t* supply_projectile_action);//5. 补给站动作标识：0x0102。发送频率：动作改变后发送 
//// static void get_supply_projectile_booking(uint8_t DataBuffer[], uint16_t data_length, ext_supply_projectile_booking_t* supply_projectile_booking);//6. 请求补给站补弹弹丸：cmd_id (0x0103)。发送频率：上限 10Hz。RM 对抗赛尚未开放 
//static void get_referee_warning(uint8_t DataBuffer[], uint16_t data_length, ext_referee_warning_t* referee_warning); //7. 裁判警告信息：cmd_id (0x0104)。发送频率：警告发生后发送 
//static void get_dart_remaining_time(uint8_t DatBuffer[], uint16_t data_length, ext_dart_remaining_time_t* dart_remaining_time);//9. 飞镖发射口倒计时：cmd_id (0x0105)。发送频率：1Hz 周期发送，发送范围：己方机器人。
static void get_robot_status(uint8_t DataBuffer[], uint16_t data_length, ext_game_robot_status_t* robot_status);//8. 比赛机器人状态：0x0201。发送频率：10Hz 
static void get_power_heat_data(uint8_t DataBuffer[], uint16_t data_length, ext_power_heat_data_t* power_heat_data);//9. 实时功率热量数据：0x0202。发送频率：50Hz 
//static void get_robot_pos(uint8_t DataBuffer[], uint16_t data_length, ext_game_robot_pos_t* robot_pos);//10. 机器人位置：0x0203。发送频率：10Hz 
//static void get_buff(uint8_t DataBuffer[], uint16_t data_length, ext_buff_t* buff);//11. 机器人增益：0x0204。发送频率：状态改变后发送 
//static void get_aerial_robot_energy(uint8_t DataBuffer[], uint16_t data_length, aerial_robot_energy_t* aerial_robot_energy);//12. 空中机器人能量状态：0x0205。发送频率：10Hz 
//static void get_robot_hurt(uint8_t DataBuffer[], uint16_t data_length, ext_robot_hurt_t* robot_hurt);// 13. 伤害状态：0x0206。发送频率：伤害发生后发送 
static void get_shoot_data(uint8_t DataBuffer[], uint16_t data_length, ext_shoot_data_t* shoot_data);//14. 实时射击信息：0x0207。发送频率：射击后发送
//static void get_bullet_remaining(uint8_t DataBuffer[], uint16_t data_length, ext_bullet_remaining_t* bullet_remaining);//15. 弹丸剩余发射数：0x0208。发送频率：1Hz 周期发送，空中机器人以及哨兵机器人主控发送 
//static void get_RFID_status(uint8_t DataBuffer[], uint16_t data_length, ext_rfid_status_t* RFID_status);//18. 机器人 RFID 状态：0x0209。发送频率：1Hz，发送范围：单一机器人。 
//static void get_dart_client_cmd(uint8_t DataBuffer[], uint16_t data_length, ext_dart_client_cmd_t* dart_client_cmd);//19. 飞镖机器人客户端指令数据：0x020A。发送频率：10Hz，发送范围：单一机器人。 


//** 机器人间交互数据的数据包 
/**
 * 交互数据包括一个统一的数据段头结构。数据段包含了内容ID，发送者以及接收者的ID和内容数据段，
 * 整个交互数据的包总共长最大为 128 个字节，减去 frame_header,cmd_id 和 frame_tail 共 9 个字节以
 * 及数据段头结构的 6 个字节，故而发送的内容数据段最大为 113。整个交互数据 0x0301 的包上行频率 为 10Hz。
 */
 //由于存在多个内容 ID，但整个 cmd_id 上行频率最大为 10Hz，请合理安排带宽
//**ID 说明 
//**  1. 机器人 ID：
//**    1，英雄(红);2，工程(红);3/4/5，步兵(红);
//**    6，空中(红);7，哨兵(红);9，雷达站(红);
//**    101，英雄(蓝);102，工程(蓝);103/104/105，步兵(蓝);
//**    106，空中(蓝);107，哨兵(蓝); 109，雷达站(蓝)。 
//**  2. 客户端 ID：
//**    0x0101 为英雄操作手客户端(红);
//**    0x0102，工程操作手客户端((红); 
//**    0x0103/0x0104/0x0105，步兵操作手客户端(红);
//**    0x0106，空中操作手客户端((红); 
//**    0x0165，英雄操作手客户端(蓝);
//**    0x0166，工程操作手客户端(蓝);
//**    0x0167/0x0168/0x0169，步兵操作手客户端步兵(蓝);
//**    0x016A，空中操作手客户端(蓝)。

////**解码学生交互数据包的数据头(二级头)
//static void get_stu_interact_header_data(uint8_t DataBuffer[], uint16_t data_length, ext_student_interactive_header_data_t* stu_interact_head);//1. 交互数据接收信息：0x0301。发送频率：上限 10Hz

////**解码自定义数据包的纯数据段
//static void get_robot_interact_data(uint8_t DataBuffer[], uint16_t data_length, robot_interactive_data_t* robot_interact);//2. 交互数据 机器人间通信：0x0301。发送频率：上限 10Hz 

/**
  * @brief  解码裁判系统数据(这个是我自己写的版本，前辈们写的版本在19哨兵的代码里面)
  * @param  用来存储接收数据的数组
  * @retval None
  */
uint8_t DataBuffer[JudgeBufBiggestSize];   //接收数据包的数据段镜像
uint16_t cmd_id;//命令ID
void Judge_Receive(uint8_t ReceiveBuffer[])
{
    extern uint8_t DataBuffer[];
    extern judge_header_t judge_head;  //接收数据包的包头控制段镜像
    extern uint16_t cmd_id;

    //先把DMA缓存中的数据镜像到内存中另外的位置，这样就能腾出缓存来接收下一个包
    get_frame_header(ReceiveBuffer, &judge_head);   //取出包头
    cmd_id = get_frame_cmdID(ReceiveBuffer);
    get_frame_data(ReceiveBuffer, judge_head.data_length, DataBuffer);  //取出数据段
    //考虑抛弃循环找包头的方式，就用DMA缓存区的头来确定包头
    if (Check_SOF(judge_head)
        && Check_frame_header_CRC(judge_head)
        && Check_wholeframe_CRC(ReceiveBuffer, judge_head.data_length))   //包头SOF检验和包头CRC8校验
    {
        switch (cmd_id)
        {
//        case CMD_ID_game_status:
//            get_game_status(DataBuffer, judge_head.data_length, &game_status);
//            break;
//        case CMD_ID_game_result:
//            get_game_result(DataBuffer, judge_head.data_length, &game_result);
//            break;
//        case CMD_ID_game_robot_HP:
//            get_robot_HP(DataBuffer, judge_head.data_length, &robot_HP);
//            break;
//        case CMD_ID_dart_status:
//            get_dart_status(DataBuffer, judge_head.data_length, &dart_status);
//            break;
//        case CMD_ID_ICRA_buff_debuff_zone_status:
//            get_ICRA_buff_debuff_zone_status(DataBuffer, judge_head.data_length, &ICRA_buff_debuff_zone_status);
//            break;
//        case CMD_ID_event_data:
//            get_event_data(DataBuffer, judge_head.data_length, &event_data);
//            break;
//        case CMD_ID_supply_projectile_action:
//            get_supply_projectile_action(DataBuffer, judge_head.data_length, &supply_projectile_action);
//            break;
            /* 未开放的接口 case CMD_ID_supply_projectile_booking:
                get_supply_projectile_booking(DataBuffer, judge_head.data_length, &supply_projectile_booking);
                break;*/
//        case CMD_ID_referee_warning:
//            get_referee_warning(DataBuffer, judge_head.data_length, &referee_warning);
//            break;
//        case CMD_ID_dart_remaining_time:
//            get_dart_remaining_time(DataBuffer, judge_head.data_length, &dart_remaining_time);
//            break;
        case CMD_ID_game_robot_status:
            get_robot_status(DataBuffer, judge_head.data_length, &robot_status);
            break;
        case CMD_ID_power_heat_data:
            get_power_heat_data(DataBuffer, judge_head.data_length, &power_heat);
            break;
//        case CMD_ID_game_robot_pos:
//            get_robot_pos(DataBuffer, judge_head.data_length, &robot_pos);
//            break;
//        case CMD_ID_buff:
//            get_buff(DataBuffer, judge_head.data_length, &buff);
//            break;
//        case CMD_ID_aerial_robot_energy:
//            get_aerial_robot_energy(DataBuffer, judge_head.data_length, &aerial_robot_energy);
//            break;
        case CMD_ID_shoot_data:
            get_shoot_data(DataBuffer, judge_head.data_length, &shoot_data);
            break;
//        case CMD_ID_robot_hurt:
//            get_robot_hurt(DataBuffer, judge_head.data_length, &robot_hurt);
//            break;
//        case CMD_ID_bullet_remaining:
//            get_bullet_remaining(DataBuffer, judge_head.data_length, &bullet_remaining);
//            break;
//        case CMD_ID_RFID_status:
//            get_RFID_status(DataBuffer, judge_head.data_length, &rfid_status);
//            break;
//        case CMD_ID_dart_client_cmd:
//            get_dart_client_cmd(DataBuffer, judge_head.data_length, &dart_client_cmd);
//            break;
//        case CMD_ID_student_interactive_header_data:
//            get_robot_interact_data(DataBuffer,judge_head.data_length,&robot_interact);
//            break;
        default:  /*这里可以报一个错误*/ break;
        }
    }

}

/**
 * @brief 检验包头是否正确为0xA5
 * @param 数据包头
 * @retval 指示是否正确的布尔变量
 */
inline static judge_BOOL_t Check_SOF(judge_header_t frame_header)
{
    return ((frame_header.SOF == START_OF_FRAME) ? judge_TRUE : judge_FALSE);
}


/**
 * @brief 获取数据包有效数据段的长度
 * @param RxBuffer
 * @return 有效数据段的长度
*/
inline static uint16_t get_frame_dataLength(uint8_t RxBuffer[])
{// ! 注：由于我自己没有试过，不能确定这里是大端还是小端
    return (uint16_t)(((RxBuffer[2] << 8) | RxBuffer[1]) & 0xffff);
}

/**
 * @brief 获取包头的seq值（不晓得有啥用这玩意）
 * @param RxBuffer
 * @return 当前包seq值
*/
inline static uint8_t get_frame_seq(uint8_t RxBuffer[])
{
    return (uint8_t)(RxBuffer[3]);
}

/**
 * @brief 获取包头的CRC8校验码
 * @param RxBuffer
 * @return CRC校验码
*/
inline static uint8_t get_frame_header_CRC8(uint8_t RxBuffer[])
{
    return (uint8_t)(RxBuffer[4]);
}


/**
 * @brief 获取包头结构体
 * @param RxBuffer
 * @param 包头结构体指针
*/
static void get_frame_header(uint8_t RxBuffer[], judge_header_t* judge_head)
{
    judge_head->SOF = RxBuffer[0];
    judge_head->data_length = get_frame_dataLength(RxBuffer);
    judge_head->seq = get_frame_seq(RxBuffer);
    judge_head->CRC8 = get_frame_header_CRC8(RxBuffer);
}


/**
 * @brief 包头CRC校验
 * @param 数据包头
 * @retval 指示是否正确的布尔变量
 */
inline static judge_BOOL_t Check_frame_header_CRC(judge_header_t judge_head)
{
    return (Verify_CRC8_Check_Sum((uint8_t*)&judge_head, 5) ? judge_TRUE : judge_FALSE);
}

/**
 * @brief 获取cmd_id
 * @param RxBuffer
 * @return cmd_id
*/
inline static uint16_t get_frame_cmdID(uint8_t RxBuffer[])
{
    return (uint16_t)(((RxBuffer[6] << 8) | RxBuffer[5]) & 0xffff);
}

/**
 * @brief 获取包尾的CRC16校验码
 * @param RxBuffer
 * @param data_length
 * @return CRC16校验码
*/
inline static uint16_t get_frame_tail_CRC16(uint8_t RxBuffer[], uint16_t data_length)
{
    return (uint16_t)(((RxBuffer[FRAME_HEAD_SIZE + CMD_ID_SIZE + data_length + 1] << 8) | RxBuffer[FRAME_HEAD_SIZE + CMD_ID_SIZE + data_length]) & 0xffff);
}


/**
 * @brief 全包CRC校验，需要知道包尾的位置
 * @param 全数据包，数据段长度
 * @retval 指示是否正确的布尔变量
 */
inline static judge_BOOL_t Check_wholeframe_CRC(uint8_t RxBuffer[], uint16_t data_length)
{
    return (Verify_CRC16_Check_Sum(RxBuffer, data_length + FRAME_HEAD_SIZE + CMD_ID_SIZE + FRAME_TAIL_SIZE) ? judge_TRUE : judge_FALSE);
}

/**
 * @brief 获取有效数据段
 * @param RxBuffer 
 * @param data_length 
 * @param data_buffer 
*/
static void get_frame_data(uint8_t RxBuffer[], uint16_t data_length, uint8_t data_buffer[])
{
    memcpy(data_buffer, RxBuffer + (FRAME_HEAD_SIZE + CMD_ID_SIZE), data_length);
}

///**
// * @brief 比赛状态数据：0x0001。发送频率：1Hz，发送范围：所有机器人。
// * @brief 0-3 bit：比赛类型
// * @brief   1：RoboMaster 机甲大师赛；
// * @brief   2：RoboMaster 机甲大师单项赛；
// * @brief   3：ICRA RoboMaster 人工智能挑战赛
// *
// * @brief 4-7 bit：当前比赛阶段
// * @brief   0：未开始比赛；
// * @brief   1：准备阶段；
// * @brief   2：自检阶段；
// * @brief   3：5s 倒计时；
// * @brief   4：对战中；
// * @brief   5：比赛结算中
// * @param DataBuffer
// * @param data_length
// * @param game_status
//*/
//static void get_game_status(uint8_t DataBuffer[], uint16_t data_length, ext_game_status_t* game_status)
//{
//    game_status->game_type = DataBuffer[0] & 0x0F;
//    game_status->game_progress = (DataBuffer[0] >> 4 & 0x0F);
//    game_status->stage_remain_time = ((uint16_t)DataBuffer[1] << 8 | DataBuffer[2]);
//}


///**
// * @brief 比赛结果数据：0x0002。发送频率：比赛结束后发送，发送范围：所有机器人。
// * @brief 0 平局 1 红方胜利 2 蓝方胜利
// * @param DataBuffer
// * @param data_length
// * @param game_result
//*/
//inline static void get_game_result(uint8_t DataBuffer[], uint16_t data_length, ext_game_result_t* game_result)
//{
//    game_result->winner = DataBuffer[0];
//}

///**
// * @brief 机器人血量数据：0x0003。发送频率：1Hz，发送范围：所有机器人。
// * @param DataBuffer
// * @param data_length
// * @param robot_HP
//*/
//static void get_robot_HP(uint8_t DataBuffer[], uint16_t data_length, ext_game_robot_HP_t* robot_HP)
//{
//    robot_HP->red_1_robot_HP = ((uint16_t)DataBuffer[0] << 8 | DataBuffer[1]);
//    robot_HP->red_2_robot_HP = ((uint16_t)DataBuffer[2] << 8 | DataBuffer[3]);
//    robot_HP->red_3_robot_HP = ((uint16_t)DataBuffer[4] << 8 | DataBuffer[5]);
//    robot_HP->red_4_robot_HP = ((uint16_t)DataBuffer[6] << 8 | DataBuffer[7]);
//    robot_HP->red_5_robot_HP = ((uint16_t)DataBuffer[8] << 8 | DataBuffer[9]);
//    robot_HP->red_7_robot_HP = ((uint16_t)DataBuffer[10] << 8 | DataBuffer[11]);
//    robot_HP->red_outpost_HP = ((uint16_t)DataBuffer[12] << 8 | DataBuffer[13]);
//    robot_HP->red_base_HP = ((uint16_t)DataBuffer[14] << 8 | DataBuffer[15]);

//    robot_HP->blue_1_robot_HP = ((uint16_t)DataBuffer[16] << 8 | DataBuffer[17]);
//    robot_HP->blue_2_robot_HP = ((uint16_t)DataBuffer[18] << 8 | DataBuffer[19]);
//    robot_HP->blue_3_robot_HP = ((uint16_t)DataBuffer[20] << 8 | DataBuffer[21]);
//    robot_HP->blue_4_robot_HP = ((uint16_t)DataBuffer[22] << 8 | DataBuffer[23]);
//    robot_HP->blue_5_robot_HP = ((uint16_t)DataBuffer[24] << 8 | DataBuffer[25]);
//    robot_HP->blue_7_robot_HP = ((uint16_t)DataBuffer[26] << 8 | DataBuffer[27]);
//    robot_HP->blue_outpost_HP = ((uint16_t)DataBuffer[28] << 8 | DataBuffer[29]);
//    robot_HP->blue_base_HP = ((uint16_t)DataBuffer[30] << 8 | DataBuffer[31]);
//}

///**
// * @brief 飞镖发射状态：0x0004。发送频率：飞镖发射后发送，发送范围：所有机器人。
// * @param DataBuffer
// * @param data_length
// * @param dart_status
//*/
//void get_dart_status(uint8_t DataBuffer[], uint16_t data_length, ext_dart_status_t* dart_status)
//{
//    dart_status->dart_belong = (uint8_t)DataBuffer[0];
//    dart_status->stage_remaining_time = (uint16_t)(DataBuffer[1] << 8 | DataBuffer[2]);
//}

///**
// * @brief 人工智能挑战赛加成与惩罚区状态：0x0005。发送频率：1Hz 周期发送，发送范围：所有机器人。
// * @param DataBuffer
// * @param data_length
// * @param ICRA_buff_debuff_zone_status
//*/
//void get_ICRA_buff_debuff_zone_status(uint8_t DataBuffer[], uint16_t data_length, ext_ICRA_buff_debuff_zone_status_t* ICRA_buff_debuff_zone_status)
//{
//    ICRA_buff_debuff_zone_status->F1_zone_status = (uint8_t)DataBuffer[0] & 0x01;
//    ICRA_buff_debuff_zone_status->F2_zone_status = (uint8_t)DataBuffer[0] & 0x10;
//    ICRA_buff_debuff_zone_status->F3_zone_status = (uint8_t)DataBuffer[1] & 0x01;
//    ICRA_buff_debuff_zone_status->F4_zone_status = (uint8_t)DataBuffer[1] & 0x10;
//    ICRA_buff_debuff_zone_status->F5_zone_status = (uint8_t)DataBuffer[2] & 0x01;
//    ICRA_buff_debuff_zone_status->F6_zone_status = (uint8_t)DataBuffer[2] & 0x10;

//    ICRA_buff_debuff_zone_status->F1_zone_buff_debuff_status = (uint8_t)DataBuffer[0] & 0x0E;
//    ICRA_buff_debuff_zone_status->F2_zone_buff_debuff_status = (uint8_t)DataBuffer[0] & 0xE0;
//    ICRA_buff_debuff_zone_status->F3_zone_buff_debuff_status = (uint8_t)DataBuffer[1] & 0x0E;
//    ICRA_buff_debuff_zone_status->F4_zone_buff_debuff_status = (uint8_t)DataBuffer[1] & 0xE0;
//    ICRA_buff_debuff_zone_status->F5_zone_buff_debuff_status = (uint8_t)DataBuffer[2] & 0x0E;
//    ICRA_buff_debuff_zone_status->F6_zone_buff_debuff_status = (uint8_t)DataBuffer[2] & 0xE0;
//}

///**
// * @brief 场地事件数据：0x0101。发送频率：1Hz 周期发送，发送范围：己方机器人。
// * @brief bit 0-1：己方停机坪占领状态
// * @brief   0 为无机器人占领；
// * @brief   1 为空中机器人已占领但未停桨；
// * @brief   2 为空中机器人已占领并停桨
// * @brief bit 2-3：己方能量机关状态：
// * @brief   bit2 为小能量机关激活状态，1 为已激活；
// * @brief   bit3 为大能量机关激活状态，1 为已激活；
// * @brief bit 4：己方基地虚拟护盾状态；
// * @brief   1 为基地有虚拟护盾血量；
// * @brief   0 为基地无虚拟护盾血量；
// * @brief bit 5 -31: 保留
// * @param DataBuffer
// * @param data_length
// * @param event_data
//*/
//static void get_event_data(uint8_t DataBuffer[], uint16_t data_length, ext_event_data_t* event_data)
//{
//    /*官方纸规定了前五位的定义，后面的6~31位保留*/
//    event_data->event_type = DataBuffer[0] & 0x1F;//(((DataBuffer[10] << 8 | DataBuffer[9])) << 8 | DataBuffer[8]) << 8 | DataBuffer[7];
//}

///**
// * @brief 补给站动作标识：0x0102。发送频率：动作触发后发送，发送范围：己方机器人
// * @param DataBuffer
// * @param DataBuffer
// * @param data_length
// * @param supply_projectile_action
//*/
//static void get_supply_projectile_action(uint8_t DataBuffer[], uint16_t data_length, ext_supply_projectile_action_t* supply_projectile_action)
//{
//    supply_projectile_action->supply_projectile_id = DataBuffer[0];
//    supply_projectile_action->supply_robot_id = DataBuffer[1];
//    supply_projectile_action->supply_projectile_step = DataBuffer[2];
//    supply_projectile_action->supply_projectile_num = DataBuffer[3];
//}

/////**
//// * @brief                   旧接口，今年不支持
//// * @param DataBuffer
//// * @param data_length
//// * @return
////*/
////static void get_supply_projectile_booking(uint8_t DataBuffer[], uint16_t data_length, ext_supply_projectile_booking_t* supply_projectile_booking)
////{
////    supply_projectile_booking->supply_projectile_id = DataBuffer[7];
////    supply_projectile_booking->supply_robot_id = DataBuffer[8];
////    supply_projectile_booking->supply_num = DataBuffer[9];
////}

///**
// * @brief 裁判警告信息：cmd_id (0x0104)。发送频率：警告发生后发送，发送范围：己方机器人。
// * @brief 1 级以及 5 级警告时，机器人 ID 为 0; 二三四级警告时，机器人 ID 为犯规机器人 ID
// * @param DataBuffer
// * @param data_length
// * @param referee_warning
//*/
//static void get_referee_warning(uint8_t DataBuffer[], uint16_t data_length, ext_referee_warning_t* referee_warning)
//{
//    referee_warning->level = DataBuffer[0];
//    referee_warning->foul_robot_id = DataBuffer[1];
//}

///**
// * @brief 飞镖发射口倒计时：cmd_id (0x0105)。发送频率：1Hz 周期发送，发送范围：己方机器人。
// * @param DatBuffer
// * @param data_length
// * @param dart_remaining_time
//*/
//void get_dart_remaining_time(uint8_t DatBuffer[], uint16_t data_length, ext_dart_remaining_time_t* dart_remaining_time)
//{
//    dart_remaining_time->dart_remaining_time = DataBuffer[0];
//}

/**
 * @brief 比赛机器人状态：0x0201。发送频率：10Hz，发送范围：单一机器人。
 * @param DataBuffer
 * @param data_length
 * @param robot_status
*/
static void get_robot_status(uint8_t DataBuffer[], uint16_t data_length, ext_game_robot_status_t* robot_status)
{
    robot_status->robot_id = DataBuffer[0];
    robot_status->robot_level = DataBuffer[1];
    robot_status->remain_HP = ((uint16_t)DataBuffer[3] << 8 | DataBuffer[2]);
    robot_status->max_HP = ((uint16_t)DataBuffer[5] << 8 | DataBuffer[4]);
    
    /*每秒冷却值*/
    /*枪口热量上限*/
    robot_status->shooter_id1_17mm_cooling_rate = ((uint16_t)DataBuffer[7] << 8 | DataBuffer[6]);
    robot_status->shooter_id1_17mm_cooling_limit = ((uint16_t)DataBuffer[9] << 8 | DataBuffer[8]);
    robot_status->shooter_id1_17mm_speed_limit = ((uint16_t)DataBuffer[11] << 8 | DataBuffer[10]);
    
    robot_status->shooter_id2_17mm_cooling_rate = ((uint16_t)DataBuffer[13] << 8 | DataBuffer[12]);
    robot_status->shooter_id2_17mm_cooling_limit = 0;
    robot_status->shooter_id2_17mm_speed_limit = 0;
    
    robot_status->shooter_id1_42mm_cooling_rate = ((uint16_t)DataBuffer[7] << 8 | DataBuffer[6]);
    robot_status->shooter_id1_42mm_cooling_limit = ((uint16_t)DataBuffer[9] << 8 | DataBuffer[8]);
    robot_status->shooter_id1_42mm_speed_limit = ((uint16_t)DataBuffer[11] << 8 | DataBuffer[10]);
    
    //机器人最大底盘功率
    robot_status->chassis_power_limit = DataBuffer[16];

    //主控电源输出情况
    robot_status->mains_power_gimbal_output  = (DataBuffer[17] & 0x01)>>0;
    robot_status->mains_power_chassis_output = (DataBuffer[17] & 0x02)>>1;
    robot_status->mains_power_shooter_output = (DataBuffer[17] & 0x04)>>2;
}

/**
 * @brief 实时功率热量数据：0x0202。发送频率：50Hz，发送范围：单一机器人。
 * @param DataBuffer
 * @param data_length
 * @param power_heat_data
*/
static void get_power_heat_data(uint8_t DataBuffer[], uint16_t data_length, ext_power_heat_data_t* power_heat_data)
{
    power_heat_data->chassis_volt = DataBuffer[1] << 8 | DataBuffer[0];
    power_heat_data->chassis_current = DataBuffer[3] << 8 | DataBuffer[2];
    power_heat_data->chassis_power = (float)((DataBuffer[7] << 8 | DataBuffer[6])<<16 | (DataBuffer[5] << 8 | DataBuffer[4]));
    power_heat_data->chassis_power_buffer = ((uint16_t)DataBuffer[9] << 8 | DataBuffer[8]);

    power_heat_data->shooter_id1_17mm_cooling_heat = ((uint16_t)DataBuffer[11] << 8 | DataBuffer[10]);
    power_heat_data->shooter_id2_17mm_cooling_heat =((uint16_t)DataBuffer[13] << 8 | DataBuffer[12]);
    power_heat_data->shooter_id1_42mm_cooling_heat = ((uint16_t)DataBuffer[15] << 8 | DataBuffer[14]);
}

///**
// * @brief 机器人位置：0x0203。发送频率：10Hz，发送范围：单一机器人。 
// * @param DataBuffer 
// * @param data_length 
// * @param robot_pos 
//*/
//static void get_robot_pos(uint8_t DataBuffer[], uint16_t data_length, ext_game_robot_pos_t* robot_pos)
//{
//    robot_pos->x = (float)((DataBuffer[3] << 8 | DataBuffer[2]) << 16 | (DataBuffer[1] << 8 | DataBuffer[0]));
//    robot_pos->y = (float)((DataBuffer[7] << 8 | DataBuffer[6]) << 16 | (DataBuffer[5] << 8 | DataBuffer[4]));
//    robot_pos->z = (float)((DataBuffer[10] << 8 | DataBuffer[10]) << 16 | (DataBuffer[9] << 8 | DataBuffer[8]));
//    robot_pos->yaw = (float)((DataBuffer[14] << 8 | DataBuffer[14]) << 16 | (DataBuffer[13] << 8 | DataBuffer[12]));
//}

///**
// * @brief 机器人增益：0x0204。发送频率：1Hz 周期发送，发送范围：单一机器人。 
// * @brief   bit 0：机器人血量补血状态 
// * @brief   bit 1：枪口热量冷却加速 
// * @brief   bit 2：机器人防御加成 
// * @brief   bit 3：机器人攻击加成 
// * @brief   其他 bit 保留
// * @param DataBuffer 
// * @param data_length 
// * @param buff 
//*/
//static void get_buff(uint8_t DataBuffer[], uint16_t data_length, ext_buff_t* buff)
//{
//    buff->power_rune_buff = DataBuffer[0];
//}


///**
// * @brief 空中机器人能量状态：0x0205。发送频率：10Hz，发送范围：单一机器人。 
// * @param DataBuffer 
// * @param data_length 
// * @param aerial_robot_energy 
//*/
//static void get_aerial_robot_energy(uint8_t DataBuffer[], uint16_t data_length, aerial_robot_energy_t* aerial_robot_energy)
//{
//    aerial_robot_energy->energy_point = ((uint16_t)DataBuffer[1]<<8 | DataBuffer[0]);
//    aerial_robot_energy->attack_time = DataBuffer[2];
//}


///**
// * @brief 伤害状态：0x0206。发送频率：伤害发生后发送，发送范围：单一机器人。
// * @brief   bit 0-3：当血量变化类型为装甲伤害，代表装甲 ID，
//                    其中数值为 0-4 号代表机器人的五个装甲片，其他血量变化类型，该变量数值为 0。 
// * @brief   bit 4-7：血量变化类型 
// * @brief      0x0 装甲伤害扣血； 
// * @brief      0x1 模块掉线扣血； 
// * @brief      0x2 超射速扣血； 
// * @brief      0x3 超枪口热量扣血； 
// * @brief      0x4 超底盘功率扣血； 
// * @brief      0x5 装甲撞击扣血 
// * @param DataBuffer 
// * @param data_length 
// * @param robot_hurt 
//*/
//static void get_robot_hurt(uint8_t DataBuffer[], uint16_t data_length, ext_robot_hurt_t* robot_hurt)
//{
//    robot_hurt->armor_id  = DataBuffer[0] & 0x0F;
//    robot_hurt->hurt_type = DataBuffer[0] >> 4 & 0x0F;
//}

/**
 * @brief 实时射击信息：0x0207。发送频率：射击后发送，发送范围：单一机器人
 * @param DataBuffer 
 * @param data_length 
 * @param shoot_data 
*/
static void get_shoot_data(uint8_t DataBuffer[], uint16_t data_length, ext_shoot_data_t* shoot_data)
{
    shoot_data->bullet_type = DataBuffer[0]; //子弹类型: 1：17mm 弹丸 2：42mm 弹丸 
    shoot_data->bullet_freq = DataBuffer[1]; //子弹射频
    shoot_data->bullet_speed = (float)((DataBuffer[5] << 8 | DataBuffer[4]) << 16 | (DataBuffer[3] << 8 | DataBuffer[2]));
}

///**
// * @brief 子弹剩余发射数：0x0208。
// * @brief 发送频率：1Hz 周期发送，
// * @brief 空中机器人，哨兵机器人以及 ICRA 机器人主控发送，发送范围：单一机器人。 
// * @param DataBuffer 
// * @param data_length 
// * @param bullet_remaining 
//*/
//static void get_bullet_remaining(uint8_t DataBuffer[], uint16_t data_length, ext_bullet_remaining_t* bullet_remaining)
//{
//    bullet_remaining->bullet_remaining_num = ((uint16_t)DataBuffer[1] << 8 | DataBuffer[0]);
//}

///**
// * @brief 机器人 RFID 状态：0x0209。发送频率：1Hz，发送范围：单一机器人.
// * @param DataBuffer
// * @param data_length
// * @param RFID_status
//*/
//void get_RFID_status(uint8_t DataBuffer[], uint16_t data_length, ext_rfid_status_t* RFID_status)
//{
//    RFID_status->rfid_status = ((uint32_t)DataBuffer[3]<<8 | DataBuffer[2])<<16 | ((uint32_t)DataBuffer[1]<<8 | DataBuffer[0]);
//}

///**
// * @brief 飞镖机器人客户端指令数据：0x020A。发送频率：10Hz，发送范围：单一机器人。 
// * @param DataBuffer
// * @param data_length
// * @param dart_client_cmd
//*/
//void get_dart_client_cmd(uint8_t DataBuffer[], uint16_t data_length, ext_dart_client_cmd_t* dart_client_cmd)
//{
//    dart_client_cmd->dart_launch_opening_status = DataBuffer[0];
//    dart_client_cmd->dart_attack_target = DataBuffer[1];
//    dart_client_cmd->target_change_time = ((uint16_t)DataBuffer[3] << 8 | DataBuffer[2]);
//    dart_client_cmd->first_dart_speed = DataBuffer[4];
//    dart_client_cmd->second_dart_speed = DataBuffer[5];
//    dart_client_cmd->third_dart_speed = DataBuffer[6];
//    dart_client_cmd->fourth_dart_speed = DataBuffer[7];
//    dart_client_cmd->last_dart_launch_time = ((uint16_t)DataBuffer[9] << 8 | DataBuffer[8]);
//    dart_client_cmd->operate_launch_cmd_time = ((uint16_t)DataBuffer[11] << 8 | DataBuffer[10]);
//}

///**
// * @brief 交互数据接收信息：0x0301。 
// * @param DataBuffer
// * @param data_length
// * @param stu_interact_head
//*/
//static void get_stu_interact_header_data(uint8_t DataBuffer[], uint16_t data_length, ext_student_interactive_header_data_t* stu_interact_head)
//{
//    stu_interact_head->data_cmd_id = DataBuffer[1] << 8 | DataBuffer[0];
//    stu_interact_head->sender_ID   = DataBuffer[3] << 8 | DataBuffer[2];
//    stu_interact_head->receiver_ID = DataBuffer[5] << 8 | DataBuffer[4];
//}

///**                             旧接口，今年不支持
// * @brief
// * @param DataBuffer
// * @param data_length
// * @param client_custom_data
//*/
//static void get_client_custom_data(uint8_t DataBuffer[], uint16_t data_length, client_custom_data_t* client_custom_data)
//{
//    client_custom_data->data1 = ((DataBuffer[16] << 8 | DataBuffer[15]) << 8 | DataBuffer[14]) << 8 | DataBuffer[13];
//    client_custom_data->data1 = ((DataBuffer[20] << 8 | DataBuffer[19]) << 8 | DataBuffer[18]) << 8 | DataBuffer[17];
//    client_custom_data->data1 = ((DataBuffer[24] << 8 | DataBuffer[23]) << 8 | DataBuffer[22]) << 8 | DataBuffer[21];
//    client_custom_data->data1 = ((DataBuffer[28] << 8 | DataBuffer[27]) << 8 | DataBuffer[26]) << 8 | DataBuffer[25];
//    client_custom_data->masks = DataBuffer[29];
//    return;
//}

///**
// * @brief 交互数据 机器人间通信：0x0301。 
// * @param DataBuffer
// * @param data_length
// * @param robot_interact
//*/
//static void get_robot_interact_data(uint8_t DataBuffer[], uint16_t data_length, robot_interactive_data_t* robot_interact)
//{
//    /* 解开学生交互包的包头，得到自定义的数据包ID、发送方和接收方ID */
//    get_stu_interact_header_data(DataBuffer, judge_head.data_length, &stu_interact_head);

//    //下面的部分，就看队内各机器人之间具体需要哪些交互通信的数据，再来做对应的解码了
//}


///**
// * @brief 从裁判系统向其他机器人发送交互数据
// * @brief 这些信息只能向友方的机器人发送
// * @param robot_interact_data 
// * @param data_length 
// * @param receiver_ID 
//*/

///**
// * @brief 从裁判系统向其他机器人发送交互数据
// * @brief 这些信息只能向友方的机器人发送
// * @param data  需要发送的有效的数据本体 
// * @param data_length   //指示data数组的长度
// * @param data_cmd_id   //数据ID号
// * @param sender_ID     //自身机器人的ID，可以从裁判系统发来的包里获得
// * @param receiver_ID   //接收方的ID号
//*/
//void Tx_robot_interact_data(uint8_t* data, uint16_t data_length,uint16_t data_cmd_id,uint16_t sender_ID,uint16_t receiver_ID)
//{
//    data_length += STU_HEAD_SIZE;
//    uint8_t* pTxBuffer = (uint8_t*)malloc((data_length)*sizeof(uint8_t));
//    pTxBuffer[0] = (uint8_t)data_cmd_id & 0xFF;
//    pTxBuffer[1] = (uint8_t)data_cmd_id>>8 & 0xFF;
//    pTxBuffer[2] = (uint8_t)sender_ID & 0xFF;
//    pTxBuffer[3] = (uint8_t)sender_ID >> 8 & 0xFF;
//    pTxBuffer[4] = (uint8_t)receiver_ID & 0xFF;
//    pTxBuffer[5] = (uint8_t)receiver_ID >> 8 & 0xFF;
//    for (uint16_t i = 0; i < data_length; i++) pTxBuffer[i + STU_HEAD_SIZE] = data[i];  //看看以后有没有更快的方法，感觉这样做拷贝可能会很慢

//    USART_SendArray(USART1,pTxBuffer,data_length);  //向串口发送数据包

//    free(pTxBuffer); //释放开辟的数据包的空间
//}

///**
// * @brief 从裁判系统向客户端发送图形删除命令
// * @brief 这些信息只能向发送机器人对应的客户端发送
// * @param client_graphic_delete
//*/
//void Tx_client_custom_graphic_delete(ext_client_custom_graphic_delete_t* client_graphic_delete)
//{
//}

///**
// * @brief 从裁判系统向客户端发送绘制图形命令
// * @brief 这些信息只能向发送机器人对应的客户端发送
// * @param client_graphic_draw
//*/
//void Tx_client_custom_graphic_single(ext_client_custom_graphic_single_t* client_graphic_draw)
//{
//}

///**
// * @brief 从裁判系统向客户端发送绘制图形命令
// * @brief 这些信息只能向发送机器人对应的客户端发送
// * @param client_graphic_draw
//*/
//void Tx_client_custom_graphic_double(ext_client_custom_graphic_double_t* client_graphic_draw)
//{
//}

///**
// * @brief 从裁判系统向客户端发送绘制图形命令
// * @brief 这些信息只能向发送机器人对应的客户端发送
// * @param client_graphic_draw
//*/
//void Tx_client_custom_graphic_five(ext_client_custom_graphic_five_t* client_graphic_draw)
//{
//}

///**
// * @brief 从裁判系统向客户端发送绘制图形命令
// * @brief 这些信息只能向发送机器人对应的客户端发送
// * @param client_graphic_draw
//*/
//void Tx_client_custom_graphic_seven(ext_client_custom_graphic_seven_t* client_graphic_draw)
//{
//}

///**
// * @brief 从裁判系统向客户端发送绘制字符命令
// * @brief 这些信息只能向发送机器人对应的客户端发送
// * @param client_graphic_draw
//*/
//void Tx_client_custom_character(ext_client_custom_character_t* client_graphic_draw)
//{
//}
