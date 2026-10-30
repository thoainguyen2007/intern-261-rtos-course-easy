# Report — Week 2 / Ex1

`student_id`: 2511006   Commit của PREDICTION.md: ______

## 1. Kích thước queue tối thiểu
| | Dự đoán | Đo được | Khớp? |
|---|---|---|---|
| `queue_raw` |1 | 0|Không |
| `queue_filtered` |1 |0 |Không |

Nếu lệch: nguyên nhân (thời gian xử lý thực tế khác ước lượng, jitter I2C, ...).
Priority của logger thấp hơn 3 task kia, cho nên Trong 1 chu kỳ, khi sensor đẩy mẫu vào queue, rồi filter lấy mẫu, xử lý rồi đẩy vào queue, rồi uart lấy mẫu từ queue rồi đẩy lên uart thì sau đó logging mới được cập nhật số mẫu trong queue, và tất nhiên lúc này số mẫu là 0. 
## 2. Đồ thị độ đầy queue theo thời gian
Cấu hình bình thường: `![](queue_fill_normal.png)`
Cấu hình UART chậm (9600 baud): `![](queue_fill_slow_uart.png)`

Giải thích hình dạng đồ thị ở mỗi cấu hình.
Ở cấu hình bình thường giải thích giống câu 1
Ở cấu hình chậm, xem giải thích ở câu 4.

## 3. Khi nào queue đầy
Mô tả **chính xác** thời điểm `queue_filtered` bắt đầu đầy khi hạ baud, và số
mẫu bị mất/trễ (đếm bằng số thứ tự mẫu gắn trong payload).
queue_filter luôn đầy sau 210ms khi hạ baud. Lí do là thời gian từ lúc đầu đến thời gian đầy chỉ bị ảnh hưởng bởi tốc độ truyền bit của uart. Timeout send của task filter dù là 0 hay max thì nó cũng sẽ làm đầy queue filter rất nhanh nếu received của uart chậm hơn send của filter.

## 4. Hành vi 3 kiểu timeout
| Timeout | Kết quả quan sát khi queue đầy |
|---|---|
| 0 | Gửi thất bại ngay lập tức, không chờ|
| giá trị hữu hạn | vd pdMS_TO_TICK(20) thì sẽ chờ 20 tick(20ms) để chờ gửi mẫu đó, hết 20 tick thì hủy mẫu đó |
| `portMAX_DELAY` | Chờ vô hạn, chờ đến khi nào queue có chỗ trống thì mới gửi mẫu, còn không có chỗ trống thì chờ mãi mãi|

## 5. Lệch so với dự đoán
Mua nhầm MPU6050 giả(thực ra là 6500, giá trị thanh ghi WHO_AM_I đổi từ 0x68 thành 0x70)
Lệch dự đoán 

## 6. Câu hỏi đánh đổi
Với `SAMPLE_PERIOD_MS` của **bạn**, nếu `FILTER_WINDOW` tăng gấp đôi thì độ trễ
từ cảm biến đến UART tăng thêm bao nhiêu ms? Trả lời bằng số từ dữ liệu của bạn,
rồi đo lại để kiểm chứng.
