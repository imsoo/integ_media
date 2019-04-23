/*

      integ_mac.h


*/
#define MEDIA_NUM 3             // 사용 매체 수
#define LIFI 0                          // Media Type
#define BLUETOOTH 1
#define CC2530 2
#define OPT_MEDIA 0xFF        // 현재 최적의 매체를 사용

static char *media_name[MEDIA_NUM] = {"LI-FI", "BLUETOOTH", "CC2530"};
extern unsigned char cur_media;  // 현재 사용할 링크
extern unsigned char opt_media;  // 현재 최적의 링크

#define RETRANSMIT_TIME 3       // 재전송 주기 300ms
#define R_SUCCESS 1
#define R_FAIL 0
static char *result_string[2] = {"FAIL", "SUCCESS"};

#define INTEG_ADDR_LEN 6     // 통합 맥 주소 길이
#define LIFI_ADDR_LEN 6     // lifi 맥 주소 길이
#define BLUETOOTH_ADDR_LEN 6     // BLUETOOTH 맥 주소 길이
#define CC2530_ADDR_LEN 8     // CC2530 맥 주소 길이
static unsigned char media_addr_len[MEDIA_NUM] = {LIFI_ADDR_LEN, BLUETOOTH_ADDR_LEN, CC2530_ADDR_LEN};

#define INTEG_FRAME_HEADER_LEN 17 // 통합 맥 프레임 헤더 길이
#define INTEG_FRAME_DATA_LEN 40  

#define INTEG_FRAME_LEN_FIELD 0

// messageType
#define DATA_MSG 0x01
#define ACK_MSG 0x02
#define PASS_MSG 0xFF


extern unsigned char my_integ_address[INTEG_ADDR_LEN];
#define HOOD_HASH_ID 0x10
extern unsigned char hood_integ_address[INTEG_ADDR_LEN];

#define MAX_SEQ_NUMBER 10                   // 순서 번호 최대
#define DEFAULT_SEQ_NUMBER 1            // 순서 번호 초기값
extern unsigned char seqNumber;         // 순서 번호

#define STATIC_ADDR 0
#define DYNAMIC_ADDR 1
static char *addr_type_name[2] = {"STATIC", "DYNAMIC"};
typedef struct integ_table {
  unsigned char integ_addr[INTEG_ADDR_LEN];
  unsigned char addr_type; // static : 0, dynamic : 1
  unsigned char *media_addr[MEDIA_NUM];
} INTEG_TABLE;

typedef struct integ_frame {
  unsigned char frame_length;
  unsigned char message_type;
  unsigned char src_address[INTEG_ADDR_LEN];
  unsigned char dest_address[INTEG_ADDR_LEN];
  unsigned char media_type;
  unsigned char seqNumber;
  unsigned char ackNumber;
  unsigned char data[INTEG_FRAME_DATA_LEN];
} INTEG_FRAME;


unsigned char get_seq_number(void);
void integ_mac_handler(void * arg);
void integ_retransmit_handler(void * arg);
void integ_mac_init(void);
void integ_print_frame(INTEG_FRAME *frame);

/** Get the Least Significant Byte (LSB) of an unsigned int*/
#define LSB(num) ((num) & 0xFF)

/** Get the Most Significant Byte (MSB) of an unsigned int*/
#define MSB(num) ((num) >> 8)
