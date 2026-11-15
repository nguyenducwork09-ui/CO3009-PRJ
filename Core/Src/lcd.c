#include "lcd.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

#define LCD_RS_PIN 0x01U
#define LCD_RW_PIN 0x02U
#define LCD_EN_PIN 0x04U
#define LCD_BL_PIN 0x08U

#define LCD_DEFAULT_ADDR_1 0x27U
#define LCD_DEFAULT_ADDR_2 0x3FU

static uint8_t lcd_i2c_addr = (LCD_DEFAULT_ADDR_1 << 1);

static uint8_t lcd_backlight_val = LCD_BL_PIN;

static void lcd_i2c_write(uint8_t data) {
  HAL_I2C_Master_Transmit(&hi2c1, lcd_i2c_addr, &data, 1, 10);
}

static void lcd_write_nibble(uint8_t nibble, uint8_t rs) {
  uint8_t data = (nibble & 0xF0U) | rs | lcd_backlight_val;

  lcd_i2c_write(data | LCD_EN_PIN);

  for (volatile uint32_t i = 0; i < 50; i++) {
    __NOP();
  }

  lcd_i2c_write(data & ~LCD_EN_PIN);
  for (volatile uint32_t i = 0; i < 50; i++) {
    __NOP();
  }
}

static void lcd_send_cmd(uint8_t cmd) {
  lcd_write_nibble(cmd & 0xF0U, 0U);

  lcd_write_nibble((cmd << 4) & 0xF0U, 0U);

  if (cmd == 0x01U || cmd == 0x02U) {
    HAL_Delay(2);
  }
}

static void lcd_send_data(uint8_t data) {
  lcd_write_nibble(data & 0xF0U, LCD_RS_PIN);
  lcd_write_nibble((data << 4) & 0xF0U, LCD_RS_PIN);
}

static void lcd_auto_detect_address(void) {
  if (HAL_I2C_IsDeviceReady(&hi2c1, (LCD_DEFAULT_ADDR_1 << 1), 2, 10) ==
      HAL_OK) {
    lcd_i2c_addr = (LCD_DEFAULT_ADDR_1 << 1);
    return;
  }

  if (HAL_I2C_IsDeviceReady(&hi2c1, (LCD_DEFAULT_ADDR_2 << 1), 2, 10) ==
      HAL_OK) {
    lcd_i2c_addr = (LCD_DEFAULT_ADDR_2 << 1);
    return;
  }

  for (uint8_t addr = 0x20U; addr <= 0x3FU; addr++) {
    if (addr == 0x38U)
      continue;

    if (HAL_I2C_IsDeviceReady(&hi2c1, (addr << 1), 1, 5) == HAL_OK) {
      lcd_i2c_addr = (addr << 1);
      return;
    }
  }
}

void lcd_init(void) {
  lcd_auto_detect_address();

  HAL_Delay(50);

  lcd_write_nibble(0x30U, 0U);
  HAL_Delay(5);
  lcd_write_nibble(0x30U, 0U);
  HAL_Delay(1);
  lcd_write_nibble(0x30U, 0U);
  HAL_Delay(1);

  lcd_write_nibble(0x20U, 0U);
  HAL_Delay(1);

  lcd_send_cmd(0x28U);

  lcd_send_cmd(0x0CU);

  lcd_clear();

  lcd_send_cmd(0x06U);
}

void lcd_clear(void) { lcd_send_cmd(0x01U); }

void lcd_set_cursor(uint8_t row, uint8_t col) {
  if (col > 15U)
    col = 15U;

  uint8_t address = (row == 0U) ? (0x80U + col) : (0xC0U + col);
  lcd_send_cmd(address);
}

void lcd_send_string(char *str) {
  if (str == NULL)
    return;

  while (*str) {
    lcd_send_data((uint8_t)(*str));
    str++;
  }
}

void lcd_display_env(float temp, float hum) {
  char line_buf[17];

  snprintf(line_buf, sizeof(line_buf), "Temp: %4.1f C   ", temp);
  lcd_set_cursor(0, 0);
  lcd_send_string(line_buf);

  snprintf(line_buf, sizeof(line_buf), "Humi: %4.1f %%   ", hum);
  lcd_set_cursor(1, 0);
  lcd_send_string(line_buf);
}

void lcd_display_traffic_status(SystemMode_t mode, uint16_t countdown1,
                                uint16_t countdown2) {
  char line_buf[17];

  switch (mode) {
  case MODE_AUTO:
    snprintf(line_buf, sizeof(line_buf), "AUTO  R1: %2u s  ", countdown1);
    lcd_set_cursor(0, 0);
    lcd_send_string(line_buf);

    snprintf(line_buf, sizeof(line_buf), "      R2: %2u s  ", countdown2);
    lcd_set_cursor(1, 0);
    lcd_send_string(line_buf);
    break;

  case MODE_MANUAL:
    lcd_set_cursor(0, 0);
    lcd_send_string("MODE: MANUAL    ");
    lcd_set_cursor(1, 0);
    lcd_send_string("Press Red/Yel/Gr");
    break;

  case MODE_TUNING:
    lcd_set_cursor(0, 0);
    lcd_send_string("MODE: TUNING    ");
    lcd_set_cursor(1, 0);
    lcd_send_string("Adjust via Up/Dn");
    break;

  default:
    break;
  }
}
