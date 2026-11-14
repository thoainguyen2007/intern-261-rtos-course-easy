# Report — Week 3 / Ex4

`student_id`: 2511006    Commit của PREDICTION.md: ______

## 1. Bảng CPU %
| Task | CPU % (lần đo 1) | CPU % (lần đo 2) |
|---|---|---|
| Stats |7% |8% |
|Idle  |92% |91% |
|Timer Service  | <1%|<1% |
| Alpha |<1% | <1%|
|Beta  | <1%| <1%|

Giải thích Idle task chiếm bao nhiêu % và vì sao.
Idle task chiếm khoảng 91-92% CPU , do cả task_alpha và task_beta đã bị chặn, không còn làm được công việc thường lệ. 
Runtime_stats_task vẫn chạy để thu thập và truyền bảng thống kê qua UART; chiếm khoảng 7-8%. 
Alpha/Beta đều có thời gian thực thi tích lũy khác 0(dưới 1%) vì đã chạy một đoạn ngắn trước khi bị chặn

## 2. Bằng chứng tìm deadlock
Log/ảnh `uxTaskGetSystemState` **trước khi sửa**: trạng thái từng task, chỉ
rõ `task_alpha`/`task_beta` đang `eBlocked` và đang chờ mutex nào. Giải thích
chính xác thứ tự lấy mutex nào gây ra deadlock (AB-BA).

```
Stats          	60245049		8%
IDLE           	611579734		91%
Tmr Svc        	2228		<1%
Alpha          	3789		<1%
Beta           	3741		<1%

Stats: Running
IDLE: Ready
Tmr Svc: Blocked
Alpha: Blocked
Beta: Blocked
Mutex A held by: Alpha
Mutex B held by: Beta
```


Trong deadlock_demo.c, hai task sử dụng chung hai mutex A và B:

task_alpha: lấy mutex A → delay 5 ms → chờ mutex B.

task_beta: lấy mutex B → delay 5 ms → chờ mutex A.

Do cả hai đều chờ vô thời hạn (portMAX_DELAY) nên không task nào có thể tiếp tục để giải phóng mutex đang giữ.

## 3. Sửa lỗi
Mô tả thay đổi bạn làm để hết deadlock (thứ tự lấy mutex thống nhất). Xác
nhận chạy ổn định sau khi sửa (thời gian chạy thử).

```Stats          	1666		1%
IDLE           	0		<1%
Alpha          	1360		1%
Tmr Svc        	2228		2%
Beta           	2420		2%

Stats: Running
IDLE: Ready
Alpha: Blocked
Beta: Blocked
Tmr Svc: Blocked
Mutex A held by: NONE
Mutex B held by: NONE
Stats          	3365943		8%
IDLE           	35804899		90%
Alpha          	84240		<1%
Beta           	129254		<1%
Tmr Svc        	2228		<1%

Stats: Running
IDLE: Ready
Alpha: Blocked
Beta: Blocked
Tmr Svc: Blocked
Mutex A held by: NONE
Mutex B held by: NONE
Stats          	6901295		8%
IDLE           	71584073		90%
Alpha          	168446		<1%
Beta           	258522		<1%
Tmr Svc        	2228		<1%

Stats: Running
IDLE: Ready
Alpha: Blocked
Beta: Blocked
Tmr Svc: Blocked
Mutex A held by: NONE
Mutex B held by: NONE
Stats          	10449871		8%
IDLE           	107350023		90%
Alpha          	252652		<1%
Beta           	387790		<1%
Tmr Svc        	2228		<1%

Stats: Running
IDLE: Ready
Alpha: Blocked
Beta: Blocked
Tmr Svc: Blocked
Mutex A held by: NONE
Mutex B held by: NONE
Stats          	14022986		8%
IDLE           	143091434		90%
Alpha          	336858		<1%
Beta           	517058		<1%
Tmr Svc        	2228		<1%

Stats: Running
IDLE: Ready
Alpha: Blocked
Beta: Blocked
Tmr Svc: Blocked
Mutex A held by: NONE
Mutex B held by: NONE
Stats          	17603093		8%
IDLE           	178825853		90%
Alpha          	421064		<1%
Beta           	646326		<1%
Tmr Svc        	2228		<1%

Stats: Running
IDLE: Ready
Alpha: Blocked
Beta: Blocked
Tmr Svc: Blocked
Mutex A held by: Beta
Mutex B held by: NONE
Stats          	21176459		8%
IDLE           	214567013		90%
Alpha          	505270		<1%
Beta           	775594		<1%
Tmr Svc        	2228		<1%

Stats: Running
IDLE: Ready
Alpha: Blocked
Beta: Blocked
Tmr Svc: Blocked
Mutex A held by: Beta
Mutex B held by: NONE
Stats          	24749331		8%
IDLE           	250308667		90%
Alpha          	589476		<1%
Beta           	904862		<1%
Tmr Svc        	2228		<1%

Stats: Running
IDLE: Ready
Alpha: Blocked
Beta: Blocked
Tmr Svc: Blocked
Mutex A held by: Beta
Mutex B held by: NONE
```

Đã thay đổi thứ tự lấy mutex của task_beta từ B → A thành A → B, đồng nhất với task_alpha. Cả hai task giải phóng mutex theo thứ tự ngược lại B → A.

Sau khi nạp lại chương trình, bộ đếm run-time của Alpha tăng từ 84.240 lên 589.476, của Beta tăng từ 129.254 lên 904.862. Điều này chứng tỏ hai task tiếp tục được thực thi, không còn bị kẹt vĩnh viễn như trước.

Mặc dù các task vẫn xuất hiện ở trạng thái Blocked, nguyên nhân là chúng đang chờ vTaskDelay() hoặc chờ mutex tạm thời, không phải deadlock.

Kết quả quan sát cũng cho thấy Mutex A và B có thể được giải phóng hoặc được một task sở hữu tạm thời, thay vì bị hai task giữ đồng thời vô thời hạn như trước khi sửa.

## 4. Watchdog
Phép tính reload IWDG. Log/video board tự reset khi deadlock được tái tạo lại
có chủ đích, kèm thời gian từ lúc treo đến lúc reset — so với `IWDG_TIMEOUT_MS`.

IWDG được cấu hình với IWDG_TIMEOUT_MS = 1000 ms, prescaler = 64 và tần số LSI giả định là 40 kHz.

Giá trị reload: (1000* 40000)/(64 * 1000) - 1 = 624


Thời gian timeout lý thuyết:(624+1)*64/40000=1000 ( ms )

Kết quả thực nghiệm:

Khi cố tình tái tạo deadlock bằng cách cho Alpha lấy mutex A → B và Beta lấy mutex B → A, log UART ghi nhận:

```
Alpha: Blocked
Beta: Blocked
Mutex A held by: Alpha
Mutex B held by: Beta

DEADLOCK TIME: 745 ms
DEADLOCK TIME: 769 ms
DEADLOCK TIME: 793 ms
DEADLOCK TIME: 817 ms
DEADLOCK TIME: 841 ms
DEADLOCK TIME: 865 ms
DEADLOCK TIME: 889 ms
DEADLOCK TIME: 913 ms
DEADLOCK TIME: 937 ms

=== BOOT ===
```

Giá trị cuối cùng ghi nhận trước khi board khởi động lại là 937 ms kể từ thời điểm phát hiện cả hai task bắt đầu chờ mutex thứ hai. Như vậy, thời gian từ deadlock đến reset lớn hơn hoặc bằng 937 ms, ước lượng xấp xỉ 1 giây.

Kết quả phù hợp với timeout IWDG được cấu hình là 1000 ms. Sai lệch có thể do thời điểm deadlock xảy ra so với lần refresh watchdog cuối, sai số LSI và khoảng thời gian giữa các lần xuất UART.

Hiện tượng === BOOT === xuất hiện lặp lại cho thấy board liên tục khởi động lại khi deadlock được tái tạo. Để xác nhận riêng nguyên nhân reset là IWDG, có thể kiểm tra thêm cờ RCC_FLAG_IWDGRST.


## 5. Lệch so với dự đoán
Mỗi mục lệch + nguyên nhân.

## 6. Câu hỏi đánh đổi
Với `SUPERVISOR_BITS` và chu kỳ check-in của bạn, nếu một task thật sự chạy
chậm (không deadlock, chỉ chậm) trong đúng 1 chu kỳ, supervisor có false-trip
(reset oan) không? Trả lời bằng số đo thời gian thực tế của chu kỳ check-in.

Nếu Stats chậm thêm một chu kỳ 500 ms, khoảng cách giữa hai lần check-in có thể tăng lên khoảng 1000 ms hoặc hơn. Khoảng này vượt timeout chờ 750 ms, khiến supervisor không nhận đủ ba bit dù task không thật sự deadlock. Supervisor sẽ ngừng refresh IWDG, dẫn đến nguy cơ reset oan.

Ngược lại, nếu Alpha hoặc Beta chỉ chậm thêm một chu kỳ khoảng 25 ms, khoảng trễ vẫn thường nhỏ hơn nhiều so với 750 ms nên ít có nguy cơ false-trip hơn.

Đánh đổi: Timeout ngắn giúp phát hiện deadlock nhanh nhưng dễ reset oan khi task xử lý chậm. Timeout dài giảm false-trip nhưng làm hệ thống phát hiện lỗi chậm hơn.
