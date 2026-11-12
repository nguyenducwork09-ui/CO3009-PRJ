#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include <stdint.h>

#define NUM_SOFTWARE_TIMERS     8

extern uint8_t timer_flag[NUM_SOFTWARE_TIMERS];

/**
 * @brief  Thiết lập giá trị đếm thời gian cho một timer mềm
 * @param  index: ID của timer (0 đến NUM_SOFTWARE_TIMERS - 1)
 * @param  duration: Thời gian chờ (tính theo tick hoặc ms)
 */
void setTimer(uint8_t index, uint32_t duration);

/**
 * @brief  Cập nhật bộ đếm các timer mềm (gọi định kỳ mỗi nhịp 10ms)
 */
void timerRun(void);

/**
 * @brief  Kiểm tra timer đã hết hạn chưa và tự động hạ cờ
 * @param  index: ID của timer
 * @retval 1 nếu timer hết hạn, 0 nếu chưa
 */
uint8_t isTimerExpired(uint8_t index);

#endif /* INC_SOFTWARE_TIMER_H_ */
