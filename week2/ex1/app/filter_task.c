/* Week 2 / Ex1 — moving-average filter, window = FILTER_WINDOW samples.
 * queue_raw -> queue_filtered.
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "main.h"
#include "params.h"
#include "pipeline.h"

void filter_task(void *argument)
{
    (void)argument;

    /* TODO 1: ring buffers for ax/ay/az, size FILTER_WINDOW, plus running sums.
     *         Do NOT recompute the full sum every sample — that is the whole
     *         point of a moving-average window. */
    int16_t buf_ax[FILTER_WINDOW] = {0};
    int16_t buf_ay[FILTER_WINDOW] = {0};
    int16_t buf_az[FILTER_WINDOW] = {0};

    int32_t sum_ax = 0;
    int32_t sum_ay = 0;
    int32_t sum_az = 0;

    uint32_t index = 0;
    uint32_t count = 0;

    for (;;) {
        raw_sample_t in;

        /* TODO 2: xQueueReceive(queue_raw, &in, portMAX_DELAY). */
        xQueueReceive(queue_raw, &in, portMAX_DELAY);

        /* TODO 3: push into the ring buffer, update running sums, compute the
         *         averaged ax/ay/az. */

        // Loại giá trị cũ ra khỏi tổng
        sum_ax -= buf_ax[index];
        sum_ay -= buf_ay[index];
        sum_az -= buf_az[index];

        // Lưu mẫu mới vào ring buffer
        buf_ax[index] = in.ax;
        buf_ay[index] = in.ay;
        buf_az[index] = in.az;

        // Cộng mẫu mới vào tổng
        sum_ax += in.ax;
        sum_ay += in.ay;
        sum_az += in.az;

        // Chuyển sang vị trí tiếp theo, quay về 0 khi hết mảng
        index = (index + 1) % FILTER_WINDOW;

        // Đếm số mẫu
        if (count < FILTER_WINDOW-1)
        {
            count++;
            continue;
        }

        

        filtered_sample_t out;
        out.seq = in.seq;
        /* TODO 4: fill out.ax/ay/az from the running average. */
        out.ax=sum_ax/FILTER_WINDOW;
        out.ay=sum_ay/FILTER_WINDOW;
        out.az=sum_az/FILTER_WINDOW;

        /* TODO 5: xQueueSend(queue_filtered, &out, <timeout>) — same question
         *         as sensor_task: what should happen here if uart_task is
         *         slow and queue_filtered is full? */
        xQueueSend(queue_filtered, &out, 0);
        // xQueueSend(queue_filtered, &out, pdMS_TO_TICKS);
        // xQueueSend(queue_filtered, &out, portMAX_DELAY);
        
    }
}
