#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

SemaphoreHandle_t  bin_sem;

void taskA(void* param){
    // 释放信号量
    for(;;){
        xSemaphoreGive(bin_sem);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void taskB(void* param){
    // 等待信号量
    for(;;){
        if(pdTRUE == xSemaphoreTake(bin_sem, portMAX_DELAY)){
            ESP_LOGI("bin", "task B take binsem success!");
        }
    }
}


void app_main(void)
{
    bin_sem = xSemaphoreCreateBinary();
    xTaskCreatePinnedToCore(taskA, "taskA", 2048, NULL, 10, NULL, 0);
    xTaskCreatePinnedToCore(taskB, "taskB", 2048, NULL, 10, NULL, 1);
}
