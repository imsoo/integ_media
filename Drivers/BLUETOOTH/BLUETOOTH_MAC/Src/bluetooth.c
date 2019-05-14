/* 
bluetooth.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     //for strncmp
#include "stm32f4xx_hal.h"
#include "integ_mac.h"
#include "uart.h"
#include "bluetooth.h"
#include "display.h"

unsigned char bt_mac_addr[8];
unsigned char bt_broadcast_addr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char bt_state;
unsigned char bt_index;
unsigned char bt_count;
unsigned char bt_dis_count;
unsigned char btBuf[100];
unsigned char bluetooth_init(unsigned char deviceType)
{
  bt_state = 0;
  bt_index = 0;
  bt_count = 0;
  bt_dis_count = 0;
  
  if(deviceType == MASTER) {
    //printf("MASTER 로 초기화\r\n");
    BT_M_Init();
  }
  // SLAVE
  else {
    //printf("SLAVE 로 초기화\r\n");
    BT_S_Init();
  }
  return R_SUCCESS;
}

unsigned char bluetooth_send(unsigned char* dest_addr, unsigned char* data, int data_length)
{
  //printf("[BLUETOOTH] 전송 시도\r\n");
  HAL_UART_Transmit(&huart2, data, data_length, 1000);
  return 0;
}

void bluetooth_print_buf() {
  int i;
  
  for (i = 0; i < 10; i++) {
    printf("%c", btBuf[i]);
  }
  printf("\r\n");
  
}

//AT_COMMAND
uint8_t AT[] = "AT";
uint8_t OK[3];

//SET BAUD 115200
uint8_t SET_BAUD[] = "AT+BAUD4";
uint8_t BAUD[8];

//MAC QURRY
uint8_t QR_MAC_ADDRESS[] = "AT+ADDR?";
uint8_t SAVE_ADDRESS[20];
uint8_t MAC1[] = "OK+ADDR:A810871D1386";
uint8_t MAC2[] = "OK+ADDR:A810871B48F5";

//SET MODE
uint8_t SET_MODE[] = "AT+MODE0";
uint8_t MODE[8];

//FACTORY SETUP
uint8_t SET_RENEW[] = "AT+RENEW";
uint8_t RENEW[8];

//RESET MODULE
uint8_t SET_RESET[] = "AT+RESET";
uint8_t BT_RESET[8];

// START DISC
uint8_t SET_SCAN[] = "AT+DISC?";

//MASTER & SLAVE
uint8_t SET_MASTER[] = "AT+ROLE1";
uint8_t SET_SLAVE[] = "AT+ROLE0";
uint8_t RESULT_ROLE[8];
uint8_t QR_ROLE[] = "AT+ROLE?";
uint8_t ROLE[8];

//SET IMME(CENTRAL)
//IMME0 = AUTO  SCAN - CONNECT
//IMME1 = COMMAND - SCAN - CONNECT
uint8_t SET_IMME0[] = "AT+IMME0";
uint8_t SET_IMME1[] = "AT+IMME1";
uint8_t IMME[8];
//START
uint8_t BT_START1[] = "AT+START";
uint8_t START1[8];

//CONN RESULT
uint8_t NOTI[] = "AT+NOTP1";
uint8_t NOTI1[8];
uint8_t CONN_RESULT[8];
uint8_t CON_MAC1[] = "AT+CONA810871D1386";
uint8_t CON_MAC2[] = "AT+CONA810871B48F5";

uint8_t CONNL[] = "AT+CONNL";


void task_bt_update(void *arg)
{
  // AT
  if(strncmp((char*)arg, "1", 1)) {
    insert_display_message(INTEG_MSG, "[BLUETOOTH] 연결 됨\r\n"); 
    STATUS_TABLE[CONNECT_STATUS][BLUETOOTH] = CON;
  }
  else {
    insert_display_message(INTEG_MSG, "[BLUETOOTH] 연결 끊김\r\n"); 
    STATUS_TABLE[CONNECT_STATUS][BLUETOOTH] = DISCON;
  }
  display();
  integ_find_opt_link(NULL);
}

void task_connect(void *arg)
{
  HAL_UART_Transmit(&huart2, CON_MAC2, 18, 1000);
  HAL_Delay(100);
}

/* 블루투스 모듈 모드 설정 */
// BlueTooth Init
// Master
void BT_M_Init()
{  
  // BAUD
  HAL_UART_Transmit(&huart2, SET_BAUD, 8, 1000);
  HAL_Delay(100);
  
  // AT
  HAL_UART_Transmit(&huart2, AT, 2, 1000);
  HAL_Delay(100);  
  
  // MODE 0
  HAL_UART_Transmit(&huart2, SET_MODE, 8, 1000);
  HAL_Delay(100);
  
  // notp
  HAL_UART_Transmit(&huart2, NOTI, 8, 1000);
  HAL_Delay(300);
  
  // SET IMME1
  HAL_UART_Transmit(&huart2, SET_IMME1, 8, 1000);
  HAL_Delay(10);
  
  // SET ROLE1
  HAL_UART_Transmit(&huart2, SET_MASTER, 8, 1000);
  HAL_Delay(10);
  
  // SET_RESET
  HAL_UART_Transmit(&huart2, SET_RESET, 8, 1000);
  HAL_Delay(100);
  
  // SET START
  HAL_UART_Transmit(&huart2, BT_START1, 8, 1000);
  HAL_Delay(100);
  
  // QR_MAC
  HAL_UART_Transmit(&huart2, QR_MAC_ADDRESS, 8, 1000);
  HAL_Delay(100);
  
  
  // 주소 저장
  char temp[2];
  int index;
  for (index = 0; index < BLUETOOTH_ADDR_LEN; index++) {
    memcpy(temp, btBuf + (index * 2) + 8, 2);
    bt_mac_addr[index] = strtol(temp, NULL, 16);
  }
  HAL_Delay(100);
  
  
  // DISC
  HAL_UART_Transmit(&huart2, SET_SCAN, 8, 1000);
  HAL_Delay(100);
  
  // BT_CONNET()
  HAL_UART_Transmit(&huart2, CON_MAC2, 18, 1000);
  HAL_Delay(100);
}
//Slave
void BT_S_Init()
{
  // RENEW
  //HAL_UART_Transmit(&huart2, SET_RENEW, 8, 1000);
  HAL_Delay(1000);
  
  // BAUD
  HAL_UART_Transmit(&huart2, SET_BAUD, 8, 1000);
  HAL_Delay(100);
  
  // AT
  HAL_UART_Transmit(&huart2, AT, 2, 1000);
  HAL_Delay(100);
  
  // MODE 0
  HAL_UART_Transmit(&huart2, SET_MODE, 8, 1000);
  HAL_Delay(300);
  
  // notp
  HAL_UART_Transmit(&huart2, NOTI, 8, 1000);
  HAL_Delay(100);
  
  // SET IMME1
  HAL_UART_Transmit(&huart2, SET_IMME0, 8, 1000);
  HAL_Delay(10);
  
  // ROLE0 : SLAVE
  HAL_UART_Transmit(&huart2, SET_SLAVE, 8, 1000);
  HAL_Delay(10);
  
  // SET_RESET
  HAL_UART_Transmit(&huart2, SET_RESET, 8, 1000);
  HAL_Delay(10);
  
  // SET START
  //HAL_UART_Transmit(&huart2, BT_START1, 8, 1000);
  HAL_Delay(100);
  
  // DISC
  // HAL_UART_Transmit(&huart2, SET_SCAN, 8, 1000);
  HAL_Delay(100);
  
  // QR_MAC
  HAL_UART_Transmit(&huart2, QR_MAC_ADDRESS, 8, 1000);
  HAL_Delay(100);
  
  
  // 주소 저장
  char temp[2];
  int index;
  for (index = 0; index < BLUETOOTH_ADDR_LEN; index++) {
    memcpy(temp, btBuf + (index * 2) + 8, 2);
    bt_mac_addr[index] = strtol(temp, NULL, 16);
  }
  HAL_Delay(100);
}

unsigned char* bt_get_mac_addr(unsigned char addr_type) {
  if (addr_type == BROADCAST_ADDR) {
    return bt_broadcast_addr;
  }
  else {
    return bt_mac_addr;
  }
}

// AT 커맨더 진입
void BT_AT()
{
  if(HAL_UART_Transmit(&huart2, AT, 2, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, OK, 2, 1000);
    HAL_UART_Transmit(&huart3, OK, 2, 1000);
  }
  HAL_Delay(200);
}
//속도 설정 - 115200
void BT_SetBaud()
{
  if(HAL_UART_Transmit(&huart2, SET_BAUD, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, BAUD, 8, 1000);
    HAL_UART_Transmit(&huart3, BAUD, 8, 1000);
  }
}
// Mac 주소 물어보기
void BT_MacAddress()
{
  if(HAL_UART_Transmit(&huart2, QR_MAC_ADDRESS, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, SAVE_ADDRESS, 20, 1000);
    HAL_UART_Transmit(&huart3, SAVE_ADDRESS, 20, 1000);
  }
}
// 모드 설정
void BT_ControlMode()
{
  if(HAL_UART_Transmit(&huart2, SET_MODE, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, MODE, 8, 1000);
    HAL_UART_Transmit(&huart3, MODE, 8, 1000);
  }
  HAL_Delay(200);
  /*
  0: Transmission Mode
  1: PIO collection Mode + Mode 0
  2: Remote Control Mode + Mode 0
  */
}
// Master 설정
void BT_SetMaster()
{
  if(HAL_UART_Transmit(&huart2, SET_MASTER, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, RESULT_ROLE, 8, 1000);
    HAL_UART_Transmit(&huart3, RESULT_ROLE, 8, 1000);
  }
}
// SLAVE 설정
void BT_SetSlave()
{
  if(HAL_UART_Transmit(&huart2, SET_SLAVE, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, RESULT_ROLE, 8, 1000);
    HAL_UART_Transmit(&huart3, RESULT_ROLE, 8, 1000);
  }
}
// 마스터 / 슬레이브 변경
void BT_SetChangeRole()
{
  if(HAL_UART_Transmit(&huart2, QR_ROLE, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, ROLE, 8, 1000);
    HAL_UART_Transmit(&huart3, ROLE, 8, 1000);
    
    if(strncmp((char *)ROLE, "OK+Get:0", 8) == 0)
      BT_SetMaster();
    else if (strncmp((char *)ROLE, "OK+Get:1", 8) == 0)
      BT_SetSlave();
  }
}
// 초기화
void BT_Reset()
{
  if(HAL_UART_Transmit(&huart2, SET_RENEW, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, RENEW, 8, 1000);
    
    if(!strncmp((char *)RENEW, "OK+RENEW", 8))  
    {
      if(HAL_UART_Transmit(&huart2, AT+RESET, 8, 1000)==HAL_OK)
      {
        HAL_UART_Receive(&huart2, RESET, 8, 1000);
        HAL_UART_Transmit(&huart3, RESET, 8, 1000);
      }
    }
  }
}

//IMMIE 1
void BT_IMME()
{
  if(HAL_UART_Transmit(&huart2, SET_IMME1, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, IMME, 8, 1000);
    HAL_UART_Transmit(&huart3, IMME, 8, 1000);
  }
  BT_START();
}

//START
void BT_START()
{
  if(HAL_UART_Transmit(&huart2, BT_START1, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, START1, 8, 1000);
    HAL_UART_Transmit(&huart3, START1, 8, 1000);
  }
}

//CONNECT
void BT_CONNET()
{
  //uint8_t* i="a";
  
  //HAL_UART_Transmit(&huart2, CONNL, 8, 1000);
  
  
  if(!strncmp((char *)SAVE_ADDRESS, (char *)MAC1, 20))  
  {
    if(HAL_UART_Transmit(&huart2, CON_MAC2, 18, 1000)==HAL_OK)
    {
      //HAL_UART_Receive(&huart2, CONN_RESULT, 8, 10000);
      //HAL_UART_Transmit(&huart3, CONN_RESULT, 8, 1000);
    }
  }
  else if (!strncmp((char *)SAVE_ADDRESS, (char *)MAC2, 20))  
  {
    if(HAL_UART_Transmit(&huart2, CON_MAC1, 18, 1000)==HAL_OK)
    {
      //HAL_UART_Receive(&huart2, CONN_RESULT, 8, 10000);
      //HAL_UART_Transmit(&huart3, CONN_RESULT, 8, 1000);
    }
  }
  HAL_Delay(200);
  
  //HAL_UART_Transmit(&huart2, QR_MAC_ADDRESS, 8, 1000);
  //HAL_Delay(100);
  
}