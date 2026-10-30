# Report — Week 1 / Ex3

`student_id`: ______    Commit của PREDICTION.md: ______

## 1. Bảng thống kê (tính từ CSV, không đọc tay)
| # | Delay | Tick | min (ms) | max (ms) | tb (ms) | jitter = max−min (us) | drift tổng (ms) |
|---|---|---|---|---|---|---|---|
| 1 | `vTaskDelay` | 1000 |18.9980 |19.0010 |18.9991 |3.00 |-0.89 |
| 2 | `xTaskDelayUntil` | 1000 |18.9980 |19.0010 |18.9991 |3.00 |-0.90 |
| 3 | `vTaskDelay` | 100 |9.9980 | 9.10.0010|9.9995 |3.00 |-9000.47 |
| 4 | `xTaskDelayUntil` | 100 |9.9980 |10.0010 |9.9995 |3.00 |-9000.47 |

- Script dùng để tính: `analyse.py`
- CSV gốc: `data1.csv`, `data2.csv`, `data3.csv`, `data4.csv`

## 2. Tại sao `vTaskDelay` bị drift
Công thức drift theo `PERIOD_MS` và `WORK_JITTER_US` của **bạn**, rồi so với số đo:
Do WORK_JITTER_US = 200 us < 1 ms (1 tick), tải luôn chạy xong trước khi ngắt SysTick kế tiếp nổ. vTaskDelay không bị trượt tick nên drift lý thuyết bằng 0 ms. Sai lệch -0.89 ms đo được hoàn toàn do thạch anh chạy chậm (-46.8 ppm).

| | Tính theo công thức | Đo được | Lệch |
|---|---|---|---|
| Drift tổng (ms) | 0.00 | -0.89 | -0.89 |

## 3. Tại sao tick 100 Hz sai nhiều hơn
`PERIOD_MS` = 19 ; một tick = 1 ms. Chứng minh bằng số đo, không bằng lý thuyết:
Số đo tb = 9.9995 ms chứng minh chu kỳ thực tế bị co về đúng 10 ms (1 tick) thay vì 19 ms. Do pdMS_TO_TICKS(19) làm tròn nguyên thành 1 tick, hệ thống mất 9 ms mỗi chu kỳ, tạo drift -9000.47 ms sau 1000 chu kỳ.

## 4. `xTaskDelayUntil` trả về gì
Giá trị trả về nghĩa là gì, và trong 4 lần chạy của bạn nó có bao giờ báo
"đã trễ deadline" không? Bao nhiêu lần / `SAMPLE_COUNT`?
Hàm trả về pdTRUE nếu task thức dậy đúng hạn và pdFALSE nếu trễ deadline. Trong 4 lần chạy, hàm trả về pdTRUE 100% (0 lần trễ / 1000 SAMPLE_COUNT) vì tải tối đa 200 us luôn nhỏ hơn chu kỳ tick.

## 5. Lệch so với dự đoán
Mỗi dòng lệch trong bảng → lệch bao nhiêu + nguyên nhân vật lý.
Dòng 1, 2 lệch -0.89 ms đến -0.90 ms: do thạch anh 8 MHz trên kit bị sai số ppm vật lý theo nhiệt độ.
Dòng 3, 4 lệch -0.47 ms (so với -9000 ms): cũng do sai số ppm của thạch anh tích lũy trong 1000 chu kỳ.
## 6. Câu hỏi đánh đổi
Tick 1000 Hz cho độ chính xác tốt hơn nhưng **tốn** gì? Dựa vào dữ liệu của bạn:
đo CPU overhead của tick ISR ở 100 Hz và 1000 Hz (gợi ý: cho một task ưu tiên thấp
nhất đếm vòng lặp trong 1 giây, so sánh 2 cấu hình). Điền số:

| Tick rate | Số vòng lặp idle đếm được trong 1 s | Overhead suy ra (%) |
|---|---|---|
| 100 Hz | | |
| 1000 Hz | | |

Với ứng dụng `PERIOD_MS` = ____ của bạn, bạn chọn tick rate nào? Bảo vệ bằng số trên.
