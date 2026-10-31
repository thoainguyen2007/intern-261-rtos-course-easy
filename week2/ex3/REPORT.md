# Report — Week 2 / Ex3

`student_id`: 2511006    Commit của PREDICTION.md: ______

## 1. Ảnh đo
Binary semaphore: `![](bs.png)`
Mutex: `![](mutex.png)`
Câu 6: `![](cau6.png)`

Trên mỗi ảnh, đánh dấu: thời điểm `task_high` sẵn sàng chạy (t0), thời điểm nó
thực sự chạy (t1). Thời gian block = t1 − t0.
D2=1 là thời điểm t0; D3 =1 là thời điểm t1.

## 2. Bảng thời gian block
Trong cấu hình Binary semaphore, ta thấy có 4 trường hợp(xem bảng)
Trong cấu hình mutex, ta thấy có 2 trường hợp
| Cấu hình | t0 (us) | t1 (us) | Block (us) | Đạt `DEADLINE_MS`? |
|---|---|---|---|---|
| Binary semaphore_1 |0 |11 |11 | Đạt|
| Binary semaphore_2 | 25|46 | 21|Không|
| Binary semaphore_3(trường hợp quan trọng) | 50| 81|31 |Không |
| Binary semaphore_4 | 82|82 |0 |Đạt |
| Mutex_1 |25 |42 |17 |Đạt |
| Mutex_2 |50 |50 |0 |Đạt |

Giải thích 4 trường hợp BS: 
Trường hợp 1 và 2: `task_high` ready rất lâu sau khi `task_mid` chạy, vì vậy thời gian bị block do phải chờ `task_mid` giảm, nên vẫn đạt deadline

Trường hợp 2: `task_high` ready sau không quá lâu sau khi `task_mid` chạy, vì vậy thời gian bị block lâu hơn nên trễ deadline.

Trường hợp 3: `task_high` ready trước cả `task_mid`, phải chờ `task_mid` chạy xong, nhả `task_low` thì mới chạy được.
Trường hợp 4: đây là hệ quả từ trường hợp 3, sau khi `task_high` ready(t=50) trước `task_mid`. Tuy nhiên do dùng vTaskDelayUntil nên biến lastwake ghi nhận từ khi `task_high` ready trước `task_mid` và xác định thời điểm hết delay là sau đó 25ms (lúc t=50+25=75ms). Ta có thể kiếm chứng bằng cách quan sát D2=1 tại các thời điểm t=0,25,50 ms.  Do đó sau khi `task_high` chạy xong thì tiếp tục chạy do có ưu tiên cao, ko bị delay. 

Giải thích 2 trường hợp mutex: 
Trường hợp 1: D3=1 khi D0 đang bắng 1, tức là task high ready khi task low đang cầm tài nguyên, do đó phải chờ tới khi task 1 xong.
Trường hợp 2: D3=1 khi D0=0, tức là tức là task high ready khi task low đang nhả tài nguyên, do đó chạy luôn không cần chờ

## 3. Vì sao mutex ngắn hơn
Giải thích bằng priority inheritance: `task_low` được nâng lên ưu tiên nào, khi
nào, và việc đó ngăn `task_mid` chen vào như thế nào.
Dùng mutex thì thời giàn task_high bị block ngắn hơn do không phải chờ task mid chạy xong, chờ task_low nhả tài nguyên mà chỉ cần chờ task low nhả tài nguyên do khi này task low được nâng priority lên bằng task high, cao hơn task mid, task mid không preempt đc nên không cần chờ task mid nữa.

## 4. task_mid trong lúc inversion
Mô tả trạng thái kênh `task_mid` trên ảnh binary semaphore trong khoảng
inversion — chạy bao nhiêu lần, mỗi lần bao lâu.
task mid chạy theo đúng chu kỳ HIGH trong 30ms và LOW trong 5ms (giống trong code).

## 5. Lệch so với dự đoán
Task high bị block ngắn hơn dự đoán, do trong dự đoán sau khi task mid chạy xong thì task low vẫn chạy hết. Tuy nhiên do dwt vẫn đếm trong lúc task low không chạy, cho nên khi task mid chạy qua, quay về task low thì task low lập tức trả tài nguyên do dwt đã đếm đủ từ trước đó.
Ngoài ra còn xảy ra các trường hợp bs_4 như đã phân tích.


## 6. Câu hỏi đánh đổi
Với bộ tham số của **bạn**, nếu `MID_WORK_MS` tăng gấp đôi thì thời gian block ở
cấu hình binary semaphore tăng thêm bao nhiêu? Trả lời bằng số từ dữ liệu của
bạn, rồi đo lại để kiểm chứng. `task_high` còn đạt `DEADLINE_MS` không?

task_mid chạy lâu gấp đôi(từ 30ms lên 60ms). Do đó thời gian bị block tối đa là 60+20ms=80ms, tối thiểu là 0ms (xảy ra trường hợp đã delay trong lúc block như phỉa trên đã giải thích). 

Kết quả kiếm chứng: thời gian bị block tối đa không thể lên tới 80ms, do mid có thể chạy ngay sau khi high chạy xong, mà high bị delayuntil 25ms, do đó high thường ready trễ hơn tầm 25ms sau khi task low chạy. Ngoài ra ta thấy có nhiều lần high ko bị block liên tiếp, đó là do delayuntil.
