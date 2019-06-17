/*      

    frame_queue.h

*/

void frame_queue_init(void);
unsigned int frame_queue_insert(unsigned char *ch);
INTEG_FRAME* frame_queue_delete(void);
void frame_queue_remove(unsigned char seq_num, unsigned char frag_num);
void frame_queue_add_retrans_num(unsigned char seq_num);
void frame_queue_change_media(unsigned char media_type, unsigned char to_media_type);

void re_frame_queue_init(void);
void re_frame_queue_remove(unsigned char seq_num, unsigned char frag_num);
void re_frame_queue_change_media(unsigned char media_type, unsigned char to_media_type);
unsigned int re_frame_queue_insert(unsigned char *ch);
INTEG_FRAME* re_frame_queue_delete(void);