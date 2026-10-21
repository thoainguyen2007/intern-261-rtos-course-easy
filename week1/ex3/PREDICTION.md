# Dự đoán — Week 1 / Ex3

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006   `PERIOD_MS` = 19    `WORK_JITTER_US` = 200    `SAMPLE_COUNT` = 1000

1. Bảng dự đoán (điền trước khi đo):

| # | Delay | Tick | Chu kỳ tb (ms) | Jitter (us) | Drift tổng sau SAMPLE_COUNT (ms) |
|---|---|---|---|---|---|
| 1 | `vTaskDelay` | 1000 | 19.1| 200|0.1*1000=100 |
| 2 | `xTaskDelayUntil` | 1000 | 19|0 | 0|
| 3 | `vTaskDelay` | 100 |10.1 | 200|(19-10.1)*1000=8900 |
| 4 | `xTaskDelayUntil` | 100 | 10|0 |(19-10)*1000=9000 |

2. Cấu hình nào có drift lớn nhất? Vì sao, viết bằng công thức có `WORK_JITTER_US`.
Cấu hình 4 có drift lớn nhất với drift được tính theo công thức (19-10)*1000=9000

3. `PERIOD_MS` = 19 . Với tick 100 Hz, một tick = 10 ms, nên chu kỳ thực tế sẽ
   bị làm tròn thành 10 ms. Sai số hệ thống: |10-19|/19=47,36 %.
4. Jitter ở tick 1000 Hz so với 100 Hz: lớn hơn / nhỏ hơn / bằng? Vì sao?
Bằng nhau do jitter có ảnh hưởng hay không chỉ phụ thuộc vào dùng xtaskdelayuntil hay vtaskdelay

5. Nếu task này **không** phải ưu tiên cao nhất, đại lượng nào trong bảng bị ảnh hưởng
   nhiều nhất? jitter vì có thể bị ngắt giữa chừng do các task khác.
