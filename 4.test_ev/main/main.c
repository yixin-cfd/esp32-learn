#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#define NUM0_BIT BIT0
#define NUM1_BIT BIT1


static EventGroupHandle_t test_event;

void taskA(void* param){
    // 定时设置不同的事件位
    for(;;){
        xEventGroupSetBits(test_event, NUM0_BIT);
        vTaskDelay(pdMS_TO_TICKS(1000));
        xEventGroupSetBits(test_event, NUM1_BIT);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void taskB(void* param){
    // 等待事件位，并打印
    EventBits_t ev;
    for(;;){
        ev = xEventGroupWaitBits(test_event, NUM0_BIT|NUM1_BIT, pdTRUE, pdFALSE, pdMS_TO_TICKS(5000));
        if(ev & NUM0_BIT){
            ESP_LOGI("ev", "get BIT0 event");
        }
        if(ev & NUM1_BIT){
            ESP_LOGI("ev", "get BIT1 event");
        }
    }
}

void app_main(void)
{
    test_event = xEventGroupCreate();
    xTaskCreatePinnedToCore(taskA, "taskA", 2048, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(taskB, "taskB", 2048, NULL, 5, NULL, 1);
}
