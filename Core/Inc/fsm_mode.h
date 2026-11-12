#ifndef INC_FSM_MODE_H_
#define INC_FSM_MODE_H_

#include <stdint.h>
#include "global.h"

/**
 * @brief  Khởi tạo quản lý chế độ: add task polling nút Mode (tồn tại suốt chương trình)
 *         và vào chế độ mặc định MODE_AUTO. Gọi 1 lần sau SCH_Init().
 */
void fsm_mode_init(void);

/**
 * @brief  Chuyển sang chế độ mới: xóa task của fsm hiện tại (fsm_x_end)
 *         rồi add task của fsm mới (fsm_x_start)
 * @param  new_mode: Chế độ cần chuyển tới (MODE_AUTO, MODE_MANUAL, MODE_TUNING)
 */
void fsm_mode_switch(SystemMode_t new_mode);

/**
 * @brief  Task polling nút Mode: mỗi lần nhấn chuyển AUTO -> MANUAL -> TUNING -> AUTO
 */
void Button_Mode_Polling(void);

#endif /* INC_FSM_MODE_H_ */
