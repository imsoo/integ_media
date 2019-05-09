/*
display.h
*/
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
void init_display_buffer();

void insert_display_message(unsigned char *message);
void print_info(void *arg);
void print_message(void *arg);
void display();