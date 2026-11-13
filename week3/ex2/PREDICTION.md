# Dự đoán — Week 3 / Ex2

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: ______

1. Với nút nhấn cơ khí thông thường, nảy phím thường kéo dài bao lâu
   (tra cứu/ước lượng)? So với `DEBOUNCE_MS` của bạn: đủ lớn để lọc hết không?
   Debounce nút nhấn thường từ 5-20ms, với `DEBOUNCE_MS`=30 thì đủ lớn để lịc
2. Nếu `DEBOUNCE_MS` quá lớn, điều gì xảy ra khi người dùng nhấn nhả rất
   nhanh liên tiếp (double-click)?
   Nếu `DEBOUNCE_MS` quá lớn, khi nhấn nhanh 2 hay nhiều lần liên tiếp, nếu thời gian giữa 2 lần nhấn "thực sự" nhỏ hơn `DEBOUNCE_MS` thì sẽ bị nhầm thành dội phím và bị bỏ qua==>mất dữ liệu.
3. Khi callback của `debounce_timer` bị block 50 ms (thử nghiệm bước 4), bạn
   dự đoán `longpress_timer` sẽ bị trễ bao nhiêu? Giải thích bằng cơ chế
   daemon task + queue lệnh, không chỉ đoán số.
   Khi debounce_callback() gọi vTaskDelay(50), Timer Daemon Task bị BLOCKED trong 50 tick. Nếu longpress_timer hết hạn trong thời gian này thì callback của nó không thể chạy ngay, mà phải chờ daemon hoạt động lại. Độ trễ có thể lên đến khoảng 50 ms, tùy thời điểm hết hạn và thời gian xử lý các lệnh trong Timer Command Queue.
4. `LONGPRESS_MS` của bạn là bao nhiêu? Giữ phím đúng ngưỡng này, bạn dự đoán
   callback `longpress_callback` chạy lần đầu ở thời điểm nào tính từ lúc
   nhấn xuống?
   `LONGPRESS_MS`=600. 
   Vì long-press timer bắt đầu ngay khi phát hiện cạnh nhấn xuống trong ISR, callback đầu tiên dự kiến chạy sau khoảng 600 ms tính từ lúc nhấn, có thể trễ thêm một chút do tick và scheduling.