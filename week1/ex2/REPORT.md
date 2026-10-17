# Report — Week 1 / Ex2

`student_id`: 2511006    Commit của PREDICTION.md: ______

## 1. CPU share đo được
| Cấu hình | A (%) | B (%) | C (%) | Time slice đo được | Ảnh |
|---|---|---|---|---|---|
| B2: preempt=1, ưu tiên khác nhau |0% |0% |100% | — |`![](do_ex2_1.png)` |
| B3: preempt=0, ưu tiên khác nhau | 0%|0% |100% | — |`![](do_ex2_2.png)` |
| B4: preempt=1, cùng ưu tiên |33.3% |33.3% |33.3% | 1ms |`![](do_ex2_3.png)` |
| B5: sau khi sửa starvation | 12%|24% |18% |- | `![](do_ex2_4.png)`|

Cách tính CPU share từ ảnh PulseView (mô tả ngắn): đo thời gian chiếm CPU của từng task, (riêng B5 đo thêm thời gian ko có task nào chiếm CPU) từ đó lấy thời gian từng task chiếm chia cho tổng thời gian là ra được %.

## 2. Preemption nhìn thấy ở đâu
Trên ảnh **bước 4** (cùng ưu tiên, round-robin), chỉ ra ít nhất **2 thời điểm**
preemption xảy ra. Với mỗi thời điểm: dấu thời gian, task bị cắt, task chiếm CPU,
và vì sao (hết time slice, không phải block).

| t (us) | Task bị preempt | Task chiếm CPU | Lý do |
|---|---|---|---|
|17645.08|Task A|Task B|Hết time slice, ngắt SysTick chuyển quyền sang task kế tiếp có cùng mức ưu tiên|
|18646.08|Task B|Task C|Hết time slice, ngắt SysTick chuyển quyền sang task kế tiếp có cùng mức ưu tiên|
|19646.08|Task C|Task A|Hết time slice (ngắt SysTick chuyển quyền vòng tròn quay lại Task A)|

Ở **bước 2** (ưu tiên khác nhau, đều busy-loop không block): giải thích vì sao ảnh
của bạn cho thấy **0 thời điểm preemption**, dù chạy bao lâu đi nữa — dựa vào điều
kiện preemption của FreeRTOS, không phải "vì ảnh chụp chưa đủ dài".

Điều kiện preemption trong FreeRTOS là phải có một task có độ ưu tiên cao hơn task đang chạy chuyển sang trạng thái Ready. Ở Bước 2, Task C có độ ưu tiên cao nhất. Khi Task C đang chạy vòng lặp vô tận, không có task nào khác trong hệ thống có độ ưu tiên lớn hơn nên không thể giành CPU với task C.

## 3. Time slice
Time slice đo được ở bước 4: 1 ms. `configTICK_RATE_HZ` = 1000.
Sai số: 0%. Giải thích sai số:(em cũng ko biết sao nhưng mà của em không có sai số)

## 4. DWT wrap-around
Tại sao so sánh `DWT->CYCCNT > target` bị sai sau ~59 giây, và cách viết đúng:

CYCCNT là thanh ghi 32 bit nên sẽ đếm tới 2^32-1. Khi hết thì sẽ quay lại
(2^32-1)/72MHz~59s Đó là là do bị sai sau 59s.
Để dễ hiểu thì xét trường hợp CYCCNT đếm tới 100 đi. Vd start là 90, cần busy trong 20 chu kỳ
Vậy thì khi hết 20 chu kỳ CYCCNT đang ở 10. 
Tuy nhiên, khi quay lại đếm từ 0 thì sẽ xuất hiện phép trừ 0-90=-90. 
Như vậy khi này sẽ ra âm và hiển nhiên <cycles và thoát vòng lặp luôn.
Để khắc phục cái này thì sẽ đổi thành uint32_t, khi này -90 =10 (modulo 100).
Vì vậy ta sẽ code là `(uint32_t)(DWT->CYCCNT - target>0) ` 

## 5. Sửa starvation
Cách tôi chọn: thêm hàm vTaskDelay(1) vào cuối mỗi task . Cách này đánh đổi gì? 
Vì sao nó hoạt động (dựa vào trạng thái task trong scheduler): task chạy hết thời gian sẽ không lặp lại mà bị blocked do hàm vTaskDelay(1) trong 1 tick, khi đó sẽ cho phép các task có ưu tiên thấp hơn chạy.
Đánh đổi: Bắt task đang chạy blocked trong 1 tick(trong trường hợp này là 1ms do configTICK_RATE_HZ=1000). Mà thời gian work của 3 task là 120+240+180=540us, tức là có tới 460us bị lãng phí
## 6. Lệch so với dự đoán
Thời gian delay của task C không đúng 1ms mà chỉ có 0.82ms. Vd task C bắt đầu tại tick thứ 10(10ms) thì tới 10.18ms task C kết thúc và gọi hàm vTaskDelay(1). Tuy nhiên tại thời điểm này bộ đếm xTickCount vẫn đang ở tick 10, nên FreeRTOS sẽ xác định thời gian hết blocked của task C là ở tick 11 tức là thời điểm 11ms. Do đó khoảng thời gian giữa 2 lần không phải là 1ms.

## 7. Câu hỏi đánh đổi
Với **bộ ưu tiên của bạn**, nếu `TASK_C_WORK_US` tăng gấp 3 lần thì bảng CPU share ở
bước 2 thay đổi thế nào? Trả lời bằng số suy ra từ dữ liệu của bạn, rồi **đo lại để
kiểm chứng**. Nếu lệch, giải thích.
Tăng lên thì là 3*180=540us. Như vậy thì mức ưu tiên vẫn như vậy, không có gì thay đổi vì vậy task C vẫn chiếm 100%. 
