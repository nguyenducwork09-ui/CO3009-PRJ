// #include "fsm_automatic.h"
#include "../Inc/fsm_automatic.h"
#include "../Inc/scheduler.h"
#include "../Inc/traffic_light.h"

/*
 * Chế độ tự động: 7 task
 *   - 6 task màu (Red/Green/Yellow_Control x 2 tuyến): mỗi task chỉ bật màu của mình
 *     và nạp lại bộ đếm lùi. Thời điểm chạy do Delay/Period quyết định.
 *   - 1 task Countdown: mỗi 1s giảm bộ đếm lùi của 2 tuyến.
 * Không ghi LCD: task LCD trong fsm_mode đọc bộ đếm qua fsm_automatic_get_countdown().
 */

/* ===== Quy đổi thời gian -> tick (1 tick = 10ms) ===== */
#define TICK_MS         10
#define SEC(x)          ((x) * 1000 / TICK_MS)

/* ===== Bộ đếm lùi ===== */
static uint16_t road1_counter = 0;
static uint16_t road2_counter = 0;

/* ===== Task functions ===== */
/* Road 1 */
void Red_Control_1(void)
{
    traffic_set(ROAD_1, LIGHT_RED);
    road1_counter = time_red;
}
void Green_Control_1(void)
{
    traffic_set(ROAD_1, LIGHT_GREEN);
    road1_counter = time_green;
}
void Yellow_Control_1(void)
{
    traffic_set(ROAD_1, LIGHT_AMBER);
    road1_counter = time_amber;
}

/* Road 2 */
void Red_Control_2(void)
{
    traffic_set(ROAD_2, LIGHT_RED);
    road2_counter = time_red;
}
void Green_Control_2(void)
{
    traffic_set(ROAD_2, LIGHT_GREEN);
    road2_counter = time_green;
}
void Yellow_Control_2(void)
{
    traffic_set(ROAD_2, LIGHT_AMBER);
    road2_counter = time_amber;
}

/* Đếm lùi cả 2 tuyến */
void Countdown(void)
{
    if (road1_counter > 0) road1_counter--;
    if (road2_counter > 0) road2_counter--;
}

/* ===== sTask control (Delay/Period được tính trong fsm_automatic_start) ===== */
/* Road 1: Đỏ -> Xanh -> Vàng */
sTask task_Red_Control_1    = { .pTask = Red_Control_1 };
sTask task_Green_Control_1  = { .pTask = Green_Control_1 };
sTask task_Yellow_Control_1 = { .pTask = Yellow_Control_1 };

/* Road 2: Xanh -> Vàng -> Đỏ */
sTask task_Green_Control_2  = { .pTask = Green_Control_2 };
sTask task_Yellow_Control_2 = { .pTask = Yellow_Control_2 };
sTask task_Red_Control_2    = { .pTask = Red_Control_2 };

sTask task_Countdown        = { .pTask = Countdown };

static void add(sTask *t, uint32_t delay, uint32_t period)
{
    t->Delay  = delay;
    t->Period = period;
    t->TaskID = SCH_Add_Task(t->pTask, t->Delay, t->Period);
}

void fsm_automatic_start(void){
    /* Tính Delay/Period theo time_* hiện tại (có thể đã bị đổi ở chế độ Tuning) */
    uint32_t cycle = SEC(time_red + time_green + time_amber);

    road1_counter = 0;
    road2_counter = 0;

    /* Countdown add TRƯỚC task màu: khi trùng tick, đếm về 0 rồi task màu mới nạp lại.
     * Delay = 1s để giây đầu tiên hiển thị đủ giá trị. */
    add(&task_Countdown,        SEC(1),                         SEC(1));

    add(&task_Red_Control_1,    0,                              cycle);
    add(&task_Green_Control_1,  SEC(time_red),                  cycle);
    add(&task_Yellow_Control_1, SEC(time_red + time_green),     cycle);

    add(&task_Green_Control_2,  0,                              cycle);
    add(&task_Yellow_Control_2, SEC(time_green),                cycle);
    add(&task_Red_Control_2,    SEC(time_green + time_amber),   cycle);
}

void fsm_automatic_end(void){
    SCH_Delete_Task(task_Countdown.TaskID);

    SCH_Delete_Task(task_Red_Control_1.TaskID);
    SCH_Delete_Task(task_Green_Control_1.TaskID);
    SCH_Delete_Task(task_Yellow_Control_1.TaskID);

    SCH_Delete_Task(task_Green_Control_2.TaskID);
    SCH_Delete_Task(task_Yellow_Control_2.TaskID);
    SCH_Delete_Task(task_Red_Control_2.TaskID);
}

uint16_t fsm_automatic_get_countdown(uint8_t road){
    return (road == ROAD_1) ? road1_counter : road2_counter;
}
