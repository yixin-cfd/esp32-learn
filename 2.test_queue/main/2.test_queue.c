#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h>

QueueHandle_t   queue_handle = NULL;

typedef struct{
    int value;
}queue_data_t;

void taskA(void* param){
    // 从队列中接收数据，并打印
    queue_data_t data;
    for(;;){
        if(pdTRUE == xQueueReceive(queue_handle, &data, 100)){
            ESP_LOGI("queue", "recieve queue value: %d", data.value);
        }
    }
}

void taskB(void* param){
    queue_data_t data;
    memset(&data, 0, sizeof(data));
    // 每隔1秒向队列发送数据
    for(;;){
        xQueueSend(queue_handle, &data, 100);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ++data.value;
    }
}


void app_main(void)
{
    queue_handle = xQueueCreate(10, sizeof(queue_data_t));
    xTaskCreatePinnedToCore(taskA, "recive", 2048, NULL, 10, NULL, 1);
    xTaskCreatePinnedToCore(taskB, "send", 2048, NULL, 10, NULL, 1);
}
