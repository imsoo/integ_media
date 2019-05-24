/*

      integ_mac.h


*/

// 단편화 테스트 용 데이터
extern unsigned char testBuf_2[120];

#define MEDIA_NUM 3             // 사용 매체 수
#define LIFI 0                          // Media Type
#define BLUETOOTH 1
#define CC2530 2
#define OPT_MEDIA 0xF0        // 현재 최적의 매체를 사용

static char *media_name[MEDIA_NUM] = {"LI-FI", "BLUETOOTH", "CC2530"};
extern unsigned char cur_media;  // 현재 사용할 링크
extern unsigned char opt_media;  // 현재 최적의 링크
extern unsigned char integ_init_state; // 통합 MAC 초기화 상태

// 연결 성태 저장 테이블
#define STATUS_NUM 2    // 상태 개수
#define INIT_STATUS 0     
#define CONNECT_STATUS 1
#define CON 1
#define DISCON 0
static char *STATUS_NAME[STATUS_NUM] = {"Init", "Connect"};
extern unsigned char STATUS_TABLE[STATUS_NUM][MEDIA_NUM];


#define FIND_OPT_PERIOD 5 // 최적 노드 검색 주기 500ms
#define RETRANSMIT_TIME 3      // 재전송 주기 300ms
#define RETRANSMIT_NUM 3      // 3번 ACK 안오는 경우 재전송
#define R_SUCCESS 1     // 실행 결과 : 성공
#define R_FAIL 0                // 실행 결과 : 실패
static char *result_string[2] = {"FAIL", "SUCCESS"};

#define INTEG_ADDR_LEN 6     // 통합 맥 주소 길이
#define LIFI_ADDR_LEN 6     // lifi 맥 주소 길이
#define BLUETOOTH_ADDR_LEN 6     // BLUETOOTH 맥 주소 길이
#define CC2530_ADDR_LEN 8     // CC2530 맥 주소 길이
#define MEDIA_ADDR_LEN_MAX 8    // 각 매체 주소중 길이가장 긴 값
static unsigned char media_addr_len[MEDIA_NUM] = {LIFI_ADDR_LEN, BLUETOOTH_ADDR_LEN, CC2530_ADDR_LEN};  /// 각 매체 주소 길이 인덱스로 접근 시

#define INTEG_FRAME_HEADER_LEN 19 // 통합 맥 프레임 헤더 길이
#define INTEG_FRAME_DATA_LEN 39     // 통합 맥 프레임 데이터 길이
#define INTEG_FRAME_TOTAL_LEN 58 + 1   // 통합 맥 프레임 헤더 + 데이터 길이

#define INTEG_FRAME_LEN_FIELD 0     // 프레임 길이 필드는 통합 프레임의 0번

// messageType
#define DATA_MSG 0x01   // 일반 데이터를 담은 메시지
#define ACK_MSG 0x02    // 응답 메시지
#define ADV_MSG 0x04    // 노드 자신의 존재를 브로드캐스팅 할때
#define ADV_IND 0x05      // ADV_MSG에 대한 응답 
#define PASS_MSG 0xFF   // 아무것도 없는 메시지 

// deviceType
#define MASTER 0x00
#define SLAVE 0x01

// fragmentaion 관련
static int media_mtu_size[MEDIA_NUM] = {20, 43, 91};
extern unsigned char fragment_id;
#define DEFAULT_FRAGMENT_ID 0            // 단편화 식별자 초기값
#define MIN_MTU_SIZE 48 // 매체들 MTU 크기 중 가장 작은 값



// SEQ
#define MAX_SEQ_NUMBER 10                   // 순서 번호 최대
#define DEFAULT_SEQ_NUMBER 0            // 순서 번호 초기값
extern unsigned char seqNumber;         // 순서 번호


#define STATIC_ADDR 0   // 수집하지 않은 주소 (자신의 주소, 브로드캐스트)
#define DYNAMIC_ADDR 1  // 외부로 부터 수집한 줏조
static char *addr_type_name[2] = {"STATIC", "DYNAMIC"};
extern unsigned char my_integ_address[INTEG_ADDR_LEN];
extern unsigned char hood_integ_address[INTEG_ADDR_LEN];

// 통합 MAC 테이블 구조체
typedef struct integ_table {
  unsigned char integ_addr[INTEG_ADDR_LEN];     // 통합 MAC 주소
  unsigned char addr_type; // static : 0, dynamic : 1
  unsigned char media_addr[MEDIA_NUM][MEDIA_ADDR_LEN_MAX];      // 각 매체 주소 
} INTEG_TABLE;


// 통합 MAC 프레임 구조체
typedef struct integ_frame {
  unsigned char frame_length;   // 프레임 길이
  unsigned char message_type;   // 메시지 유형
  unsigned char src_address[INTEG_ADDR_LEN];    // 근원지 주소
  unsigned char dest_address[INTEG_ADDR_LEN];   // 목적지 주소
  unsigned char media_type;     // 매체 유형
  unsigned char seqNumber;      // 순서 번호
  unsigned char ackNumber;      // 응답 번호
  unsigned char fragment_id;            // 단편화 식별자
  unsigned char fragment_offset;        // 프레그 먼트 오프셋
  unsigned char *data;// 페이로드
} INTEG_FRAME;


extern INTEG_FRAME advertising_frame;

unsigned char get_seq_number(void);
void integ_mac_handler(void * arg);
void integ_retransmit_handler(void * arg);
void integ_find_opt_link(void *);
void integ_mac_init(void);
void integ_print_frame(INTEG_FRAME *frame);

#define MAC_ADDR 0x00
#define BROADCAST_ADDR 0xFF
unsigned char* integ_get_mac_addr(unsigned char addr_type);

/** Get the Least Significant Byte (LSB) of an unsigned int*/
#define LSB(num) ((num) & 0xFF)

/** Get the Most Significant Byte (MSB) of an unsigned int*/
#define MSB(num) ((num) >> 8)
