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
 * @brief  Task duy nhất của chế độ tự động (add 1 lần trong main_app_init, chạy mỗi 10ms)
 *         - Cờ chế độ != MODE_AUTO : đưa fsm về AUTO_INIT rồi bỏ qua
 *         - Cờ chế độ == MODE_AUTO : chạy máy trạng thái (chuyển pha bằng software timer)
 */
void fsm_automatic_run(void);

/**
 * @brief  Lấy số giây đếm lùi còn lại của một tuyến (dùng cho task LCD)
 * @param  road: Tuyến đường (ROAD_1 hoặc ROAD_2)
 * @retval Số giây còn lại
 */
uint16_t fsm_automatic_get_countdown(uint8_t road);

#endif /* INC_FSM_AUTOMATIC_H_ */
