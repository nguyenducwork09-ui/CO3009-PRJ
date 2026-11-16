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

#endif /* INC_BUTTON_H_ */
