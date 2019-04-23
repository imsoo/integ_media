#include "stm32f4xx_hal.h"
#include "uart.h"
#include "mac_interface_uart.h"

volatile int sreqFlag = 0;
volatile unsigned char stateStatus = 0xFF;
volatile int stateFlag = 1;
unsigned char macBuf[200];

unsigned char sof = 0xFE;

signed int uartSreq(int state)
{
  sreqFlag = state;
  uartWrite();
  while(sreqFlag != STATE_IDLE);
  return 0;
}

void uartWrite()
{
  unsigned char frameBytes = 0x03 + macBuf[0];
  unsigned char fcs = calcFCS(macBuf, frameBytes);
  // SOF
  HAL_UART_Transmit(&huart4, &sof, 1, 1000);
  
  // Frame
  HAL_UART_Transmit(&huart4, macBuf, frameBytes, 1000);
  
  // FCS
  HAL_UART_Transmit(&huart4, &fcs, 1, 1000);
}

unsigned char calcFCS(unsigned char *pMsg, unsigned char len)
{
  unsigned char result = 0;
  while (len--)
  {
    result ^= *pMsg++;
  }
  return result;
}