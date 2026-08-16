#include "scpi-uart.h"
#include "scpi-def.h"

#define SCPI_UART_RXBUF_SIZE 128U

static UART_HandleTypeDef *scpi_huart;
static uint8_t scpi_rx_byte;

static volatile uint8_t scpi_rx_buf[SCPI_UART_RXBUF_SIZE];
static volatile uint16_t scpi_rx_head;
static volatile uint16_t scpi_rx_tail;

void SCPI_UART_Init(UART_HandleTypeDef *huart) {
    scpi_huart = huart;

    SCPI_Init(&scpi_context,
              scpi_commands,
              &scpi_interface,
              scpi_units_def,
              SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4,
              scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
              scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    HAL_UART_Receive_IT(scpi_huart, &scpi_rx_byte, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart != scpi_huart) {
        return;
    }

    uint16_t next_head = (uint16_t)((scpi_rx_head + 1U) % SCPI_UART_RXBUF_SIZE);
    if (next_head != scpi_rx_tail) {
        scpi_rx_buf[scpi_rx_head] = scpi_rx_byte;
        scpi_rx_head = next_head;
    }

    HAL_UART_Receive_IT(scpi_huart, &scpi_rx_byte, 1);
}

void SCPI_UART_Process(void) {
    while (scpi_rx_tail != scpi_rx_head) {
        char c = (char)scpi_rx_buf[scpi_rx_tail];
        scpi_rx_tail = (uint16_t)((scpi_rx_tail + 1U) % SCPI_UART_RXBUF_SIZE);
        SCPI_Input(&scpi_context, &c, 1);
    }
}

size_t SCPI_Write(scpi_t *context, const char *data, size_t len) {
    (void) context;
    HAL_UART_Transmit(scpi_huart, (uint8_t *) data, (uint16_t) len, HAL_MAX_DELAY);
    return len;
}

scpi_result_t SCPI_Flush(scpi_t *context) {
    (void) context;
    return SCPI_RES_OK;
}

int SCPI_Error(scpi_t *context, int_fast16_t err) {
    (void) context;
    (void) err;
    return 0;
}

scpi_result_t SCPI_Control(scpi_t *context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    (void) context;
    (void) ctrl;
    (void) val;
    return SCPI_RES_OK;
}
