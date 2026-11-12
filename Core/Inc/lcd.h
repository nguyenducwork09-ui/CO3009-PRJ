#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <stdint.h>
#include "global.h"

/**
 * @brief  Khởi tạo màn hình LCD qua giao tiếp I2C
 */
void lcd_init(void);

/**
 * @brief  Xóa toàn bộ nội dung hiển thị trên LCD
 */
void lcd_clear(void);

/**
 * @brief  Di chuyển con trỏ tới hàng và cột chỉ định
 * @param  row: Hàng (0 hoặc 1)
 * @param  col: Cột (0 đến 15)
 */
void lcd_set_cursor(uint8_t row, uint8_t col);

/**
 * @brief  Gửi chuỗi ký tự hiển thị lên LCD
 * @param  str: Chuỗi ký tự kết thúc bằng null
 */
void lcd_send_string(char *str);

/**
 * @brief  Hiển thị thông tin môi trường (nhiệt độ & độ ẩm) lên LCD
 * @param  temp: Giá trị nhiệt độ
 * @param  hum: Giá trị độ ẩm
 */
void lcd_display_env(float temp, float hum);

/**
 * @brief  Hiển thị trạng thái chế độ hoạt động và đếm lùi thời gian đèn
 * @param  mode: Chế độ hoạt động (MODE_AUTO, MODE_MANUAL, MODE_TUNING)
 * @param  countdown1: Thời gian đếm lùi tuyến 1
 * @param  countdown2: Thời gian đếm lùi tuyến 2
 */
void lcd_display_traffic_status(SystemMode_t mode, uint16_t countdown1, uint16_t countdown2);

#endif /* INC_LCD_H_ */
