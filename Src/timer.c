/*      

timer.c

*/
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "timer.h"

TIM_HandleTypeDef htim6 = NULL;

struct timer *Thead = NULL;

void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  //htim6.Init.Prescaler = 16000 - 1;
  htim6.Init.Prescaler = 45000 - 1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 200 - 1;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    //Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    //Error_Handler();
  }

  HAL_TIM_Base_Start_IT(&htim6);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    if (!Thead)
      return;
    if (--Thead->time == 0)
      timer_expire();
    HAL_TIM_Base_Start_IT(&htim6);
  }
}

struct timer *get_timer()
{
  struct timer *tp;
  
  tp = (struct timer *)malloc(sizeof(*tp));
  return(tp);
}

void timer_expire(void)
{
  struct timer *tp;
  
  for ( ; Thead != NULL && !Thead->time; ) {
    tp = Thead, Thead = tp->link;
    
    task_insert(&tp->task);
    
    free(tp);
  }
}

void insert_timer(struct task *tskp, int ms)
{
  int total;
  struct timer *tp, *cp, *pp;
  
  tp = get_timer();
  tp->task = *tskp;
  tp->time = ms;
  
  if (Thead == NULL) {
    Thead = tp;
    tp->link = NULL;
    return;
  }
  
  pp = NULL;
  total = 0;
  
  for (cp = Thead; cp; cp = cp->link) {
    total += cp->time;
    if (total >= ms)
      break;
    pp = cp;
  }
  
  if (pp == NULL) {
    cp->time -= tp->time;
    tp->link = cp;
    Thead = tp;
  }
  else if (cp == NULL) {
    tp->time -= total;
    pp->link = tp;
    tp->link = NULL;
  }
  else {
    total -= cp->time;
    tp->time -= total;
    cp->time -= tp->time;
    pp->link = tp;
    tp->link = cp;
  }
}

void tour_timer()
{
  struct timer *cp;
  int total = 0;
  
  printf("\n");
  for (cp = Thead; cp != NULL; cp = cp->link) {
    total += cp->time;
    printf("-->%d(%d) ", cp->time, total);
  }
  
  printf("\n");
}

void free_timer()
{
  struct timer *cp;
  
  for ( ; Thead != NULL; ) {
    cp = Thead;
    Thead = cp->link;
    free(cp);
  }
}