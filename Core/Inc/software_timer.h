#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include <stdint.h>

#define NUM_SOFTWARE_TIMERS     8U
#define TIMER_TICK_MS           10U   /* Mỗi nhịp đếm tương ứng 10ms */

/* ===== ID các timer mềm (mỗi fsm dùng timer riêng, không dùng chung) ===== */
#define TIMER_AUTO_PHASE        0   /* fsm_automatic: hết thời gian pha đèn hiện tại */
#define TIMER_AUTO_SECOND       1   /* fsm_automatic: nhịp 1s để đếm lùi            */
#define TIMER_TUNING_BLINK      2   /* fsm_tuning   : nhịp chớp đèn 500ms           */

/* Macro quy đổi tiện lợi giữa giây/mili-giây và số tick */
#define SEC_TO_TICKS(sec)       ((uint32_t)(sec) * 100U)
#define MS_TO_TICKS(ms)         ((uint32_t)(ms) / TIMER_TICK_MS)

/* Mảng cờ báo trạng thái hết hạn của từng timer */
extern uint8_t timer_flag[NUM_SOFTWARE_TIMERS];

/**
 * @brief  Khởi tạo toàn bộ bộ đếm timer mềm về 0
 */
void timer_init(void);

/**
 * @brief  Thiết lập giá trị đếm thời gian cho một timer mềm
 * @param  index: ID của timer (0 đến NUM_SOFTWARE_TIMERS - 1)
 * @param  duration: Thời gian chờ, tính theo ms (driver tự quy đổi ra tick 10ms)
 */
void setTimer(uint8_t index, uint32_t duration);

/**
 * @brief  Cập nhật bộ đếm các timer mềm (gọi định kỳ mỗi nhịp 10ms)
 */
void timerRun(void);

/**
 * @brief  Kiểm tra timer đã hết hạn chưa và tự động hạ cờ
 * @param  index: ID của timer (0 đến NUM_SOFTWARE_TIMERS - 1)
 * @retval 1 nếu timer hết hạn, 0 nếu chưa
 */
uint8_t isTimerExpired(uint8_t index);

#endif /* INC_SOFTWARE_TIMER_H_ */
