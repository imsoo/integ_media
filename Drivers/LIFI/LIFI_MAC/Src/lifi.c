/* 
  lifi.c
*/
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "integ_mac.h"
#include "uart.h"
#include "lifi.h"

unsigned char lifi_send_index, lifi_recv_index, lifi_flag, lifi_rx_data;
unsigned char lifiBuf[LIFI_BUF_LEN];
unsigned char lifi_mac_addr[8];
unsigned char lifi_broadcast_addr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char lifi_init(unsigned char deviceType)
{
  lifi_send_index = lifi_recv_index = lifi_flag = 0;
  return R_SUCCESS;
  //return R_FAIL;
}

unsigned char lifi_send(unsigned char* dest_addr, unsigned char* data, int data_length)
{
  unsigned char i;
  memcpy(lifiBuf, data, INTEG_FRAME_HEADER_LEN);
  memcpy(lifiBuf + INTEG_FRAME_HEADER_LEN, ((INTEG_FRAME*) data)->data, data_length - INTEG_FRAME_HEADER_LEN);
  
  
  lifi_send_index = 0;
  for(i = 0; i < data_length; i++) {
    HAL_UART_Transmit(&huart5,&lifiBuf[lifi_send_index++],1,100);   // DATA Àü¼Û
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