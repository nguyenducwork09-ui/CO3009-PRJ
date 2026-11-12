//#include "fsm_manual.h"
#include "../Inc/fsm_manual.h"
#include "../Inc/scheduler.h"
#include "../Inc/traffic_light.h"
#include "../Inc/button.h"

/*
 * 3 task polling nút (Đỏ / Vàng / Xanh): chạy mỗi 10ms, đổi trạng thái manual khi nút được nhấn.
 *   (Gán chân, đọc GPIO, chống dội do driver button.h / button.c đảm nhiệm)
 * 1 task hiển thị đèn: xuất trạng thái manual ra 2 cụm đèn.
 */

/* ===== Quy đổi thời gian -> tick (1 tick = 10ms) ===== */
#define TICK_MS         10
#define MS(x)           ((x) / TICK_MS)

/* ===== Trạng thái manual ===== */
static ManualState_t manual_state = MANUAL_RED1_GREEN2;

/* ===== Task functions ===== */
/* Nút Đỏ: Đỏ tuyến 1 - Xanh tuyến 2 */
void Button_Red_Polling(void)
{
    if (btn_red()) {
        manual_state = MANUAL_RED1_GREEN2;
    }
}

/* Nút Vàng: Vàng cả 2 tuyến */
void Button_Yellow_Polling(void)
{
    if (btn_yellow()) {
        manual_state = MANUAL_AMBER1_AMBER2;
    }
}

/* Nút Xanh: Xanh tuyến 1 - Đỏ tuyến 2 */
void Button_Green_Polling(void)
{
    if (btn_green()) {
        manual_state = MANUAL_GREEN1_RED2;
    }
}

/* Hiển thị đèn theo trạng thái manual */
void Manual_Light_Display(void)
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

/* ===== sTask control ===== */
/*                                   pTask                  Delay  Period    */
sTask task_Button_Red_Polling    = { Button_Red_Polling,    0,     MS(10)  };
sTask task_Button_Yellow_Polling = { Button_Yellow_Polling, 0,     MS(10)  };
sTask task_Button_Green_Polling  = { Button_Green_Polling,  0,     MS(10)  };
sTask task_Manual_Light_Display  = { Manual_Light_Display,  0,     MS(100) };

void fsm_manual_start(void){
    manual_state = MANUAL_RED1_GREEN2;

    /* Add 3 task polling nút trước */
    task_Button_Red_Polling.TaskID    = SCH_Add_Task(task_Button_Red_Polling.pTask,    task_Button_Red_Polling.Delay,    task_Button_Red_Polling.Period);
    task_Button_Yellow_Polling.TaskID = SCH_Add_Task(task_Button_Yellow_Polling.pTask, task_Button_Yellow_Polling.Delay, task_Button_Yellow_Polling.Period);
    task_Button_Green_Polling.TaskID  = SCH_Add_Task(task_Button_Green_Polling.pTask,  task_Button_Green_Polling.Delay,  task_Button_Green_Polling.Period);

    /* Add task hiển thị đèn sau cùng */
    task_Manual_Light_Display.TaskID  = SCH_Add_Task(task_Manual_Light_Display.pTask,  task_Manual_Light_Display.Delay,  task_Manual_Light_Display.Period);
}

void fsm_manual_end(void){
    SCH_Delete_Task(task_Button_Red_Polling.TaskID);
    SCH_Delete_Task(task_Button_Yellow_Polling.TaskID);
    SCH_Delete_Task(task_Button_Green_Polling.TaskID);
    SCH_Delete_Task(task_Manual_Light_Display.TaskID);
}
