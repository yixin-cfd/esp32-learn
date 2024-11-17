#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

#define LED_GPIO GPIO_NUM_2

#define FULL_EV_BIT BIT0
#define EMPTY_EV_BIT BIT1

static  EventGroupHandle_t ledc_event_handle;

bool IRAM_ATTR ledc_finish_cb(const ledc_cb_param_t *param, void *user_arg)
{
    BaseType_t taskWoken;
    if (param->duty)
    {
        xEventGroupSetBitsFromISR(ledc_event_handle, FULL_EV_BIT, &taskWoken); //通知
    }
    else
    {
        xEventGroupSetBitsFromISR(ledc_event_handle, EMPTY_EV_BIT, &taskWoken);//通知
    }
    return taskWoken;
}

void led_run_task(void *param)
{
    EventBits_t ev;
    ESP_LOGI("led", "run");
    for (;;)
    {
        ev = xEventGroupWaitBits(ledc_event_handle, FULL_EV_BIT | EMPTY_EV_BIT, pdTRUE, pdFALSE, pdMS_TO_TICKS(5000));

        // 启动新一轮渐变
        if (ev & FULL_EV_BIT)
        {
            ESP_LOGI("led", "full");
            ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0, 2000); // PWM 渐变
            ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
        }
        if (ev & EMPTY_EV_BIT)
        {
            ESP_LOGI("led", "empty");
            ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 8191, 2000); // PWM 渐变
            ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
        }
        ledc_cbs_t cbs = {
            .fade_cb = ledc_finish_cb};
        ledc_cb_register(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, &cbs, NULL);
    }
}

void app_main(void)
{
    gpio_config_t led_cfg = {
        .pin_bit_mask = (1 << LED_GPIO),
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&led_cfg);      // LED 初始化

    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
        .freq_hz = 5000, // 频率
        .duty_resolution = LEDC_TIMER_13_BIT,  // 分辨率
    }; // 定时器初始化

    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .gpio_num = LED_GPIO,
        .duty = 0,  // 占空比
        .intr_type = LEDC_INTR_DISABLE, // 不使能中断
    };  // 定时器通道
    ledc_channel_config(&ledc_channel);

    ledc_fade_func_install(0); // PWM

    ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 8191, 2000); // PWM 渐变

    ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);

    ledc_event_handle = xEventGroupCreate();

    ledc_cbs_t cbs = {
        .fade_cb = ledc_finish_cb}; // 回调函数

    ledc_cb_register(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, &cbs, NULL);

    xTaskCreatePinnedToCore(led_run_task, "taskA", 2048, NULL, 5, NULL, 1);
}
