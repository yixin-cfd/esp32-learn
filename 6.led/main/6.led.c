#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#define LED_GPIO    GPIO_NUM_2

void led_run_task(void* param){
    uint8_t gpio_level = 0;
    for(;;){
        gpio_level = (gpio_level == 0)?1:0;
        gpio_set_level(LED_GPIO, gpio_level);
        vTaskDelay(pdMS_TO_TICKS(200));
        ESP_LOGI("led", "led_level: %d", gpio_level);
    }
}

void app_main(void)
{
    gpio_config_t led_cfg = {
        .pin_bit_mask = (1 << LED_GPIO),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&led_cfg);
    xTaskCreatePinnedToCore(led_run_task, "led", 2048, NULL, 5, NULL, 1);
}
