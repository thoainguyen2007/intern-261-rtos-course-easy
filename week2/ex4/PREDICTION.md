# Dự đoán — Week 2 / Ex4

> **Commit TRƯỚC khi nạp chương trình.**

`student_id`: 2511006    `EVENT_BIT_COUNT` = 4  `SYNC_TIMEOUT_MS` = 120

1. RAM event group chiếm (bytes) dự đoán: ______ . Dựa vào đâu (cấu trúc dữ
   liệu nào trong FreeRTOS)?
   Trong file even_groups.c:
   ```
   typedef struct EventGroupDef_t
    {
        EventBits_t uxEventBits;
        List_t xTasksWaitingForBits; /**< List of tasks waiting for a bit to be set. */

        #if ( configUSE_TRACE_FACILITY == 1 )
            UBaseType_t uxEventGroupNumber;
        #endif

        #if ( ( configSUPPORT_STATIC_ALLOCATION == 1 ) && ( configSUPPORT_DYNAMIC_ALLOCATION == 1 ) )
            uint8_t ucStaticallyAllocated; /**< Set to pdTRUE if the event group is statically allocated to ensure no attempt is made to free the memory. */
        #endif
    } EventGroup_t;
   ```

   Dự đoán: 28 bytes.
   uxEventBits (4 bytes), xTasksWaitingForBits (20 bytes) và uxEventGroupNumber (4 bytes, do configUSE_TRACE_FACILITY = 1).
   Tổng 28 bytes.
2. RAM thêm khi dùng task notification cho 1 điểm đồng bộ: dự đoán = 0
   bytes. Vì sao?

   RAM thêm khi dùng task notification cho 1 điểm đồng bộ = 0 bytes.
   Vì notification sử dụng vùng nhớ có sẵn trong TCB của task, không cần cấp phát thêm một đối tượng đồng bộ riêng.
3. Dự đoán thời gian đánh thức (producer gọi → sync_task chạy) của notification
   so với event-group-bit: nhanh hơn / chậm hơn / xấp xỉ? Vì sao (nghĩ đến số
   lớp hàm bên dưới mỗi API)?
   Task notification đánh thức nhanh hơn event-group-bit vì notification gửi tín hiệu trực tiếp đến TCB của task nhận, trong khi event group phải quản lý danh sách task đang chờ và kiểm tra điều kiện các bit.
4. Nếu `sync_task` bị timeout (`SYNC_TIMEOUT_MS` hết hạn) vì một producer
   không set bit kịp, `xEventGroupWaitBits` trả về gì? Làm sao phân biệt được
   "đủ bit" với "timeout"?
   Khi sync_task bị timeout, xEventGroupWaitBits() trả về
   trạng thái các bit sự kiện tại thời điểm kết thúc chờ.
   Nếu (bits & ALL_BITS_MASK) == ALL_BITS_MASK thì đã đủ bit.
   Ngược lại, nếu thiếu ít nhất một bit thì việc chờ đã timeout.
   (EVENT_BIT_COUNT = 4, ALL_BITS_MASK = 0x0F)
5. Nêu trước một tình huống bạn nghĩ **không thể** thay toàn bộ event group
   bằng notification — sẽ kiểm chứng lại ở REPORT.md.

   Noti chỉ có thể 1/nhiều task gửi tín hiệu cho 1 task, vì vậy trường hợp nhiều task chờ 1 tín hiệu thì ko thể thay thế bằng notification.
