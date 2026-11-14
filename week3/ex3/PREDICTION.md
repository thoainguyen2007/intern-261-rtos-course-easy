# Dự đoán — Week 3 / Ex3

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006

1. Thời gian truyền 1 byte ở `RX_BAUD` của bạn là bao nhiêu us? (8N1, tính cả
   start/stop bit). Trong thời gian đó `parser_task` có kịp xử lý 1 byte từ
   stream buffer không, nếu nó là task ưu tiên cao nhất hệ thống?
   8N1-->10bit. Baud rate là 57600 bit/s==>1 giây truyền được 5760 lần, mỗi lần 1byte dữ liệu==>1s được 5760 byte dữ liệu==>1 byte dữ liệu truyền trong khoảng 173.6us
2. `STREAM_BUFFER_BYTES` của bạn chứa tối đa bao nhiêu lệnh dài `MAX_CMD_LEN`
   byte trước khi đầy?
   MAX_CMD_LEN=24, STREAM_BUFFER_BYTES=64==>64/24=2. Vậy tối đa 2 lệnh dài. 
   
   Nếu chỉ hỏi tối đa bn lệnh thì 1 lệnh hợp lệ ngắn nhất là 3byte==>tối đa 64/3=21 byte
3. Nếu `xStreamBufferSendFromISR` trả về 0 (stream buffer đầy) trong
   `uart_rx_isr_handler`, byte đó sẽ bị mất. Theo bạn, trường hợp nào thực tế
   khiến điều này xảy ra (gửi quá nhanh / `parser_task` ưu tiên quá thấp /
   cái khác)?
   Có 2 lí do chính là gửi quá nhanh hoặc parser xử lý quá chậm. Trường hợp gửi quá nhanh thì khá là khó do ko thể nào con người nhập được 5720 ký tự trong 1s. Còn parser xử lý chậm có thể là do đặt priority thấp hơn demo task.
4. Dự đoán: gửi lệnh bằng gõ tay bình thường có mất byte không? Gửi bằng
   script gửi liên tục không nghỉ thì sao?
   Gõ tay bth cực kì khó, thậm chí là không thể mất byte. Nhưng gửi bằng script với tốc dộ cao thì có thế mất.
