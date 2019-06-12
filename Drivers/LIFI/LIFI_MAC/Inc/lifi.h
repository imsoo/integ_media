/* 
  lifi.h
*/
#define LIFI_BUF_LEN 100
#define LIFI_MAX_RECV 28

extern unsigned char lifiBuf[LIFI_BUF_LEN];
extern unsigned char lifi_send_index, lifi_recv_index, lifi_flag, lifi_rx_data;
static unsigned char lifi_sof_arr[3] = {0xFE, 0xFE, 0xFE};

unsigned char lifi_init(unsigned char deviceType);
unsigned char lifi_send(unsigned char* dest_addr, unsigned char* data, int data_length);
unsigned char* lifi_get_mac_addr(unsigned char addr_type);
