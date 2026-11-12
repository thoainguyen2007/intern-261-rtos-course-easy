# Report — Week 3 / Ex1

`student_id`: 2511006    Commit của PREDICTION.md: ______

## 1. Bằng chứng configASSERT
Mô tả/log/ảnh lúc priority sai (bước 4 README) + giải thích chính xác **vì sao**
assert được kích hoạt (nêu đúng tên macro và điều kiện).
Đặt thêm PC13 sáng khi ấn nút, tắt sau khi gọi hàm vTaskNotifyGiveFromISR.
Hiện tượng: ấn nút xong đèn sáng mãi không tắt==>treo sau khi gọi hàm vTaskNotifyGiveFromISR.
Giải thích: Do configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY = 5, tức là ISR có mức ưu tiên từ 0-4 ko được gọi các API FromISR.
## 2. Latency ISR → task
| Cấu hình | Dự đoán (us) | Đo được (us) | Đạt `LATENCY_BUDGET_US`? |
|---|---|---|---|
| Có `portYIELD_FROM_ISR` |vài us |16us |Đạt |
| Không có `portYIELD_FROM_ISR` | 0-1000us|648, 1006, 623,270 | ko đạt|

Ảnh logic analyzer đính kèm cho cả 2 cấu hình.

## 3. Giải thích quan hệ priority
Giải thích bằng lời của bạn: số NVIC priority nhỏ/lớn nghĩa là gì, vì sao
`configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` tồn tại, điều gì xảy ra nếu một
ISR ưu tiên cao hơn ngưỡng này gọi API FreeRTOS.

Số NVIC càng nhỏ thì mức ưu tiên càng cao. Để tránh tất cả các ISR đều bị che khi có một task dùng critical để bảo vệ thì người ta thêm `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY`, tức là các ISR có mức ưu tiên nhỏ hơn `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` thì sẽ không bị che ngắt, đảm bảo hoạt động thời gian thực. Tuy nhiên các ISR này sẽ không được phép gọi các task khác để tránh trương hợp dùng tài nguyên mà task trước đó đang dùng. Nếu cố chấp gọi thì sẽ bị treo.

## 4. Lệch so với dự đoán
Mỗi mục lệch + nguyên nhân vật lý (jitter NVIC, pipeline, v.v.).

Khi ack_task BLOCKED, CPU thường chạy Idle Task có priority 0. Khi ISR đánh thức ack_task (priority 4), nếu có portYIELD_FROM_ISR() thì CPU có thể chuyển sang ack_task ngay sau ISR. Nếu không có Yield, CPU có thể quay lại Idle Task và chờ hết tick(theo cấu hình hiện tại thì 1 tick là 1ms). VÌ vậy thời gian latency có thể >1000us do chờ hết tick và thời gian ngắn chuyển task.
## 5. Câu hỏi đánh đổi
Nếu tăng `ISR_NVIC_PRIO` của bạn lên một bậc (ưu tiên thấp hơn) trong khi vẫn
hợp lệ, latency đo được thay đổi bao nhiêu? Trả lời bằng số đo thật, không
đoán.

Kết quả không đổi do miễn mức ưu tiên từ 5-15 thì cũng như nhau.