#define CC_RESET_Pin GPIO_PIN_9
#define CC_RESET_GPIO_Port GPIOC

unsigned char startMac(unsigned char deviceType);
unsigned char* macReset();
void macSoftReset();
void macGetReq(unsigned char attr);
void macSyncReq();
void macAssociateReq();
void macAssociateRsp(void *arg);
void macStartReq();
unsigned char macDataReq(unsigned char* dest_addr, unsigned char* data, int data_length);
void macSetReq(unsigned char attr, unsigned char *attrValue);
void macScanReq(unsigned char scanType);
void printMacBuf();
void utilCallbackSubCmd();
void mtUtilGetPrimaryIEEE();
unsigned char* cc2530_get_mac_addr(unsigned char addr_type);