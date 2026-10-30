# Report — Week 2 / Ex2

`student_id`: 2511006    Commit của PREDICTION.md: ______

## 1. Lỗi tái hiện (UART_GUARD_MODE = 0)
Log UART dán nguyên văn, đánh dấu vị trí ký tự lẫn lộn:

```
WWWW0132SSS EEESQQQQ00==0000000002002822 B 8ABD ABDCABDCABDCABDCABDCABDCABDCABDCADCABDCABDCABDCA




WWWW1203SSS EEESQQQQ00==0000000002002922 C 9BCA BCADBCADBCADBCADBCADBCADBCADBCADBCADBCADBCADB

D


WWWW2013SSS EEESQQQQ00==0000000003003033 A 0CAB CABDCABDCABDCABDCABDCABDCABDCABDCBDCABDCABDCABDC




WWWW0321SSS EEESQQQQ00==0000000003003133 D 1ADC ADCBADCBADCBADCBADCBADCBADCBADCBADCBADCBADCBA

B


WWWW3201SSS EEESQQQQ00==0000000003003233 C 2DCA DCABDCABDCABDCABDCABDCABDCABDCABDABDCABDCABDCABD




WWWW2130SSS EEESQQQQ00==0000000003003333 B 3CBD CBDACBDACBDACBDACBDACBDACBDACBDACBDACBDACBDAC

A


WWWW1320SSS EEESQQQQ00==0000000003003433 D 4BDC BDCABDCABDCABDCABDCABDCABDCABDCABCABDCABDCABDCAB

```

Tái hiện lại bao nhiêu lần / bao nhiêu lần thấy lỗi: ______ / ______.

## 2. Bảng so sánh 3 cách
| Cách | Độ trễ in trung bình (us) | Độ trễ in max (us) | Ảnh hưởng task khác | RAM thêm (bytes) |
|---|---|---|---|---|
| Mutex | 10002|12576 | writer_task chờ mutex, còn các task khác chạy bình thường|80 |
| Critical section |7766 | 12418|Chặn chuyển task và trì hoãn ngắt |0 |
| Gatekeeper |3096 |3106 | gatekeeper có ưu tiên cao hơn nên chiếm quyền các task khác|1464 |

Giải thích kết quả: 

Mutex có latency trung bình cao nhất (10002 µs) do nhiều writer phải chờ sử dụng UART. Các writer cùng priority 2 và taskYIELD() được gọi giữa các byte truyền, làm tăng khả năng chuyển ngữ cảnh.

Critical section có latency trung bình thấp hơn Mutex (7766 µs) nhưng gây tác động mạnh đến khả năng đáp ứng ngắt và scheduler.

Gatekeeper có latency trung bình thấp nhất trong cấu hình thử nghiệm (3096 µs). Gatekeeper có priority 3, cao hơn writer (priority 2), nên thường được scheduler chạy ngay sau khi Queue có dữ liệu. Khi Gatekeeper đang truyền UART, taskYIELD() không buộc CPU chuyển sang writer có ưu tiên thấp hơn. Vì vậy thời gian đo gần với thời gian truyền một message.



Cách đo độ trễ in (mô tả ngắn, kèm ảnh logic analyzer nếu dùng pin đánh dấu):

Sử dụng bộ đếm chu kỳ DWT->CYCCNT

Latency được đo từ lúc writer yêu cầu gửi đến khi quá trình truyền UART hoàn tất.

Mutex: Đọc DWT trước xSemaphoreTake(), đọc lần nữa sau msg_send(). Kết quả bao gồm thời gian chờ Mutex và truyền UART.

Critical section: Đọc DWT trước taskENTER_CRITICAL() và sau taskEXIT_CRITICAL(). Kết quả bao gồm thời gian chờ được chạy và thời gian truyền UART.

Gatekeeper: Writer lưu thời điểm bắt đầu trước khi gửi Queue. Gatekeeper lấy message, truyền UART rồi đọc thời điểm kết thúc. Kết quả bao gồm thời gian chờ Queue và truyền UART.

Đo tổng cộng 100 message, tính latency trung bình và lớn nhất, sau đó mới in báo cáo để hạn chế ảnh hưởng của việc in thống kê lên phép đo.
## 3. Vì sao critical section nguy hiểm nhất
Giải thích bằng số đo của bạn: đoạn in dài bao nhiêu us khi ở trong critical
section, và điều đó chặn ngắt nào trong hệ thống bao lâu.

Ở MODE 2, thời gian lớn nhất đo được trong Critical Section là 3058 µs (~3,06 ms) khi truyền 32 byte qua UART.
Trong khoảng thời gian này, FreeRTOS không thể chuyển task và các ngắt bị che, đặc biệt là SysTick, bị trì hoãn, làm ảnh hưởng đến khả năng đáp ứng thời gian thực của hệ thống. 


## 4. Lệch so với dự đoán
Thứ tự latency dự đoán: Critical Section < Mutex < Gatekeeper:

Kết quả thực tế: Gatekeeper (3096 µs) < Critical Section (7766 µs) < Mutex (10002 µs)

Nguyên nhân chính có thể giải thích bằng ưu tiên scheduling: Gatekeeper có priority 3, cao hơn các writer priority 2, nên nhận Queue và truyền UART gần như ngay lập tức. Trong khi đó Mutex có nhiều writer ngang ưu tiên tranh chấp và Critical Section tạo các khoảng không thể chuyển task.

## 5. Câu hỏi đánh đổi
Với `WRITER_COUNT` của **bạn**, nếu tăng gấp đôi thì cách nào bị ảnh hưởng độ trễ
nhiều nhất? Trả lời bằng số từ dữ liệu của bạn, rồi đo lại để kiểm chứng.


Với `WRITER_COUNT = 4`, khi tăng gấp đôi lên `WRITER_COUNT = 8`, phương pháp bị ảnh hưởng độ trễ nhiều nhất là **Mutex**.
 Khi số writer tăng, nhiều task phải tranh chấp cùng một Mutex, làm thời gian chờ UART tăng mạnh. Critical section cũng tăng độ trễ do các writer phải lần lượt truyền UART. Gatekeeper gần như không thay đổi vì task Gatekeeper có priority 3, cao hơn writer (priority 2), nên thường được chạy ngay khi Queue có dữ liệu.

Sau khi đo lại với 8 writers, Mutex có mức tăng latency trung bình lớn nhất, trong khi Critical section tăng 6053 µs và Gatekeeper giảm 6 µs. 
| Cách | AVG 4 writers (µs) | AVG 8 writers (µs) | Mức thay đổi (µs) |
|---|---:|---:|---:|
| Mutex | 10002 | 21928 |+11926 |
| Critical section | 7766 | 13819 | +6053 |
| Gatekeeper | 3096 | 3090 | -6 |
