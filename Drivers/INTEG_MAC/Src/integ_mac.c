#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
#include "mem_pool.h"

#define STM32_UUID ((uint32_t *)0x1FFF7A10)

unsigned char testBuf_2[120] = {'1','1','1','1','1','1','1','1',
'2','2','2','2','2','2','2','2',
'3','3','3','3','3','3','3','3',
'4','4','4','4','4','4','4','4',
'5','5','5','5','5','5','5','5',
'6','6','6','6','6','6','6','6',
'7','7','7','7','7','7','7','7',
'8','8','8','8','8','8','8','8',
'9','9','9','9','9','9','9','9',
'1','1','1','1','1','1','1','1',
'2','2','2','2','2','2','2','2',
'3','3','3','3','3','3','3','3',
'4','4','4','4','4','4','4','4',
'5','5','5','5','5','5','5','5',
'6','6','6','6','6','6','6','6'
};

unsigned char testBuf_recv[120];

// dispaly 용 버퍼
char message_buffer[COL_NUMS];

// 각 매체 용 함수 포인터
unsigned char (*fun_init[MEDIA_NUM])(unsigned char) = {lifi_init, bluetooth_init, startMac};   // 각 매체 초기화 함수 포인터
unsigned char (*fun_send[MEDIA_NUM])(unsigned char* , unsigned char* , int ) = {lifi_send, bluetooth_send, macDataReq};    // 각 매체 데이터 전송 함수 포인터
unsigned char* (*fun_get_addr[MEDIA_NUM])(unsigned char) = {lifi_get_mac_addr, bt_get_mac_addr, cc2530_get_mac_addr};    // 각 매체 주소 얻기 함수 포인터


// 이웃 주소
unsigned char hood_integ_address[INTEG_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/*
unsigned char hood_lifi_address[LIFI_ADDR_LEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
unsigned char hood_bluetooth_address[BLUETOOTH_ADDR_LEN] =  {0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
unsigned char hood_cc2530_address[CC2530_ADDR_LEN] = {0x11, 0x67, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};
*/

// 내 주소
unsigned char integ_macAddr[INTEG_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char my_integ_address[INTEG_ADDR_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char my_lifi_address[LIFI_ADDR_LEN] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55};
unsigned char my_bluetooth_address[BLUETOOTH_ADDR_LEN] =  {0x33, 0x33, 0x33, 0x33, 0x33, 0x33};
unsigned char my_cc2530_address[CC2530_ADDR_LEN] = {0x11, 0x67, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11};

// 브로드캐스트 용 통합 MAC 주소
unsigned char integ_broadcast_addr[INTEG_ADDR_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

INTEG_FRAME advertising_frame;
INTEG_FRAME t_frame;          // 임시용 프레임 변수

unsigned char seqNumber;        // 순서 번호
unsigned char cur_media;              // 현재 사용하는 매체
unsigned char prev_media;             // 이전에 사용한 매체
unsigned char opt_media;              // 최적의 매체
unsigned char deviceType;            // 장치 유형

// 매체 상태 테이블
unsigned char STATUS_TABLE[STATUS_NUM][MEDIA_NUM] = {{R_FAIL, R_FAIL, R_FAIL}, {DISCON, DISCON, DISCON}};

// 통합 MAC 초기화 상태 (1 : 초기화 완료)
unsigned char integ_init_state = 0;
unsigned char fragment_id;

#define TRANSMIT_FRAME 1
#define RECEIVE_FRAME 0
void integ_mac_handler(void * arg)
{
  struct task task, retrans_task;
  struct node *table = NULL;   // MAC 테이블 참조용 변수
  INTEG_FRAME *frame = NULL;    // 통합 프레임 큐에서 인출한 프레임을 가리키는 포인터
  unsigned char message_type;   // 메시지 유형 
  unsigned char result;         // 함수 실행 결과 저장용 변수
  unsigned char frame_state; // 송신용 수신용 구분
  unsigned char mac_table_key;  // 맥테이블 접근 용 키
  int i;        // for 반복문 용
  
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
    
    // 맥테이블 접근 용 키 주소 첫바이트
    mac_table_key = frame->dest_address[0];
    
    // 맥테이블 가져오기
    table = FindHashData(mac_table_key);
    
    switch(message_type) {
    case DATA_MSG:      
      // 데이터 송신 명령 
      if(frame_state == TRANSMIT_FRAME) {
        cur_media = frame->media_type;
        
        // 재전송 프레임인 경우 최적의 매체로 전송
        if((cur_media & OPT_MEDIA) == OPT_MEDIA) {
          prev_media = cur_media & 0x0F;
          
          // ackNumber 필드 를 재전송한 횟수로 사용한다. (임시)
          frame->ackNumber++;
          
          // 재전송 대기열의 프레임 추가
          re_frame_queue_insert((unsigned char *)frame);
          sprintf(message_buffer, "%d 번 데이터 전송 실패 \r\n", frame->ackNumber);
          insert_display_message(prev_media, message_buffer);
          
          cur_media = prev_media;
          // 재전송 횟수가 3회이면 매체 연결 상태 끊김으로 변경
          if(frame->ackNumber >= RETRANSMIT_NUM) {
            // 이전 전송 매체 연결 상태 변경
            STATUS_TABLE[CONNECT_STATUS][prev_media] = DISCON;
            integ_find_opt_link(NULL);
            cur_media = opt_media;
          }
          frame->media_type = OPT_MEDIA | cur_media;
          
          // 단편화 패킷이면서 최적매체가 변경된 경우
          if((frame->fragment_offset & 0x80) == 0x80 && (prev_media != cur_media)) {
            
            // Don't fragment 
            if((frame->fragment_offset & 0x40) != 0x40) {

              if (media_mtu_size[prev_media] < media_mtu_size[cur_media]) { //  단편화 조각을 합침
                int max_octet_length = (media_mtu_size[cur_media]  - INTEG_FRAME_HEADER_LEN) / 8;
                frame->frame_length = INTEG_FRAME_HEADER_LEN + max_octet_length * 8;
                
                // 합쳐진 프레임들 제거
                int count = 0;
                while (count != ((frame->frame_length - INTEG_FRAME_HEADER_LEN) / (MIN_MTU_SIZE - INTEG_FRAME_HEADER_LEN))) {                
                  // 통합 프레임 큐에서 제거
                  frame_queue_remove((frame->seqNumber + count) % MAX_SEQ_NUMBER);
                  // 재전송 대기열의 프레임 제거
                  re_frame_queue_remove((frame->seqNumber + count) % MAX_SEQ_NUMBER);
                }
                frame->message_type = DATA_MSG;
              }
              
              else {      //단편화 조각을 분할
                int count = (media_mtu_size[prev_media] - INTEG_FRAME_HEADER_LEN) / (media_mtu_size[cur_media] - INTEG_FRAME_HEADER_LEN);
                int cur_octet_length = (media_mtu_size[cur_media]  - INTEG_FRAME_HEADER_LEN) / 8;
                for (i = 0; i < count; i++) {
                  // 통합 프레임 큐에서 제거
                  frame_queue_remove((frame->seqNumber + i) % MAX_SEQ_NUMBER);
                  // 재전송 대기열의 프레임 제거
                  re_frame_queue_remove((frame->seqNumber + i) % MAX_SEQ_NUMBER);
                  
                  memcpy(&t_frame, frame, INTEG_FRAME_HEADER_LEN);
                  t_frame.seqNumber = (frame->seqNumber + i) % MAX_SEQ_NUMBER;
                  t_frame.frame_length = INTEG_FRAME_HEADER_LEN + (cur_octet_length * 8);
                  t_frame.message_type = DATA_MSG;
                  t_frame.fragment_offset = t_frame.fragment_offset + (i * cur_octet_length);
                  t_frame.fragment_offset |= 0x40;        // DF 설정
                  t_frame.data = get_mem();
                  memcpy(t_frame.data, frame->data + (i * 8), cur_octet_length * 8);
                  frame_queue_insert((unsigned char *)&t_frame);
                  
                  
                }
              }
            }
          }
          
          sprintf(message_buffer, "[SEQ : %d] 데이터 재전송 (→ 목적지 : %02X)\r\n", frame->seqNumber, frame->dest_address[0]);
          // sprintf(message_buffer, "[SEQ : %d] 데이터 재전송 (→ 목적지 : %02X) : %s\r\n", frame->seqNumber, frame->dest_address[0], frame->data);
          insert_display_message(cur_media, message_buffer);
        }
        // 처음 전송인 경우
        else {
          // 전송하려는 프레임이 각 매체 최소 MTU 크기보다 큰 경우 
          if(frame->frame_length > MIN_MTU_SIZE) {
            
            // 단편화 준비
            int i;
            int total_data_len = frame->frame_length - INTEG_FRAME_HEADER_LEN;
            
            int fragment_count = ceil((total_data_len / (double) (MIN_MTU_SIZE - INTEG_FRAME_HEADER_LEN)));
            
            //printf("%d %d %d", fragment_count, frame->frame_length, MIN_MTU_SIZE);
            
            memcpy(&t_frame, frame, INTEG_FRAME_HEADER_LEN);
            t_frame.fragment_id = DEFAULT_FRAGMENT_ID;
            t_frame.fragment_offset = 0;       
            t_frame.media_type = opt_media;
            //t_frame.media_type = 0xF0 | 0x01;
            
            int min_octet_length = (MIN_MTU_SIZE - INTEG_FRAME_HEADER_LEN) / 8;
            int max_octet_length = (MAX_MTU_SIZE - INTEG_FRAME_HEADER_LEN) / 8;
            for (i = 0; i < fragment_count; i++) {
              t_frame.frame_length = INTEG_FRAME_HEADER_LEN + min_octet_length * 8;
              if (i >= 1) {
                t_frame.seqNumber = get_seq_number();
              }
              
              t_frame.fragment_offset = i * min_octet_length; 
              if (i != (fragment_count - 1)) {             // 마지막 패킷 아니면 MF 설정
                t_frame.fragment_offset |= 0x80;
              }
              else {
                t_frame.fragment_offset |= 0x40;        // 마지막 패킷 DF 설정
              } 
              
              t_frame.data = get_mem();
              memcpy(t_frame.data, frame->data + ((t_frame.fragment_offset & 0x3F) * 8), min_octet_length * 8);
              frame_queue_insert((unsigned char *)&t_frame);
            }
            continue;
          }
          // 전송하려는 프레임이 각 매체 최소 MTU 크기보다 작은 경우 단편화 없이 바로 전송
          else {
            frame->media_type ^= OPT_MEDIA;  
            re_frame_queue_insert((unsigned char *)frame);
            frame->media_type = cur_media;
            
            sprintf(message_buffer, "[SEQ : %d] 데이터 전송 ( → 목적지 : %02X) \r\n", frame->seqNumber, frame->dest_address[0]);
            // sprintf(message_buffer, "[SEQ : %d] 데이터 전송 ( → 목적지 : %02X) : %s\r\n", frame->seqNumber, frame->dest_address[0], frame->data);
            insert_display_message(cur_media, message_buffer);
          }
        }
        
        // 재전송 Task 추가
        retrans_task.fun = integ_retransmit_handler;
        strcpy(retrans_task.arg, "");
        insert_timer(&retrans_task, RETRANSMIT_TIME);
        
        // INTEG ADDR -> MAC ADDR 변환
        
        if(table != NULL) {
          // 송신
          //printf("cur_media = %d\r\n", cur_media);
          fun_send[cur_media](table->data.media_addr[cur_media], (unsigned char *)frame, frame->frame_length);
        }
      }
      // 데이터 수신 시 ACK 생성
      else if(frame_state == RECEIVE_FRAME) {
        //printf("** Data 수신\r\n");
        
        // 단편화 프레임 수신 시
        // MF가 세팅되어 있거나, fragment_offset이 0이 아닌 경우
        unsigned char more_flag = frame->fragment_offset & 0x80;
        unsigned char offset = frame->fragment_offset & 0x3F;
        
        // 조립
        if ((more_flag == 0x80 )|| (offset != 0)){
          memcpy(testBuf_recv + (offset * 8), frame->data, frame->frame_length - INTEG_FRAME_HEADER_LEN);
        }

        
        // ACK 패킷 생성
        t_frame.frame_length = frame->frame_length;
        t_frame.message_type = ACK_MSG;
        t_frame.media_type = frame->media_type & 0x0F;
        t_frame.ackNumber = frame->seqNumber + 1;
        t_frame.data = NULL;
        memcpy(t_frame.dest_address, frame->src_address, INTEG_ADDR_LEN);
        memcpy(t_frame.src_address, my_integ_address, INTEG_ADDR_LEN);
        
        sprintf(message_buffer, "[SEQ : %d] 데이터 수신 ( ← 근원지 : %02X) \r\n",  frame->seqNumber, frame->src_address[0]);
        // sprintf(message_buffer, "[SEQ : %d] 데이터 수신 ( ← 근원지 : %02X) : %s\r\n",  frame->seqNumber, frame->src_address[0], frame->data);
        insert_display_message(t_frame.media_type, message_buffer);
        
        // 메모리 -> 풀 반환
        return_mem(frame->data);
        frame->data = NULL;
        
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
        if(table != NULL) {
          // 송신
          //printf("cur_media = %d\r\n", cur_media);
          fun_send[cur_media](table->data.media_addr[cur_media], (unsigned char *)frame, frame->frame_length);
        }
        
        sprintf(message_buffer, "[ACK : %d] ACK 송신 ( → 목적지 : %02X) \r\n",  frame->ackNumber, frame->dest_address[0]);
        insert_display_message(cur_media, message_buffer);
        
      }
      // ACK 수신 시
      else if(frame_state == RECEIVE_FRAME) {
        int ackNumber;
        
        // 수신 매체 연결 상태 변경
        STATUS_TABLE[CONNECT_STATUS][frame->media_type] = CON;
        
        ackNumber = frame->ackNumber - 1;
        // 재전송 대기열의 프레임 제거
        re_frame_queue_remove(ackNumber % MAX_SEQ_NUMBER);
        
        sprintf(message_buffer, "[ACK : %d] ACK 수신 ( ←근원지 : %02X) \r\n",  frame->ackNumber, frame->src_address[0]);
        insert_display_message(frame->media_type, message_buffer);
        
        // 메모리 -> 풀 반환
        return_mem(frame->data);
        frame->data = NULL;
      }
      break;
    case ADV_MSG:
      // ADV_MSG 송신 프레임
      if(frame_state == TRANSMIT_FRAME) {
        // INTEG ADDR -> MAC ADDR 변환
        if(table != NULL) {
          // 송신
          
          for(i = 0; i < MEDIA_NUM; i++) {
            frame->media_type = i;
            //printf("%s ADV_MSG 송신\r\n", media_name[frame->media_type]);
            fun_send[i](table->data.media_addr[i], (unsigned char *)frame, frame->frame_length);
            HAL_Delay(30);
          }
        }
        frame->data = NULL;
        //frame_queue_insert((unsigned char *)frame);
      }
      // ADV_MSG 수신 받은 경우 
      else if(frame_state == RECEIVE_FRAME) {
        //printf("%s ADV 수신\r\n", media_name[frame->media_type]);
        
        // ADV_MSG 송신자의 맥테이블 가져오기
        mac_table_key = frame->src_address[0];
        table = FindHashData(mac_table_key);
        
        // 새로운 이웃은 맥테이블에 추가
        if(table == NULL) {
          // 이웃 맥 테이블 구성
          table = get_hashNode();
          table->id = mac_table_key;
          table->data.addr_type = DYNAMIC_ADDR;
          memcpy(table->data.integ_addr, frame->src_address, INTEG_ADDR_LEN);
          memcpy(table->data.media_addr, frame->data, MEDIA_ADDR_LEN_MAX * MEDIA_NUM);
          AddHashData(table->id, table);
          
          
          sprintf(message_buffer, "새로운 이웃노드 (통합 MAC 주소 : %02X) 발견 MAC TABLE 추가\r\n", table->data.integ_addr[0]);
          insert_display_message(INTEG_MSG, message_buffer);
        }
        
        // ADV_MSG 송신자에게 MAC 테이블을 포함해서 ADV_IND 전송
        t_frame.data = get_mem();
        t_frame.frame_length = frame->frame_length;
        t_frame.message_type = ADV_IND;
        t_frame.media_type = frame->media_type;
        memcpy(t_frame.src_address, my_integ_address, INTEG_ADDR_LEN);
        memcpy(t_frame.dest_address, frame->src_address, INTEG_ADDR_LEN);
        memcpy(t_frame.data, advertising_frame.data, MEDIA_ADDR_LEN_MAX * MEDIA_NUM);
        frame_queue_insert((unsigned char *)&t_frame);
        
        // 메모리 -> 풀 반환
        return_mem(frame->data);
        frame->data = NULL;
      }
      break;
      
    case ADV_IND:
      // ADV_IND 송신 프레임
      if(frame_state == TRANSMIT_FRAME) {
        //printf("%s ADV_IND 송신\r\n", media_name[frame->media_type]);
        
        // INTEG ADDR -> MAC ADDR 변환
        if(table != NULL) {
          // 송신
          HAL_Delay(30);
          fun_send[frame->media_type](table->data.media_addr[frame->media_type], (unsigned char *)frame, frame->frame_length);
        }
        // 메모리 -> 풀 반환
        return_mem(frame->data);
        frame->data = NULL;
      }
      // ADV_IND 수신 받은 경우 
      else if(frame_state == RECEIVE_FRAME) {
        //printf("%s ADV_IND 수신\r\n", media_name[frame->media_type]);
        
        // ADV_IND 송신자의 맥테이블 가져오기
        mac_table_key = frame->src_address[0];
        table = FindHashData(mac_table_key);
        
        // 새로운 이웃은 맥테이블에 추가
        if(table == NULL) {
          // 이웃 맥 테이블 구성
          table = get_hashNode();
          table->id = mac_table_key;
          table->data.addr_type = DYNAMIC_ADDR;
          memcpy(table->data.integ_addr, frame->src_address, INTEG_ADDR_LEN);
          memcpy(table->data.media_addr, frame->data, MEDIA_ADDR_LEN_MAX * MEDIA_NUM);
          AddHashData(table->id, table);
          
          sprintf(message_buffer, "새로운 이웃노드 (통합 MAC 주소 : %02X) 발견 MAC TABLE 추가\r\n", table->data.integ_addr[0]);
          insert_display_message(INTEG_MSG, message_buffer);
        }
        
        // 메모리 -> 풀 반환
        return_mem(frame->data);
        frame->data = NULL;
      }
      break;
      
    case PASS_MSG:
      //printf("재전송 취소\r\n");
      
      // 메모리 -> 풀 반환
      return_mem(frame->data);
      frame->data = NULL;
      break;
    default:
      sprintf(message_buffer, "잘못된 통합 MAC 프레임 (Error Type : %02X)\r\n", frame->message_type);
      insert_display_message(INTEG_MSG, message_buffer);
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
  
  insert_display_message(INTEG_MSG, "통합 MAC 초기화 시작\r\n");
  
  fragment_id = DEFAULT_FRAGMENT_ID;
  seqNumber = DEFAULT_SEQ_NUMBER;        // 순서번호 초기화
  frame_queue_init();                                   // 통합 프레임 큐 초기화
  re_frame_queue_init();                                // 재전송 프레임 큐 초기화
  
  
  advertising_frame.data = get_mem();
  
  // 임시 
  if (LSB(STM32_UUID[0]) == 0x2c) {
    hood_integ_address[0] = 0x2E;
    /*
    hood_cc2530_address[0] = 0x2E;
    hood_lifi_address[0] = 0x2E;
    hood_bluetooth_address[0] = 0x2E;
    */
    deviceType = MASTER;
  }
  else {
    hood_integ_address[0] = 0x2c;
    /*
    hood_cc2530_address[0] = 0x2c;
    hood_lifi_address[0] = 0x2c;
    hood_bluetooth_address[0] = 0x2c;
    */
    deviceType = SLAVE;
  }
  
  // MCU <---> 매체 통신 초기화
  
  // 매체 초기화
  for(i = 0; i < MEDIA_NUM; i++) {
    result = fun_init[i](deviceType);
    STATUS_TABLE[INIT_STATUS][i] = result;
    STATUS_TABLE[CONNECT_STATUS][i] = result;
    sprintf(message_buffer, "초기화 %s \r\n", result_string[result]);
    insert_display_message(i, message_buffer); 
  }
  
  // 각 매체 주소 가져오기
  memcpy(my_integ_address, integ_get_mac_addr(MAC_ADDR), INTEG_ADDR_LEN);               // 통합
  memcpy(my_cc2530_address, cc2530_get_mac_addr(MAC_ADDR), CC2530_ADDR_LEN);        // CC2530
  memcpy(my_bluetooth_address, bt_get_mac_addr(MAC_ADDR), BLUETOOTH_ADDR_LEN);    // BT
  memcpy(my_lifi_address, lifi_get_mac_addr(MAC_ADDR), LIFI_ADDR_LEN);                           // LI-FI
  
  // advertising frame 생성
  advertising_frame.frame_length = INTEG_FRAME_HEADER_LEN + MEDIA_NUM * MEDIA_ADDR_LEN_MAX;
  advertising_frame.message_type = ADV_MSG;
  memcpy(advertising_frame.src_address, my_integ_address, INTEG_ADDR_LEN);
  memcpy(advertising_frame.dest_address, integ_broadcast_addr, INTEG_ADDR_LEN);
  for(i = 0; i < MEDIA_NUM; i++) {
    memcpy(advertising_frame.data + (i * MEDIA_ADDR_LEN_MAX), fun_get_addr[i](MAC_ADDR), MEDIA_ADDR_LEN_MAX);
  }
  
  // advertising 송신 frame 삽입
  frame_queue_insert((unsigned char *)&advertising_frame);
  
  // 자신의 맥 테이블 구성
  table = get_hashNode();
  table->id = LSB(STM32_UUID[0]);
  
  table->data.addr_type = STATIC_ADDR;
  memcpy(table->data.integ_addr, my_integ_address, INTEG_ADDR_LEN);
  memcpy(table->data.media_addr, advertising_frame.data, MEDIA_ADDR_LEN_MAX * MEDIA_NUM);
  AddHashData(table->id, table);
  
  // Boradcast MAC table 구성
  table = get_hashNode();
  table->id = 0xFF;
  
  table->data.addr_type = STATIC_ADDR;
  memcpy(table->data.integ_addr, integ_broadcast_addr, INTEG_ADDR_LEN);
  for(i = 0; i < MEDIA_NUM; i++) {
    memcpy(table->data.media_addr[i], fun_get_addr[i](BROADCAST_ADDR), MEDIA_ADDR_LEN_MAX);
  }
  AddHashData(table->id, table);
  
  
  /*
  // 이웃 맥 테이블 구성
  table = get_hashNode();
  table->id = HOOD_HASH_ID;
  table->data.addr_type = DYNAMIC_ADDR;
  memcpy(table->data.integ_addr, hood_integ_address, INTEG_ADDR_LEN);
  
  memcpy(table->data.media_addr[LIFI], hood_lifi_address, MEDIA_ADDR_LEN_MAX);
  memcpy(table->data.media_addr[BLUETOOTH], hood_bluetooth_address, MEDIA_ADDR_LEN_MAX);
  memcpy(table->data.media_addr[CC2530], hood_cc2530_address, MEDIA_ADDR_LEN_MAX);
  AddHashData(table->id, table);
  */
  
  // 최적 매체 선택
  integ_find_opt_link(NULL);
  
  // 초기화 완료 설정
  integ_init_state = 1;
  insert_display_message(INTEG_MSG, "통합 MAC 초기화 완료\r\n");
  
  // 첫 통신 시작은 최적 매체 선택
  cur_media = opt_media;
  
  // 통합 MAC Handler TASK 삽입
  integ_mac_handler("");
  
  
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
  int i,prev_media;
  
  
  prev_media = opt_media;
  
  // 주기적인 TASK 삽입
  task.fun = integ_find_opt_link;
  strcpy(task.arg, "");
  //insert_timer(&task, FIND_OPT_PERIOD);
  
  for(i = 0; i < MEDIA_NUM; i++) {
    if(STATUS_TABLE[INIT_STATUS][i] && STATUS_TABLE[CONNECT_STATUS][i]) {
      opt_media = i;
      break;
      //printf("최적매체 : %s\r\n", media_name[opt_media]);
    }
  }
  
  // 연결된 매체가 없는 경우
  if(i == MEDIA_NUM) {
    opt_media = rand() % MEDIA_NUM;
    sprintf(message_buffer, "연결 가능 매체 없음. [%s] 으로 매체 변경 \r\n", media_name[opt_media]);
    insert_display_message(INTEG_MSG, message_buffer);
  }
  // 연결된 매체가 있는 경우
  // 새로운 최적매체 인 경우 변경 알림
  else if (prev_media != opt_media) {
    sprintf(message_buffer, "[%s] 에서 [%s] 으로 최적 매체 변경 \r\n",  media_name[prev_media],  media_name[opt_media]);
    insert_display_message(INTEG_MSG, message_buffer);
    return;
  }
  // 현재 최적매체가 기존 매체와 같은 경우 PASS
  else {
    
  }
  
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

// 통합 MAC 주소 GET
unsigned char* integ_get_mac_addr(unsigned char addr_type) {
  if (addr_type == BROADCAST_ADDR) {
    return integ_broadcast_addr;
  }
  else {
    integ_macAddr[0] = LSB(STM32_UUID[0]);
    return integ_macAddr;
  }
}

unsigned char get_seq_number(void)
{
  unsigned char return_value = seqNumber;
  seqNumber = (seqNumber + 1) % MAX_SEQ_NUMBER;
  return return_value;
}