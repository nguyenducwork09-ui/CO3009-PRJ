//#include "fsm_tuning.h"
#include "../Inc/fsm_tuning.h"
#include "../Inc/main_app.h"
#include "../Inc/software_timer.h"
#include "../Inc/traffic_light.h"
#include "../Inc/button.h"

/*
 * Chế độ chỉnh thời gian: 1 task duy nhất fsm_tuning_run() (10ms, chạy suốt chương trình)
 *   - Không phải lượt của mình (cờ != MODE_TUNING): về TUNING_INIT rồi thoát
 *   - Đúng lượt:
 *       BTN_1 (Up)  : tăng giá trị thời gian của đèn đang chỉnh
 *       BTN_2 (Down): giảm giá trị
 *       BTN_3 (Set) : lưu giá trị rồi chuyển sang đèn kế tiếp (RED -> AMBER -> GREEN -> RED)
 *       TIMER_TUNING_BLINK hết hạn (500ms): đảo trạng thái chớp đèn đang chỉnh ở cả 2 tuyến
 * Không ghi LCD: task LCD trong main_app đọc qua fsm_tuning_get_state()/get_value().
 *
 * Ràng buộc để 2 tuyến đồng bộ ở chế độ tự động: time_red = time_green + time_amber
 *   - Lưu RED         -> tính lại time_green = time_red - time_amber
 *   - Lưu AMBER/GREEN -> tính lại time_red  = time_green + time_amber
 */

#define TIME_MAX        99      /* giây, vừa 2 chữ số */
#define BLINK_MS        500

/* ===== Trạng thái tuning ===== */
static TuningState_t tuning_state = TUNING_INIT;
static uint16_t      tuning_value = DEFAULT_TIME_RED;   /* giá trị đang chỉnh (chưa lưu) */
static uint8_t       blink_on     = 0;

/* Giới hạn [min, max] của giá trị đang chỉnh để giữ được ràng buộc ở trên */
static void tuning_limits(uint16_t *min, uint16_t *max)
{
    switch (tuning_state) {
    case TUNING_RED:   *min = time_amber + 1; *max = TIME_MAX;              break;
    case TUNING_AMBER: *min = 1;              *max = TIME_MAX - time_green; break;
    case TUNING_GREEN: *min = 1;              *max = TIME_MAX - time_amber; break;
    default:           *min = 1;              *max = TIME_MAX;              break;
    }
}

/* Nạp giá trị hiện tại của đèn đang chỉnh vào tuning_value */
static void tuning_load(void)
{
    switch (tuning_state) {
    case TUNING_RED:   tuning_value = time_red;   break;
    case TUNING_AMBER: tuning_value = time_amber; break;
    case TUNING_GREEN: tuning_value = time_green; break;
    default:           break;
    }
}

/* Lưu giá trị đang chỉnh + chuyển sang đèn kế tiếp */
static void tuning_save_and_next(void)
{
    switch (tuning_state) {
    case TUNING_RED:
        time_red     = tuning_value;
        time_green   = time_red - time_amber;
        tuning_state = TUNING_AMBER;
        break;
    case TUNING_AMBER:
        time_amber   = tuning_value;
        time_red     = time_green + time_amber;
        tuning_state = TUNING_GREEN;
        break;
    case TUNING_GREEN:
        time_green   = tuning_value;
        time_red     = time_green + time_amber;
        tuning_state = TUNING_RED;
        break;
    default:
        tuning_state = TUNING_RED;
        break;
    }
    tuning_load();
}

/* Chớp đèn đang chỉnh: đảo sáng/tắt mỗi lần gọi */
static void tuning_blink(void)
{
    LightColor_t color;

    switch (tuning_state) {
    case TUNING_RED:   color = LIGHT_RED;   break;
    case TUNING_AMBER: color = LIGHT_AMBER; break;
    case TUNING_GREEN: color = LIGHT_GREEN; break;
    default:           color = LIGHT_OFF;   break;
    }

    blink_on = !blink_on;
    if (blink_on) {
        traffic_set(ROAD_1, color);
        traffic_set(ROAD_2, color);
    } else {
        traffic_turn_off_all();
    }
}

/* ===== Task function ===== */
void fsm_tuning_run(void)
{
    /* Không phải lượt của mình: reset để lần vào sau khởi động lại từ đầu */
    if (main_app_get_mode() != MODE_TUNING) {
        tuning_state = TUNING_INIT;
        return;
    }

    /* Vừa vào chế độ: bắt đầu chỉnh đèn đỏ, tắt đèn, chạy timer chớp */
    if (tuning_state == TUNING_INIT) {
        tuning_state = TUNING_RED;
        tuning_load();
        blink_on = 0;
        traffic_turn_off_all();
        setTimer(TIMER_TUNING_BLINK, BLINK_MS);
        return;
    }

    /* Xử lý nút */
    uint16_t min, max;
    tuning_limits(&min, &max);

    if (isButtonPressed(BTN_1)) {           /* Up: vượt max thì quay về min */
        tuning_value = (tuning_value >= max) ? min : tuning_value + 1;
    }
    if (isButtonPressed(BTN_2)) {           /* Down: nhỏ hơn min thì quay về max */
        tuning_value = (tuning_value <= min) ? max : tuning_value - 1;
    }
    if (isButtonPressed(BTN_3)) {           /* Set */
        tuning_save_and_next();
    }

    /* Chớp đèn theo software timer */
    if (isTimerExpired(TIMER_TUNING_BLINK)) {
        tuning_blink();
        setTimer(TIMER_TUNING_BLINK, BLINK_MS);
    }
}

TuningState_t fsm_tuning_get_state(void){
    return tuning_state;
}

uint16_t fsm_tuning_get_value(void){
    return tuning_value;
}
