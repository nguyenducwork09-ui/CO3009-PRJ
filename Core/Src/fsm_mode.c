//#include "fsm_mode.h"
#include "../Inc/fsm_mode.h"
#include "../Inc/scheduler.h"
#include "../Inc/button.h"
#include "../Inc/traffic_light.h"
#include "../Inc/lcd.h"
#include "../Inc/fsm_automatic.h"
#include "../Inc/fsm_manual.h"
#include "../Inc/fsm_tuning.h"
#include <stdio.h>

/*
 * File tổng:
 *   - 3 task chạy suốt chương trình (add 1 lần trong fsm_init, không bao giờ xóa):
 *       Button_Reading (10ms) : driver đọc nút + chống dội + bật cờ
 *       Mode_Handler   (10ms) : nhấn BTN_USER -> AUTO -> MANUAL -> TUNING -> AUTO
 *       LCD_Display    (250ms): nơi DUY NHẤT ghi LCD, vẽ theo current_mode
 *   - Đổi fsm = fsm_<cũ>_end() (xóa task) + fsm_<mới>_start() (add task)
 *
 * Số task tối đa: 3 (chạy suốt) + 7 (AUTO) = 10 = SCH_MAX_TASKS
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

/* Đọc bỏ cờ nút còn sót từ chế độ cũ, tránh chế độ mới hiểu nhầm là vừa nhấn */
static void flush_button_flags(void)
{
    (void)isButtonPressed(BTN_1);
    (void)isButtonPressed(BTN_2);
    (void)isButtonPressed(BTN_3);
    (void)isButtonPressed(BTN_4);
}

void fsm_mode_switch(SystemMode_t new_mode)
{
    if (new_mode == current_mode) return;

    mode_end(current_mode);
    flush_button_flags();
    current_mode = new_mode;
    mode_start(current_mode);
}

/* ===== Task functions ===== */
void Button_Reading(void)
{
    button_reading();
}

void Mode_Handler(void)
{
    if (isButtonPressed(BTN_USER)) {
        switch (current_mode) {
        case MODE_AUTO:   fsm_mode_switch(MODE_MANUAL); break;
        case MODE_MANUAL: fsm_mode_switch(MODE_TUNING); break;
        case MODE_TUNING: fsm_mode_switch(MODE_AUTO);   break;
        default:          fsm_mode_switch(MODE_AUTO);   break;
        }
    }
}

/* Dòng 0: thông tin theo chế độ | Dòng 1: nhiệt độ, độ ẩm
 * In số thực bằng phần nguyên + 1 chữ số thập phân (newlib-nano mặc định không hỗ trợ %f) */
void LCD_Display(void)
{
    char line0[17];
    char line1[17];

    switch (current_mode) {
    case MODE_AUTO:
        snprintf(line0, sizeof(line0), "AUTO R1:%2u R2:%2u",
                 fsm_automatic_get_countdown(ROAD_1),
                 fsm_automatic_get_countdown(ROAD_2));
        break;

    case MODE_MANUAL: {
        const char *s;
        switch (fsm_manual_get_state()) {
        case MANUAL_RED1_GREEN2:   s = "1:R 2:G"; break;
        case MANUAL_GREEN1_RED2:   s = "1:G 2:R"; break;
        case MANUAL_AMBER1_AMBER2: s = "1:A 2:A"; break;
        default:                   s = "1:- 2:-"; break;
        }
        snprintf(line0, sizeof(line0), "MANUAL   %s", s);
        break;
    }

    case MODE_TUNING: {
        const char *s;
        switch (fsm_tuning_get_state()) {
        case TUNING_RED:   s = "RED  "; break;
        case TUNING_AMBER: s = "AMBER"; break;
        case TUNING_GREEN: s = "GREEN"; break;
        default:           s = "-----"; break;
        }
        snprintf(line0, sizeof(line0), "TUNE %s: %2u s", s, fsm_tuning_get_value());
        break;
    }

    default:
        snprintf(line0, sizeof(line0), "                ");
        break;
    }

    int t10 = (int)(current_temperature * 10.0f);
    int h10 = (int)(current_humidity    * 10.0f);
    snprintf(line1, sizeof(line1), "T:%2d.%dC H:%2d.%d%%",
             t10 / 10, (t10 < 0 ? -t10 : t10) % 10,
             h10 / 10, h10 % 10);

    /* Ghi đè đủ 16 ký tự mỗi dòng, không lcd_clear() để tránh nháy màn hình */
    char pad0[17], pad1[17];
    snprintf(pad0, sizeof(pad0), "%-16s", line0);
    snprintf(pad1, sizeof(pad1), "%-16s", line1);

    lcd_set_cursor(0, 0);
    lcd_send_string(pad0);
    lcd_set_cursor(1, 0);
    lcd_send_string(pad1);
}

/* ===== sTask control ===== */
/*                             pTask           Delay  Period   */
sTask task_Button_Reading = { Button_Reading, 0,     MS(10)  };
sTask task_Mode_Handler   = { Mode_Handler,   0,     MS(10)  };
sTask task_LCD_Display    = { LCD_Display,    0,     MS(250) };

void fsm_init(void)
{
    SCH_Init();
    button_init();
    traffic_init();
    lcd_init();

    /* Thứ tự add: đọc nút trước, xử lý nút sau, LCD cuối */
    task_Button_Reading.TaskID = SCH_Add_Task(task_Button_Reading.pTask, task_Button_Reading.Delay, task_Button_Reading.Period);
    task_Mode_Handler.TaskID   = SCH_Add_Task(task_Mode_Handler.pTask,   task_Mode_Handler.Delay,   task_Mode_Handler.Period);
    task_LCD_Display.TaskID    = SCH_Add_Task(task_LCD_Display.pTask,    task_LCD_Display.Delay,    task_LCD_Display.Period);

    current_mode = MODE_AUTO;
    mode_start(current_mode);
}
