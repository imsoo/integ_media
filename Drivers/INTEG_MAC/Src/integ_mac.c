#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "integ_mac.h"
#include "frame_queue.h"
#include "task.h"
#include "timer.h"
#include "hash.h"
#include "bluetooth.h"                // blluetooth
#include "lifi.h"                          // lifi
#include "mac_interface.h"      // CC2530
#include "uart.h"
#include "display.h"

#define STM32_UUID ((uint32_t *)0x1FFF7A10)

// dispaly 용 버퍼
unsigned char message_buffer[COL_NUMS];

// 각 매체 용 함수 포인터
unsigned char (*fun_init[MEDIA_NUM])(unsigned char) = {lifi_init, bluetooth_init, startMac};   // 초기화
unsigned char (*fun_send[MEDIA_NUM])(unsigned char* , unsigned char* , int ) = {lifi_send, bluetooth_send, macDataReq};    // 데이터 전송

// 이웃 주소
unsigned char hood_integ_address[INTEG_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char hood_lifi_address[LIFI_ADDR_LEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
unsigned char hood_bluetooth_address[BLUETOOTH_ADDR_LEN] =  {0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
unsigned char hood_cc2530_address[CC2530_ADDR_LEN] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};

// 내 주소
unsigned char my_integ_address[INTEG_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char my_lifi_address[LIFI_ADDR_LEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
unsigned char my_bluetooth_address[BLUETOOTH_ADDR_LEN] =  {0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
unsigned char my_cc2530_address[CC2530_ADDR_LEN] = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};

unsigned char seqNumber;        // 순서 번호
unsigned char cur_media;              // 현재 사용하는 매체
unsigned char prev_media;             // 이전에 사용한 매체
unsigned char opt_media;              // 최적의 매체
unsigned char deviceType;            // 장치 유형

unsigned char STATUS_TABLE[STATUS_NUM][MEDIA_NUM] = {{R_FAIL, R_FAIL, R_FAIL}, {DISCON, DISCON, DISCON}};

#define TRANSMIT_FRAME 1
#define RECEIVE_FRAME 0
void integ_mac_handler(void * arg)
{
  struct task task, retrans_task;
  INTEG_FRAME *frame = NULL;
  INTEG_FRAME t_frame;
  unsigned char message_type;
  unsigned char result;
  unsigned char frame_state; // 송신용 수신용
  
  while((frame = frame_queue_delete()) != NULL) {
    //integ_print_frame(frame);
    message_type = frame->message_type;
    
    // 근원지 주소 필드와 자신의 주소 비교
    result = memcmp(my_integ_address, frame->src_address, INTEG_ADDR_LEN);
    if(result == 0) {
      frame_state = TRANSMIT_FRAME;
    }
    else {
      frame_state = RECEIVE_FRAME;
    }
    
    switch(message_type) {
    case DATA_MSG:      
      // 데이터 송신 명령 
      if(frame_state == TRANSMIT_FRAME) {
        //sprintf(message_buffer, "* [INTEG] 데이터 송신 프레임 (목적지 : %02x)\r\n", frame->dest_address[0]);
        //insert_display_message(message_buffer);
        //printf("** Data 송신 seqNum %d\r\n", frame->seqNumber);
        cur_media = frame->media_type;
        
        // 재전송 프레임인 경우 최적의 매체로 전송
        if((cur_media & OPT_MEDIA) == OPT_MEDIA) {
          prev_media = cur_media & 0x0F;
          
          // 이전 전송 매체 연결 상태 변경
          STATUS_TABLE[CONNECT_STATUS][prev_media] = DISCON;
          
          integ_find_opt_link(NULL);
          cur_media = opt_media;
          frame->media_type = OPT_MEDIA | cur_media;
          
          // 재전송 대기열의 프레임 추가
          re_frame_queue_insert((unsigned char *)frame);
          sprintf(message_buffer, "* [%s] 데이터 전송 실패 \r\n",  media_name[prev_media]);
          insert_display_message(message_buffer);
          
          if(prev_media != cur_media) {
            sprintf(message_buffer, "* [%s] 에서 [%s] 으로 매체 변경 \r\n",  media_name[prev_media],  media_name[cur_media]);
            insert_display_message(message_buffer);
          }
          
          sprintf(message_buffer, "* [%s][SEQ : %d] 데이터 재전송 (목적지 : %02x) : %s\r\n", media_name[cur_media], frame->seqNumber, frame->dest_address[0], frame->data);
          insert_display_message(message_buffer);
        }
        // 처음 전송인 경우
        else {
          frame->media_type ^= OPT_MEDIA;  
          re_frame_queue_insert((unsigned char *)frame);
          frame->media_type = cur_media;
          
          sprintf(message_buffer, "* [%s][SEQ : %d] 데이터 전송 (목적지 : %02x) : %s\r\n", media_name[cur_media], frame->seqNumber, frame->dest_address[0], frame->data);
          insert_display_message(message_buffer);
        }
        
        // 재전송 Task 추가
        retrans_task.fun = integ_retransmit_handler;
        strcpy(retrans_task.arg, "");
        insert_timer(&retrans_task, RETRANSMIT_TIME);
        
        // INTEG ADDR -> MAC ADDR 변환
        struct node *table = FindHashData(HOOD_HASH_ID);
        
        if(table != NULL) {
          // 송신
          //printf("cur_media = %d\r\n", cur_media);
          fun_send[cur_media](table->data.media_addr[cur_media], (unsigned char *)frame, frame->frame_length);
        }
      }
      // 데이터 수신 시 ACK 생성
      else if(frame_state == RECEIVE_FRAME) {
        //printf("** Data 수신\r\n");
      
        // ACK 패킷 생성
        t_frame.frame_length = frame->frame_length;
        t_frame.message_type = ACK_MSG;
        t_frame.media_type = frame->media_type & 0x0F;
        t_frame.ackNumber = frame->seqNumber + 1;
        memcpy(t_frame.dest_address, frame->src_address, INTEG_ADDR_LEN);
        memcpy(t_frame.src_address, my_integ_address, INTEG_ADDR_LEN);
        
        sprintf(message_buffer, "* [%s][SEQ : %d] 데이터 수신 (근원지 : %02x) : %s\r\n",  media_name[t_frame.media_type],  frame->seqNumber, frame->src_address[0], frame->data);
        insert_display_message(message_buffer);
        
        // 수신 매체 연결 상태 변경
        STATUS_TABLE[CONNECT_STATUS][t_frame.media_type] = CON;
        
        frame_queue_insert((unsigned char *)&t_frame);
        //printf("** ACK 생성\r\n");
      }
      break;
    case ACK_MSG:
      // ACK 송신 명령
      if(frame_state == TRANSMIT_FRAME) {
        //printf("** ACK 송신\r\n");
        
        // ACK 송신은 받은 매체로
        cur_media = frame->media_type;
        
        // INTEG ADDR -> MAC ADDR
        struct node *table = FindHashData(HOOD_HASH_ID);
        if(table != NULL) {
          // 송신
          //printf("cur_media = %d\r\n", cur_media);
          fun_send[cur_media](table->data.media_addr[cur_media], (unsigned char *)frame, frame->frame_length);
        }
        
        sprintf(message_buffer, "* [%s][ACK : %d] ACK 송신 (목적지 : %02x) \r\n", media_name[cur_media],  frame->ackNumber, frame->dest_address[0]);
        insert_display_message(message_buffer);
      }
      // ACK 수신 시
      else if(frame_state == RECEIVE_FRAME) {
        int ackNumber;
        
        // 수신 매체 연결 상태 변경
        STATUS_TABLE[CONNECT_STATUS][frame->media_type] = CON;
        
        ackNumber = frame->ackNumber - 1;
        printf("** ACK 수신 ackNum : %d\r\n", ackNumber + 1);
        // 재전송 대기열의 프레임 제거
        re_frame_queue_remove(ackNumber % MAX_SEQ_NUMBER);
        
        sprintf(message_buffer, "* [%s][ACK : %d] ACK 수신 (근원지 : %02x) \r\n", media_name[frame->media_type],  frame->ackNumber, frame->src_address[0]);
        insert_display_message(message_buffer);
      }
      break;
    case PASS_MSG:
      //printf("재전송 취소\r\n");
      break;
    }
  }
  HAL_Delay(1);
  task.fun = integ_mac_handler;
  strcpy(task.arg, "");
  task_insert(&task);
}


/*
재전송 큐에 있는 프레임 하나를 프레임 큐에 넣음.
*/
void integ_retransmit_handler(void * arg)
{
  INTEG_FRAME* t_frame;
  t_frame = re_frame_queue_delete();
  
  if(t_frame != NULL) {         //  재전송 할 프레임 있는 경우 꺼내서 프레임 큐에 삽입
    frame_queue_insert((unsigned char *)t_frame);
  }
}

void integ_mac_init(void)
{
  int i, result;
  struct node *table;   // MAC Table 구성
  
  insert_display_message("* [INTEG] 통합 MAC 초기화 시작\r\n");
  
  seqNumber = DEFAULT_SEQ_NUMBER;
  frame_queue_init();
  re_frame_queue_init();
  
  my_integ_address[0] = LSB(STM32_UUID[0]);
  my_cc2530_address[0] = LSB(STM32_UUID[0]);
  my_lifi_address[0] = LSB(STM32_UUID[0]);
  my_bluetooth_address[0] = LSB(STM32_UUID[0]);
  if (LSB(STM32_UUID[0]) == 0x2c) {
    hood_integ_address[0] = 0x2E;
    hood_cc2530_address[0] = 0x2E;
    hood_lifi_address[0] = 0x2E;
    hood_bluetooth_address[0] = 0x2E;
    deviceType = MASTER;
  }
  else {
    hood_integ_address[0] = 0x2c;
    hood_cc2530_address[0] = 0x2c;
    hood_lifi_address[0] = 0x2c;
    hood_bluetooth_address[0] = 0x2c;
    deviceType = SLAVE;
  }
  
  table = get_hashNode();
  table->id = LSB(STM32_UUID[0]);
  table->data.addr_type = STATIC_ADDR;
  memcpy(table->data.integ_addr, my_integ_address, INTEG_ADDR_LEN);
  table->data.media_addr[LIFI] = my_lifi_address;
  table->data.media_addr[BLUETOOTH] = my_bluetooth_address;
  table->data.media_addr[CC2530] = my_cc2530_address;
  AddHashData(table->id, table);
  
  table = get_hashNode();
  table->id = HOOD_HASH_ID;
  table->data.addr_type = DYNAMIC_ADDR;
  memcpy(table->data.integ_addr, hood_integ_address, INTEG_ADDR_LEN);
  table->data.media_addr[LIFI] = hood_lifi_address;
  table->data.media_addr[BLUETOOTH] = hood_bluetooth_address;
  table->data.media_addr[CC2530] = hood_cc2530_address;
  AddHashData(table->id, table);
  
  // MCU <---> 매체 통신 초기화
  
  // 매체 초기화
  for(i = 0; i < MEDIA_NUM; i++) {
    result = fun_init[i](deviceType);
    STATUS_TABLE[INIT_STATUS][i] = result;
    STATUS_TABLE[CONNECT_STATUS][i] = result;
    sprintf(message_buffer, "* [%s] 초기화 %s \r\n", media_name[i], result_string[result]);
    insert_display_message(message_buffer); 
  }
  
  // 최적 매체 선택
  integ_find_opt_link(NULL);
  
  // 첫 통신 시작은 최적 매체 선택
  cur_media = opt_media;
  
  // 통합 MAC Handler TASK 삽입
  integ_mac_handler("");
  
  insert_display_message("* [INTEG] 통합 MAC 초기화 완료\r\n");
  
  /*
  struct task task;
  task.fun = integ_find_opt_link;
  strcpy(task.arg, "");
  insert_timer(&task, FIND_OPT_PERIOD);
  */
}

void integ_find_opt_link(void * arg)
{
  struct task task;
  int i;
  
  for(i = 0; i < MEDIA_NUM; i++) {
    if(STATUS_TABLE[INIT_STATUS][i] && STATUS_TABLE[CONNECT_STATUS][i]) {
      opt_media = i;
      return;
    }
  }
  //opt_media = (rand() % 2) + 1;
  opt_media = (opt_media + 1) % 2 + 1;

  /*
  // 주기적인 TASK 삽입
  task.fun = integ_find_opt_link;
  strcpy(task.arg, "");
  insert_timer(&task, FIND_OPT_PERIOD);
  */
}

// 프레임 출력
void integ_print_frame(INTEG_FRAME *frame)
{
  int i;
  
  printf("----------\r\n");
  printf("Source Address : ");
  for(i = 0; i < INTEG_ADDR_LEN; i++) {
    printf("%02x ", frame->src_address[i]);
  }
  printf(" | Dest Address : ");
  for(i = 0; i < INTEG_ADDR_LEN; i++) {
    printf("%02x ", frame->dest_address[i]);
  }
  printf("\r\nLength : %d | msgType : %d | mediaType : %d | seqNumber : %d | ackNumber : %d\r\n", frame->frame_length, frame->message_type, frame->media_type, frame->seqNumber, frame->ackNumber);
  printf("----------\r\n");
  
}

unsigned char get_seq_number(void)
{
  unsigned char return_value = seqNumber;
  seqNumber = (seqNumber + 1) % MAX_SEQ_NUMBER;
  return return_value;
}