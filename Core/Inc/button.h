#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <stdint.h>
#include "global.h"

/**
 * @brief  Khởi tạo GPIO cho các nút bấm
 */
void button_init(void);

/**
 * @brief  Đọc trạng thái các nút và chống dội phím (gọi theo chu kỳ 10ms bởi Scheduler)
 */
void button_reading(void);

/**
 * @brief  Kiểm tra nút nhấn có vừa được nhấn hay không (Single press)
 * @param  index: ID nút (BTN_1, BTN_2, BTN_3, BTN_4, BTN_USER)
 * @retval 1 nếu nút được nhấn, 0 nếu không
 */
uint8_t isButtonPressed(uint8_t index);

/**
 * @brief  Kiểm tra nút nhấn có đang được nhấn giữ >= 1 giây hay không (Long press)
 * @param  index: ID của nút (BTN_1, BTN_2, BTN_3, BTN_4, BTN_USER)
 * @retval 1 nếu nút được giữ đủ 1s, 0 nếu không
 */
uint8_t isButtonPressed1s(uint8_t index);

/* ===== Skeleton cho các fsm (driver cần hiện thực) =====
 * Quy ước chung cho các hàm btn_x():
 *   - Nút: 1 đầu nối GND, 1 đầu nối GPIO (Input, internal Pull-up) -> nhấn = mức 0
 *   - Được fsm gọi mỗi 10ms, driver tự đọc chân + chống dội
 *   - Trả về 1 đúng 1 lần tại cạnh nhấn (nhả -> nhấn), còn lại trả về 0
 */

/**
 * @brief  Poll nút Đỏ (chế độ Manual)
 * @retval 1 nếu nút vừa được nhấn, 0 nếu không
 */
uint8_t btn_red(void);

/**
 * @brief  Poll nút Vàng (chế độ Manual)
 * @retval 1 nếu nút vừa được nhấn, 0 nếu không
 */
uint8_t btn_yellow(void);

/**
 * @brief  Poll nút Xanh (chế độ Manual)
 * @retval 1 nếu nút vừa được nhấn, 0 nếu không
 */
uint8_t btn_green(void);

/**
 * @brief  Poll nút Up - tăng giá trị (chế độ Tuning)
 * @retval 1 nếu nút vừa được nhấn, 0 nếu không
 */
uint8_t btn_up(void);

/**
 * @brief  Poll nút Down - giảm giá trị (chế độ Tuning)
 * @retval 1 nếu nút vừa được nhấn, 0 nếu không
 */
uint8_t btn_down(void);

/**
 * @brief  Poll nút Set - lưu giá trị (chế độ Tuning)
 * @retval 1 nếu nút vừa được nhấn, 0 nếu không
 */
uint8_t btn_set(void);

/**
 * @brief  Poll nút Mode - đổi chế độ AUTO -> MANUAL -> TUNING
 * @retval 1 nếu nút vừa được nhấn, 0 nếu không
 */
uint8_t btn_mode(void);

#endif /* INC_BUTTON_H_ */
