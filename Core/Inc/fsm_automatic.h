#ifndef INC_FSM_AUTOMATIC_H_
#define INC_FSM_AUTOMATIC_H_

#include <stdint.h>
#include "global.h"

/* Các trạng thái con trong chế độ tự động */
typedef enum {
    AUTO_INIT = 0,
    AUTO_RED1_GREEN2,
    AUTO_RED1_AMBER2,
    AUTO_GREEN1_RED2,
    AUTO_AMBER1_RED2
} AutoState_t;

/**
 * @brief  Khởi tạo máy trạng thái tự động (thiết lập trạng thái ban đầu và timer)
 */
void fsm_automatic_init(void);

/**
 * @brief  Vào chế độ tự động: add các task của chế độ vào scheduler (chỉ gọi 1 lần)
 */
void fsm_automatic_start(void);

/**
 * @brief  Thoát chế độ tự động: xóa các task của chế độ khỏi scheduler
 */
void fsm_automatic_end(void);

#endif /* INC_FSM_AUTOMATIC_H_ */
