#ifndef INC_FSM_MANUAL_H_
#define INC_FSM_MANUAL_H_

#include <stdint.h>
#include "global.h"

/* Các trạng thái trong chế độ điều khiển bằng tay */
typedef enum {
    MANUAL_INIT = 0,
    MANUAL_RED1_GREEN2,
    MANUAL_RED1_AMBER2,     /* chỉ có được khi kế thừa từ chế độ tự động */
    MANUAL_GREEN1_RED2,
    MANUAL_AMBER1_RED2,     /* chỉ có được khi kế thừa từ chế độ tự động */
    MANUAL_AMBER1_AMBER2
} ManualState_t;

/**
 * @brief  Task duy nhất của chế độ bằng tay (add 1 lần trong main_app_init, chạy mỗi 10ms)
 *         - Cờ chế độ != MODE_MANUAL : đưa fsm về MANUAL_INIT rồi bỏ qua
 *         - Vừa vào chế độ: giữ nguyên pha đèn hiện tại (đọc traffic_phase ở global)
 *         - Cờ chế độ == MODE_MANUAL : xử lý nút, đổi đèn
 */
void fsm_manual_run(void);

/**
 * @brief  Lấy trạng thái hiện tại của chế độ bằng tay (dùng cho task LCD)
 * @retval Trạng thái manual
 */
ManualState_t fsm_manual_get_state(void);

#endif /* INC_FSM_MANUAL_H_ */
