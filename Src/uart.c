/*      

uart.c

*/
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "uart.h"
#include "queue.h"
#include "task.h"
#include "mac_interface_uart.h"
#include "mac_interface.h"      // ASSO RSP
#include "bluetooth.h"
#include "integ_mac.h"
#include "frame_queue.h"

#define ETX 0x04

#define putc(c,f)  fputc((c),(f))
#define putchar(c) fputc((c),stdout)
#define getc(f) fgetc(f)
#define getchar() fgetc(stdin)

FILE __stdout;
FILE __stdin;


UART_HandleTypeDef huart2;      // <---> BT
UART_HandleTypeDef huart3;      // <---> PC
UART_HandleTypeDef huart4;      // <---> CC2530
UART_HandleTypeDef huart5;      // <---> LIFI

uint8_t rxData;
uint8_t rxData_b;
uint8_t txData;
unsigned char uart_busy;
unsigned char uart_back;

int fputc(int ch, FILE *f)
{
  if (ch == '\n')
    fputc('\r', f);
  __disable_interrupt();
  if (!uart_busy) {
    txData = ch;
    HAL_UART_Transmit_IT(&huart3, (uint8_t *)&txData, 1);
    uart_busy = 1;
  }
  else {
    while(qo_insert(ch) == 0) {
      __enable_interrupt();
      HAL_Delay(100);
      __disable_interrupt();
    }
  }
  __enable_interrupt();
  
  return(ch);
}

int fgetc(FILE *f)
{
  uint8_t ch;
  do {
    __disable_interrupt();
    ch = qi_delete();
    __enable_interrupt();
  } while (ch == 0);
  
  if (ch == ETX)
    return (-1);
  else
    return (ch);
}

char *fgets(char *s, int n, FILE *f)
{
  int ch;
  char *p = s;
  
  while (n > 1) {
    ch = fgetc(f);
    if (ch == EOF) {
      *p = '\0';
      return NULL;
    }
    *p++ = ch;
    n--;
    if (ch == '\n')
      break;
  }
  if (n)
    *p = '\0';
  
  return s;
}

void uart_echo(uint8_t ch)
{
  if (ch == '\n')
    uart_echo('\r');
  
  // Backspace 입력시 한칸 뒤로가서 공백으로 만들기
  if (ch == '\b' && uart_back == 0) {
    if (qi_remove() == 0)
      return;
    uart_back = 1;
    uart_echo('\b');
    uart_echo(0x20);
    uart_back = 0;
  }
  
  if (!uart_busy) {
    txData = ch;
    HAL_UART_Transmit_IT(&huart3, (uint8_t *)&txData, 1);
    uart_busy = 1; 
  }
  else 
    qo_insert(ch);
}
// UART2 <---> BT
void UART2_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    //Error_Handler();
  }
  HAL_UART_Receive_IT(&huart2, btBuf, 1);
}

// UART3 <---> PC
void UART3_Init(void)
{
  uart_busy = 0;
  /* Queue Init */ 
  q_init();
  /* USART3 Init */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    // Error_Handler();
  }
  HAL_UART_Transmit_IT(&huart3, &txData, 1);
  HAL_UART_Receive_IT(&huart3, &rxData, 1);
}

// UART4 <---> CC2530
void UART4_Init(void)
{
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    // Error_Handler();
  }
  HAL_UART_Receive_IT(&huart4, macBuf, 4);
}

// UART5 <---> LIFI
void UART5_Init(void)
{
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 38400;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    // Error_Handler();
  }
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  struct task task;
  unsigned char t_char;
  if(huart->Instance == huart2.Instance) {
    t_char = btBuf[bt_index];
    //printf("%c ", t_char);
    switch(bt_state) {
    case 0:
      if(t_char == 'O') { bt_state = 1;     bt_index++; }
      else{ bt_state = 0; HAL_UART_Receive(&huart2, btBuf + bt_index + 1, t_char - 1, 1000); bt_index = 0; 
      frame_queue_insert(btBuf); 
      }
      break;
    case 1:
      if(t_char == 'K') { bt_state = 2;     bt_index++;}
      break;
    case 2:
      if(t_char == '+') { bt_state = 3;     bt_index++;}
      else { bt_state = 1; bt_index = 1; btBuf[0] = t_char;}
      break;
    case 3:
      if(t_char == 'S') { bt_state = 0; HAL_UART_Receive(&huart2, btBuf + bt_index + 1, 4, 1000); bt_index = 0;}
      else if(t_char == 'A') { bt_state = 0; HAL_UART_Receive(&huart2, btBuf + bt_index + 1, 16, 1000); bt_index = 0;}
      else if(t_char == 'L') { bt_state = 0; HAL_UART_Receive(&huart2, btBuf + bt_index + 1, 3, 1000); bt_index = 0; }
      else if(t_char == 'C') { bt_state = 0; HAL_UART_Receive(&huart2, btBuf + bt_index + 1, 3, 1000); bt_index = 0; }
      else {}
      break;
    }
    HAL_UART_Receive_IT(&huart2, btBuf + bt_index, 1);
    
    /*
    dataLength = btBuf[0];
    HAL_UART_Receive(&huart2, btBuf + 1, dataLength - 1, 1000);
    frame_queue_insert(btBuf);
    HAL_UART_Receive_IT(&huart2, btBuf, 1);
    */
  }
  else if (huart->Instance == huart3.Instance)
  {
    if (rxData != ETX) {
      if (rxData == '\r')
        rxData = '\n';
      uart_echo(rxData);
    }
    
    // Backspace는 입력 큐에 넣지 않음
    if (rxData != '\b')
      qi_insert(rxData);
    HAL_UART_Receive_IT(&huart3, (uint8_t *)&rxData, 1);
    
    if (rxData == ETX || rxData == '\n') {
      task.fun = task_cmd;
      strcpy(task.arg, "");
      task_insert(&task);
    }
  }
  else if (huart->Instance == huart4.Instance) {
    
    if(macBuf[1] != 0x00)
      HAL_UART_Receive(&huart4, macBuf + 4, macBuf[1] + 1, 1000);
    
    if((macBuf[2] >> 5) == 0x03) {      // SRSP
      if(sreqFlag == STATE_SREQ)
        sreqFlag = 0;
    }
    else if((macBuf[2] >> 5) == 0x02) {  // AREQ
      if(macBuf[3] == 0x83) {// BEACON
        if(sreqFlag == STATE_SYNC_CNF) {
          sreqFlag = 0;
          stateStatus = 0x00;
        }
      }
      else {
        if(macBuf[3] == 0x81) {// MAC_ASSOCIATE_IND 
          task.fun = macAssociateRsp;
          strcpy(task.arg, "");
          task_insert(&task);
        }
        else if(macBuf[3] == 0x85) { // MAC_DATA_IND
#define DATA_FIELD 48
          frame_queue_insert(macBuf + 48);
        }
        else {
          if(macBuf[3] == 0x8C) {    // MAC_SCAN_CNF
            stateStatus = macBuf[U_AREQ_STATUS_FIELD];
            sreqFlag = 0;
          }
          
          else if(macBuf[3] == 0x8E) {    // MAC_START_CNF
            stateStatus = macBuf[U_AREQ_STATUS_FIELD];
            sreqFlag = 0;
          }
          
          else if(macBuf[3] == 0x82) {    // MAC_ASSOCIATE_CNF
            stateStatus = macBuf[U_AREQ_STATUS_FIELD];
            sreqFlag = 0;
          }
          
          else if(macBuf[3] == 0x84 && sreqFlag == STATE_DATA_CNF) {    // MAC_DATA_CNF
            stateStatus = macBuf[U_AREQ_STATUS_FIELD];
            sreqFlag = 0;
          }
          
          else if(macBuf[3] == 0x8D && sreqFlag == STATE_ASSOCIATE_RSP_CNF) {    // MAC_COMM_STATUS_IND
            stateStatus = macBuf[U_AREQ_STATUS_FIELD];
            sreqFlag = 0;
          }
        }
      }
      /*
      for (int i=0; i<macBuf[1]+5; i++)
      printf("%02X ", macBuf[i]);
      printf("\r\n");
      */
    }
    HAL_UART_Receive_IT(&huart4, macBuf, 4);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  uint8_t ch;
  
  if (huart->Instance == huart3.Instance)
  {
    if ((ch = qo_delete()) == 0) {
      uart_busy = 0;
    }
    else {
      txData = ch; 
      HAL_UART_Transmit_IT(&huart3, &txData, 1);
    }
  }
  else  if (huart->Instance == huart4.Instance) {
    
  }
}

