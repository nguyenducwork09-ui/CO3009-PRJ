#ifndef INC_DHT20_H_
#define INC_DHT20_H_

#include <stdint.h>

/**
 * @brief  Khởi tạo giao tiếp I2C với cảm biến DHT20
 * @retval 1 nếu thành công (thiết bị phản hồi), 0 nếu thất bại
 */
uint8_t dht20_init(void);

/**
 * @brief  Đọc giá trị nhiệt độ và độ ẩm từ DHT20
 * @param  temperature: Con trỏ lưu giá trị nhiệt độ (°C)
 * @param  humidity: Con trỏ lưu giá trị độ ẩm tương đối (%RH)
 * @retval 1 nếu đọc thành công và dữ liệu CRC hợp lệ, 0 nếu lỗi
 */
uint8_t dht20_read(float *temperature, float *humidity);

#endif /* INC_DHT20_H_ */
