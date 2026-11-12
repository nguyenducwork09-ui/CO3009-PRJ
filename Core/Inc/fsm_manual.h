#ifndef INC_FSM_MANUAL_H_
#define INC_FSM_MANUAL_H_

#include <stdint.h>
#include "global.h"

/* Các trạng thái trong chế độ điều khiển bằng tay */
typedef enum {
    MANUAL_INIT = 0,
    MANUAL_RED1_GREEN2,
    MANUAL_GREEN1_RED2,
    MANUAL_AMBER1_AMBER2
} ManualState_t;

/**
 * @brief  Khởi tạo máy trạng thái chế độ bằng tay
 */
void fsm_manual_init(void);

/**
 * @brief  Vào chế độ bằng tay: add các task của chế độ vào scheduler (chỉ gọi 1 lần)
 */
void fsm_manual_start(void);

/**
 * @brief  Thoát chế độ bằng tay: xóa các task của chế độ khỏi scheduler
 */
void fsm_manual_end(void);

/**
 * @brief  Lấy trạng thái hiện tại của chế độ bằng tay (dùng cho task LCD)
 * @retval Trạng thái manual
 */
ManualState_t fsm_manual_get_state(void);

#endif /* INC_FSM_MANUAL_H_ */
