#ifndef INC_FSM_MANUAL_H_
#define INC_FSM_MANUAL_H_

#include <stdint.h>
#include "global.h"

/* Các trạng thái trong chế độ điều khiển bằng tay */
typedef enum {
    MANUAL_INIT = 0,
    MANUAL_RED1_GREEN2,
    MANUAL_GREEN1_RED2
} ManualState_t;

/**
 * @brief  Khởi tạo máy trạng thái chế độ bằng tay
 */
void fsm_manual_init(void);

/**
 * @brief  Thực thi xử lý chuyển trạng thái đèn theo nút bấm thủ công
 */
void fsm_manual_run(void);

#endif /* INC_FSM_MANUAL_H_ */
