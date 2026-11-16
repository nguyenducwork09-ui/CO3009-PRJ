//#include "fsm_manual.h"
#include "../Inc/fsm_manual.h"
#include "../Inc/scheduler.h"
#include "../Inc/traffic_light.h"
#include "../Inc/button.h"

/*
 * Chế độ bằng tay: 1 task xử lý nút (đọc cờ từ driver, chạy mỗi 10ms)
 *   BTN_1 (Đỏ) : Đỏ tuyến 1 - Xanh tuyến 2
 *   BTN_2 (Vàng): Vàng cả 2 tuyến
 *   BTN_3 (Xanh): Xanh tuyến 1 - Đỏ tuyến 2
 * Việc đọc nút + chống dội do task button_reading (fsm_mode) lo.
 */

/* ===== Quy đổi thời gian -> tick (1 tick = 10ms) ===== */
#define TICK_MS         10
#define MS(x)           ((x) / TICK_MS)

/* ===== Trạng thái manual ===== */
static ManualState_t manual_state = MANUAL_RED1_GREEN2;

/* Xuất trạng thái ra 2 cụm đèn */
static void manual_apply(void)
{
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
void Manual_Button_Handler(void)
{
    if (isButtonPressed(BTN_1)) {
        manual_state = MANUAL_RED1_GREEN2;
        manual_apply();
    }
    if (isButtonPressed(BTN_2)) {
        manual_state = MANUAL_AMBER1_AMBER2;
        manual_apply();
    }
    if (isButtonPressed(BTN_3)) {
        manual_state = MANUAL_GREEN1_RED2;
        manual_apply();
    }
}

/* ===== sTask control ===== */
/*                                   pTask                  Delay  Period  */
sTask task_Manual_Button_Handler = { Manual_Button_Handler, 0,     MS(10) };

void fsm_manual_start(void){
    manual_state = MANUAL_RED1_GREEN2;
    manual_apply();

    task_Manual_Button_Handler.TaskID = SCH_Add_Task(task_Manual_Button_Handler.pTask,
                                                     task_Manual_Button_Handler.Delay,
                                                     task_Manual_Button_Handler.Period);
}

void fsm_manual_end(void){
    SCH_Delete_Task(task_Manual_Button_Handler.TaskID);
}

ManualState_t fsm_manual_get_state(void){
    return manual_state;
}
