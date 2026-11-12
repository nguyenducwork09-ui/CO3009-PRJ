#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

#define SCH_MAX_TASKS   10

typedef struct {
    void     (*pTask)(void);   /* Con trỏ tới hàm task */
    uint32_t Delay;            /* Số tick còn lại (tương đối) */
    uint32_t Period;           /* Chu kỳ, 0 = one-shot task */
    uint8_t  RunMe;            /* Cờ báo task sẵn sàng chạy */
    uint32_t TaskID;           /* Định danh task */
} sTask;

/**
 * @brief  Khởi tạo scheduler
 */
void SCH_Init(void);

/**
 * @brief  Thêm một tác vụ vào scheduler
 * @param  pFunc: Con trỏ tới hàm task
 * @param  DELAY: Thời gian chờ trước khi chạy lần đầu (ticks)
 * @param  PERIOD: Chu kỳ lặp lại (ticks), 0 nếu là task chạy một lần (one-shot)
 * @retval TaskID nếu thêm thành công, hoặc mã lỗi
 */
uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t DELAY, uint32_t PERIOD);

/**
 * @brief  Xóa một tác vụ khỏi scheduler dựa trên TaskID
 * @param  taskID: ID của task cần xóa
 * @retval 1 nếu xóa thành công, 0 nếu không tìm thấy
 */
uint8_t SCH_Delete_Task(uint32_t taskID);

/**
 * @brief  Cập nhật scheduler mỗi nhịp ngắt timer (độ phức tạp O(1))
 *         Gọi trong hàm ngắt timer (ví dụ: HAL_TIM_PeriodElapsedCallback)
 */
void SCH_Update(void);

/**
 * @brief  Bộ điều phối gọi thực thi các task đã đến hạn (gọi trong vòng lặp while(1))
 */
void SCH_Dispatch_Tasks(void);

#endif /* INC_SCHEDULER_H_ */
