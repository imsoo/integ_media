/*
stub.h
*/

unsigned char stub_init(unsigned char deviceType);
unsigned char stub_send(unsigned char* dest_addr, unsigned char* data, int data_length);
unsigned char* stub_get_mac_addr(unsigned char addr_type);
