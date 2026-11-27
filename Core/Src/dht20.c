#include "dht20.h"
#include "global.h"
#include "main.h"

extern I2C_HandleTypeDef hi2c1;

#define DHT20_I2C_ADDR_7BIT 0x38U
#define DHT20_I2C_ADDR_8BIT (DHT20_I2C_ADDR_7BIT << 1) /* 0x70 */

#define DHT20_CMD_TRIGGER 0xACU
#define DHT20_CMD_STATUS 0x71U
#define DHT20_CMD_RESET_REG 0xBEU

static uint8_t dht20_calc_crc(const uint8_t *data, uint8_t len) {
  uint8_t crc = 0xFFU;

  for (uint8_t i = 0U; i < len; i++) {
    crc ^= data[i];
    for (uint8_t bit = 0U; bit < 8U; bit++) {
      if (crc & 0x80U) {
        crc = (crc << 1) ^ 0x31U;
      } else {
        crc <<= 1;
      }
    }
  }

  return crc;
}

uint8_t dht20_init(void) {
  HAL_Delay(100);
  if (HAL_I2C_IsDeviceReady(&hi2c1, DHT20_I2C_ADDR_8BIT, 3, 20) != HAL_OK) {
    return 0U;
  }

  uint8_t status = 0U;
  if (HAL_I2C_Master_Receive(&hi2c1, DHT20_I2C_ADDR_8BIT, &status, 1, 20) !=
      HAL_OK) {
    return 0U;
  }

  if ((status & 0x18U) != 0x18U) {
    uint8_t init_cmd[3] = {DHT20_CMD_RESET_REG, 0x08U, 0x00U};
    HAL_I2C_Master_Transmit(&hi2c1, DHT20_I2C_ADDR_8BIT, init_cmd, 3, 20);
    HAL_Delay(10);
  }

  return 1U;
}

uint8_t dht20_read(float *temperature, float *humidity) {
  if (temperature == (void *)0 || humidity == (void *)0) {
    return 0U;
  }

  uint8_t trigger_cmd[3] = {DHT20_CMD_TRIGGER, 0x33U, 0x00U};
  if (HAL_I2C_Master_Transmit(&hi2c1, DHT20_I2C_ADDR_8BIT, trigger_cmd, 3,
                              20) != HAL_OK) {
    return 0U;
  }

  HAL_Delay(80);

  uint8_t raw_data[7] = {0};
  if (HAL_I2C_Master_Receive(&hi2c1, DHT20_I2C_ADDR_8BIT, raw_data, 7, 30) !=
      HAL_OK) {
    return 0U;
  }

  if ((raw_data[0] & 0x80U) != 0U) {
    return 0U;
  }

  if (dht20_calc_crc(raw_data, 6) != raw_data[6]) {
    return 0U;
  }

  uint32_t raw_hum = ((uint32_t)raw_data[1] << 12) |
                     ((uint32_t)raw_data[2] << 4) |
                     ((uint32_t)raw_data[3] >> 4);

  uint32_t raw_temp = (((uint32_t)raw_data[3] & 0x0FU) << 16) |
                      ((uint32_t)raw_data[4] << 8) | ((uint32_t)raw_data[5]);

  *humidity = ((float)raw_hum * 100.0f) / 1048576.0f;
  *temperature = (((float)raw_temp * 200.0f) / 1048576.0f) - 50.0f;

  current_temperature = *temperature;
  current_humidity = *humidity;

  return 1U;
}
