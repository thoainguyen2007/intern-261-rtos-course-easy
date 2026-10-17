# Dự đoán — Week 1 / Ex2

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006    Ưu tiên của tôi: A=2 B=3 C=4

1. Bước 2 (`configUSE_PREEMPTION`=1, ưu tiên khác nhau): task nào chạy? task nào
   **không bao giờ** chạy? CPU share dự đoán:

   | Task | CPU share dự đoán (%) |
   |---|---|
   | A | 0%|
   | B |0% |
   | C | 100%|

2. Bước 3 (`configUSE_PREEMPTION`=0): dự đoán thay đổi gì so với bước 2? Vì sao?
Vẫn như cũ do `configUSE_PREEMPTION`=0 thì vào chế độ Co-operative Scheduling. Khi này việc chuyển task không còn phụ thuộc vào Scheduler mà tự phụ thuộc vào trạng thái hiện tại của các task(tức là nếu có task cao hơn ready thì Scheduler cũng không thể ép task đang chạy đi vào blocked). Vd khi task C đang chạy,  task A ready và có dòng chuyển về trạng thái blocked trong task C thì khi task C vào blocked thì sẽ tự chuyển sang task A. Còn nếu task C ko thể chuyển về blocked thì sẽ vẫn luôn ở C. Trong bài này task C có mức ưu tiên cao hơn nên khi mới bắt đầu, cả 3 đều ready, nhma task C sẽ running. Tuy nhiên task C chỉ là vòng lặp vô hạn nên task A vs task B sẽ không bao giờ được gọi
3. Bước 4 (3 task cùng ưu tiên): time slice dự đoán = 1 ms. Dựa vào đâu?
Khi 3 task cùng mức ưu tiên và `configUSE_TIME_SLICING`=1 thì time slice=1/configTICK_RATE_HZ=1/1000=1ms. `configUSE_TIME_SLICING`=1 tức là sẽ luân chuyển mỗi tick sẽ luân phiên từng task cùng mức ưu tiên. 

4. Bước 5: tôi sẽ sửa starvation bằng cách: thêm hàm vTaskDelay(1) . Cách này đánh đổi gì?
Bắt task đang chạy blocked trong 1 tick(trong trường hợp này là 1ms do configTICK_RATE_HZ=1000). Mà thời gian work của 3 task là 120+240+180=540us, tức là có tới 460us bị lãng phí(tuy nhiên em nghĩ có thể chỉnh bằng cách tăng configTICK_RATE_HZ lên)

5. Chân nào sẽ không bao giờ đổi trạng thái trên logic analyzer ở bước 2? Chân PA1 và PA5