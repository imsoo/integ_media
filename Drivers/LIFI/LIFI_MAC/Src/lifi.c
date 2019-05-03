/* 
  lifi.c
*/
#include <stdio.h>
#include "integ_mac.h"

unsigned char lifi_init(unsigned char deviceType)
{
  return R_FAIL;
}

unsigned char lifi_send(unsigned char* dest_addr, unsigned char* data, int data_length)
{
  printf("LIFI_SEND_전송_시도\r\n");
  return 0;
}