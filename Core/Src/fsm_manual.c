//#include "fsm_manual.h"
#include "../Inc/fsm_manual.h"
#include "../Inc/main_app.h"
#include "../Inc/traffic_light.h"
#include "../Inc/button.h"

/*
 * Chế độ bằng tay: 1 task duy nhất fsm_manual_run() (10ms, chạy suốt chương trình)
 *   - Không phải lượt của mình (cờ != MODE_MANUAL): về MANUAL_INIT rồi thoát
 *   - Đúng lượt: đọc cờ nút từ driver
 *       BTN_1 (Đỏ) : Đỏ tuyến 1 - Xanh tuyến 2
 *       BTN_2 (Vàng): Vàng cả 2 tuyến
 *       BTN_3 (Xanh): Xanh tuyến 1 - Đỏ tuyến 2
 * Việc đọc nút + chống dội do task Button_Reading (main_app) lo.
 * Chế độ này chỉ phản ứng theo nút, không cần software timer.
 */

/* ===== Trạng thái manual ===== */
static ManualState_t manual_state = MANUAL_INIT;

/* Chuyển trạng thái + xuất ra 2 cụm đèn */
static void manual_enter(ManualState_t next)
{
    manual_state = next;

    switch (manual_state) {
    case MANUAL_RED1_GREEN2:
        traffic_set(ROAD_1, LIGHT_RED);
        traffic_set(ROAD_2, LIGHT_GREEN);
        break;
    case MANUAL_GREEN1_RED2:
        traffic_set(ROAD_1, LIGHT_GREEN);
        traffic_set(ROAD_2, LIGHT_RED);
        break;
    case MANUAL_AMBER1_AMBER2:
        traffic_set(ROAD_1, LIGHT_AMBER);
        traffic_set(ROAD_2, LIGHT_AMBER);
        break;
    case MANUAL_INIT:
    default:
        traffic_turn_off_all();
        break;
    }
}

/* ===== Task function ===== */
void fsm_manual_run(void)
{
    /* Không phải lượt của mình: reset để lần vào sau khởi động lại từ đầu */
    if (main_app_get_mode() != MODE_MANUAL) {
        manual_state = MANUAL_INIT;
        return;
    }

    if (manual_state == MANUAL_INIT) {
        manual_enter(MANUAL_RED1_GREEN2);
        return;
    }

    if (isButtonPressed(BTN_1)) manual_enter(MANUAL_RED1_GREEN2);
    if (isButtonPressed(BTN_2)) manual_enter(MANUAL_AMBER1_AMBER2);
    if (isButtonPressed(BTN_3)) manual_enter(MANUAL_GREEN1_RED2);
}

ManualState_t fsm_manual_get_state(void){
    return manual_state;
}
