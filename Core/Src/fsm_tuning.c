//#include "fsm_tuning.h"
#include "../Inc/fsm_tuning.h"
#include "../Inc/scheduler.h"
#include "../Inc/traffic_light.h"
#include "../Inc/button.h"
#include "../Inc/lcd.h"
#include <stdio.h>

/*
 * 3 task polling nút (Up / Down / Set): chạy mỗi 10ms.
 *   - Up/Down: tăng/giảm giá trị thời gian của đèn đang chỉnh.
 *   - Set    : lưu giá trị rồi chuyển sang đèn kế tiếp (RED -> AMBER -> GREEN -> RED).
 * 1 task hiển thị: chạy mỗi 500ms, chớp đèn đang chỉnh ở cả 2 tuyến + in giá trị lên LCD.
 *
 * Ràng buộc để 2 tuyến đồng bộ ở chế độ tự động: time_red = time_green + time_amber
 *   - Lưu RED        -> tính lại time_green = time_red - time_amber
 *   - Lưu AMBER/GREEN -> tính lại time_red  = time_green + time_amber
 */

/* ===== Quy đổi thời gian -> tick (1 tick = 10ms) ===== */
#define TICK_MS         10
#define MS(x)           ((x) / TICK_MS)

#define TIME_MAX        99      /* giây, vừa 2 chữ số */

/* ===== Trạng thái tuning ===== */
static TuningState_t tuning_state = TUNING_RED;
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

/* ===== Task functions ===== */
/* Nút Up: tăng giá trị, vượt max thì quay về min */
void Button_Up_Polling(void)
{
    if (btn_up()) {
        uint16_t min, max;
        tuning_limits(&min, &max);
        tuning_value = (tuning_value >= max) ? min : tuning_value + 1;
    }
}

/* Nút Down: giảm giá trị, nhỏ hơn min thì quay về max */
void Button_Down_Polling(void)
{
    if (btn_down()) {
        uint16_t min, max;
        tuning_limits(&min, &max);
        tuning_value = (tuning_value <= min) ? max : tuning_value - 1;
    }
}

/* Nút Set: lưu giá trị, chuyển sang đèn kế tiếp */
void Button_Set_Polling(void)
{
    if (btn_set()) {
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
}

/* Hiển thị: chớp đèn đang chỉnh (500ms sáng / 500ms tắt) + in giá trị lên LCD */
void Tuning_Display(void)
{
    LightColor_t color;
    const char  *name;
    char         buf[17];

    switch (tuning_state) {
    case TUNING_RED:   color = LIGHT_RED;   name = "RED  "; break;
    case TUNING_AMBER: color = LIGHT_AMBER; name = "AMBER"; break;
    case TUNING_GREEN: color = LIGHT_GREEN; name = "GREEN"; break;
    default:           color = LIGHT_OFF;   name = "-----"; break;
    }

    blink_on = !blink_on;
    if (blink_on) {
        traffic_set(ROAD_1, color);
        traffic_set(ROAD_2, color);
    } else {
        traffic_turn_off_all();
    }

    snprintf(buf, sizeof(buf), "TUNING %s    ", name);
    lcd_set_cursor(0, 0);
    lcd_send_string(buf);

    snprintf(buf, sizeof(buf), "Value: %2u s     ", tuning_value);
    lcd_set_cursor(1, 0);
    lcd_send_string(buf);
}

/* ===== sTask control ===== */
/*                                 pTask                Delay  Period    */
sTask task_Button_Up_Polling   = { Button_Up_Polling,   0,     MS(10)  };
sTask task_Button_Down_Polling = { Button_Down_Polling, 0,     MS(10)  };
sTask task_Button_Set_Polling  = { Button_Set_Polling,  0,     MS(10)  };
sTask task_Tuning_Display      = { Tuning_Display,      0,     MS(500) };

void fsm_tuning_start(void){
    tuning_state = TUNING_RED;
    tuning_load();
    blink_on = 0;

    /* Add 3 task polling nút trước */
    task_Button_Up_Polling.TaskID   = SCH_Add_Task(task_Button_Up_Polling.pTask,   task_Button_Up_Polling.Delay,   task_Button_Up_Polling.Period);
    task_Button_Down_Polling.TaskID = SCH_Add_Task(task_Button_Down_Polling.pTask, task_Button_Down_Polling.Delay, task_Button_Down_Polling.Period);
    task_Button_Set_Polling.TaskID  = SCH_Add_Task(task_Button_Set_Polling.pTask,  task_Button_Set_Polling.Delay,  task_Button_Set_Polling.Period);

    /* Add task hiển thị sau cùng */
    task_Tuning_Display.TaskID      = SCH_Add_Task(task_Tuning_Display.pTask,      task_Tuning_Display.Delay,      task_Tuning_Display.Period);
}

void fsm_tuning_end(void){
    SCH_Delete_Task(task_Button_Up_Polling.TaskID);
    SCH_Delete_Task(task_Button_Down_Polling.TaskID);
    SCH_Delete_Task(task_Button_Set_Polling.TaskID);
    SCH_Delete_Task(task_Tuning_Display.TaskID);
}
