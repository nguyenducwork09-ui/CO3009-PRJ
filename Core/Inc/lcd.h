#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "global.h"
#include <stdint.h>


void lcd_init(void);

void lcd_clear(void);

void lcd_set_cursor(uint8_t row, uint8_t col);

void lcd_send_string(char *str);

void lcd_display_env(float temp, float hum);

void lcd_display_traffic_status(SystemMode_t mode, uint16_t countdown1,
                                uint16_t countdown2);

#endif /* INC_LCD_H_ */
