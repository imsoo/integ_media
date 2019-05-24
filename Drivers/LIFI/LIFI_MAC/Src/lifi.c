/* 
  lifi.c
*/
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "integ_mac.h"
#include "uart.h"
#include "lifi.h"

unsigned char rx3_data[10];
int index = 0;
int volatile l_flag;

unsigned char lifi_mac_addr[8];
unsigned char lifi_broadcast_addr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char lifi_init(unsigned char deviceType)
{
  return R_FAIL;
}

unsigned char lifi_send(unsigned char* dest_addr, unsigned char* data, int data_length)
{
  HAL_UART_Transmit(&huart5,data,data_length,100);
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