/* Week 2 / Ex1 — reads MPU6050 over I2C1 every SAMPLE_PERIOD_MS, pushes a
 * raw_sample_t into queue_raw.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "main.h"
#include "params.h"
#include "pipeline.h"



extern I2C_HandleTypeDef hi2c1;


#define MPU_ADDR        (0x68 << 1)
#define WHO_AM_I    0x75
#define PWR_MGMT_1  0x6B
#define ACCEL_XOUT_H        0x3B

void sensor_task(void *argument)
{
    (void)argument;
    uint32_t seq = 0;

    /* TODO 1: HAL_I2C init / verify MPU6050 WHO_AM_I. Do this once here, not
     *         inside the loop. */

    uint8_t id = 0;
    uint8_t wake = 0x00;
    HAL_StatusTypeDef status;

    // Đọc thanh ghi WHO_AM_I
    status = HAL_I2C_Mem_Read(
        &hi2c1, MPU_ADDR, WHO_AM_I,
        I2C_MEMADD_SIZE_8BIT, &id, 1, 100
    );

    // Kiểm tra MPU6050
    configASSERT(status == HAL_OK);
    configASSERT(id == 0x70);
    //Giá trị thanh ghi WHO_AM_I là 0x68 đối với MPU 6050, nhma hình như mua hàng fake nên nó thành 
    //MPU 6500 nên giá trị thanh ghi WHO_AM_I là 0x70. Thật ra bỏ phần kiểm tra này cũng đc (khi 
    // đó ko cần quan tâm 68 hay 70) nhma do đề kêu verify nên ko né đc.



    // Đánh thức MPU6050
    status = HAL_I2C_Mem_Write(
        &hi2c1, MPU_ADDR, PWR_MGMT_1,
        I2C_MEMADD_SIZE_8BIT, &wake, 1, 100
    );

    configASSERT(status == HAL_OK);

    TickType_t last_wake = xTaskGetTickCount();
    for (;;) {
        raw_sample_t s;
        s.seq = seq++;

        /* TODO 2: HAL_I2C_Mem_Read the accel registers into s.ax/ay/az. */
        uint8_t data[6];

        status = HAL_I2C_Mem_Read(
            &hi2c1,
            MPU_ADDR,
            ACCEL_XOUT_H,
            I2C_MEMADD_SIZE_8BIT,
            data,
            6,
            100
        );

        if (status == HAL_OK)
        {
            s.ax = (int16_t)((data[0] << 8) | data[1]);
            s.ay = (int16_t)((data[2] << 8) | data[3]);
            s.az = (int16_t)((data[4] << 8) | data[5]);
        

        /* TODO 3: xQueueSend(queue_raw, &s, <timeout>) — pick and justify the
         *         timeout in PREDICTION.md question 3/4. What happens to this
         *         sample if the queue is full and the timeout expires? */
            xQueueSend(queue_raw, &s, portMAX_DELAY);
        }
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
    }
}
