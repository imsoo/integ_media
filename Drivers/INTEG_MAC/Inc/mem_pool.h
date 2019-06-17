/* 
  mem_pool.h
*/

#define MAX_MEM_POOL_SIZE 500
#define MEM_SIZE 96

void mem_pool_init(void);
unsigned char *get_mem(void);
unsigned char return_mem(unsigned char *mem);