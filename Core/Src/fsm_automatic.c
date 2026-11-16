// #include "fsm_automatic.h"
#include "../Inc/fsm_automatic.h"
#include "../Inc/main_app.h"
#include "../Inc/software_timer.h"
#include "../Inc/traffic_light.h"

/*
 * Chế độ tự động: 1 task duy nhất fsm_automatic_run() (10ms, chạy suốt chương trình)
 *   - Không phải lượt của mình (cờ != MODE_AUTO): về AUTO_INIT rồi thoát
 *   - Đúng lượt: chạy máy trạng thái 4 pha
 *       RED1_GREEN2 (time_green) -> RED1_AMBER2 (time_amber)
 *    -> GREEN1_RED2 (time_green) -> AMBER1_RED2 (time_amber) -> lặp lại
 *   - Thời gian pha dùng TIMER_AUTO_PHASE, nhịp đếm lùi 1s dùng TIMER_AUTO_SECOND
 * Không ghi LCD: task LCD trong main_app đọc bộ đếm qua fsm_automatic_get_countdown().
 */

#define SEC_TO_MS(x)    ((uint32_t)(x) * 1000U)

/* ===== Trạng thái + bộ đếm lùi ===== */
static AutoState_t auto_state    = AUTO_INIT;
static uint16_t    road1_counter = 0;
static uint16_t    road2_counter = 0;

/* Vào một pha mới: bật đèn, nạp bộ đếm (nếu tuyến đó đổi màu), chạy lại 2 timer */
static void auto_enter(AutoState_t next)
{
    uint16_t phase_time = 0;

    switch (next) {
    case AUTO_RED1_GREEN2:
        traffic_set(ROAD_1, LIGHT_RED);
        traffic_set(ROAD_2, LIGHT_GREEN);
        road1_counter = time_red;
        road2_counter = time_green;
        phase_time    = time_green;
        break;
    case AUTO_RED1_AMBER2:
        traffic_set(ROAD_2, LIGHT_AMBER);       /* tuyến 1 vẫn đỏ, giữ bộ đếm */
        road2_counter = time_amber;
        phase_time    = time_amber;
        break;
    case AUTO_GREEN1_RED2:
        traffic_set(ROAD_1, LIGHT_GREEN);
        traffic_set(ROAD_2, LIGHT_RED);
        road1_counter = time_green;
        road2_counter = time_red;
        phase_time    = time_green;
        break;
    case AUTO_AMBER1_RED2:
        traffic_set(ROAD_1, LIGHT_AMBER);       /* tuyến 2 vẫn đỏ, giữ bộ đếm */
        road1_counter = time_amber;
        phase_time    = time_amber;
        break;
    case AUTO_INIT:
    default:
        break;
    }

    auto_state = next;
    setTimer(TIMER_AUTO_PHASE,  SEC_TO_MS(phase_time));
    setTimer(TIMER_AUTO_SECOND, SEC_TO_MS(1));
}

/* ===== Task function ===== */
void fsm_automatic_run(void)
{
    /* Không phải lượt của mình: reset để lần vào sau khởi động lại từ đầu */
    if (main_app_get_mode() != MODE_AUTO) {
        auto_state = AUTO_INIT;
        return;
    }

    /* Đếm lùi mỗi 1s (xử lý TRƯỚC chuyển pha: khi trùng tick thì đếm về 0 rồi pha mới nạp lại) */
    if (auto_state != AUTO_INIT && isTimerExpired(TIMER_AUTO_SECOND)) {
        if (road1_counter > 0) road1_counter--;
        if (road2_counter > 0) road2_counter--;
        setTimer(TIMER_AUTO_SECOND, SEC_TO_MS(1));
    }

    switch (auto_state) {
    case AUTO_INIT:
        auto_enter(AUTO_RED1_GREEN2);
        break;

    case AUTO_RED1_GREEN2:
        if (isTimerExpired(TIMER_AUTO_PHASE)) auto_enter(AUTO_RED1_AMBER2);
        break;

    case AUTO_RED1_AMBER2:
        if (isTimerExpired(TIMER_AUTO_PHASE)) auto_enter(AUTO_GREEN1_RED2);
        break;

    case AUTO_GREEN1_RED2:
        if (isTimerExpired(TIMER_AUTO_PHASE)) auto_enter(AUTO_AMBER1_RED2);
        break;

    case AUTO_AMBER1_RED2:
        if (isTimerExpired(TIMER_AUTO_PHASE)) auto_enter(AUTO_RED1_GREEN2);
        break;

    default:
        auto_state = AUTO_INIT;
        break;
    }
}

uint16_t fsm_automatic_get_countdown(uint8_t road){
    return (road == ROAD_1) ? road1_counter : road2_counter;
}
