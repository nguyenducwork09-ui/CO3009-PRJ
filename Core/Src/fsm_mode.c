//#include "fsm_mode.h"
#include "../Inc/fsm_mode.h"
#include "../Inc/scheduler.h"
#include "../Inc/button.h"
#include "../Inc/lcd.h"
#include "../Inc/fsm_automatic.h"
#include "../Inc/fsm_manual.h"
#include "../Inc/fsm_tuning.h"

/*
 * Đổi fsm = fsm_<cũ>_end() (xóa task) + fsm_<mới>_start() (add task)
 * Task polling nút Mode được add 1 lần và không bao giờ bị xóa.
 */

/* ===== Quy đổi thời gian -> tick (1 tick = 10ms) ===== */
#define TICK_MS         10
#define MS(x)           ((x) / TICK_MS)

/* ===== Gọi start / end theo chế độ ===== */
static void mode_start(SystemMode_t mode)
{
    switch (mode) {
    case MODE_AUTO:   fsm_automatic_start(); break;
    case MODE_MANUAL: fsm_manual_start();    break;
    case MODE_TUNING: fsm_tuning_start();    break;
    default:          break;
    }
}

static void mode_end(SystemMode_t mode)
{
    switch (mode) {
    case MODE_AUTO:   fsm_automatic_end(); break;
    case MODE_MANUAL: fsm_manual_end();    break;
    case MODE_TUNING: fsm_tuning_end();    break;
    default:          break;
    }
}

void fsm_mode_switch(SystemMode_t new_mode)
{
    if (new_mode == current_mode) return;

    mode_end(current_mode);
    lcd_clear();                /* mỗi chế độ hiển thị LCD khác nhau */
    current_mode = new_mode;
    mode_start(current_mode);
}

/* ===== Task function ===== */
void Button_Mode_Polling(void)
{
    if (btn_mode()) {
        switch (current_mode) {
        case MODE_AUTO:   fsm_mode_switch(MODE_MANUAL); break;
        case MODE_MANUAL: fsm_mode_switch(MODE_TUNING); break;
        case MODE_TUNING: fsm_mode_switch(MODE_AUTO);   break;
        default:          fsm_mode_switch(MODE_AUTO);   break;
        }
    }
}

/* ===== sTask control ===== */
/*                               pTask                Delay  Period   */
sTask task_Button_Mode_Polling = { Button_Mode_Polling, 0,     MS(10) };

void fsm_mode_init(void)
{
    task_Button_Mode_Polling.TaskID = SCH_Add_Task(task_Button_Mode_Polling.pTask,
                                                   task_Button_Mode_Polling.Delay,
                                                   task_Button_Mode_Polling.Period);
    current_mode = MODE_AUTO;
    mode_start(current_mode);
}
