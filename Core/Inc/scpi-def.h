#ifndef SCPI_DEF_H
#define SCPI_DEF_H

#include "scpi/scpi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCPI_INPUT_BUFFER_LENGTH 256
#define SCPI_ERROR_QUEUE_SIZE 17

#define SCPI_IDN1 "STMicroelectronics"
#define SCPI_IDN2 "STM32L432-Nucleo32"
#define SCPI_IDN3 NULL
#define SCPI_IDN4 "1.0"

extern const scpi_command_t scpi_commands[];
extern scpi_interface_t scpi_interface;
extern char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
extern scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];
extern scpi_t scpi_context;

size_t SCPI_Write(scpi_t *context, const char *data, size_t len);
scpi_result_t SCPI_Flush(scpi_t *context);
int SCPI_Error(scpi_t *context, int_fast16_t err);
scpi_result_t SCPI_Control(scpi_t *context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val);
scpi_result_t SCPI_Reset(scpi_t *context);

#ifdef __cplusplus
}
#endif

#endif /* SCPI_DEF_H */
