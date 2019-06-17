/* 
lifi.c
*/
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "integ_mac.h"
#include "uart.h"
#include "lifi.h"
unsigned char asc[128];
unsigned char lifiBuf_index;
unsigned char lifiBuf1[LIFI_BUF_LEN];
unsigned char lifiBuf2[LIFI_BUF_LEN];
unsigned char lifiBuf3[LIFI_BUF_LEN];

unsigned char lifi_state;
unsigned char lifi_index;
unsigned char lifi_recv_count;
unsigned char lifi_sof_count;
unsigned char lifiBuf_recv_count;

unsigned char lifi_send_index, lifi_recv_index, lifi_flag, lifi_rx_data;
unsigned char lifi_mac_addr[8];
unsigned char lifi_broadcast_addr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char lifi_init(unsigned char deviceType)
{
  lifiBuf_index = 0;
  lifiBuf_recv_count = 1;
  lifi_state = 0;
  lifi_index = 0;
  lifi_recv_count = 0;
  lifi_sof_count = 0;
  
  lifi_send_index = lifi_recv_index = lifi_flag = 0;
  //return R_SUCCESS;
  return R_FAIL;
}

unsigned char lifi_send(unsigned char* dest_addr, unsigned char* data, int data_length)
{
  int i, j;
  memcpy(lifiBuf_p[0], lifi_sof_arr, 3);
  memcpy(lifiBuf_p[0] + 3, data, INTEG_FRAME_HEADER_LEN);
  memcpy(lifiBuf_p[0] + INTEG_FRAME_HEADER_LEN + 3, ((INTEG_FRAME*) data)->data, data_length - INTEG_FRAME_HEADER_LEN);
  
  lifi_send_index = 0;
  for(j = 0; j < 3; j++) {
    //HAL_UART_Transmit(&huart5,lifiBuf_p[0],data_length+3,100);   // DATA 전송
    
    
    for(i = 0; i < data_length + 3; i++) {
      //printf("%02x %d", lifiBuf_p[0][i], data_length);
      HAL_UART_Transmit(&huart5,&lifiBuf_p[0][i],1,1);   // DATA 전송
      HAL_Delay(2);
    }
    
    HAL_Delay(10);
  }
  
  return 0;
}

unsigned char* lifi_get_mac_addr(unsigned char addr_type) {
  if (addr_type == BROADCAST_ADDR) {
    return lifi_broadcast_addr;
  }
  else {
    return lifi_mac_addr;
  }
}

unsigned char lifi_calcFCS(unsigned char *pMsg, unsigned char len)
{
  unsigned char result = 0;
  while (len--)
  {
    result ^= *pMsg++;
  }
  return result;
}