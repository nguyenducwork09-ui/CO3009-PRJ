//#include "fsm_manual.h"
#include "../Inc/fsm_manual.h"
#include "../Inc/traffic_light.h"
#include "../Inc/button.h"

/*
 * Chế độ bằng tay: 1 task duy nhất fsm_manual_run() (10ms, chạy suốt chương trình)
 *   - Không phải lượt của mình (mode_flag != MODE_MANUAL): về MANUAL_INIT rồi thoát
 *   - Vừa vào chế độ: GIỮ NGUYÊN pha đèn hiện tại (đọc traffic_phase ở global),
 *     không đổi đèn đột ngột khi chuyển từ tự động sang
 *   - Đúng lượt: đọc cờ nút từ driver
 *       BTN_1 (Đỏ) : Đỏ tuyến 1 - Xanh tuyến 2
 *       BTN_2 (Vàng): Vàng cả 2 tuyến
 *       BTN_3 (Xanh): Xanh tuyến 1 - Đỏ tuyến 2
 * Việc đọc nút + chống dội do task Button_Reading (main_app) lo.
 * Chế độ này chỉ phản ứng theo nút, không cần software timer.
 */

/* ===== Trạng thái manual ===== */
static ManualState_t manual_state = MANUAL_INIT;

/* Chuyển trạng thái + xuất ra 2 cụm đèn + cập nhật pha đèn global */
static void manual_enter(ManualState_t next)
{
    manual_state = next;

    switch (manual_state) {
    case MANUAL_RED1_GREEN2:
        traffic_set(ROAD_1, LIGHT_RED);
        traffic_set(ROAD_2, LIGHT_GREEN);
        traffic_phase = PHASE_RED1_GREEN2;
        break;
    case MANUAL_RED1_AMBER2:
        traffic_set(ROAD_1, LIGHT_RED);
        traffic_set(ROAD_2, LIGHT_AMBER);
        traffic_phase = PHASE_RED1_AMBER2;
        break;
    case MANUAL_GREEN1_RED2:
        traffic_set(ROAD_1, LIGHT_GREEN);
        traffic_set(ROAD_2, LIGHT_RED);
        traffic_phase = PHASE_GREEN1_RED2;
        break;
    case MANUAL_AMBER1_RED2:
        traffic_set(ROAD_1, LIGHT_AMBER);
        traffic_set(ROAD_2, LIGHT_RED);
        traffic_phase = PHASE_AMBER1_RED2;
        break;
    case MANUAL_AMBER1_AMBER2:
        traffic_set(ROAD_1, LIGHT_AMBER);
        traffic_set(ROAD_2, LIGHT_AMBER);
        traffic_phase = PHASE_AMBER1_AMBER2;
        break;
    case MANUAL_INIT:
    default:
        traffic_turn_off_all();
        traffic_phase = PHASE_OFF;
        break;
    }
}

/* Quy pha đèn hiện tại (global) về trạng thái manual tương ứng */
static ManualState_t manual_from_phase(TrafficPhase_t phase)
{
    switch (phase) {
    case PHASE_RED1_GREEN2:   return MANUAL_RED1_GREEN2;
    case PHASE_RED1_AMBER2:   return MANUAL_RED1_AMBER2;
    case PHASE_GREEN1_RED2:   return MANUAL_GREEN1_RED2;
    case PHASE_AMBER1_RED2:   return MANUAL_AMBER1_RED2;
    case PHASE_AMBER1_AMBER2: return MANUAL_AMBER1_AMBER2;
    case PHASE_OFF:
    default:                  return MANUAL_RED1_GREEN2;  /* chưa có pha nào: mặc định an toàn */
    }
}

/* ===== Task function ===== */
void fsm_manual_run(void)
{
    /* Không phải lượt của mình: reset để lần vào sau khởi động lại từ đầu */
    if (mode_flag != MODE_MANUAL) {
        manual_state = MANUAL_INIT;
        return;
    }

    /* Vừa vào chế độ: giữ nguyên đèn đang sáng */
    if (manual_state == MANUAL_INIT) {
        manual_enter(manual_from_phase(traffic_phase));
        return;
    }

    if (isButtonPressed(BTN_1)) manual_enter(MANUAL_RED1_GREEN2);
    if (isButtonPressed(BTN_2)) manual_enter(MANUAL_AMBER1_AMBER2);
    if (isButtonPressed(BTN_3)) manual_enter(MANUAL_GREEN1_RED2);
}

ManualState_t fsm_manual_get_state(void){
    return manual_state;
}
