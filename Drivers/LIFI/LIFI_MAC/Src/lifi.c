/* 
  lifi.c
*/
#include <stdio.h>
#include "integ_mac.h"

unsigned char lifi_mac_addr[8];
unsigned char lifi_broadcast_addr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char lifi_init(unsigned char deviceType)
{
  return R_FAIL;
}

unsigned char lifi_send(unsigned char* dest_addr, unsigned char* data, int data_length)
{
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