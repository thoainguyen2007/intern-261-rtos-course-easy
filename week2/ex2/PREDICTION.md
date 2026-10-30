# Dự đoán — Week 2 / Ex2

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006   `WRITER_COUNT` = 4 task  `MSG_LEN` = 32bytes

1. Ở `UART_GUARD_MODE = 0`: dự đoán lỗi lẫn lộn sẽ xuất hiện sau khoảng bao
   nhiêu dòng log? Vì sao MSG_LEN và WRITER_PERIOD_MS của bạn ảnh hưởng đến
   con số này?
   Cố định thiết lập các task có cùng priority, preemption=1 và time_slicing=1 và baudrate=115200, cấu hình 8N1. MSG_LEN=32byte nên thời gian tối thiểu để truyền 1 msg là 32*(8+1+1)/115200=2,78 ms>1
   Time_slicing xảy ra mỗi 1ms, do đó ngay từ dòng log đầu tiên sẽ có lẫn lộn. MSG_LEN làm tgian truyền >1 thì chắc chắn sẽ bị do time_slicing. 

   GS ms đầu là task1, ms 2 là task2, ms 3 là task 3, ms4 là task 4, ms5 là task 1, ms 6 là task 2, ms 7 là task3, ms8 là task4, ms 9 là task1. tới 9,78 thì task1 vào block(ready tại 12ms), task 2 chạy từ 9,78 tới 10ms thì chuyển qua task3, task3 chạy tới 10,78 thì chuyển qua task4(t3 ready tại 13ms), task 4 chạy tới 11 thì chuyển qua task2(task 1 bị delay), task 2 chạy tới 11+(0,78-0,22)=11,56(bị block tới 14ms) thì tới task4 chạy tiếp tới 11,56+0,78-0,22=12,12(bị block tới 15ms) thì nhường lại cho task 1. Tới đây thứ tự chạy cx thay đổi từ 1,2,3,4 thành 1,3,2,4. Thì ta thấy write_period dù lớn nhỏ j cx sẽ ảnh hưởng

2. Dự đoán thứ tự **độ trễ in** (latency) từ thấp đến cao giữa mutex / critical
   section / gatekeeper. Vì sao?

Critical Section <= Mutex <= Gatekeeper.

Critical Section cho phép writer truyền toàn bộ message mà không bị scheduler chuyển sang writer khác theo tick thông thường. Còn Mutex phải thực hiện thao tác Take/Give và có thể khiến writer chờ nếu mutex đang bị task khác giữ. Gatekeeper cần thêm thao tác gửi/nhận Queue và chuyển ngữ cảnh sẽ còn chậm hơn cả mutex


   
3. Với critical section, task nào trong hệ thống có nguy cơ bị ảnh hưởng xấu
   nhất nếu đoạn in quá dài? (gợi ý: nghĩ đến ngắt, không chỉ task)

Ngắt SysTick và các task phụ thuộc vào FreeRTOS tick có nguy cơ bị ảnh hưởng nhiều nhất. Do khi một writer truyền UART trong Critical Section, các ngắt thuộc phạm vi bị che không thể thực thi đúng thời điểm.

Với UART 115200 baud, 8N1 và MSG_LEN = 32, thời gian truyền lý thuyết khoảng 2,78 ms. Nếu FreeRTOS tick là 1 ms, Critical Section có thể làm trì hoãn nhiều lần cập nhật tick, khiến cho vTaskDelay() không còn phản ánh chính xác thời gian thực và làm trễ các task cần đáp ứng đúng hạn.

4. RAM dùng thêm của gatekeeper so với mutex: dự đoán tăng hay giảm, vì sao?
Gatekeeper sử dụng nhiều RAM hơn Mutex do Gatekeeper cần thêm Queue để chứa các message và stack cùng TCB cho một task riêng.

5. Nếu `WRITER_COUNT` tăng gấp đôi, cách nào trong 3 cách bị ảnh hưởng về độ trễ
   nhiều nhất? Vì sao?

Khi WRITER_COUNT tăng từ 4 lên 8,  Gatekeeper có thể bị ảnh hưởng latency nhiều nhất do số lượng message gửi vào Queue tăng, trong khi chỉ có một Gatekeeper truyền UART nên các message có thể phải chờ lâu hơn trong Queue trước khi được truyền.

