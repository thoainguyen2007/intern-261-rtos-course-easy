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
- CSV gốc: `______` (commit kèm)
- Ảnh: `![](ex3_cfg1.png)` `![](ex3_cfg2.png)` `![](ex3_cfg3.png)` `![](ex3_cfg4.png)`

## 2. Tại sao `vTaskDelay` bị drift
Công thức drift theo `PERIOD_MS` và `WORK_JITTER_US` của **bạn**, rồi so với số đo:

| | Tính theo công thức | Đo được | Lệch |
|---|---|---|---|
| Drift tổng (ms) | | | |

## 3. Tại sao tick 100 Hz sai nhiều hơn
`PERIOD_MS` = ____ ; một tick = ____ ms. Chứng minh bằng số đo, không bằng lý thuyết:

## 4. `xTaskDelayUntil` trả về gì
Giá trị trả về nghĩa là gì, và trong 4 lần chạy của bạn nó có bao giờ báo
"đã trễ deadline" không? Bao nhiêu lần / `SAMPLE_COUNT`?

## 5. Lệch so với dự đoán
Mỗi dòng lệch trong bảng → lệch bao nhiêu + nguyên nhân vật lý.

## 6. Câu hỏi đánh đổi
Tick 1000 Hz cho độ chính xác tốt hơn nhưng **tốn** gì? Dựa vào dữ liệu của bạn:
đo CPU overhead của tick ISR ở 100 Hz và 1000 Hz (gợi ý: cho một task ưu tiên thấp
nhất đếm vòng lặp trong 1 giây, so sánh 2 cấu hình). Điền số:

| Tick rate | Số vòng lặp idle đếm được trong 1 s | Overhead suy ra (%) |
|---|---|---|
| 100 Hz | | |
| 1000 Hz | | |

Với ứng dụng `PERIOD_MS` = ____ của bạn, bạn chọn tick rate nào? Bảo vệ bằng số trên.
