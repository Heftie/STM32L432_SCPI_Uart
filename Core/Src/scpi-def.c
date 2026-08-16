#include "scpi-def.h"
#include "scpi-uart.h"
#include "main.h"

/* USER CODE BEGIN 0 */
static scpi_bool_t led_state = FALSE;

static scpi_result_t SCPI_OutputState(scpi_t *context) {
    scpi_bool_t param;

    if (!SCPI_ParamBool(context, &param, TRUE)) {
        return SCPI_RES_ERR;
    }

    led_state = param;
    if (led_state) {
        BSP_LED_On(LED_GREEN);
    } else {
        BSP_LED_Off(LED_GREEN);
    }

    return SCPI_RES_OK;
}

static scpi_result_t SCPI_OutputStateQ(scpi_t *context) {
    SCPI_ResultBool(context, led_state);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t *context) {
    (void) context;
    led_state = FALSE;
    BSP_LED_Off(LED_GREEN);
    return SCPI_RES_OK;
}
/* USER CODE END 0 */

const scpi_command_t scpi_commands[] = {
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    { .pattern = "*CLS", .callback = SCPI_CoreCls, },
    { .pattern = "*ESE", .callback = SCPI_CoreEse, },
    { .pattern = "*ESE?", .callback = SCPI_CoreEseQ, },
    { .pattern = "*ESR?", .callback = SCPI_CoreEsrQ, },
    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ, },
    { .pattern = "*OPC", .callback = SCPI_CoreOpc, },
    { .pattern = "*OPC?", .callback = SCPI_CoreOpcQ, },
    { .pattern = "*RST", .callback = SCPI_CoreRst, },
    { .pattern = "*SRE", .callback = SCPI_CoreSre, },
    { .pattern = "*SRE?", .callback = SCPI_CoreSreQ, },
    { .pattern = "*STB?", .callback = SCPI_CoreStbQ, },
    { .pattern = "*TST?", .callback = SCPI_CoreTstQ, }, /* provided by libscpi, always returns 0 */
    { .pattern = "*WAI", .callback = SCPI_CoreWai, },

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    { .pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ, },
    { .pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ, },
    { .pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ, },

    { .pattern = "STATus:QUEStionable[:EVENt]?", .callback = SCPI_StatusQuestionableEventQ, },
    { .pattern = "STATus:QUEStionable:ENABle", .callback = SCPI_StatusQuestionableEnable, },
    { .pattern = "STATus:QUEStionable:ENABle?", .callback = SCPI_StatusQuestionableEnableQ, },
    { .pattern = "STATus:PRESet", .callback = SCPI_StatusPreset, },

    /* USER CODE BEGIN commands */
    /* Nucleo-32 on-board LED, exposed as a boolean output channel */
    { .pattern = "OUTPut[:STATe]", .callback = SCPI_OutputState, },
    { .pattern = "OUTPut[:STATe]?", .callback = SCPI_OutputStateQ, },
    /* USER CODE END commands */

    SCPI_CMD_LIST_END,
};

scpi_interface_t scpi_interface = {
    .error = SCPI_Error,
    .write = SCPI_Write,
    .control = SCPI_Control,
    .flush = SCPI_Flush,
    .reset = SCPI_Reset,
};

char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

scpi_t scpi_context;
