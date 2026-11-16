#ifndef INC_FSM_MODE_H_
#define INC_FSM_MODE_H_

#include <stdint.h>
#include "global.h"

/**
 * @brief  Khởi tạo toàn bộ hệ thống, gọi 1 lần trước while(1):
 *         - SCH_Init + init các module (button, traffic light, LCD)
 *         - Add 3 task chạy suốt: đọc nút, xử lý nút Mode, hiển thị LCD
 *         - Vào chế độ mặc định MODE_AUTO
 *         Sau đó while(1) chỉ cần gọi SCH_Dispatch_Tasks().
 */
void fsm_init(void);

/**
 * @brief  Chuyển sang chế độ mới: xóa task của fsm hiện tại (fsm_x_end),
 *         xóa cờ nút cũ, rồi add task của fsm mới (fsm_x_start)
 * @param  new_mode: Chế độ cần chuyển tới (MODE_AUTO, MODE_MANUAL, MODE_TUNING)
 */
void fsm_mode_switch(SystemMode_t new_mode);

#endif /* INC_FSM_MODE_H_ */
