/*      

    uart.h

*/
extern UART_HandleTypeDef huart2;      // <---> BT
extern UART_HandleTypeDef huart3;      // <---> PC
extern UART_HandleTypeDef huart4;      // <---> CC2530
extern UART_HandleTypeDef huart5;      // <---> LIFI


int fputc(int ch, FILE *f);
int fgetc(FILE *f);
char *fgets(char *s, int n, FILE *f);

void UART2_Init(void);
void UART3_Init(void);
void UART4_Init(void);
void UART5_Init(void);
void uart_echo(uint8_t ch);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);