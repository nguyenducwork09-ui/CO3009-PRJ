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
 * @brief  Task duy nhất của chế độ chỉnh thời gian (add 1 lần trong main_app_init, chạy mỗi 10ms)
 *         - Cờ chế độ != MODE_TUNING : đưa fsm về TUNING_INIT rồi bỏ qua
 *         - Cờ chế độ == MODE_TUNING : xử lý nút Up/Down/Set + chớp đèn (software timer)
 */
void fsm_tuning_run(void);

/**
 * @brief  Lấy đèn đang được chỉnh (dùng cho task LCD)
 * @retval TUNING_RED, TUNING_AMBER hoặc TUNING_GREEN
 */
TuningState_t fsm_tuning_get_state(void);

/**
 * @brief  Lấy giá trị thời gian đang chỉnh (chưa lưu) (dùng cho task LCD)
 * @retval Số giây
 */
uint16_t fsm_tuning_get_value(void);

#endif /* INC_FSM_TUNING_H_ */
