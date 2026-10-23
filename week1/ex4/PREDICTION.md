# Dự đoán — Week 1 / Ex4

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006    `HEAP_SIZE_BYTES` = 9216    `DYN_TASK_STACK_WORDS` = 160
`STATIC_TASK_STACK_WORDS` = 128   `HOG_DEPTH_BYTES` = 640    `TARGET_FREE_HEAP_BYTES` = 2048

1. Ngân sách RAM dự đoán (tổng phải = 20480 bytes):

| Mục | Bytes dự đoán |
|---|---|
| `.data` |128 |
| `.bss` (không kể heap FreeRTOS) |1800 |
| Heap FreeRTOS (`ucHeap`) |9216 |
| Stack của main (MSP) |1024 |
| Còn lại / chưa dùng | 8312|
| **Tổng** | **20480** |

2. `xPortGetFreeHeapSize()` ngay sau khi scheduler chạy: 7352 bytes.
   `HEAP_SIZE_BYTES` − số này = 1864 . Số đó là của những gì? của dyn_task và ram_report_task.
3. High water mark dự đoán (words): dyn 115 , stat 83 , idle 105 , timer 95 .
4. `dyn_task` có stack 160 words = 640 bytes. Mảng `HOG_DEPTH_BYTES` = 640 bytes.
   Hook tràn stack **có** chạy không? Vì sao?
   Có tràn do kích thước mảng bằng luôn kích thước stack, stack chắc chắn còn phải có thêm dung lượng để lưu context nữa. Vì vậy chắc chắn tràn

5. Heap trong `.map` và `xPortGetFreeHeapSize()` sẽ lệch nhau. Lệch vì:
Lệch vì heap trong .map là heap cấp phát ban đầu, còn xPortGetFreeHeapSize() đo lượng heap còn trống nên không thể nào bằng nhau do heap đã cấp bộ nhớ cho các task rồi.

6. Stack nhỏ nhất an toàn của `dyn_task` dự đoán: 60 words. Dựa vào đâu?
