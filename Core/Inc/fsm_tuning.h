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
 * @brief  Vào chế độ chỉnh thời gian: add các task của chế độ vào scheduler (chỉ gọi 1 lần)
 */
void fsm_tuning_start(void);

/**
 * @brief  Thoát chế độ chỉnh thời gian: xóa các task của chế độ khỏi scheduler
 */
void fsm_tuning_end(void);

#endif /* INC_FSM_TUNING_H_ */
