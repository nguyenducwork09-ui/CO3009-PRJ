#ifndef INC_MAIN_APP_H_
#define INC_MAIN_APP_H_

#include <stdint.h>
#include "global.h"

/**
 * @brief  Khởi chạy toàn bộ ứng dụng, gọi 1 lần trước while(1):
 *         - SCH_Init + init các module (button, traffic light, LCD)
 *         - Đặt cờ chế độ = MODE_AUTO (0)
 *         - Add 6 task chạy suốt chương trình (không bao giờ xóa):
 *             Button_Reading, Button_Logic, fsm_automatic_run,
 *             fsm_manual_run, fsm_tuning_run, LCD_Display
 *         Sau đó while(1) chỉ cần gọi SCH_Dispatch_Tasks().
 */
void main_app_init(void);

/**
 * @brief  Đọc cờ chế độ (các task fsm tự polling hàm này để biết có tới lượt mình không)
 * @retval MODE_AUTO (0), MODE_MANUAL (1) hoặc MODE_TUNING (2)
 */
SystemMode_t main_app_get_mode(void);

#endif /* INC_MAIN_APP_H_ */
