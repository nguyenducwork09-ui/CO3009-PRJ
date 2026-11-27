#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>

#define SCH_MAX_TASKS 20U
#define NO_TASK_ID 0U
#define SCH_TICK_MS 10U

typedef struct {
  void (*pTask)(void); /* Con trỏ tới hàm task callback */
  uint32_t Delay;      /* Số tick còn lại tương đối so với task trước */
  uint32_t Period;     /* Chu kỳ lặp lại (ticks), 0 = one-shot task */
  uint8_t RunMe;       /* Cờ báo task tại đầu danh sách sẵn sàng chạy */
  uint32_t TaskID;     /* Định danh duy nhất của task */
} sTask;

void SCH_Init(void);

uint32_t SCH_Add_Task(void (*pFunc)(void), uint32_t DELAY, uint32_t PERIOD);

uint8_t SCH_Delete_Task(uint32_t taskID);

void SCH_Update(void);

void SCH_Dispatch_Tasks(void);

uint32_t SCH_Get_Time(void);

#endif /* INC_SCHEDULER_H_ */
