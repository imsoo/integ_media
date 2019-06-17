/* 
  lifi.h
*/
#define LIFI_BUF_LEN 26
#define LIFI_MAX_RECV 26

extern unsigned char asc[128];


extern unsigned char lifiBuf_recv_count;
extern unsigned char lifiBuf_index;
extern unsigned char lifiBuf1[LIFI_BUF_LEN];
extern unsigned char lifiBuf2[LIFI_BUF_LEN];
extern unsigned char lifiBuf3[LIFI_BUF_LEN];
static unsigned char *lifiBuf_p[3] = {lifiBuf1, lifiBuf2, lifiBuf3};

extern unsigned char lifi_send_index, lifi_recv_index, lifi_flag, lifi_rx_data;

extern unsigned char lifi_state;
extern unsigned char lifi_index;
extern unsigned char lifi_recv_count;
extern unsigned char lifi_sof_count;

static unsigned char lifi_sof_arr[3] = {0xAE, 0xBE, 0xCE};
static unsigned char lifi_eof_arr[3] = {0xFF, 0xFF, 0xFF};

unsigned char lifi_init(unsigned char deviceType);
unsigned char lifi_send(unsigned char* dest_addr, unsigned char* data, int data_length);
unsigned char* lifi_get_mac_addr(unsigned char addr_type);
unsigned char lifi_calcFCS(unsigned char *pMsg, unsigned char len);
