# Report — Week 2 / Ex4

`student_id`: 2511006    Commit của PREDICTION.md: ______

## 1. Bảng so sánh RAM
| | Bytes thực tế | Cách tính |
|---|---|---|
| Event group (3 điểm đồng bộ) | 40 bytes | 12280 - 12240 |
| Event group (2 điểm) + notification (1 điểm) | 40 bytes | 40 + 0 |
| Chênh lệch | 0 bytes | 40 - 40 |

## 2. Bảng so sánh tốc độ đánh thức
| Cách | Thời gian đánh thức (cycle) | Thời gian đánh thức (us) |
|---|---|---|
| Event-group-bit (producer_1 hoặc 2) | 1075 | 14.93 |
| Task notification (producer_0) | 867 | 12.04 |

Mô tả cách đo (2 điểm DWT: lúc gọi set/notify, lúc sync_task bắt đầu chạy):

Dùng DWT Cycle Counter, ghi t0 trước khi producer phát tín hiệu và t1 khi sync_task tiếp tục chạy. Lấy t1 - t0 để tính cycle, sau đó chia 72 để đổi sang µs.


## 3. Khi nào KHÔNG dùng được notification
Tình huống bạn dự đoán ở PREDICTION.md, kiểm chứng lại: đúng hay sai, vì sao.

## 4. Timeout
Log/ảnh một lần `sync_task` bị timeout (cố tình làm 1 producer chậm hơn
`SYNC_TIMEOUT_MS`), và giá trị trả về của `xEventGroupWaitBits` lúc đó.

```
===== EX4 TIMEOUT =====
TIMEOUT_MS=120
RETURN_BITS=0x7
EXPECTED_BITS=0xF
=======================
```

Sau 2 giây, producer_3 ngừng set bit. Giá trị trả về là 0x7 (0111), thiếu bit 3 so với 0xF (1111), nên sync_task bị timeout sau 120 ms.
## 5. Lệch so với dự đoán

| Nội dung | Dự đoán | Thực tế | Nguyên nhân |
|---|---|---|---|
| RAM Event Group | 28 bytes | 40 bytes | Heap_4 có thêm overhead và căn chỉnh bộ nhớ |
| RAM Notification | 0 bytes thêm | 0 bytes thêm | Notification nằm sẵn trong TCB |
| Tốc độ đánh thức | Notification nhanh hơn | 12.04 µs so với 14.93 µs | Phù hợp dự đoán, nhưng số mẫu và cách đo khác nhau |
| Timeout | Trả về các bit đã set | 0x7 | Producer_3 không set bit kịp |

## 6. Câu hỏi đánh đổi
Với `EVENT_BIT_COUNT` của **bạn**, nếu tăng thêm 1 bit (1 producer nữa) thì chu
kỳ đồng bộ trung bình đo được thay đổi thế nào? Trả lời bằng số từ dữ liệu của
bạn, rồi đo lại để kiểm chứng.

| Số producer | Chu kỳ đồng bộ trung bình |
|---|---|
| 4 producer (20 mẫu) | 5.70 ms |
| 5 producer (20 mẫu, lần 1) | 6.20 ms |
| 5 producer (20 mẫu, lần 2) | 5.85 ms |
| 5 producer (20 mẫu, lần 3) | 6.45 ms |
| 5 producer (20 mẫu, lần 4) | 5.65 ms |
| 5 producer (trung bình 80 mẫu) | 6.04 ms |

Khi tăng từ 4 lên 5 producer, chu kỳ đồng bộ trung bình tăng khoảng 0.34 ms (6%). Vì phải chờ thêm một sự kiện nên thời gian có xu hướng tăng. Kết quả dao động do các producer có chu kỳ ngẫu nhiên.



