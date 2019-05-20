/* 
  bluetooth.h
*/
extern unsigned char bt_state;
extern unsigned char bt_index;
extern unsigned char bt_count;
extern unsigned char bt_dis_count;
extern unsigned char btBuf[100];
unsigned char bluetooth_init(unsigned char deviceType);
unsigned char bluetooth_send(unsigned char* dest_addr, unsigned char* data, int data_length);

// BlueTooth Init
void BT_M_Init();
void BT_S_Init();
void task_bt_update(void *arg);
void task_connect(void *arg);
unsigned char* bt_get_mac_addr(unsigned char addr_type);

// AT 커맨더 진입
void BT_AT();
//속도 설정 - 115200
void BT_SetBaud();
// Mac 주소 물어보기
void BT_MacAddress();
// 모드 설정
void BT_ControlMode();
// 마스터 / 슬레이브 변경
void BT_SetChangeRole();
// Master 설정
void BT_SetMaster();
// SLAVE 설정
void BT_SetSlave();
// 초기화
void BT_Reset();
//IMME 1
void BT_IMME();
//START
void BT_START();
//CONNECT
void BT_CONNET();
void bluetooth_print_buf(); // 임시
