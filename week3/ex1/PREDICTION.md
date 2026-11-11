# Dự đoán — Week 3 / Ex1

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006

1. Với `ISR_NVIC_PRIO` của bạn và nhóm priority `NVIC_PRIORITYGROUP_4`, số này
   nằm trong khoảng cho phép (`configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` ..
   `configLIBRARY_LOWEST_INTERRUPT_PRIORITY`) không? Trình bày phép so sánh.

   Với `NVIC_PRIORITYGROUP_4` thì có 16 mức ưu tiên từ 0-15.
   `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY`=5.
   `configLIBRARY_LOWEST_INTERRUPT_PRIORITY`=15.
   `ISR_NVIC_PRIO`=8.
   Vậy `ISR_NVIC_PRIO` nằm trong khoảng cho phép (5<8<15).
2. Khi đặt priority EXTI cao hơn ngưỡng cho phép rồi gọi
   `vTaskNotifyGiveFromISR`, bạn dự đoán hành vi quan sát được là gì (reset?
   treo? thông báo gì trên UART/debugger)?

   Treo do bị vòng lặp vô hạn
3. Dự đoán latency ISR → task (có `portYIELD_FROM_ISR`) theo đơn vị us, dựa
   trên tần số CPU 72 MHz và số lệnh ước lượng trong đường đi ISR → context
   switch. So với `LATENCY_BUDGET_US`: đạt hay không?
   Latency phụ thuộc vào thời gian xử lý ISR, chắc là tầm vài us nên sẽ đạt. 
4. Nếu bỏ `portYIELD_FROM_ISR`, `ack_task` sẽ chạy **khi nào** (ngay lập tức
   hay phải chờ tick ngắt kế tiếp)? Latency tăng thêm cỡ bao nhiêu?
   Phải chờ tick ngắt kế tiếp, tức là latency sẽ tăng thêm từ 0-1000us.(do đang cấu hình 1 tick là 1ms)
