#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

#include <stdint.h>
#include "global.h"

/**
 * @brief  Khởi tạo các chân GPIO điều khiển module đèn giao thông
 */
void traffic_init(void);

/**
 * @brief  Đặt trạng thái đèn cho một tuyến đường cụ thể
 * @param  road: Tuyến đường (ROAD_1 hoặc ROAD_2)
 * @param  color: Trạng thái đèn (LIGHT_RED, LIGHT_AMBER, LIGHT_GREEN, LIGHT_OFF)
 */
void traffic_set(uint8_t road, LightColor_t color);

/**
 * @brief  Tắt tất cả đèn ở cả 2 tuyến
 */
void traffic_turn_off_all(void);

#endif /* INC_TRAFFIC_LIGHT_H_ */
