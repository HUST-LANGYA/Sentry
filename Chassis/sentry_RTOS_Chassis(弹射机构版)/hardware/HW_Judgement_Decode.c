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
/////*extern*/ ext_supply_projectile_booking_t supply_projectile_booking; //δ���ŵĽӿ�
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


//** ��ͷ���
static judge_BOOL_t Check_SOF(judge_header_t frame_header);  //֡��ʼ�����
static uint16_t get_frame_dataLength(uint8_t RxBuffer[]);  //������ݶγ���
static uint8_t get_frame_seq(uint8_t RxBuffer[]); //��ð����seq
static uint8_t get_frame_header_CRC8(uint8_t RxBuffer[]); //��ð�ͷCRC8У��λ
static void get_frame_header(uint8_t RxBuffer[], judge_header_t* judge_head);  //�����ͷ
static judge_BOOL_t Check_frame_header_CRC(judge_header_t judgeRx_header);  //��ͷCRCУ��
static uint16_t get_frame_cmdID(uint8_t RxBuffer[]);   //���������ID

//** ��β���
static uint16_t get_frame_tail_CRC16(uint8_t RxBuffer[], uint16_t data_length);// ��ð�β(��У����)(����ȡ���Ч�������ûɶ����)
static judge_BOOL_t Check_wholeframe_CRC(uint8_t RxBuffer[], uint16_t data_length);// ȫ��CRCУ�飬��Ҫ֪����β��λ��

//** ���ݶ����
static void get_frame_data(uint8_t RxBuffer[], uint16_t data_length, uint8_t data_buffer[]);   //������ݶξ���(��Ҫ֪�����ݶγ���)
//static void get_game_status(uint8_t DataBuffer[], uint16_t data_length, ext_game_status_t* game_status); //1.����״̬���ݣ�0x0001������Ƶ�ʣ�1Hz 
//static void get_game_result(uint8_t DataBuffer[], uint16_t data_length, ext_game_result_t* game_result); //2.����������ݣ�0x0002������Ƶ�ʣ������������� 
//static void get_robot_HP(uint8_t DataBuffer[], uint16_t data_length, ext_game_robot_HP_t* robot_HP); //3.������Ѫ�����ݣ�0x0003������Ƶ�ʣ�1Hz 
//static void get_dart_status(uint8_t DataBuffer[], uint16_t data_length, ext_dart_status_t* dart_status);//4. ���ڷ���״̬��0x0004������Ƶ�ʣ����ڷ�����ͣ����ͷ�Χ�����л����ˡ� 
//static void get_ICRA_buff_debuff_zone_status(uint8_t DataBuffer[], uint16_t data_length, ext_ICRA_buff_debuff_zone_status_t* ICRA_buff_debuff_zone_status);//5. �˹�������ս���ӳ���ͷ���״̬��0x0005������Ƶ�ʣ�1Hz ���ڷ��ͣ����ͷ�Χ�����л����ˡ� 
//static void get_event_data(uint8_t DataBuffer[], uint16_t data_length, ext_event_data_t* event_data);//4. �����¼����ݣ�0x0101������Ƶ�ʣ��¼��ı���� 
//static void get_supply_projectile_action(uint8_t DataBuffer[], uint16_t data_length, ext_supply_projectile_action_t* supply_projectile_action);//5. ����վ������ʶ��0x0102������Ƶ�ʣ������ı���� 
//// static void get_supply_projectile_booking(uint8_t DataBuffer[], uint16_t data_length, ext_supply_projectile_booking_t* supply_projectile_booking);//6. ���󲹸�վ�������裺cmd_id (0x0103)������Ƶ�ʣ����� 10Hz��RM �Կ�����δ���� 
//static void get_referee_warning(uint8_t DataBuffer[], uint16_t data_length, ext_referee_warning_t* referee_warning); //7. ���о�����Ϣ��cmd_id (0x0104)������Ƶ�ʣ����淢������ 
//static void get_dart_remaining_time(uint8_t DatBuffer[], uint16_t data_length, ext_dart_remaining_time_t* dart_remaining_time);//9. ���ڷ���ڵ���ʱ��cmd_id (0x0105)������Ƶ�ʣ�1Hz ���ڷ��ͣ����ͷ�Χ�����������ˡ�
static void get_robot_status(uint8_t DataBuffer[], uint16_t data_length, ext_game_robot_status_t* robot_status);//8. ����������״̬��0x0201������Ƶ�ʣ�10Hz 
static void get_power_heat_data(uint8_t DataBuffer[], uint16_t data_length, ext_power_heat_data_t* power_heat_data);//9. ʵʱ�����������ݣ�0x0202������Ƶ�ʣ�50Hz 
//static void get_robot_pos(uint8_t DataBuffer[], uint16_t data_length, ext_game_robot_pos_t* robot_pos);//10. ������λ�ã�0x0203������Ƶ�ʣ�10Hz 
//static void get_buff(uint8_t DataBuffer[], uint16_t data_length, ext_buff_t* buff);//11. ���������棺0x0204������Ƶ�ʣ�״̬�ı���� 
//static void get_aerial_robot_energy(uint8_t DataBuffer[], uint16_t data_length, aerial_robot_energy_t* aerial_robot_energy);//12. ���л���������״̬��0x0205������Ƶ�ʣ�10Hz 
//static void get_robot_hurt(uint8_t DataBuffer[], uint16_t data_length, ext_robot_hurt_t* robot_hurt);// 13. �˺�״̬��0x0206������Ƶ�ʣ��˺��������� 
static void get_shoot_data(uint8_t DataBuffer[], uint16_t data_length, ext_shoot_data_t* shoot_data);//14. ʵʱ�����Ϣ��0x0207������Ƶ�ʣ��������
//static void get_bullet_remaining(uint8_t DataBuffer[], uint16_t data_length, ext_bullet_remaining_t* bullet_remaining);//15. ����ʣ�෢������0x0208������Ƶ�ʣ�1Hz ���ڷ��ͣ����л������Լ��ڱ����������ط��� 
//static void get_RFID_status(uint8_t DataBuffer[], uint16_t data_length, ext_rfid_status_t* RFID_status);//18. ������ RFID ״̬��0x0209������Ƶ�ʣ�1Hz�����ͷ�Χ����һ�����ˡ� 
//static void get_dart_client_cmd(uint8_t DataBuffer[], uint16_t data_length, ext_dart_client_cmd_t* dart_client_cmd);//19. ���ڻ����˿ͻ���ָ�����ݣ�0x020A������Ƶ�ʣ�10Hz�����ͷ�Χ����һ�����ˡ� 


//** �����˼佻�����ݵ����ݰ� 
/**
 * �������ݰ���һ��ͳһ�����ݶ�ͷ�ṹ�����ݶΰ���������ID���������Լ������ߵ�ID���������ݶΣ�
 * �����������ݵİ��ܹ������Ϊ 128 ���ֽڣ���ȥ frame_header,cmd_id �� frame_tail �� 9 ���ֽ���
 * �����ݶ�ͷ�ṹ�� 6 ���ֽڣ��ʶ����͵��������ݶ����Ϊ 113�������������� 0x0301 �İ�����Ƶ�� Ϊ 10Hz��
 */
 //���ڴ��ڶ������ ID�������� cmd_id ����Ƶ�����Ϊ 10Hz��������Ŵ���
//**ID ˵�� 
//**  1. ������ ID��
//**    1��Ӣ��(��);2������(��);3/4/5������(��);
//**    6������(��);7���ڱ�(��);9���״�վ(��);
//**    101��Ӣ��(��);102������(��);103/104/105������(��);
//**    106������(��);107���ڱ�(��); 109���״�վ(��)�� 
//**  2. �ͻ��� ID��
//**    0x0101 ΪӢ�۲����ֿͻ���(��);
//**    0x0102�����̲����ֿͻ���((��); 
//**    0x0103/0x0104/0x0105�����������ֿͻ���(��);
//**    0x0106�����в����ֿͻ���((��); 
//**    0x0165��Ӣ�۲����ֿͻ���(��);
//**    0x0166�����̲����ֿͻ���(��);
//**    0x0167/0x0168/0x0169�����������ֿͻ��˲���(��);
//**    0x016A�����в����ֿͻ���(��)��

////**����ѧ���������ݰ�������ͷ(����ͷ)
//static void get_stu_interact_header_data(uint8_t DataBuffer[], uint16_t data_length, ext_student_interactive_header_data_t* stu_interact_head);//1. �������ݽ�����Ϣ��0x0301������Ƶ�ʣ����� 10Hz

////**�����Զ������ݰ��Ĵ����ݶ�
//static void get_robot_interact_data(uint8_t DataBuffer[], uint16_t data_length, robot_interactive_data_t* robot_interact);//2. �������� �����˼�ͨ�ţ�0x0301������Ƶ�ʣ����� 10Hz 

/**
  * @brief  �������ϵͳ����(��������Լ�д�İ汾��ǰ����д�İ汾��19�ڱ��Ĵ�������)
  * @param  �����洢�������ݵ�����
  * @retval None
  */
uint8_t DataBuffer[JudgeBufBiggestSize];   //�������ݰ������ݶξ���
uint16_t cmd_id;//����ID
void Judge_Receive(uint8_t ReceiveBuffer[])
{
    extern uint8_t DataBuffer[];
    extern judge_header_t judge_head;  //�������ݰ��İ�ͷ���ƶξ���
    extern uint16_t cmd_id;

    //�Ȱ�DMA�����е����ݾ����ڴ��������λ�ã����������ڳ�������������һ����
    get_frame_header(ReceiveBuffer, &judge_head);   //ȡ����ͷ
    cmd_id = get_frame_cmdID(ReceiveBuffer);
    get_frame_data(ReceiveBuffer, judge_head.data_length, DataBuffer);  //ȡ�����ݶ�
    //��������ѭ���Ұ�ͷ�ķ�ʽ������DMA��������ͷ��ȷ����ͷ
    if (Check_SOF(judge_head)
        && Check_frame_header_CRC(judge_head)
        && Check_wholeframe_CRC(ReceiveBuffer, judge_head.data_length))   //��ͷSOF����Ͱ�ͷCRC8У��
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
            /* δ���ŵĽӿ� case CMD_ID_supply_projectile_booking:
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
        default:  /*������Ա�һ������*/ break;
        }
    }

}

/**
 * @brief �����ͷ�Ƿ���ȷΪ0xA5
 * @param ���ݰ�ͷ
 * @retval ָʾ�Ƿ���ȷ�Ĳ�������
 */
inline static judge_BOOL_t Check_SOF(judge_header_t frame_header)
{
    return ((frame_header.SOF == START_OF_FRAME) ? judge_TRUE : judge_FALSE);
}


/**
 * @brief ��ȡ���ݰ���Ч���ݶεĳ���
 * @param RxBuffer
 * @return ��Ч���ݶεĳ���
*/
inline static uint16_t get_frame_dataLength(uint8_t RxBuffer[])
{// ! ע���������Լ�û���Թ�������ȷ�������Ǵ�˻���С��
    return (uint16_t)(((RxBuffer[2] << 8) | RxBuffer[1]) & 0xffff);
}

/**
 * @brief ��ȡ��ͷ��seqֵ����������ɶ�������⣩
 * @param RxBuffer
 * @return ��ǰ��seqֵ
*/
inline static uint8_t get_frame_seq(uint8_t RxBuffer[])
{
    return (uint8_t)(RxBuffer[3]);
}

/**
 * @brief ��ȡ��ͷ��CRC8У����
 * @param RxBuffer
 * @return CRCУ����
*/
inline static uint8_t get_frame_header_CRC8(uint8_t RxBuffer[])
{
    return (uint8_t)(RxBuffer[4]);
}


/**
 * @brief ��ȡ��ͷ�ṹ��
 * @param RxBuffer
 * @param ��ͷ�ṹ��ָ��
*/
static void get_frame_header(uint8_t RxBuffer[], judge_header_t* judge_head)
{
    judge_head->SOF = RxBuffer[0];
    judge_head->data_length = get_frame_dataLength(RxBuffer);
    judge_head->seq = get_frame_seq(RxBuffer);
    judge_head->CRC8 = get_frame_header_CRC8(RxBuffer);
}


/**
 * @brief ��ͷCRCУ��
 * @param ���ݰ�ͷ
 * @retval ָʾ�Ƿ���ȷ�Ĳ�������
 */
inline static judge_BOOL_t Check_frame_header_CRC(judge_header_t judge_head)
{
    return (Verify_CRC8_Check_Sum((uint8_t*)&judge_head, 5) ? judge_TRUE : judge_FALSE);
}

/**
 * @brief ��ȡcmd_id
 * @param RxBuffer
 * @return cmd_id
*/
inline static uint16_t get_frame_cmdID(uint8_t RxBuffer[])
{
    return (uint16_t)(((RxBuffer[6] << 8) | RxBuffer[5]) & 0xffff);
}

/**
 * @brief ��ȡ��β��CRC16У����
 * @param RxBuffer
 * @param data_length
 * @return CRC16У����
*/
inline static uint16_t get_frame_tail_CRC16(uint8_t RxBuffer[], uint16_t data_length)
{
    return (uint16_t)(((RxBuffer[FRAME_HEAD_SIZE + CMD_ID_SIZE + data_length + 1] << 8) | RxBuffer[FRAME_HEAD_SIZE + CMD_ID_SIZE + data_length]) & 0xffff);
}


/**
 * @brief ȫ��CRCУ�飬��Ҫ֪����β��λ��
 * @param ȫ���ݰ������ݶγ���
 * @retval ָʾ�Ƿ���ȷ�Ĳ�������
 */
inline static judge_BOOL_t Check_wholeframe_CRC(uint8_t RxBuffer[], uint16_t data_length)
{
    return (Verify_CRC16_Check_Sum(RxBuffer, data_length + FRAME_HEAD_SIZE + CMD_ID_SIZE + FRAME_TAIL_SIZE) ? judge_TRUE : judge_FALSE);
}

/**
 * @brief ��ȡ��Ч���ݶ�
 * @param RxBuffer 
 * @param data_length 
 * @param data_buffer 
*/
static void get_frame_data(uint8_t RxBuffer[], uint16_t data_length, uint8_t data_buffer[])
{
    memcpy(data_buffer, RxBuffer + (FRAME_HEAD_SIZE + CMD_ID_SIZE), data_length);
}

///**
// * @brief ����״̬���ݣ�0x0001������Ƶ�ʣ�1Hz�����ͷ�Χ�����л����ˡ�
// * @brief 0-3 bit����������
// * @brief   1��RoboMaster ���״�ʦ����
// * @brief   2��RoboMaster ���״�ʦ��������
// * @brief   3��ICRA RoboMaster �˹�������ս��
// *
// * @brief 4-7 bit����ǰ�����׶�
// * @brief   0��δ��ʼ������
// * @brief   1��׼���׶Σ�
// * @brief   2���Լ�׶Σ�
// * @brief   3��5s ����ʱ��
// * @brief   4����ս�У�
// * @brief   5������������
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
// * @brief ����������ݣ�0x0002������Ƶ�ʣ������������ͣ����ͷ�Χ�����л����ˡ�
// * @brief 0 ƽ�� 1 �췽ʤ�� 2 ����ʤ��
// * @param DataBuffer
// * @param data_length
// * @param game_result
//*/
//inline static void get_game_result(uint8_t DataBuffer[], uint16_t data_length, ext_game_result_t* game_result)
//{
//    game_result->winner = DataBuffer[0];
//}

///**
// * @brief ������Ѫ�����ݣ�0x0003������Ƶ�ʣ�1Hz�����ͷ�Χ�����л����ˡ�
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
// * @brief ���ڷ���״̬��0x0004������Ƶ�ʣ����ڷ�����ͣ����ͷ�Χ�����л����ˡ�
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
// * @brief �˹�������ս���ӳ���ͷ���״̬��0x0005������Ƶ�ʣ�1Hz ���ڷ��ͣ����ͷ�Χ�����л����ˡ�
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
// * @brief �����¼����ݣ�0x0101������Ƶ�ʣ�1Hz ���ڷ��ͣ����ͷ�Χ�����������ˡ�
// * @brief bit 0-1������ͣ��ƺռ��״̬
// * @brief   0 Ϊ�޻�����ռ�죻
// * @brief   1 Ϊ���л�������ռ�쵫δͣ����
// * @brief   2 Ϊ���л�������ռ�첢ͣ��
// * @brief bit 2-3��������������״̬��
// * @brief   bit2 ΪС�������ؼ���״̬��1 Ϊ�Ѽ��
// * @brief   bit3 Ϊ���������ؼ���״̬��1 Ϊ�Ѽ��
// * @brief bit 4�������������⻤��״̬��
// * @brief   1 Ϊ���������⻤��Ѫ����
// * @brief   0 Ϊ���������⻤��Ѫ����
// * @brief bit 5 -31: ����
// * @param DataBuffer
// * @param data_length
// * @param event_data
//*/
//static void get_event_data(uint8_t DataBuffer[], uint16_t data_length, ext_event_data_t* event_data)
//{
//    /*�ٷ�ֽ�涨��ǰ��λ�Ķ��壬�����6~31λ����*/
//    event_data->event_type = DataBuffer[0] & 0x1F;//(((DataBuffer[10] << 8 | DataBuffer[9])) << 8 | DataBuffer[8]) << 8 | DataBuffer[7];
//}

///**
// * @brief ����վ������ʶ��0x0102������Ƶ�ʣ������������ͣ����ͷ�Χ������������
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
//// * @brief                   �ɽӿڣ����겻֧��
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
// * @brief ���о�����Ϣ��cmd_id (0x0104)������Ƶ�ʣ����淢�����ͣ����ͷ�Χ�����������ˡ�
// * @brief 1 ���Լ� 5 ������ʱ�������� ID Ϊ 0; �����ļ�����ʱ�������� ID Ϊ��������� ID
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
// * @brief ���ڷ���ڵ���ʱ��cmd_id (0x0105)������Ƶ�ʣ�1Hz ���ڷ��ͣ����ͷ�Χ�����������ˡ�
// * @param DatBuffer
// * @param data_length
// * @param dart_remaining_time
//*/
//void get_dart_remaining_time(uint8_t DatBuffer[], uint16_t data_length, ext_dart_remaining_time_t* dart_remaining_time)
//{
//    dart_remaining_time->dart_remaining_time = DataBuffer[0];
//}

/**
 * @brief ����������״̬��0x0201������Ƶ�ʣ�10Hz�����ͷ�Χ����һ�����ˡ�
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
    
    /*ÿ����ȴֵ*/
    /*ǹ����������*/
    robot_status->shooter_id1_17mm_cooling_rate = ((uint16_t)DataBuffer[7] << 8 | DataBuffer[6]);
    robot_status->shooter_id1_17mm_cooling_limit = ((uint16_t)DataBuffer[9] << 8 | DataBuffer[8]);
    robot_status->shooter_id1_17mm_speed_limit = ((uint16_t)DataBuffer[11] << 8 | DataBuffer[10]);
    
    robot_status->shooter_id2_17mm_cooling_rate = ((uint16_t)DataBuffer[13] << 8 | DataBuffer[12]);
    robot_status->shooter_id2_17mm_cooling_limit = 0;
    robot_status->shooter_id2_17mm_speed_limit = 0;
    
    robot_status->shooter_id1_42mm_cooling_rate = ((uint16_t)DataBuffer[7] << 8 | DataBuffer[6]);
    robot_status->shooter_id1_42mm_cooling_limit = ((uint16_t)DataBuffer[9] << 8 | DataBuffer[8]);
    robot_status->shooter_id1_42mm_speed_limit = ((uint16_t)DataBuffer[11] << 8 | DataBuffer[10]);
    
    //�����������̹���
    robot_status->chassis_power_limit = DataBuffer[16];

    //���ص�Դ������
    robot_status->mains_power_gimbal_output  = (DataBuffer[17] & 0x01)>>0;
    robot_status->mains_power_chassis_output = (DataBuffer[17] & 0x02)>>1;
    robot_status->mains_power_shooter_output = (DataBuffer[17] & 0x04)>>2;
}

/**
 * @brief ʵʱ�����������ݣ�0x0202������Ƶ�ʣ�50Hz�����ͷ�Χ����һ�����ˡ�
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
// * @brief ������λ�ã�0x0203������Ƶ�ʣ�10Hz�����ͷ�Χ����һ�����ˡ� 
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
// * @brief ���������棺0x0204������Ƶ�ʣ�1Hz ���ڷ��ͣ����ͷ�Χ����һ�����ˡ� 
// * @brief   bit 0��������Ѫ����Ѫ״̬ 
// * @brief   bit 1��ǹ��������ȴ���� 
// * @brief   bit 2�������˷����ӳ� 
// * @brief   bit 3�������˹����ӳ� 
// * @brief   ���� bit ����
// * @param DataBuffer 
// * @param data_length 
// * @param buff 
//*/
//static void get_buff(uint8_t DataBuffer[], uint16_t data_length, ext_buff_t* buff)
//{
//    buff->power_rune_buff = DataBuffer[0];
//}


///**
// * @brief ���л���������״̬��0x0205������Ƶ�ʣ�10Hz�����ͷ�Χ����һ�����ˡ� 
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
// * @brief �˺�״̬��0x0206������Ƶ�ʣ��˺��������ͣ����ͷ�Χ����һ�����ˡ�
// * @brief   bit 0-3����Ѫ���仯����Ϊװ���˺�������װ�� ID��
//                    ������ֵΪ 0-4 �Ŵ�������˵����װ��Ƭ������Ѫ���仯���ͣ��ñ�����ֵΪ 0�� 
// * @brief   bit 4-7��Ѫ���仯���� 
// * @brief      0x0 װ���˺���Ѫ�� 
// * @brief      0x1 ģ����߿�Ѫ�� 
// * @brief      0x2 �����ٿ�Ѫ�� 
// * @brief      0x3 ��ǹ��������Ѫ�� 
// * @brief      0x4 �����̹��ʿ�Ѫ�� 
// * @brief      0x5 װ��ײ����Ѫ 
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
 * @brief ʵʱ�����Ϣ��0x0207������Ƶ�ʣ�������ͣ����ͷ�Χ����һ������
 * @param DataBuffer 
 * @param data_length 
 * @param shoot_data 
*/
static void get_shoot_data(uint8_t DataBuffer[], uint16_t data_length, ext_shoot_data_t* shoot_data)
{
    shoot_data->bullet_type = DataBuffer[0]; //�ӵ�����: 1��17mm ���� 2��42mm ���� 
    shoot_data->bullet_freq = DataBuffer[1]; //�ӵ���Ƶ
    shoot_data->bullet_speed = (float)((DataBuffer[5] << 8 | DataBuffer[4]) << 16 | (DataBuffer[3] << 8 | DataBuffer[2]));
}

///**
// * @brief �ӵ�ʣ�෢������0x0208��
// * @brief ����Ƶ�ʣ�1Hz ���ڷ��ͣ�
// * @brief ���л����ˣ��ڱ��������Լ� ICRA ���������ط��ͣ����ͷ�Χ����һ�����ˡ� 
// * @param DataBuffer 
// * @param data_length 
// * @param bullet_remaining 
//*/
//static void get_bullet_remaining(uint8_t DataBuffer[], uint16_t data_length, ext_bullet_remaining_t* bullet_remaining)
//{
//    bullet_remaining->bullet_remaining_num = ((uint16_t)DataBuffer[1] << 8 | DataBuffer[0]);
//}

///**
// * @brief ������ RFID ״̬��0x0209������Ƶ�ʣ�1Hz�����ͷ�Χ����һ������.
// * @param DataBuffer
// * @param data_length
// * @param RFID_status
//*/
//void get_RFID_status(uint8_t DataBuffer[], uint16_t data_length, ext_rfid_status_t* RFID_status)
//{
//    RFID_status->rfid_status = ((uint32_t)DataBuffer[3]<<8 | DataBuffer[2])<<16 | ((uint32_t)DataBuffer[1]<<8 | DataBuffer[0]);
//}

///**
// * @brief ���ڻ����˿ͻ���ָ�����ݣ�0x020A������Ƶ�ʣ�10Hz�����ͷ�Χ����һ�����ˡ� 
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
// * @brief �������ݽ�����Ϣ��0x0301�� 
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

///**                             �ɽӿڣ����겻֧��
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
// * @brief �������� �����˼�ͨ�ţ�0x0301�� 
// * @param DataBuffer
// * @param data_length
// * @param robot_interact
//*/
//static void get_robot_interact_data(uint8_t DataBuffer[], uint16_t data_length, robot_interactive_data_t* robot_interact)
//{
//    /* �⿪ѧ���������İ�ͷ���õ��Զ�������ݰ�ID�����ͷ��ͽ��շ�ID */
//    get_stu_interact_header_data(DataBuffer, judge_head.data_length, &stu_interact_head);

//    //����Ĳ��֣��Ϳ����ڸ�������֮�������Ҫ��Щ����ͨ�ŵ����ݣ���������Ӧ�Ľ�����
//}


///**
// * @brief �Ӳ���ϵͳ�����������˷��ͽ�������
// * @brief ��Щ��Ϣֻ�����ѷ��Ļ����˷���
// * @param robot_interact_data 
// * @param data_length 
// * @param receiver_ID 
//*/

///**
// * @brief �Ӳ���ϵͳ�����������˷��ͽ�������
// * @brief ��Щ��Ϣֻ�����ѷ��Ļ����˷���
// * @param data  ��Ҫ���͵���Ч�����ݱ��� 
// * @param data_length   //ָʾdata����ĳ���
// * @param data_cmd_id   //����ID��
// * @param sender_ID     //��������˵�ID�����ԴӲ���ϵͳ�����İ�����
// * @param receiver_ID   //���շ���ID��
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
//    for (uint16_t i = 0; i < data_length; i++) pTxBuffer[i + STU_HEAD_SIZE] = data[i];  //�����Ժ���û�и���ķ������о��������������ܻ����

//    USART_SendArray(USART1,pTxBuffer,data_length);  //�򴮿ڷ������ݰ�

//    free(pTxBuffer); //�ͷſ��ٵ����ݰ��Ŀռ�
//}

///**
// * @brief �Ӳ���ϵͳ��ͻ��˷���ͼ��ɾ������
// * @brief ��Щ��Ϣֻ�����ͻ����˶�Ӧ�Ŀͻ��˷���
// * @param client_graphic_delete
//*/
//void Tx_client_custom_graphic_delete(ext_client_custom_graphic_delete_t* client_graphic_delete)
//{
//}

///**
// * @brief �Ӳ���ϵͳ��ͻ��˷��ͻ���ͼ������
// * @brief ��Щ��Ϣֻ�����ͻ����˶�Ӧ�Ŀͻ��˷���
// * @param client_graphic_draw
//*/
//void Tx_client_custom_graphic_single(ext_client_custom_graphic_single_t* client_graphic_draw)
//{
//}

///**
// * @brief �Ӳ���ϵͳ��ͻ��˷��ͻ���ͼ������
// * @brief ��Щ��Ϣֻ�����ͻ����˶�Ӧ�Ŀͻ��˷���
// * @param client_graphic_draw
//*/
//void Tx_client_custom_graphic_double(ext_client_custom_graphic_double_t* client_graphic_draw)
//{
//}

///**
// * @brief �Ӳ���ϵͳ��ͻ��˷��ͻ���ͼ������
// * @brief ��Щ��Ϣֻ�����ͻ����˶�Ӧ�Ŀͻ��˷���
// * @param client_graphic_draw
//*/
//void Tx_client_custom_graphic_five(ext_client_custom_graphic_five_t* client_graphic_draw)
//{
//}

///**
// * @brief �Ӳ���ϵͳ��ͻ��˷��ͻ���ͼ������
// * @brief ��Щ��Ϣֻ�����ͻ����˶�Ӧ�Ŀͻ��˷���
// * @param client_graphic_draw
//*/
//void Tx_client_custom_graphic_seven(ext_client_custom_graphic_seven_t* client_graphic_draw)
//{
//}

///**
// * @brief �Ӳ���ϵͳ��ͻ��˷��ͻ����ַ�����
// * @brief ��Щ��Ϣֻ�����ͻ����˶�Ӧ�Ŀͻ��˷���
// * @param client_graphic_draw
//*/
//void Tx_client_custom_character(ext_client_custom_character_t* client_graphic_draw)
//{
//}
