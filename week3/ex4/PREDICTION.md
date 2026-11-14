# Dự đoán — Week 3 / Ex4

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006

1. Chạy `app/deadlock_demo.c` y nguyên như được cấp. Dự đoán: hệ thống sẽ
   treo sau khoảng bao lâu (ngay lập tức, hay sau vài chu kỳ)? Vì sao không
   phải ngay lập tức (gợi ý: cần cả 2 task "đua" vào đúng thời điểm)?

   Dự đoán sau vài chu kỳ hoặc rất sớm sau khi chạy. Không nhất thiết treo ngay lập tức vì hai task phải đồng thời giữ hai mutex khác nhau và cùng chờ mutex còn lại

2. Khi treo, `uxTaskGetSystemState` sẽ báo trạng thái gì cho `task_alpha` và
   `task_beta`? Dự đoán tên mutex mỗi task đang chờ (dựa trên đọc code, chưa
   chạy công cụ).

   task_alpha: eBlocked, giữ mutex A và chờ mutex B.
   task_beta: eBlocked, giữ mutex B và chờ mutex A.
   
3. Tính reload value cho IWDG với `IWDG_TIMEOUT_MS` của bạn (LSI ~40 kHz).
Giả sử LSI = 40 kHz, chọn prescaler = 64 và timeout = 1000 ms: ==> 40000/64=625

Giá trị nạp thanh ghi reload là 624 (đếm từ 0).

4. Sau khi sửa lỗi deadlock và thêm supervisor + IWDG, nếu bạn cố tình tái
   tạo lại tình huống deadlock cũ (tạm thời), dự đoán board sẽ phản ứng thế
   nào và sau bao lâu.

   Supervisor sẽ không nhận đủ tín hiệu check-in, ngừng refresh watchdog. STM32 dự kiến tự reset trong khoảng 1000 ms kể từ lần refresh cuối cùng.
