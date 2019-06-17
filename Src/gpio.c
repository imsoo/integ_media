/*      

gpio.c

*/
#include <string.h>
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "utilities.h"
#include "task.h"
#include "integ_mac.h"
#include "bluetooth.h"

void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  
  // CC2530 RESET_PIN
  /*Configure GPIO pin : PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  //HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
  
  /*Configure GPIO pin : PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  
  /* EXTI interrupt init */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 3, 1);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

// BT STATE : PC8 Falling interrupt Callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch(GPIO_Pin)
  {
  case GPIO_PIN_8:
    // falling BT ¿¬°á ²÷±è
    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8) == GPIO_PIN_RESET) {
      if(integ_init_state && (bt_enable_flag == 1)) {
        bt_dis_count++;
        //printf("BT disconnected\r\n");
        // BT Àç¿¬°á
        if(bt_dis_count > 4) {
          struct task task;
          task.fun = task_connect;
          strcpy(task.arg, "");
          task_insert(&task);
          bt_dis_count = 0;
        }
      }
    }
    else {
    }
    break;
  }
}