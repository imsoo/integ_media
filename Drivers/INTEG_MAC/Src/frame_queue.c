/*      

    frame_queue.c

*/
#include <stdio.h>
#include <string.h> // memmove
#include "integ_mac.h"
#include "frame_queue.h"

#define FRAME_QUEUE_SIZE 10
#define RE_FRAME_QUEUE_SIZE 10
INTEG_FRAME frame_queue[FRAME_QUEUE_SIZE];
INTEG_FRAME re_frame_queue[RE_FRAME_QUEUE_SIZE];
unsigned int front, rear;
unsigned int re_front, re_rear;

void frame_queue_init(void)
{
   front = rear = 0;
}

unsigned int frame_queue_insert(unsigned char *ch)
{
   if ((rear + 1) % FRAME_QUEUE_SIZE == front)	// full
      return(0);
   rear = (rear + 1) % FRAME_QUEUE_SIZE;
   memmove(&frame_queue[rear], ch, ch[INTEG_FRAME_LEN_FIELD]);
   return(1);
}

INTEG_FRAME* frame_queue_delete(void)
{
   if (rear == front)	// empty
      return NULL;
   front = (front + 1) % FRAME_QUEUE_SIZE;
   return(&frame_queue[front]);
}

void re_frame_queue_init(void)
{
   re_front = re_rear = 0;
}

void re_frame_queue_remove(unsigned char index)
{
  int i;
  
  // юс╫ц
  for(i = 0; i < RE_FRAME_QUEUE_SIZE; i++) {
    if(re_frame_queue[i].seqNumber == index) {
      re_frame_queue[i].message_type = PASS_MSG;
    }
  }
}

unsigned int re_frame_queue_insert(unsigned char *ch)
{
   if ((re_rear + 1) % RE_FRAME_QUEUE_SIZE == re_front)	// full
      return(0);
   re_rear = (re_rear + 1) % RE_FRAME_QUEUE_SIZE;
   memmove(&re_frame_queue[re_rear], ch, ch[INTEG_FRAME_LEN_FIELD]);
   return(1);
}

INTEG_FRAME* re_frame_queue_delete(void)
{
   if (re_rear == re_front)	// empty
      return NULL;
   re_front = (re_front + 1) % RE_FRAME_QUEUE_SIZE;
   return(&re_frame_queue[re_front]);
}

