/*
stub.c
*/

#include "stub.h"
#include "integ_mac.h"

unsigned char stub_mac_addr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char stub_broadcast_addr[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

unsigned char stub_init(unsigned char deviceType)
{
  return R_FAIL;
}

unsigned char stub_send(unsigned char* dest_addr, unsigned char* data, int data_length)
{

  return 0;
}

unsigned char* stub_get_mac_addr(unsigned char addr_type) {
  if (addr_type == BROADCAST_ADDR) {
    return stub_broadcast_addr;
  }
  else {
    return stub_mac_addr;
  }
}