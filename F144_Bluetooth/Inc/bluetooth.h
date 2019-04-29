/*      
    bluetooth.h
*/

// BlueTooth Init
void BT_M_Init();
void BT_S_Init();
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