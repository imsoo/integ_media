#define U_SRSP_PAYLOAD_START      4  //may be different for UART
#define U_SRSP_LENGTH_FIELD       1  //may be different for UART
#define U_AREQ_STATUS_FIELD     4

// state
#define STATE_IDLE 0
#define STATE_AREQ 0
#define STATE_SREQ 1
#define STATE_SCAN_CNF 2
#define STATE_START_CNF 3
#define STATE_ASSOCIATE_CNF 4
#define STATE_SYNC_CNF 5
#define STATE_DATA_CNF 6
#define STATE_ASSOCIATE_RSP_CNF 7

//
#define STATE_INIT 1
#define STATE_SCAN 2
#define STATE_SYNC 3
#define STATE_ASSOCIATE 4
#define STATE_INIT_FINISH 10

extern volatile int sreqFlag;
extern volatile unsigned char stateStatus;
extern volatile int stateFlag;
extern unsigned char macBuf[200];

signed int uartSreq();
void uartWrite();
unsigned char calcFCS(unsigned char *pMsg, unsigned char len);