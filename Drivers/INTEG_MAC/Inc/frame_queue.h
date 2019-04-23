/*      

    frame_queue.h

*/

void frame_queue_init(void);
unsigned int frame_queue_insert(unsigned char *ch);
INTEG_FRAME* frame_queue_delete(void);

void re_frame_queue_init(void);
void re_frame_queue_remove(unsigned char index);
unsigned int re_frame_queue_insert(unsigned char *ch);
INTEG_FRAME* re_frame_queue_delete(void);