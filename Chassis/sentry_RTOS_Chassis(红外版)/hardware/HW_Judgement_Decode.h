#ifndef __HW_JUDGEMENT_DECODE_H
#define __HW_JUDGEMENT_DECODE_H

//#include <stdint.h>
//#define __packed //这个换到keil后记得把这一行删掉！！
//#define uint32_t uint16_t //这个换到keil后记得把这一行删掉！！

//这个库提供把裁判系统的数据包解码的各种函数，
//以及相关需要的宏定义，单独做成库放出来可能会方便移植和管理一些

#ifndef JudgeBufBiggestSize
#define JudgeBufBiggestSize 40
#endif
#ifndef JudgeSendBufSize
#define JudgeSendBufSize 22
#endif

// TODO 还要添加一些关于数据长度的宏定义，比如包头长度、cmdID长度、包尾长度、各种不同的数据段长度
// TODO 还要添加所有的cmdID的宏定义 （留给梯队做这个吧要不）

//SIZE
#define FRAME_HEAD_SIZE 5 //bytes
#define CMD_ID_SIZE 2     //bytes
#define FRAME_TAIL_SIZE 2 //bytes
#define STU_HEAD_SIZE 6   //bytes

//** cmd_id
#define CMD_ID_game_status 0x0001                  //比赛状态数据，1Hz 周期发送
#define CMD_ID_game_result 0x0002                  //比赛结果数据，比赛结束后发送
#define CMD_ID_game_robot_HP 0x0003                //比赛机器人血量数据，1Hz 周期发送
#define CMD_ID_dart_status 0x0004                  //飞镖发射状态，飞镖发射时发送
#define CMD_ID_ICRA_buff_debuff_zone_status 0x0005 //人工智能挑战赛加成与惩罚区状态，1Hz 周期发送
#define CMD_ID_event_data 0x0101                   //场地事件数据，1Hz 周期发送
#define CMD_ID_supply_projectile_action 0x0102     //场地补给站动作标识数据，动作发生后发送
//#define CMD_ID_supply_projectile_booking 0x0103       //请求补给站补弹数据，由参赛队发送，上限 10Hz。（RM 对抗赛尚未开放） (2020年的协议附录里移除了此项）
#define CMD_ID_referee_warning 0x0104                 //裁判警告数据，警告发生后发送
#define CMD_ID_dart_remaining_time 0x0105             //飞镖发射口倒计时，1Hz 周期发送
#define CMD_ID_game_robot_status 0x0201               //机器人状态数据，10Hz 周期发送
#define CMD_ID_power_heat_data 0x0202                 //实时功率热量数据，50Hz 周期发送
#define CMD_ID_game_robot_pos 0x0203                  //机器人位置数据，10Hz 发送
#define CMD_ID_buff 0x0204                            //机器人增益数据，1Hz 周期发送
#define CMD_ID_aerial_robot_energy 0x0205             //空中机器人能量状态数据，10Hz 周期发送，只有空中机器人主控发送
#define CMD_ID_robot_hurt 0x0206                      //伤害状态数据，伤害发生后发送
#define CMD_ID_shoot_data 0x0207                      //实时射击数据，子弹发射后发送
#define CMD_ID_bullet_remaining 0x0208                //弹丸剩余发射数，仅空中机器人，哨兵机器人以及 ICRA 机器人发送，1Hz 周期发送
#define CMD_ID_RFID_status 0x0209                     //机器人 RFID 状态，1Hz 周期发送
#define CMD_ID_dart_client_cmd 0x020A                 //飞镖机器人客户端指令数据，10Hz 周期发送
#define CMD_ID_student_interactive_header_data 0x0301 //机器人间交互数据，发送方触发发送，
//#define DATA_CMD_ID_client_custom_data 0xD180         //(2020年的协议附录里移除了此项）
#define CMD_ID_robot_interactive_data 0x0302
//#define DATA_CMD_ID_robot_interactive_data_xxx  0x0200~02FF 均可，看以后的需求来自定义
//2021的规则改成0x302了
#define CMD_ID_minimap_interact 0x303                   //小地图交互信息
#define DATA_CMD_ID_client_custom_graphic_delete 0x0100 //客户端删除图形
#define DATA_CMD_ID_client_custom_graphic_single 0x0101 //客户端绘制一个图形
#define DATA_CMD_ID_client_custom_graphic_double 0x0102 //客户端绘制二个图形
#define DATA_CMD_ID_client_custom_graphic_five 0x0103   //客户端绘制五个图形
#define DATA_CMD_ID_client_custom_graphic_seven 0x0104  //客户端绘制七个图形
#define DATA_CMD_ID_client_custom_character 0x0110      //客户端绘制字符

//** robot_ID
#define RED_hero 1
#define RED_drone 2
#define RED_infantry_1 3
#define RED_infantry_2 4
#define RED_infantry_3 5
#define RED_aerial_robot 6
#define RED_sentry 7
#define RED_Radar_station 9 //雷达站
#define RED_Out_Post 10     //前哨站
#define RED_Base 11         //基地
#define BLUE_hero 101
#define BLUE_drone 102
#define BLUE_infantry_1 103
#define BLUE_infantry_2 104
#define BLUE_infantry_3 105
#define BLUE_aerial_robot 106
#define BLUE_sentry 107
#define BLUE_Radar_station 109
#define BLUE_Out_Post 110
#define BLUE_Base 111

//** client_ID
#define CLIENT_RED_hero 0x0101
#define CLIENT_RED_drone 0x0102
#define CLIENT_RED_infantry_1 0x0103
#define CLIENT_RED_infantry_2 0x0104
#define CLIENT_RED_infantry_3 0x0105
#define CLIENT_RED_aerial_robot 0x0106
#define CLIENT_BLUE_hero 0x0165
#define CLIENT_BLUE_drone 0x0166
#define CLIENT_BLUE_infantry_1 0x0167
#define CLIENT_BLUE_infantry_2 0x0168
#define CLIENT_BLUE_infantry_3 0x0169
#define CLIENT_BLUE_aerial_robot 0x016A

typedef enum judge_BOOL_t
{
    judge_FALSE = 0,
    judge_TRUE = 1
} judge_BOOL_t; //设定一个仅用于裁判系统数据解码中的bool变量

//  裁判系统数据包报头结构体（自己写的）
#define START_OF_FRAME 0xA5
typedef __packed struct
{
    uint8_t SOF;          //start of frame 数据帧起始字节，固定值为 0xA5
    uint16_t data_length; //数据帧中 data 的长度
    uint8_t seq;          //包序号
    uint8_t CRC8;         //帧头 CRC8 校验
} judge_header_t;
//  裁判系统数据包所有控制段的结构体 （自己写的）
typedef __packed struct
{
    judge_header_t frame_head; //帧头
    uint16_t cmd_id;           //命令码ID
    uint16_t frame_tail;       //帧尾 CRC16，整包校验
} judge_control_t;

//  所有裁判系统接口给的数据包的数据段结构体（从官方给的手册里抄来的）
typedef __packed struct
{
    uint8_t game_type : 4;
    uint8_t game_progress : 4;
    uint16_t stage_remain_time;
} ext_game_status_t;
typedef __packed struct
{
    uint8_t winner;
} ext_game_result_t;
typedef __packed struct
{
    uint16_t red_1_robot_HP;
    uint16_t red_2_robot_HP;
    uint16_t red_3_robot_HP;
    uint16_t red_4_robot_HP;
    uint16_t red_5_robot_HP;
    uint16_t red_7_robot_HP;
    uint16_t red_outpost_HP;
    uint16_t red_base_HP;
    uint16_t blue_1_robot_HP;
    uint16_t blue_2_robot_HP;
    uint16_t blue_3_robot_HP;
    uint16_t blue_4_robot_HP;
    uint16_t blue_5_robot_HP;
    uint16_t blue_7_robot_HP;
    uint16_t blue_outpost_HP;
    uint16_t blue_base_HP;
} ext_game_robot_HP_t;
typedef __packed struct
{
    uint8_t dart_belong;
    uint16_t stage_remaining_time;
} ext_dart_status_t;
typedef __packed struct
{
    uint8_t F1_zone_status : 1;
    uint8_t F1_zone_buff_debuff_status : 3;
    uint8_t F2_zone_status : 1;
    uint8_t F2_zone_buff_debuff_status : 3;
    uint8_t F3_zone_status : 1;
    uint8_t F3_zone_buff_debuff_status : 3;
    uint8_t F4_zone_status : 1;
    uint8_t F4_zone_buff_debuff_status : 3;
    uint8_t F5_zone_status : 1;
    uint8_t F5_zone_buff_debuff_status : 3;
    uint8_t F6_zone_status : 1;
    uint8_t F6_zone_buff_debuff_status : 3;

    uint16_t red1_bullet_left;
    uint16_t red2_bullet_left;
    uint16_t blue1_bullet_left;
    uint16_t blue2_bullet_left;
} ext_ICRA_buff_debuff_zone_status_t;
typedef __packed struct
{
    uint32_t event_type;
} ext_event_data_t;
typedef __packed struct
{
    uint8_t supply_projectile_id;
    uint8_t supply_robot_id;
    uint8_t supply_projectile_step;
    uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;
//typedef __packed struct
//{
//    uint8_t supply_projectile_id;
//    uint8_t supply_robot_id;
//    uint8_t supply_num;
//} ext_supply_projectile_booking_t;  //未开放的接口
typedef __packed struct
{
    uint8_t level;
    uint8_t foul_robot_id;
} ext_referee_warning_t;
typedef __packed struct
{
    uint8_t dart_remaining_time;
} ext_dart_remaining_time_t;
typedef __packed struct
{
    uint8_t robot_id;
    uint8_t robot_level;
    uint16_t remain_HP;
    uint16_t max_HP;

    uint16_t shooter_id1_17mm_cooling_rate;
    uint16_t shooter_id1_17mm_cooling_limit;    //热量上限
    uint16_t shooter_id1_17mm_speed_limit;
    uint16_t shooter_id2_17mm_cooling_rate;
    uint16_t shooter_id2_17mm_cooling_limit;
    uint16_t shooter_id2_17mm_speed_limit;
    uint16_t shooter_id1_42mm_cooling_rate;
    uint16_t shooter_id1_42mm_cooling_limit;
    uint16_t shooter_id1_42mm_speed_limit;

    uint16_t chassis_power_limit;
    uint8_t mains_power_gimbal_output : 1;
    uint8_t mains_power_chassis_output : 1;
    uint8_t mains_power_shooter_output : 1;
} ext_game_robot_status_t;
typedef __packed struct
{
    uint16_t chassis_volt;
    uint16_t chassis_current;
    float chassis_power;
    uint16_t chassis_power_buffer;
    uint16_t shooter_id1_17mm_cooling_heat;  //当前热量
    uint16_t shooter_id2_17mm_cooling_heat;
    uint16_t shooter_id1_42mm_cooling_heat;
} ext_power_heat_data_t;
typedef __packed struct
{
    float x;
    float y;
    float z;
    float yaw;
} ext_game_robot_pos_t;
typedef __packed struct
{
    uint8_t power_rune_buff;
} ext_buff_t;
typedef __packed struct
{
    uint8_t energy_point;
    uint8_t attack_time;
} aerial_robot_energy_t;
typedef __packed struct
{
    uint8_t armor_id : 4;
    uint8_t hurt_type : 4;
} ext_robot_hurt_t;
typedef __packed struct
{
    uint8_t bullet_type;
    uint8_t bullet_freq;
    float bullet_speed;
} ext_shoot_data_t;
typedef __packed struct
{
    uint16_t bullet_remaining_num;
} ext_bullet_remaining_t;
typedef __packed struct
{
    uint32_t rfid_status;
} ext_rfid_status_t;
typedef __packed struct
{
    uint8_t dart_launch_opening_status;
    uint8_t dart_attack_target;
    uint16_t target_change_time;
    uint8_t first_dart_speed;
    uint8_t second_dart_speed;
    uint8_t third_dart_speed;
    uint8_t fourth_dart_speed;
    uint16_t last_dart_launch_time;
    uint16_t operate_launch_cmd_time;
} ext_dart_client_cmd_t;

typedef __packed struct
{
    uint16_t data_cmd_id;
    uint16_t sender_ID;
    uint16_t receiver_ID;
} ext_student_interactive_header_data_t;
//typedef __packed struct
// {
//     float data1;
//     float data2;
//     float data3;
//     uint8_t masks;
// } client_custom_data_t; //旧版接口
typedef __packed struct
{
    uint8_t *data;
} robot_interactive_data_t;
typedef __packed struct
{
    uint8_t operate_tpye;
    uint8_t layer;
} ext_client_custom_graphic_delete_t;
// typedef __packed struct
// {
//     uint8_t operate_tpye;
//     uint8_t graphic_tpye;
//     uint8_t graphic_name[5];
//     uint8_t layer;
//     uint8_t color;
//     uint8_t width;
//     uint16_t start_x;
//     uint16_t start_y;
//     uint16_t radius;
//     uint16_t end_x;
//     uint16_t end_y;
//     int16_t start_angle;
//     int16_t end_angle;
//     uint8_t text_lenght;
//     uint8_t text[30];
// } ext_client_graphic_draw_t; //2019版老接口
typedef __packed struct
{
    uint8_t graphic_name[3];
    uint32_t operate_tpye : 3;
    uint32_t graphic_tpye : 3;
    uint32_t layer : 4;
    uint32_t color : 4;
    uint32_t start_angle : 9;
    uint32_t end_angle : 9;
    uint32_t width : 10;
    uint32_t start_x : 11;
    uint32_t start_y : 11;
    uint32_t radius : 10;
    uint32_t end_x : 11;
    uint32_t end_y : 11;
} graphic_data_struct_t;

typedef __packed struct
{
    graphic_data_struct_t grapic_data_struct;
} ext_client_custom_graphic_single_t;
typedef __packed struct
{
    graphic_data_struct_t grapic_data_struct[2];
} ext_client_custom_graphic_double_t;
typedef __packed struct
{
    graphic_data_struct_t grapic_data_struct[5];
} ext_client_custom_graphic_five_t;
typedef __packed struct
{
    graphic_data_struct_t grapic_data_struct[7];
} ext_client_custom_graphic_seven_t;
typedef __packed struct
{
    graphic_data_struct_t grapic_data_struct;
    uint8_t data[30];
} ext_client_custom_character_t;

//** 接收数据包的接口
void Judge_Receive(uint8_t RxBuffer[]); //目前只对外提供最终在中断服务里调用的解码函数的接口

//** 发送包含自定义数据段的数据包
//void Tx_client_custom_data(client_custom_data_t *client_custom_data);        //打包自定义数据段及两级包头，并放到UART->TDR上  //旧版接口
void Tx_robot_interact_data(uint8_t *data, uint16_t data_length, uint16_t data_cmd_id, uint16_t sender_ID, uint16_t receiver_ID); //打包机器人间交互数据段及两级包头，并放到UART->TDR上
void Tx_client_custom_graphic_delete(ext_client_custom_graphic_delete_t *client_graphic_delete);                                  //2. 客户端删除图形 机器人间通信：0x0301。
void Tx_client_custom_graphic_single(ext_client_custom_graphic_single_t *client_graphic_draw);                                    //打包自定义图形数据包及两级包头，并放到UART->TDR上
void Tx_client_custom_graphic_double(ext_client_custom_graphic_double_t *client_graphic_draw);                                    //打包自定义图形数据包及两级包头，并放到UART->TDR上
void Tx_client_custom_graphic_five(ext_client_custom_graphic_five_t *client_graphic_draw);                                        //打包自定义图形数据包及两级包头，并放到UART->TDR上
void Tx_client_custom_graphic_seven(ext_client_custom_graphic_seven_t *client_graphic_draw);                                      //打包自定义图形数据包及两级包头，并放到UART->TDR上
void Tx_client_custom_character(ext_client_custom_character_t *client_graphic_draw);                                              //打包自定义图形数据包及两级包头，并放到UART->TDR上

#endif //__HW_JUDGEMENT_DECODE_H
