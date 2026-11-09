#ifndef INC_FSM_TUNING_H_
#define INC_FSM_TUNING_H_

#include <stdint.h>
#include "global.h"

/* Các chế độ chỉnh thời gian cho từng loại đèn */
typedef enum {
    TUNING_INIT = 0,
    TUNING_RED,
    TUNING_AMBER,
    TUNING_GREEN
} TuningState_t;

/**
 * @brief  Khởi tạo máy trạng thái chỉnh thời gian
 */
void fsm_tuning_init(void);

/**
 * @brief  Thực thi xử lý điều chỉnh, lưu thời gian đèn và hiển thị thông tin lên LCD
 */
void fsm_tuning_run(void);

#endif /* INC_FSM_TUNING_H_ */
