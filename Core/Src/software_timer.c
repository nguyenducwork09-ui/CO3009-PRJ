/**
 * @file    software_timer.c
 * @brief   Trình điều khiển các bộ đếm thời gian mềm (Software Timers).
 * @details Mở rộng từ mô hình Lab 2 sang mảng 8 bộ đếm độc lập (NUM_SOFTWARE_TIMERS).
 *          - setTimer(): nạp thời gian (ms), quy đổi sang tick (10ms) và hạ cờ.
 *          - timerRun(): giảm biến đếm mỗi nhịp 10ms, bật cờ khi hết giờ.
 *          - isTimerExpired(): kiểm tra và tự động xóa cờ báo (auto clear).
 */

#include "software_timer.h"

/* Mảng đếm thời gian còn lại (tính bằng ticks) */
static uint32_t timer_counter[NUM_SOFTWARE_TIMERS] = {0};

/* Mảng cờ báo timer đã hết hạn (1: hết hạn, 0: đang chạy hoặc chưa đặt) */
uint8_t timer_flag[NUM_SOFTWARE_TIMERS] = {0};

void timer_init(void)
{
    for (uint8_t i = 0; i < NUM_SOFTWARE_TIMERS; i++) {
        timer_counter[i] = 0;
        timer_flag[i] = 0;
    }
}

void setTimer(uint8_t index, uint32_t duration)
{
    if (index >= NUM_SOFTWARE_TIMERS) {
        return;
    }

    if (duration == 0) {
        timer_counter[index] = 0;
        timer_flag[index] = 1; /* Hết hạn ngay lập tức */
    } else {
        /* Quy đổi thời gian từ mili-giây sang số nhịp tick 10ms */
        uint32_t ticks = duration / TIMER_TICK_MS;
        if (ticks == 0 && duration > 0) {
            ticks = 1;
        }
        timer_counter[index] = ticks;
        timer_flag[index] = 0;
    }
}

void timerRun(void)
{
    for (uint8_t i = 0; i < NUM_SOFTWARE_TIMERS; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] == 0) {
                timer_flag[i] = 1;
            }
        }
    }
}

uint8_t isTimerExpired(uint8_t index)
{
    if (index >= NUM_SOFTWARE_TIMERS) {
        return 0;
    }

    if (timer_flag[index] == 1) {
        timer_flag[index] = 0; /* Tự động hạ cờ sau khi đọc */
        return 1;
    }

    return 0;
}
