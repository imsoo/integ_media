/* 
  bluetooth.h
*/

unsigned char bluetooth_init(unsigned char deviceType);
unsigned char bluetooth_send(unsigned char* dest_addr, unsigned char* data, int data_length);