/*
display.h
*/
#define MSG_HEADER_LENGTH 16
#define MSG_NUM 0x05
#define LIFI_MSG 0x00
#define BT_MSG 0x01
#define CC2530_MSG 0x02
#define INTEG_MSG 0x03
#define SYSTEM_MSG 0x04


#define ROW_LINES 12
#define FIRST_ROW 0
#define LAST_ROW ROW_LINES - 1

#define COL_NUMS 79
#define FIRST_COL 0
#define LAST_COL COL_NUMS - 1

#define RECT_CHAR_START 22
#define RECT_CHAR_END 54
#define RECT_CHAR_LEN 10
#define RECT_FIRST_CON_START RECT_CHAR_START - 6
#define RECT_SECOND_CON_START (RECT_CHAR_START + RECT_CHAR_LEN + 4)

static char *str_message[MSG_NUM] = {"  LI-FI  ", "BLUETOOTH", " CC2530  ", "INTEG-MAC", " SYSTEM  "};
void init_display_buffer();

void insert_display_message(int message_type, char *message);
void print_info(void *arg);
void print_message(void *arg);
void display();