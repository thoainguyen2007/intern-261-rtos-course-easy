# Report — Week 3 / Ex3

`student_id`: ______    Commit của PREDICTION.md: ______

## 1. Thời gian byte & độ sâu stream buffer
Phép tính thời gian 1 byte ở `RX_BAUD`, và số lệnh tối đa chứa được trong
`STREAM_BUFFER_BYTES`.

   8N1-->10bit. Baud rate là 57600 bit/s==>1 giây truyền được 5760 lần, mỗi lần 1byte dữ liệu==>1s được 5760 byte dữ liệu==>1 byte dữ liệu truyền trong khoảng 173.6us
==>Công thức là 10/baudrate (s)

    1 lệnh hợp lệ ngắn nhất là 3byte==>tối đa [64/3]=21 byte

## 2. Demo 3 loại lệnh
Bằng chứng (log/ảnh) `demo_task` phản ứng đúng với `P`, `S`, `T` — mỗi loại ít
nhất 1 ví dụ có số liệu trước/sau.

Lệnh P: Ban đầu demo_task có chu kỳ 200 ms. Sau khi gửi P1000\r, chu kỳ được thay đổi thành 1000 ms và LED nhấp nháy chậm hơn.

Lệnh S: Gửi S1\r để thay đổi priority của demo_task từ 3 xuống 1. Việc thay đổi priority ảnh hưởng đến thứ tự scheduling, không trực tiếp thay đổi chu kỳ GPIO. Chưa có log xác nhận giá trị priority thực tế.

Lệnh T: Gửi T0\r làm demo_task bị suspend, LED dừng nhấp nháy. Gửi T1\r để resume, LED tiếp tục hoạt động.

## 3. Test gửi nhanh
| | Số lệnh gửi | Số lệnh nhận đúng | Mất/lỗi |
|---|---|---|---|
| Gõ tay bình thường | | | |
| Gửi liên tục (script) | | | |

Nếu có mất byte: giải thích bằng cơ chế (stream buffer đầy, parser bị trễ,
hay nguyên nhân khác) — không chỉ nói "bị mất".
Mất do stream buffer bị đầy do parser ko xử lý kịp so với tốc độ gửi byte của uart.

## 4. Vì sao stream buffer, không phải queue
Stream Buffer phù hợp vì UART nhận dữ liệu liên tục từng byte và các lệnh có độ dài khác nhau. ISR đưa byte vào buffer, còn parser_task đọc và ghép thành lệnh khi gặp \r.

Queue phù hợp hơn để truyền các phần tử có kích thước cố định, trong khi Stream Buffer thuận tiện và ít overhead hơn cho luồng byte UART.

## 5. Lệch so với dự đoán
Mỗi mục lệch + nguyên nhân.

## 6. Câu hỏi đánh đổi
Với `RX_BAUD` của bạn, nếu tăng gấp đôi `STREAM_BUFFER_BYTES`, tốc độ gửi tối
đa không mất byte tăng thêm bao nhiêu (đo thật, không suy luận)?

Tăng buffer giúp giảm mất dữ liệu khi parser bị trễ. Tuy nhiên, tốc độ truyền vật lý vẫn là 57600 baud, tương đương tối đa 5760 byte/s với UART 8N1.
