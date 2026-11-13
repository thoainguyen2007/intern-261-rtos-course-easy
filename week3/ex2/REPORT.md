# Report — Week 3 / Ex2

`student_id`: 2511006   Commit của PREDICTION.md: ______

## 1. Nảy phím trước/sau
Ảnh logic analyzer: tín hiệu thô (nhiều cạnh nảy) và tín hiệu "PRESS" sau khi
qua `debounce_timer` (một cạnh sạch).

(xem ảnh pulseview.png)

## 2. Callback bị block — thí nghiệm bước 4
| | Dự đoán | Đo được |
|---|---|---|
| Độ trễ `longpress_callback` khi `debounce_callback` block 50ms | 0-50| 0|

Log timestamp (`xTaskGetTickCount()`) của cả hai callback quanh lúc bị block.
Với LONGPRESS_MS = 600, kết quả UART cho thấy các sự kiện HELD vẫn xuất hiện đúng chu kỳ 600 ms (tick 2010, 2610, 3210, 3810).

Nguyên nhân là debounce callback bắt đầu chạy khoảng 30 ms sau khi nhấn và block Timer Daemon Task trong 50 ms, tức đến khoảng 80 ms. Trong khi đó, long-press timer chỉ hết hạn ở 600 ms, nên daemon đã hoạt động trở lại trước thời điểm này.

Nếu giả sử giảm LONGPRESS_MS xuống 60 ms, long-press timer sẽ hết hạn trong khoảng daemon đang bị block (30–80 ms). Khi đó callback HELD không thể chạy ở 60 ms mà phải chờ đến khoảng 80 ms, gây độ trễ dự kiến khoảng 20 ms.

## 3. Giải thích daemon task
Giải thích **bằng cơ chế**, không chỉ hiện tượng: vì sao một callback block sẽ
ảnh hưởng timer khác, liên hệ đến timer command queue và việc có đúng MỘT
daemon task xử lý tất cả software timer.
Các Software Timer dùng chung một Timer Daemon Task để thực thi callback. Khi một callback gọi vTaskDelay(50), daemon bị BLOCKED, khiến những callback timer khác đến hạn trong khoảng thời gian này phải chờ.

Các lệnh điều khiển timer từ ISR cũng được đưa vào Timer Command Queue và phải chờ daemon xử lý.

Vì vậy, callback của Software Timer không nên chứa các lệnh gây block. 
## 4. Long-press
Thời gian giữ thực tế đo được so với `LONGPRESS_MS`: khớp không?

Khoảng thời gian từ PRESS đến HELD đầu tiên:73201 - 72631 = 570 ms

Do debounce timer xác nhận PRESS sau khoảng 30 ms, còn long-press timer bắt đầu đếm từ ISR khi nhấn nút, ta có: 570 + 30 = 600 ms

Chu kỳ giữa các lần HELD tiếp theo:

73801 - 73201 = 600 ms

74401 - 73801 = 600 ms

75001 - 74401 = 600 ms

Trong log còn ghi nhận các sự kiện HELD tiếp tục tại tick 75601, 76201, 76801 và 77401.

Kết luận: Kết quả UART phù hợp với ngưỡng long-press 600 ms và xác nhận callback lặp lại mỗi 600 ms khi giữ nút.





Bonus: Giả sử LONGPRESS_MS = 60 ms và debounce_callback() gọi vTaskDelay(200).

t = 0 ms: Nhấn nút, long-press timer bắt đầu.

t = 30 ms: Debounce callback chạy, daemon bị block đến t = 230 ms.

t = 60, 120, 180 ms: Long-press timer đến hạn nhưng callback HELD không thể chạy.

t = 230 ms: Daemon hết block, thực hiện bù 3 lần HELD gần như liên tiếp.

t = 240 ms: HELD tiếp tục chạy theo chu kỳ bình thường.

Kết luận: Timer vẫn tính thời gian trong lúc daemon bị block, nhưng callback không thể chạy đúng hạn. Khi hết block, các callback của auto-reload timer có thể chạy dồn lại. 

## 5. Lệch so với dự đoán
Mỗi mục lệch + nguyên nhân.

## 6. Câu hỏi đánh đổi
Với `DEBOUNCE_MS` của bạn, nếu giảm xuống còn một nửa thì tín hiệu sau debounce
còn sạch không? Đo lại trên board của bạn, không suy luận suông.

Thử giảm DEBOUNCE_MS từ 30 ms xuống 15 ms, giữ nguyên LONGPRESS_MS = 600 ms.

Kết quả UART cho thấy khoảng cách từ PRESS đến HELD đầu tiên là 585 ms, phù hợp với thời gian debounce 15 ms và ngưỡng long-press 600 ms.

Khi giảm DEBOUNCE_MS xuống 15 ms, kết quả PulseView cho thấy PB1 xuất hiện nhiều cạnh dội liên tiếp, đặc biệt từ khoảng +14 ms đến +20 ms. PA7 không thay đổi theo các cạnh dội mà chỉ chuyển mức một lần tại khoảng +34 ms. Độ trễ từ cạnh dội cuối đến khi PA7 đổi mức là khoảng 14–15 ms, phù hợp với giá trị debounce đã cấu hình. 

Nhận xét: Giảm debounce từ 30 ms xuống 15 ms giúp phản hồi nút nhanh hơn. Tuy nhiên, nếu nút dội kéo dài hoặc tín hiệu tạm ổn định quá 15 ms rồi tiếp tục dội, PA7 có thể thay đổi sai. Vì vậy, 30 ms an toàn hơn, còn 15 ms có độ trễ thấp hơn.