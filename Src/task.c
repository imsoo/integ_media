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
  char buf[64], *cp0, *cp1, *cp2;
  int deviceType = 0;
  if (fgets(buf, 64, stdin) == NULL) {
    display();
    return;
  }
  cp0 = strtok(buf, " \t\n\r");
  cp1 = strtok(NULL, " \t\n\r");
  
  // 아무것도 입력안한 경우
  if (cp0 == NULL) {
    //printf("$ ");
    insert_display_message("* [SYSTEM] \r\n");
    display();
    return;
  }
  else if(!strcmp(cp0, "info")) {
    //PrintAllHashData();
    //print_uart_state();
    BT_CONNET();
  }
  else if(!strcmp(cp0, "s")) {
    INTEG_FRAME frame;
    frame.frame_length = INTEG_FRAME_HEADER_LEN + 0x06;
    memcpy(frame.src_address, my_integ_address, INTEG_ADDR_LEN); 
    memcpy(frame.dest_address, hood_integ_address, INTEG_ADDR_LEN); 
    frame.media_type = cur_media;
    frame.message_type = DATA_MSG;
    frame.data[0] = 0x48;
    frame.data[1] = 0x65;
    frame.data[2] = 0x6c;
    frame.data[3] = 0x6c;
    frame.data[4] = 0x6f;
    frame.seqNumber = get_seq_number();
    frame.data[5] = frame.seqNumber + 0x30;
    frame_queue_insert((unsigned char *)&frame);
    //macDataReq(broadcast_addr, &frame, frame.frame_length);
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
    insert_display_message("* [SYSTEM] !!!-Wrong command\r\n");
    //printf("* [SYSTEM] !!!-Wrong command\r\n");
  }
  display();
}