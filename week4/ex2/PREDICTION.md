# Dự đoán — Week 4 / Ex2

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006

1. Lỗi gọi API queue không phải `FromISR` bên trong ISR sẽ gây ra hiện
   tượng gì khi chạy (configASSERT? treo? reset?). Giải thích bằng cơ chế
   (ngăn xếp ISR, gọi `vTaskSuspendAll`/scheduler từ ngữ cảnh ngắt), không
   chỉ "nó bị lỗi".

   Bị treo do lỗi congifASSERT.  Nguyên nhân là xQueueSend() dành cho task context (sử dụng PSP), trong khi ISR sử dụng MSP. Bên trong xQueueSend() có sử dụng critical section và các cơ chế quản lý scheduler dành cho task.

   Nếu không có configASSERT, việc vào và thoát critical section sai ngữ cảnh có thể làm mất trạng thái che ngắt (BASEPRI) ban đầu, khiến dữ liệu đang được bảo vệ bị truy cập giữa chừng.

   Ngoài ra, nếu thời gian chờ khác 0, API có thể gọi vTaskSuspendAll() và đưa task đang bị ngắt vào trạng thái Blocked.

2. Sau khi sửa đúng API `FromISR`, dự đoán số mẫu ghi UART mỗi giây =
   `SAMPLE_RATE_HZ` / `LOG_BLOCK_SAMPLES` khối/giây. Tính ra con số.
   Số mẫu mỗi giây=`SAMPLE_RATE_HZ` / `LOG_BLOCK_SAMPLES`=400/32=12,5 khối /giây
3. Với `UART_BAUD` của bạn, một khối `LOG_BLOCK_SAMPLES` mẫu mất bao lâu để
   truyền xong? Có nguy cơ UART chưa gửi xong khối cũ mà khối mới đã sẵn
   sàng không?

   Do dùng 8N1 nên mỗi lần truyền được 8 bit data, 1 bit start và 1 bit stop

   1 lần truyền tổng cộng 10 bit, baud rate là 460800 bit/s

   ==>1 giây truyền được 460800/10=46080 lần

   Mỗi lần truyền được 8 bit data==>1 giây truyền được 46080*8=3680640 bit data

   Một khối 32 mẫu tức là 32*16=512 byte data=4096 bit data ==> 1 giây truyền được 3680640/4096= 90 khối
   
   ==>1 khối truyền trong 1/90~0,011s~11ms 

   Trong khi đó thời gian tạo ra 1 khối là 32/400=0,08s=80ms. Như vậy sẽ khó có thể có nguy cơ tràn. 
