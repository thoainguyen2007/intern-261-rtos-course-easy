# Report — Week 1 / Ex4

`student_id`: 2511006    Commit của PREDICTION.md: 

## 1. Ngân sách RAM từ `.map`
| Mục | Symbol / section trong `.map` | Bytes |
|---|---|---|
| `.data` |.data | 0x20=96|
| `.bss` (không kể heap) |.bss |0x2ebc-0x2400=0xabc=2748 |
| Heap FreeRTOS (`ucHeap`) |.bss.ucHeap |0x2400=9216 |
| Stack của main (MSP) |._user_heap_stack |0x400=1024 |
| Còn lại / chưa dùng | — |7908 |
| **Tổng** | | **20480** |

`.map` commit tại: `______`

## 2. Số lúc chạy (dán nguyên văn log UART)
```
================ FREE RTOS RAM PICTURE ================
Free Heap          : 7344 bytes
Min Ever Free Heap : 7344 bytes
HWM dyn_task       : 135 words ( 540 bytes)
HWM stat_task      : 104 words ( 416 bytes)
HWM report_task    : 108 words ( 432 bytes)
HWM idle_task      : 105 words ( 420 bytes)
HWM timer_task     :  85 words ( 340 bytes)
```

| Đại lượng | Words | Bytes |
|---|---|---|
| `xPortGetFreeHeapSize()` | — | 7344|
| `xPortGetMinimumEverFreeHeapSize()` | — | 7344|
| HWM `dyn_task` |135 | 540|
| HWM `stat_task` |104 |416 |
| HWM `ram_report_task` |108 | 432|
| HWM idle task | 105|420 |
| HWM timer task |85 |340 |

Heap còn trống >= `TARGET_FREE_HEAP_BYTES` (2048)

## 3. Đối chiếu `.map` vs runtime
Heap trong `.map` = 9216 ; `xPortGetFreeHeapSize()` = 7344 ; lệch = 1872 bytes.
Lệch này gồm những gì (liệt kê từng mục kèm số bytes):
+ dyn_task: 740 bytes gồm Stack(640 bytes) , TCB(84 bytes) ,Block Header overhead (heap_4 gồm 2 khối cho TCB và Stack): 2*8=16 bytes
+ ram_report_task:  1124 bytes gồm  Stack (1024 bytes)   TCB: (84 bytes) Block Header overhead (heap_4 gồm 2 khối cho TCB và Stack): 2*8=16 bytes
+ Alignment / Padding nội bộ của các khối heap: 8 bytes.


## 4. Dynamic vs static
HWM của `dyn_task` và `stat_task` lệch 31 words dù làm cùng một việc. Vì sao?
Ban đầu dyn_task là 160, còn stat_task là 128 (tức là ban đầu vốn đã lệch 32word). Việc lệch thêm 1 word là do data alignment.
`xTaskCreate` và `xTaskCreateStatic` khác nhau ở đâu về **nơi** TCB và stack nằm?
`xTaskCreate`: lưu vào vùng nhớ heap của FreeRTOS (tức là mảng ucHeap nằm trong phân vùng .bss của RAM) `xTaskCreateStatic`:  lưu thẳng vào .bss

## 5. Tràn stack
Log/ảnh chứng minh hook đã chạy:
```
[HOOK] PHAT HIEN TRAN STACK: tran_stack
```
Tại sao trong `vApplicationStackOverflowHook` không được gọi hàm UART blocking
(hoặc: nếu bạn vẫn gọi được, giải thích tại sao nó không crash — và tại sao đó là may):

## 6. Stack nhỏ nhất an toàn
| Lần thử | Stack (words) | HWM (words) | Kết quả |
|---|---|---|---|
| 1| 30| 5| Chưa tràn|
| 2| 25| | Tràn|
|3| 29| 5| Chưa tràn|
|4| 28| |Tràn|


Giá trị cuối cùng chọn: 29 words. Biên an toàn: 29words. Lý do chọn biên đó:
Lúc đầu cấp 160 word và còn trống 135 word, tức là dùng 25 work. Nhưng do #define configCHECK_FOR_STACK_OVERFLOW=2 nên FreeRTOS kiểm tra thêm 4byte sau con trỏ SP, tức là ở word thứ 29. Do đó nếu chỉ cấp 28 word thì word thứ 29 là của task khác và bị ghi đè lên nên FreeRTOS báo hook.

## 7. Lệch so với dự đoán


## 8. Câu hỏi đánh đổi
Với `HEAP_SIZE_BYTES` = 9216 của bạn: nếu phải thêm một task nữa có stack 128 words,
**lấy RAM từ đâu**? Dựa vào bảng ngân sách của bạn, đưa ra 2 phương án kèm chi phí
cụ thể (bytes), rồi chọn một và **thực hiện, đo lại để chứng minh**.
Giữ nguyên như bình thường do heap còn như rất nhiều(7344 bytes =1836words)
