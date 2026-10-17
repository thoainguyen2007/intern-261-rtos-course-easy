# Report — Week 1 / Ex1

`student_id`: ______    Commit của PREDICTION.md: ______

## 1. Số đo
| Đại lượng | Giá trị |
|---|---|
| `BLINK_PERIOD_MS` (từ params.h) | 127 |
| Chu kỳ đo được — min | 126|
| Chu kỳ đo được — max | 126|
| Chu kỳ đo được — trung bình | 126|
| Sai số trung bình vs `BLINK_PERIOD_MS` | 1|
| `uxTaskGetStackHighWaterMark()` của bringup_task | words |

Ảnh PulseView (>= 10 chu kỳ, đọc được giá trị): `![](do_ex1.png)`

## 2. Ba handler của kernel
| Tên trong port ARM_CM3 | Tên trong vector table | Nhiệm vụ |
|---|---|---|
| `vPortSVCHandler` | SVC_Handler|Khởi chạy task đầu tiên bằng lệnh Supervisor Call |
| `xPortPendSVHandler` |PendSV_Handler | Thực hiện chuyển ngữ cảnh (Context Switching) lưu và nạp lại thanh ghi của task ở mức ngắt thấp nhất.|
| `xPortSysTickHandler` | SysTick_Handler|Cung cấp nhịp tick cho kernel (`xTaskIncrementTick`), kiểm tra unblock task trễ hạn và kích hoạt PendSV. |

Log lỗi link trước khi sửa (dán nguyên văn):

```
multiple definition of `SVC_Handler'; CMakeFiles/template.dir/Core/Src/stm32f1xx_it.c.obj:D:/RTOS/intern-261-rtos-course-easy/template/Core/Src/stm32f1xx_it.c:146: first defined here
multiple definition of `PendSV_Handler'; CMakeFiles/template.dir/Core/Src/stm32f1xx_it.c.obj:D:/RTOS/intern-261-rtos-course-easy/template/Core/Src/stm32f1xx_it.c:172: first defined here
multiple definition of `SysTick_Handler'; CMakeFiles/template.dir/Core/Src/stm32f1xx_it.c.obj:D:/RTOS/intern-261-rtos-course-easy/template/Core/Src/stm32f1xx_it.c:185: first defined here
collect2.exe: error: ld returned 1 exit status
```

Cách sửa: xóa 3 hàm đó trong file stm32f1xx_it.c

## 3. Tại sao HAL timebase phải sang TIM4
Phải trả lời được: `HAL_IncTick()` chạy ở đâu, `configKERNEL_INTERRUPT_PRIORITY`
ảnh hưởng gì đến nó, và `HAL_Delay()` / `vTaskDelay()` khác nhau chỗ nào.
HAL_IncTick() chạy trong ngắt phần cứng của TIM4 thay vì ngắt SysTick mặc định.

Ảnh hưởng của configKERNEL_INTERRUPT_PRIORITY: FreeRTOS gán SysTick mức ưu tiên ngắt thấp nhất (15). Nếu HAL dùng chung SysTick, khi CPU rơi vào một ngắt ngoại vi khác (priority 0–14) mà gọi hàm chờ của HAL, SysTick bị chặn $\rightarrow$ HAL_IncTick() không tăng $\rightarrow$ hệ thống bị treo . Chuyển sang TIM4 (priority 0) giúp biến tick của HAL cập nhật độc lập và không bao giờ bị nghẽn.

Khác nhau giữa HAL_Delay() và vTaskDelay():
HAL_Delay(): Busy-waiting (chiếm CPU), chạy vòng lặp đọc biến tick làm nghẽn toàn bộ các task khác.vTaskDelay(): Non-blocking (nhường CPU), đưa task vào trạng thái Blocked để scheduler chuyển quyền cho task khác chạy

## 4. Lệch so với dự đoán
Với mỗi mục trong `PREDICTION.md` lệch so đo: lệch bao nhiêu, và **nguyên nhân vật lý**
(không được trả lời "do sai số").
Lệch 1 ms do hàm vTaskDelay((BLINK_PERIOD_MS * configTICK_RATE_HZ) / 2000), đối số nhập vào là 127*1000/2000=63 nên khi nhân 2 lên lại chỉ có 126ms.
## 5. Câu hỏi đánh đổi
`configTICK_RATE_HZ` = 1000 trong bài này. Dựa vào số đo của **bạn**, nếu hạ xuống
100 Hz thì chu kỳ LED và sai số thay đổi thế nào? Trả lời bằng con số, không bằng lý thuyết.
Chu kỳ đo được còn 120ms, tức là sai số 7ms. Vấn đề cũng tương tự do hàm do hàm vTaskDelay((BLINK_PERIOD_MS * configTICK_RATE_HZ) / 2000), đối số nhập vào là 127\*100/2000=6. Tức là delay bằng 6 tick nên cả chu kỳ là 6\*2\=12 tick. `configTICK_RATE_HZ` = 100 tức là 100 tick mỗi giây tức là mỗi tick là 10ms. Một chu kỳ là 12 tick, mỗi tick 10ms nên 1 chu kỳ là 120ms.
