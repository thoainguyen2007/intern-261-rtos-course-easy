# Dự đoán — Week 2 / Ex1

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006

1. Phép tính kích thước queue tối thiểu (dùng `SAMPLE_PERIOD_MS`, thời gian xử lý
   ước lượng của `filter_task` và `uart_task` ở `UART_BAUD`):

```
Thông số đề bài:
- SAMPLE_PERIOD_MS = 10 ms
- FILTER_WINDOW = 16 mẫu
- UART_BAUD = 230400 baud
- Mỗi phần tử của queue_raw và queue_filtered có kích thước 12 bytes.

Thông số ước lượng:
- Thời gian filter_task xử lý một mẫu: T_filter = 1 ms.
- UART truyền dữ liệu theo cấu hình 8N1 (10 bit/byte)==>mỗii dòng UART dài tối đa 36 bytes để dự phòng.

   Sensor tạo một mẫu mỗi 10 ms.
   F_sensor = 1000 / 10 = 100 mẫu/giây.
Filter sử dụng moving average với ring buffer và running sum. Mỗi mẫu mới chỉ cần trừ mẫu cũ, cộng mẫu mới rồi tính trung bình.
   Giả định T_filter = 1 ms/mẫu.
   F_filter = 1000 / 1 = 1000 mẫu/giây.
UART truyền chuỗi "seq,ax,ay,az\r\n".
   Với giả định 36 bytes/dòng và cấu hình 8N1:
   T_uart = (36 × 10 × 1000) / 230400 = 1.5625 ms.
   F_uart = 1000 / 1.5625 = 640 mẫu/giây.
Dự đoán kích thước queue:
Do T_filter = 1 ms và T_uart = 1.5625 ms đều nhỏ hơn chu kỳ lấy mẫu 10 ms, nên filter_task và uart_task có khả năng xử lý kịp dữ liệu do sensor_task tạo ra.


```

   Kích thước tối thiểu dự đoán: `queue_raw` = 1 , `queue_filtered` = 1 .
   So với `QUEUE_LENGTH` được cấp: đủ hay không? Vì sao? đủ do nhỏ hơn 4.

2. Khi hạ `UART_BAUD` xuống 9600 (tạm thời), queue nào sẽ đầy trước? Vì sao?

Thời gian truyền một mẫu ở 9600 baud:
T_uart = (36 × 10 × 1000) / 9600 = 37.5 ms.

Tốc độ truyền UART:
F_uart = 1000 / 37.5 = 26.67 mẫu/giây.

Trong khi đó sensor tạo 100 mẫu/giây và filter có khả năng xử lý nhanh hơn sensor.Vì tốc độ truyền UART thấp hơn tốc độ tạo mẫu nên dữ liệu sẽ tích tụ trong queue_filtered.

Dự đoán queue_filtered sẽ đầy trước. Nếu filter_task phải chờ khi queue_filtered đầy, nó sẽ không tiếp tục lấy dữ liệu từ queue_raw. Khi đó queue_raw cũng có thể đầy do sensor_task vẫn tiếp tục tạo mẫu.


3. Với `xQueueSend(..., 0)` (timeout = 0) khi queue đầy: điều gì xảy ra với mẫu đó?

Khi timeout = 0, xQueueSend sẽ không chờ queue có chỗ trống. Nếu queue đang đầy:

-Hàm trả về errQUEUE_FULL (giá trị tương ứng pdFALSE).

-Mẫu không được đưa vào queue==>mất mẫu

-Task gửi tiếp tục thực hiện các lệnh phía sau.

4. Với `xQueueSend(..., portMAX_DELAY)` khi queue đầy: `sensor_task` sẽ ở trạng thái
   nào? Có nguy cơ gì cho chu kỳ lấy mẫu?

Khi queue đầy, 'sensor_task' sẽ bị blocked đến khi nào queue hết đầy. Việc này làm cho task không thể tiếp tục đọc cảm biến. Nếu thời gian chờ quá lâu, task có thể bỏ lỡ các thời điểm lấy mẫu theo chu kỳ 10 ms, khiến khoảng cách giữa các lần đọc thực tế không còn đều nhau. 
5. Độ đầy `queue_raw` theo thời gian ở cấu hình bình thường sẽ trông như thế nào
   (tăng dần / dao động quanh một mức / luôn gần 0)? Vì sao?

Vì filter_task có khả năng xử lý nhanh hơn tốc độ tạo mẫu của sensor_task nên dữ liệu trong queue_raw sẽ được lấy ra tương đối nhanh. Vì vậy queue_raw thường ở mức 0 hoặc 1 phần tử.
