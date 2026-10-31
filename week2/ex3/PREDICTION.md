# Dự đoán — Week 2 / Ex3

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006    `LOW_HOLD_MS` = 20  `MID_WORK_MS` = 30
`HIGH_PERIOD_MS` = 25  `DEADLINE_MS` = 20

1. Cấu hình binary semaphore: thời gian `task_high` bị block dự đoán = 30-50 ms.
   Phép tính dựa trên đâu (liệt kê các thành phần cộng lại)?

   Gs `low_task` đã chạy được x ms (0<=x<=20), sau đó bị `high_task` cướp quyền, tuy nhiên do `low_task` đang giữ tài nguyên nên `high_task` bị block, `mid_task` run 30ms rồi `low_task` chạy phần còn lại thì `high_task` mới chạy tiếp được.
   Như vậy `high_task` bị block 30+(20-x) ms, tức là từ 30-50ms.

2. `task_high` có đạt `DEADLINE_MS` ở cấu hình binary semaphore không? Vì sao?

Không vì theo kết quả câu 1 thì `task_high` bị block tối thiểu 30ms (20<30) nên sẽ ko đạt

3. Cấu hình mutex: thời gian `task_high` bị block dự đoán = 0-20 ms. Vì sao
   ngắn hơn (hoặc không) so với câu 1?

   Gs `low_task` đã chạy được x ms (0<=x<=20), sau đó bị `high_task` cướp quyền, tuy nhiên do `low_task` đang giữ tài nguyên nên `high_task` bị block, theo cơ chế Priority Inheritance thì `low_task` sẽ được chạy trước để trả tài nguyên. Như vậy `high_task` bị block x ms <20 ms nên sẽ ngắn hơn ở câu 1.

4. Trên ảnh logic analyzer ở cấu hình binary semaphore, kênh `task_mid` sẽ ở
   trạng thái nào trong lúc `task_high` bị chặn — chạy liên tục, chạy ngắt
   quãng, hay không chạy? Vì sao?

   Theo phân tích ở câu 1 thì `task_mid` sẽ chạy liên tục 30ms.

5. Nếu đổi `task_mid` sang ưu tiên **thấp hơn** `task_low`, inversion còn xảy ra
   không? Vì sao (đây là cách phân biệt priority inversion với việc chỉ đơn
   giản là "task_low chạy lâu").

   Không còn xảy ra. Gs `low_task` đã chạy được x ms (0<=x<=20), sau đó bị `high_task` cướp quyền, tuy nhiên do `low_task` đang giữ tài nguyên nên `high_task` bị block, khi này `low_task` và `mid_task` đếu ready, tuy nhiên `low_task` có ưu tiên cao hơn nên sẽ running, do đó ko xảy ra priority inversion.
