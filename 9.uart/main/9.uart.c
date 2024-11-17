#include <stdio.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#define USER_UART UART_NUM_2
static uint8_t uart_buffer[1024];

static QueueHandle_t uart_queue;

void app_main(void)
{
    uart_event_t uart_ev;
    uart_config_t uart_cfg = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .parity = UART_PARITY_DISABLE, // 校验
        .source_clk = UART_SCLK_DEFAULT,
        .stop_bits = UART_STOP_BITS_1,
    };

    uart_param_config(USER_UART, &uart_cfg);
    uart_set_pin(USER_UART, GPIO_NUM_23, GPIO_NUM_22, -1, -1);
    uart_driver_install(USER_UART, 1024, 1024, 20, &uart_queue, 0);

    for (;;)
    {

        if (pdTRUE == xQueueReceive(uart_queue, &uart_ev, portMAX_DELAY))
        {
            switch (uart_ev.type)
            {
            case UART_DATA:
                uart_read_bytes(USER_UART, uart_buffer, uart_ev.size, pdMS_TO_TICKS(1000)); // 连接GPS串口
                uart_buffer[uart_ev.size] = '\0';
                ESP_LOGI("main", "%s ", uart_buffer);
                break;
            case UART_BUFFER_FULL:
                uart_flush(USER_UART);
                xQueueReset(uart_queue);
                break;
            case UART_FIFO_OVF:
                uart_flush(USER_UART);
                xQueueReset(uart_queue);
                break;
            default:
                break;
            }
        }
    }
}
