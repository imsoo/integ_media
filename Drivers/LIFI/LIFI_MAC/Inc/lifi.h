/* 
  lifi.h
*/

unsigned char lifi_init(unsigned char deviceType);
unsigned char lifi_send(unsigned char* dest_addr, unsigned char* data, int data_length);