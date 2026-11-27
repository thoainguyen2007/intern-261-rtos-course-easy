# Dự đoán — Week 4 / Ex3

> **Commit TRƯỚC khi đo tải cao nhất.**

`student_id`: 2511006

1. Dự đoán CPU% của `logger_task` khi hệ thống ở tải cao nhất — ước lượng
   dựa trên `SAMPLE_RATE_HZ` và thời gian xử lý 1 mẫu bạn đã ước lượng ở
   `week4/ex1`.

   Thời gian truyền tính trên một mẫu: 11111 / 32 = 347,22 µs.

   CPU% ước lượng = 400 × 347,22 / 1000000 × 100% = 13,89%.

   Dự đoán CPU% của logger khoảng 14–17%.

2. Dự đoán stack HWM (bytes còn trống) của từng task — task nào bạn nghĩ
   sẽ có margin thấp nhất? Vì sao?

   Filter: 1100 byte.

   Logger: 1100 byte.

   Measure: 600 byte.

   Noise: 320 byte.

   Timer Service: 700 byte.

   Idle: 360 byte.

   Noise có số byte còn trống thấp nhất do được cấp stack nhỏ. Measure có tỷ lệ stack còn trống thấp nhất do sử dụng các hàm thống kê và định dạng chuỗi.

3. Dự đoán latency ISR→xử lý ở worst-case có còn nằm trong
   `ISR_TO_PROC_DEADLINE_US` không khi có task nhiễu priority cao hơn
   chạy cùng? Nếu không đạt, bạn dự đoán lệch bao nhiêu %?

   Theo Ex1, latency dự kiến là 598,5 µs. Task nhiễu có priority 3, thấp hơn Filter (4) nên không trực tiếp chặn Filter.

   Dự đoán worst-case latency khoảng 650–750 µs, vẫn đạt deadline 800 µs.  
