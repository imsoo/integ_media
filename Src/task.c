/*      

task.c

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "uart.h"
#include "task.h"
#include "integ_mac.h"
#include "frame_queue.h"
#include "hash.h"
#include "bluetooth.h"
#include "display.h"
#include "image.h"

struct task Task_q[MAX_TASK];
volatile int Task_f, Task_r;

void task_init()
{
  Task_f = Task_r = 0;
}

int task_insert(struct task *tskp)
{
  if ((Task_r + 1) % MAX_TASK == Task_f)
    return(0);
  Task_r = (Task_r + 1) % MAX_TASK;
  Task_q[Task_r] = *tskp;
  return(1);
}

int task_delete(struct task *tskp)
{
  if (Task_r == Task_f)
    return(0);
  Task_f = (Task_f + 1) % MAX_TASK;
  *tskp = Task_q[Task_f];
  return(1);
}

void task_cmd(void *arg)
{
  char buf[64], *cp0, *cp1;
  int deviceType = 0, msg_len = 0;
  INTEG_FRAME frame;
  if (fgets(buf, 64, stdin) == NULL) {
    display();
    return;
  }
  cp0 = strtok(buf, " \t\n\r");
  cp1 = strtok(NULL, "\t\n\r");
  
  // 아무것도 입력안한 경우
  if (cp0 == NULL) {
    //printf("$ ");
    insert_display_message(SYSTEM_MSG, "!!!-Usage : \r\n");
    display();
    return;
  }
  else if(!strcmp(cp0, "t")) {  // test
    frame.frame_length[LENGTH_LSB] = LSB(INTEG_FRAME_HEADER_LEN + 5);
    frame.frame_length[LENGTH_MSB] = 0;
    memcpy(frame.src_address, my_integ_address, INTEG_ADDR_LEN); 
    memcpy(frame.dest_address, hood_integ_address, INTEG_ADDR_LEN); 
    integ_find_opt_link(NULL);
    //frame.media_type = cur_media;
    frame.media_type = opt_media;
    frame.message_type = DATA_MSG;

    frame.ackNumber = 0;
    frame.fragment_number = 0;
    frame.fragment_offset = 0;
    frame.seqNumber = get_seq_number();
    frame.data = testBuf;
    frame_queue_insert((unsigned char *)&frame);
    //macDataReq(broadcast_addr, &frame, frame.frame_length);
  }
  else if(!strcmp(cp0, "info")) {
    PrintAllHashData();
    HAL_Delay(3000);
  }
  else if(!strcmp(cp0, "s")) {  // s
    frame.frame_length[LENGTH_LSB] = LSB(INTEG_FRAME_HEADER_LEN + IMAGE_LENGTH);
    frame.frame_length[LENGTH_MSB] = MSB(INTEG_FRAME_HEADER_LEN + IMAGE_LENGTH);
    memcpy(frame.src_address, my_integ_address, INTEG_ADDR_LEN); 
    memcpy(frame.dest_address, hood_integ_address, INTEG_ADDR_LEN); 
    integ_find_opt_link(NULL);
    //frame.media_type = cur_media;
    frame.media_type = opt_media;
    frame.message_type = DATA_MSG;

    frame.ackNumber = 0;
    frame.fragment_number = 0;
    frame.fragment_offset = 0;
    frame.seqNumber = get_seq_number();
    frame.data = sample_image;
    frame_queue_insert((unsigned char *)&frame);
    //macDataReq(broadcast_addr, &frame, frame.frame_length);
  }
  else if (!strcmp(cp0, "send")) {
    msg_len = strlen(cp1);
    //frame.frame_length = INTEG_FRAME_HEADER_LEN + msg_len;
    memcpy(frame.src_address, my_integ_address, INTEG_ADDR_LEN); 
    memcpy(frame.dest_address, hood_integ_address, INTEG_ADDR_LEN); 
    integ_find_opt_link(NULL);
    //frame.media_type = cur_media;
    frame.media_type = opt_media;
    frame.message_type = 0x45;
    memcpy(frame.data, cp1, msg_len);
    frame.ackNumber = 0;
    frame.seqNumber = get_seq_number();
    frame_queue_insert((unsigned char *)&frame);
  }
  else if (!strcmp(cp0, "init")) {
    // 추가 인자 입력하지 않은 경우
    if (cp1 == NULL) {
      printf("!!!- Usage : init deviceType([0]MASTER, [1]SLAVE\n");
      printf("$ ");
      return;
    }
    deviceType = atoi(cp1);
    if (deviceType < 0 || deviceType > 1) {
      printf("!!!- Wrong deviceType : [0]MASTER, [1]SLAVE\n");
      printf("$ ");
      return;
    }
  }
  // 잘못된 명령어 입력한 경우 
  else {
    insert_display_message(SYSTEM_MSG, "!!!-Wrong command\r\n");
    //printf("* [SYSTEM] !!!-Wrong command\r\n");
  }
  display();
}