#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

void taskA(void* param){
    ESP_LOGI("main", "Hello World!");
    for(;;){
        
        ESP_LOGI("taskA", "print");
        vTaskDelay(pdMS_TO_TICKS(100));

        
    }
   
}   

void app_main(void)
{
    xTaskCreatePinnedToCore(taskA, "taskA", 2048, NULL, 10, NULL, 0);
}
