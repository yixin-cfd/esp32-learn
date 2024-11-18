#include <stdio.h>
#include "driver/gpio.h"
#include "mpu6050.h"
#include "esp_log.h"

void app_main(void)
{
    // float pitch, roll, yaw;    // 欧拉角
    short ax, ay, az;    // 加速度传感器原始数据
    short gx, gy, gz; // 陀螺仪原始数据
    short temperature;                // 温度
    mpu6050_config_t cfg = {
        .scl = GPIO_NUM_23,
        .sda = GPIO_NUM_22,
        .freq = 200 * 1000};
    MPU_Init(&cfg);
    for(;;){
        MPU_Get_Accelerometer(&ax, &ay, &az);
        MPU_Get_Gyroscope(&gx, &gy, &gz);
        temperature = MPU_Get_Temperature();
        ESP_LOGI("main", "ax: %d, ay: %d, az: %d, gx: %d, gy: %d, gz: %d, t: %.3f", ax, ay, az, gx, gy, gz, (float)temperature/100);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
