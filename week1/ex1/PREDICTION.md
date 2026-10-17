# Dự đoán — Week 1 / Ex1

> **Commit file này TRƯỚC khi nạp chương trình lần đầu.** Report sẽ phải giải thích
> mọi chỗ dự đoán lệch so đo. Dự đoán sai không bị trừ điểm; dự đoán sau khi đo thì bị.

`student_id`: 2511006    Ngày/giờ commit: 9:50 ngày 17/10

1. Chu kỳ LED tôi sẽ đo được: 126 ms (`BLINK_PERIOD_MS` = 127)
   Do hàm delay là vTaskDelay((BLINK_PERIOD_MS * configTICK_RATE_HZ) / 2000), đối số nhập vào là 127*1000/2000=63 nên khi nhân 2 lên lại chỉ có 126ms.
2. Sai số giữa chu kỳ đo được và `BLINK_PERIOD_MS`: ~1000us. Vì sao? Từ kết quả câu 1 ta thấy sai số là 1ms, cộng thêm độ trễ của hệ thống nên sẽ xấp xỉ 1000us.
3. Nếu để HAL timebase ở lại SysTick, điều gì sẽ xảy ra khi `vTaskDelay()` được gọi?
   (chọn và giải thích: chạy bình thường / treo / tick sai nhịp / hard fault)
   Trong bài này do không xài ngắt khác nên sẽ không bị treo. 
   Giải thích nếu bị treo: bị treo do khi này RTos sẽ dùng SysTick và ngắt của nó có mức ưu tiên thấp nhất là 15(trong khi Systick khi dùng cho HAL bình thường có mức ưu tiên cao nhất là 0). Khi này nếu như có 1 ngắt khác thì sẽ chặn SysTick và làm biến đếm uwTick ngừng tăng. Và nếu như khi này tiếp tục gọi hàm HAL_Delay() hoặc các hàm kiểm tra thời gian thì sau khi nhảy vào hàm này sẽ không thể thoát ra do uwTick đứng yên. 

4. `BRINGUP_STACK_WORDS` = 128 words = 512 bytes. Task này có đủ không? Vì sao?
   Đủ vì task đơn giản, chỉ nháy LED nên sẽ không bị tràn. 

5. Trước khi map handler, linker sẽ báo lỗi gì (đoạn nguyên văn thông báo):
multiple definition of `SVC_Handler'; CMakeFiles/template.dir/Core/Src/stm32f1xx_it.c.obj:D:/RTOS/intern-261-rtos-course-easy/template/Core/Src/stm32f1xx_it.c:146: first defined here
multiple definition of `PendSV_Handler'; CMakeFiles/template.dir/Core/Src/stm32f1xx_it.c.obj:D:/RTOS/intern-261-rtos-course-easy/template/Core/Src/stm32f1xx_it.c:172: first defined here
multiple definition of `SysTick_Handler'; CMakeFiles/template.dir/Core/Src/stm32f1xx_it.c.obj:D:/RTOS/intern-261-rtos-course-easy/template/Core/Src/stm32f1xx_it.c:185: first defined here
collect2.exe: error: ld returned 1 exit status

