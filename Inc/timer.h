/*      

    timer.h

*/
#include "stm32f4xx_hal.h"


extern struct timer *Thead;
extern TIM_HandleTypeDef htim6;

void MX_TIM6_Init(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void timer_expire(void);
struct timer *get_timer();
void insert_timer(struct task *tskp, int ms);
void tour_timer();
void free_timer();