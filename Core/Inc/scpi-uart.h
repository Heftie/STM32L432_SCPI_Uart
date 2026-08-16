#ifndef SCPI_UART_H
#define SCPI_UART_H

#include "stm32l4xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Starts the SCPI parser and the UART reception (interrupt driven). */
void SCPI_UART_Init(UART_HandleTypeDef *huart);

/* Feeds bytes received on UART into the SCPI parser. Call from the main loop. */
void SCPI_UART_Process(void);

#ifdef __cplusplus
}
#endif

#endif /* SCPI_UART_H */
