/*      

    task.h

*/

#define MAX_TASK 16

struct task {
   void (*fun)(void*);
   char arg[8];
};

struct timer {
   int time;
   struct task task;
   struct timer *link;
};

extern struct task Task_q[MAX_TASK];

void task_init();
int task_insert(struct task *tskpk);
int task_delete(struct task *tskp);
void task_cmd(void *arg);