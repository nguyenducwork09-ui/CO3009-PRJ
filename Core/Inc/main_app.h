#ifndef INC_MAIN_APP_H_
#define INC_MAIN_APP_H_

#include <stdint.h>
#include "global.h"

/**
 * @brief  Khởi chạy toàn bộ ứng dụng, gọi 1 lần trước while(1):
 *         - SCH_Init + init các module (button, traffic light, LCD)
 *         - Đặt cờ chế độ mode_flag (global) = MODE_AUTO (0)
 *         - Add 7 task chạy suốt chương trình (không bao giờ xóa):
 *             Button_Reading, Button_Logic, fsm_automatic_run,
 *             fsm_manual_run, fsm_tuning_run, LCD_Display, Sensor_Reading
 *         Sau đó while(1) chỉ cần gọi SCH_Dispatch_Tasks().
 */
void main_app_init(void);

#endif /* INC_MAIN_APP_H_ */
