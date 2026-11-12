// #include "fsm_automatic.h"
#include "../Inc/fsm_automatic.h"
#include "../Inc/scheduler.h"
#include "../Inc/traffic_light.h"
#include "../Inc/lcd.h"
#include <stdio.h>

/*
Draft:
1. Init task: (Red_Control, Green_Control, Yellow_Control, LED7SEG) x2
2. Add task
*/

/* ===== Quy đổi thời gian -> tick (1 tick = 10ms) ===== */
#define TICK_MS         10
#define SEC(x)          ((x) * 1000 / TICK_MS)

/* Chu kỳ 1 vòng đèn = Đỏ + Xanh + Vàng */
#define CYCLE           (DEFAULT_TIME_RED + DEFAULT_TIME_GREEN + DEFAULT_TIME_AMBER)

/* ===== Bộ đếm lùi hiển thị ===== */
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
void LED7SEG_1(void)
{
    char buf[17];
    snprintf(buf, sizeof(buf), "Road 1: %2u s   ", road1_counter);
    lcd_set_cursor(0, 0);
    lcd_send_string(buf);
    if (road1_counter > 0) road1_counter--;
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
void LED7SEG_2(void)
{
    char buf[17];
    snprintf(buf, sizeof(buf), "Road 2: %2u s   ", road2_counter);
    lcd_set_cursor(1, 0);
    lcd_send_string(buf);
    if (road2_counter > 0) road2_counter--;
}

/* ===== sTask control ===== */
/*                               pTask              Delay                                          Period      */
/* Road 1: Đỏ -> Xanh -> Vàng */
sTask task_Red_Control_1    = { Red_Control_1,    0,                                             SEC(CYCLE) };
sTask task_Green_Control_1  = { Green_Control_1,  SEC(DEFAULT_TIME_RED),                         SEC(CYCLE) };
sTask task_Yellow_Control_1 = { Yellow_Control_1, SEC(DEFAULT_TIME_RED + DEFAULT_TIME_GREEN),    SEC(CYCLE) };
sTask task_LED7SEG_1        = { LED7SEG_1,        0,                                             SEC(1)     };

/* Road 2: Xanh -> Vàng -> Đỏ */
sTask task_Green_Control_2  = { Green_Control_2,  0,                                             SEC(CYCLE) };
sTask task_Yellow_Control_2 = { Yellow_Control_2, SEC(DEFAULT_TIME_GREEN),                       SEC(CYCLE) };
sTask task_Red_Control_2    = { Red_Control_2,    SEC(DEFAULT_TIME_GREEN + DEFAULT_TIME_AMBER),  SEC(CYCLE) };
sTask task_LED7SEG_2        = { LED7SEG_2,        0,                                             SEC(1)     };

void fsm_automatic_start(void){
    /* Tính lại Delay/Period theo time_* hiện tại (có thể đã bị đổi ở chế độ Tuning) */
    uint32_t cycle = SEC(time_red + time_green + time_amber);

    task_Red_Control_1.Delay    = 0;                                task_Red_Control_1.Period    = cycle;
    task_Green_Control_1.Delay  = SEC(time_red);                    task_Green_Control_1.Period  = cycle;
    task_Yellow_Control_1.Delay = SEC(time_red + time_green);       task_Yellow_Control_1.Period = cycle;

    task_Green_Control_2.Delay  = 0;                                task_Green_Control_2.Period  = cycle;
    task_Yellow_Control_2.Delay = SEC(time_green);                  task_Yellow_Control_2.Period = cycle;
    task_Red_Control_2.Delay    = SEC(time_green + time_amber);     task_Red_Control_2.Period    = cycle;

    /* Add 6 task điều khiển màu trước */
    task_Red_Control_1.TaskID    = SCH_Add_Task(task_Red_Control_1.pTask,    task_Red_Control_1.Delay,    task_Red_Control_1.Period);
    task_Green_Control_1.TaskID  = SCH_Add_Task(task_Green_Control_1.pTask,  task_Green_Control_1.Delay,  task_Green_Control_1.Period);
    task_Yellow_Control_1.TaskID = SCH_Add_Task(task_Yellow_Control_1.pTask, task_Yellow_Control_1.Delay, task_Yellow_Control_1.Period);

    task_Green_Control_2.TaskID  = SCH_Add_Task(task_Green_Control_2.pTask,  task_Green_Control_2.Delay,  task_Green_Control_2.Period);
    task_Yellow_Control_2.TaskID = SCH_Add_Task(task_Yellow_Control_2.pTask, task_Yellow_Control_2.Delay, task_Yellow_Control_2.Period);
    task_Red_Control_2.TaskID    = SCH_Add_Task(task_Red_Control_2.pTask,    task_Red_Control_2.Delay,    task_Red_Control_2.Period);

    /* Add 2 task hiển thị sau cùng */
    task_LED7SEG_1.TaskID        = SCH_Add_Task(task_LED7SEG_1.pTask,        task_LED7SEG_1.Delay,        task_LED7SEG_1.Period);
    task_LED7SEG_2.TaskID        = SCH_Add_Task(task_LED7SEG_2.pTask,        task_LED7SEG_2.Delay,        task_LED7SEG_2.Period);
}

void fsm_automatic_end(void){
    SCH_Delete_Task(task_Red_Control_1.TaskID);
    SCH_Delete_Task(task_Green_Control_1.TaskID);
    SCH_Delete_Task(task_Yellow_Control_1.TaskID);

    SCH_Delete_Task(task_Green_Control_2.TaskID);
    SCH_Delete_Task(task_Yellow_Control_2.TaskID);
    SCH_Delete_Task(task_Red_Control_2.TaskID);

    SCH_Delete_Task(task_LED7SEG_1.TaskID);
    SCH_Delete_Task(task_LED7SEG_2.TaskID);
}
