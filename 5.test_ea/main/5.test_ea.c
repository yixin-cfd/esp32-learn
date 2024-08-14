#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

TaskHandle_t taskA_handle;
TaskHandle_t taskB_handle;



void taskA(void* param){
    vTaskDelay(500);    // 留taskB 创建时间
    // 定时发送一个任务通知值
    uint32_t value = 0;
    for(;;){
        xTaskNotify(taskB_handle, value, eSetValueWithOverwrite);
        vTaskDelay(pdMS_TO_TICKS(1000));
        value++;
    }
}

void taskB(void* param){
    // 接受任务通知值
    uint32_t value;
    for(;;){
        xTaskNotifyWait(0x00, ULONG_MAX, &value, portMAX_DELAY);
        ESP_LOGI("ev", "notify wait value: %lu", value);
    }
}


void app_main(void)
{
    xTaskCreatePinnedToCore(taskA, "taskA", 2048, NULL, 5, &taskA_handle, 1);
    xTaskCreatePinnedToCore(taskB, "taskB", 2048, NULL, 5, &taskB_handle, 1);
}
