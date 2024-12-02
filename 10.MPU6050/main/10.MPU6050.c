#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "mpu6050/eMPL/inv_mpu.h"
#include "mpu6050/eMPL/inv_mpu_dmp_motion_driver.h"
#include "esp_log.h"

#define mpu6050_INT GPIO_NUM_19

// 队列句柄
static QueueHandle_t gpio_evt_queue = NULL;

float pitch, roll, yaw;

// 中断服务例程
 static void IRAM_ATTR mpu6050_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    // 将中断引脚号发送到队列
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

// 任务：处理 MPU6050 数据
void mpu6050_task(void *arg)
{
    uint32_t io_num;
    while (1)
    {
        // 等待中断信号
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            // 从 MPU6050 获取数据
            uint8_t res = mpu_dmp_get_data(&pitch, &roll, &yaw);
            if (res == 0)
            {
                float temperature = MPU_Get_Temperature();
                ESP_LOGI("main", "gpio: %" PRIu32 ", pitch: %.3f, roll: %.3f, yaw: %.3f , t: %.3f", io_num, pitch, roll, yaw, temperature);
            }
            else
            {
                ESP_LOGE("main", "mpu read error: %d", res);
            }
        }
    }
}

void app_main(void)
{
    // 初始化 MPU6050
    mpu6050_config_t cfg = {
        .scl = GPIO_NUM_23,
        .sda = GPIO_NUM_22,
        .freq = 200 * 1000};
    mpu_dmp_init(&cfg);

    // 配置 GPIO
    gpio_config_t mpu6050_int_pin = {
        .pin_bit_mask = 1ULL << mpu6050_INT,
        .mode = GPIO_MODE_INPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE};
    gpio_config(&mpu6050_int_pin);

    // 创建队列
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // 安装中断服务
    gpio_install_isr_service(0);
    gpio_isr_handler_add(mpu6050_INT, mpu6050_handler, (void *)mpu6050_INT);

    // 创建任务处理 MPU6050 数据
    xTaskCreatePinnedToCore(mpu6050_task, "mpu6050_task", 2048, NULL, 10, NULL, 1);
}
