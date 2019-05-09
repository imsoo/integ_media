/*
display.c
*/
#include <stdio.h>
#include <string.h>
#include "display.h"
#include "integ_mac.h"
#include "task.h"

unsigned char display_buffer_index = 0;
unsigned char new_line[2] = {'\r', '\n'};
unsigned char display_buffer[ROW_LINES][COL_NUMS];
unsigned char *media_name_for_display[MEDIA_NUM] = {"  LI-FI   ", "BLUETOOTH ", " CC2530    "};
unsigned char rectange1[COL_NUMS] = {"┏━━━━━┓      ┏━━━━━┓      ┏━━━━━┓\r\n"};
unsigned char rectange1_1[COL_NUMS] = {"┃          ┃      ┏━━━━━┓      ┏━━━━━┓\r\n"};
unsigned char rectange2[COL_NUMS] = {"┃   MCU    ┃━X ━┃ ABCDEFG  ┃━X ━┃ NEIGHBOR ┃\r\n"};
unsigned char rectange3_1[COL_NUMS] = {"┗━━━━━┛      ┗━━━━━┛      ┗━━━━━┛\r\n"};
unsigned char rectange3[COL_NUMS] = {"┃          ┃      ┗━━━━━┛      ┗━━━━━┛\r\n"};

void init_display_buffer()
{
  int row;
  
  for(row = 0; row < ROW_LINES; row++) {
    memcpy(display_buffer[row], new_line, 2);
  }
}


void display() {
  print_info(NULL);
  print_message(NULL);
  printf("$ ");
}

void print_info(void *arg)
{
  int i;
  printf("\033[2J");
  //printf("\033[%d;%dH\r\n", 0, 0);
  printf("                 ** 매체 연결 정보 **                    ** 현재 최적 매체 **\r\n");
  for(i = 0; i < MEDIA_NUM; i++) {
    if(i == 0) {
    printf("%s", rectange1);
    }
    else {
      printf("%s", rectange1_1);
    }
    if (i == 1) {
      memcpy(rectange2 + 5, "MCU", 3);
    }
    else {
      memcpy(rectange2 + 5, "    ", 4);
    }
    memcpy(rectange2 + RECT_CHAR_START, media_name_for_display[i], RECT_CHAR_LEN);
    if(STATUS_TABLE[INIT_STATUS][i]) {
      rectange2[RECT_FIRST_CON_START] = 0xA6;
      rectange2[RECT_FIRST_CON_START+1] = 0xAC;
    }
    else {
      rectange2[RECT_FIRST_CON_START] = 'X';
      rectange2[RECT_FIRST_CON_START+1] = ' ';
    }
    
    if(STATUS_TABLE[CONNECT_STATUS][i]) {
      rectange2[RECT_SECOND_CON_START] = 0xA6;
      rectange2[RECT_SECOND_CON_START+1] = 0xAC;
    }
    else {
      rectange2[RECT_SECOND_CON_START] = 'X';
      rectange2[RECT_SECOND_CON_START+1] = ' ';
    }

    printf("%s", rectange2);
    
    if(i == 2) {
    printf("%s", rectange3_1);
    }
    else {
      printf("%s", rectange3);
    }
  }
}

void insert_display_message(unsigned char *message)
{
  strcpy(display_buffer[display_buffer_index], message);
  display_buffer_index = (display_buffer_index + 1) % ROW_LINES;
  display();
}

void print_message(void *arg)
{
  int row, col;
  printf("                 ** 메시지  정보  **\r\n"); 

  /*
  for(row = ROW_LINES - 1; row > display_buffer_index; row--) {  
      printf("%s", display_buffer[row]);
  }
  for(row = 0; row < display_buffer_index; row++) {
    printf("%s", display_buffer[row]);
  }
  */
  
  for(row = display_buffer_index; row < ROW_LINES; row++) {  
      printf("%s", display_buffer[row]);
  }
  
  for(row = 0; row < display_buffer_index; row++) {
    printf("%s", display_buffer[row]);
  }
  
}
