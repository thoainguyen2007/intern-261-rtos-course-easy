# Dự đoán — Week 4 / Ex1

> **Commit TRƯỚC khi gửi design doc cho mentor duyệt.**

`student_id`: 2511006

1. Với `SAMPLE_RATE_HZ` của bạn, chu kỳ lấy mẫu là bao nhiêu us? Dựa trên
   latency ISR→task thật bạn đã đo ở `week3/ex1`, dự đoán task xử lý có kịp
   `ISR_TO_PROC_DEADLINE_US` không? Trình bày phép tính, không đoán.
   Chu kỳ =1/`SAMPLE_RATE_HZ`=1/400=2,5ms


   Latency đã đo: 16us
   Thời gian xử lý: ~500us
   ==>500+16=516<`ISR_TO_PROC_DEADLINE_US`=800us   
   ==>Đạt

2. Dự đoán tổng RAM (queue + stack mọi task) theo thiết kế của bạn, tính
   bằng bytes. So với `MAX_HEAP_BYTES`: còn dư bao nhiêu?

Mỗi mẫu 16 byte
M queue 32 mẫu để dự phòng khi task bị trễ .
- RAM queue: 2 × 32 × 16 = 1024 byte.
- RAM stack: Task xử lý 1536 + UART 1536 + Idle 512 + Timer Service 1024 = 4608 byte.
- Tổng: 1024 + 4608 = 5632 byte.
- RAM còn dự phòng: 10240 − 5632 = 4608 byte.





3. Nếu task xử lý tạm thời chậm hơn ISR lấy mẫu (ví dụ do một task khác
   chiếm CPU), hàng đợi của bạn đầy sau bao nhiêu mẫu? Điều gì xảy ra với
   mẫu tiếp theo trong thiết kế của bạn?
   Do chọn 1 queue là 32 mẫu nên đầy sau 32 mẫu. Mẫu tiếp theo mất do dùng pdFAIL. 




4. Bạn dự đoán priority nào (ISR task xử lý vs. task ghi UART) sẽ cho kết
   quả đạt deadline tốt hơn? Vì sao?
   Task ghi uart: 2
   task xử lý: 4
   ISR: 8

   task uart sẽ xử lý lâu hơn nên ta sẽ cho priority thấp hơn. 
