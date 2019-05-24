/* 
  lifi.h
*/

extern unsigned char rx3_data[10];
static unsigned char tx5_data[22]="i can speak english \r\n";
static unsigned char testBuf[100] = {100, '1', '1', '1', '1', '1', '1', '1', '\r', '\n',
'2', '2', '2', '2', '2', '2', '2', '2', '\r', '\n',
'3', '3', '3', '3', '3', '3', '3', '3', '\r', '\n',
'4', '4', '4', '4', '4', '4', '4', '4', '\r', '\n',
'5', '5', '5', '5', '5', '5', '5', '5', '\r', '\n',
'6', '6', '6', '6', '6', '6', '6', '6', '\r', '\n',
'7', '7', '7', '7', '7', '7', '7', '7', '\r', '\n',
'8', '8', '8', '8', '8', '8', '8', '8', '\r', '\n',
'9', '9', '9', '9', '9', '9', '9', '9', '\r', '\n',
'0', '0', '0', '0', '0', '0', '0', '0', '\r', '\n'};
extern int index;
volatile extern int l_flag;

unsigned char lifi_init(unsigned char deviceType);
unsigned char lifi_send(unsigned char* dest_addr, unsigned char* data, int data_length);
unsigned char* lifi_get_mac_addr(unsigned char addr_type);