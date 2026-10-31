## Bảng kết quả đo

### 1. So sánh RAM và thời gian đánh thức

| Thông số | Event Group (4 producer) | Event Group + Notification |
|---|---:|---:|
| Heap trước khi tạo Event Group | 12280 bytes | 12280 bytes |
| Heap sau khi tạo Event Group | 12240 bytes | 12240 bytes |
| RAM Event Group | 40 bytes | 40 bytes |
| RAM Notification cấp phát thêm | Không sử dụng | 0 bytes |
| Tổng RAM object đồng bộ | 40 bytes | 40 bytes |
| Thời gian đánh thức trung bình | 1075 cycles | 867 cycles |
| Thời gian đánh thức trung bình | 14.93 µs | 12.04 µs |
| Số mẫu latency | 20 | 7 |
| Chu kỳ đồng bộ trung bình | 5.70 ms | 5.70 ms |
| Số mẫu đồng bộ | 20 | 20 |


### 2. Kết quả timeout

| Thông số | Giá trị |
|---|---|
| SYNC_TIMEOUT_MS | 120 ms |
| Thời điểm bắt đầu ngừng producer_3 | Sau 2 giây |
| Giá trị trả về | 0x7 |
| Giá trị yêu cầu | 0xF |
| Bit bị thiếu | Bit 3 |

Log UART:

```text
===== EX4 TIMEOUT =====
TIMEOUT_MS=120
RETURN_BITS=0x7
EXPECTED_BITS=0xF
=======================
```

### 3. So sánh chu kỳ đồng bộ khi tăng producer

| Cấu hình | Số mẫu | Chu kỳ trung bình |
|---|---:|---:|
| 4 producer | 20 | 5.70 ms |
| 5 producer | 20 | 6.20 ms |
