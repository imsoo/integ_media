/* 
bluetooth.c
*/
#include <stdio.h>
#include <string.h>     //for strncmp
#include "stm32f4xx_hal.h"
#include "integ_mac.h"
#include "uart.h"
#include "bluetooth.h"
unsigned char bt_state;
unsigned char bt_index;
unsigned char bt_count;


unsigned char btBuf[100];
unsigned char bluetooth_init(unsigned char deviceType)
{
  bt_state = 0;
  bt_index = 0;
  bt_count = 0;
  if(deviceType == MASTER) {
    printf("MASTER 로 초기화\r\n");
    BT_M_Init();
  }
  // SLAVE
  else {
    printf("SLAVE 로 초기화\r\n");
    BT_S_Init();
  }
  printf("초기화 성공\r\n");
  return R_SUCCESS;
}

unsigned char bluetooth_send(unsigned char* dest_addr, unsigned char* data, int data_length)
{
  printf("[BLUETOOTH] 전송 시도\r\n");
  HAL_UART_Transmit(&huart2, data, data_length, 1000);
  return 0;
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
uint8_t SET_MODE[] = "AT+MODE2";
uint8_t MODE[8];

//FACTORY SETUP
uint8_t SET_RENEW[] = "AT+RENEW";
uint8_t RENEW[8];

//RESET MODULE
uint8_t SET_RESET[] = "AT+RESET";
uint8_t BT_RESET[8];

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
uint8_t NOTI[] = "AT+NOTI1";
uint8_t NOTI1[8];
uint8_t CONN_RESULT[8];
uint8_t CON_MAC1[] = "AT+CONA810871D1386";
uint8_t CON_MAC2[] = "AT+CONA810871B48F5";

/* 블루투스 모듈 모드 설정 */
// BlueTooth Init
// Master
void BT_M_Init()
{
  if(HAL_UART_Transmit(&huart2, AT, 2, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, OK, 2, 1000);
    HAL_UART_Transmit(&huart3, "1. AT\r\n", 7, 1000);
  }
  HAL_Delay(300);
  
  if(HAL_UART_Transmit(&huart2, SET_BAUD, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, BAUD, 8, 1000);
    HAL_UART_Transmit(&huart3, "2. 115200 baud\r\n", 16, 1000);
  }
  
  HAL_Delay(300);
  if(HAL_UART_Transmit(&huart2, SET_MODE, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, MODE, 8, 1000);
    HAL_UART_Transmit(&huart3, "3. Mode2\r\n", 10, 1000);
  }
  HAL_Delay(300);
  /*if(HAL_UART_Transmit(&huart2, NOTI, 8, 1000)==HAL_OK)
  {
  HAL_UART_Receive(&huart2, NOTI1, 8, 1000);
}
  HAL_Delay(300);*/
  if(HAL_UART_Transmit(&huart2, SET_IMME0, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, IMME, 8, 1000);
    HAL_UART_Transmit(&huart3, "4. IMME0\r\n", 10, 1000);
  }
  HAL_Delay(800);
  if(HAL_UART_Transmit(&huart2, SET_MASTER, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, RESULT_ROLE, 8, 1000);
    HAL_UART_Transmit(&huart3, "5. Master\r\n", 11, 1000);
  }
  HAL_Delay(300);
  /*
  if(HAL_UART_Transmit(&huart2, BT_START1, 8, 1000)==HAL_OK)
  {
  HAL_UART_Receive(&huart2, START1, 8, 1000);
  HAL_UART_Transmit(&huart3, "6. START\r\n", 10, 1000);
}
  HAL_Delay(300);*/
  if(HAL_UART_Transmit(&huart2, QR_MAC_ADDRESS, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, SAVE_ADDRESS, 20, 1000);
  }
  HAL_Delay(1000);
}
//Slave
void BT_S_Init()
{
  if(HAL_UART_Transmit(&huart2, AT, 2, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, OK, 2, 1000);
    HAL_UART_Transmit(&huart3, "1. AT\r\n", 7, 1000);
  }
  HAL_Delay(300);
  
  if(HAL_UART_Transmit(&huart2, SET_BAUD, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, BAUD, 8, 1000);
    HAL_UART_Transmit(&huart3, "2. 115200 baud\r\n", 16, 1000);
  }
  
  HAL_Delay(300);
  if(HAL_UART_Transmit(&huart2, SET_MODE, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, MODE, 8, 1000);
    HAL_UART_Transmit(&huart3, "3. Mode2\r\n", 10, 1000);
  }
  HAL_Delay(300);
  /*
  if(HAL_UART_Transmit(&huart2, NOTI, 8, 1000)==HAL_OK)
  {
  HAL_UART_Receive(&huart2, NOTI1, 8, 1000);
}
  HAL_Delay(300);*/
  if(HAL_UART_Transmit(&huart2, SET_IMME0, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, IMME, 8, 1000);
    HAL_UART_Transmit(&huart3, "4. IMME0\r\n", 10, 1000);
  }
  HAL_Delay(800);
  if(HAL_UART_Transmit(&huart2, SET_SLAVE, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, RESULT_ROLE, 8, 1000);
    HAL_UART_Transmit(&huart3, "5. Slave\r\n", 11, 1000);
  }
  HAL_Delay(300);
  /*
  if(HAL_UART_Transmit(&huart2, BT_START1, 8, 1000)==HAL_OK)
  {
  HAL_UART_Receive(&huart2, START1, 8, 1000);
  HAL_UART_Transmit(&huart3, "6. START\r\n", 10, 1000);
}
  HAL_Delay(300);*/
  if(HAL_UART_Transmit(&huart2, QR_MAC_ADDRESS, 8, 1000)==HAL_OK)
  {
    HAL_UART_Receive(&huart2, SAVE_ADDRESS, 20, 1000);
  }
  HAL_Delay(1000);
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
  if(!strncmp((char *)SAVE_ADDRESS, (char *)MAC1, 19))  
  {
    if(HAL_UART_Transmit(&huart2, CON_MAC2, 18, 1000)==HAL_OK)
    {
      HAL_UART_Receive(&huart2, CONN_RESULT, 8, 10000);
      HAL_UART_Transmit(&huart3, CONN_RESULT, 8, 1000);
    }
  }
  else if (!strncmp((char *)SAVE_ADDRESS, (char *)MAC2, 19))  
  {
    if(HAL_UART_Transmit(&huart2, CON_MAC1, 18, 1000)==HAL_OK)
    {
      HAL_UART_Receive(&huart2, CONN_RESULT, 8, 10000);
      HAL_UART_Transmit(&huart3, CONN_RESULT, 8, 1000);
    }
  }
}