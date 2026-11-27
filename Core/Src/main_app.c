//#include "main_app.h"
#include "../Inc/main_app.h"
#include "../Inc/scheduler.h"
#include "../Inc/button.h"
#include "../Inc/traffic_light.h"
#include "../Inc/lcd.h"
#include "../Inc/fsm_automatic.h"
#include "../Inc/fsm_manual.h"
#include "../Inc/fsm_tuning.h"
#include "../Inc/dht20.h"
#include <stdio.h>

/*
 * Ứng dụng chính: khởi chạy các task + cập nhật cờ chế độ (mode_flag ở global.h).
 *
 * 6 task chạy suốt chương trình (add 1 lần trong main_app_init, không bao giờ xóa):
 *   Button_Reading    (10ms) : polling nút - driver đọc chân + chống dội + bật cờ
 *   Button_Logic      (10ms) : nhấn nút Mode -> mode_flag = (mode_flag + 1) % 3
 *                              0 (AUTO) -> 1 (MANUAL) -> 2 (TUNING) -> 0 ...
 *   fsm_automatic_run (10ms) : tự polling cờ, == MODE_AUTO   thì chạy, không thì bỏ qua
 *   fsm_manual_run    (10ms) : tự polling cờ, == MODE_MANUAL thì chạy, không thì bỏ qua
 *   fsm_tuning_run    (10ms) : tự polling cờ, == MODE_TUNING thì chạy, không thì bỏ qua
 *   LCD_Display      (250ms) : nơi DUY NHẤT ghi LCD, vẽ theo cờ
 *
 * Không có task quản lý: việc mode nào được chạy do 3 fsm tự quyết khi polling cờ.
 * Thứ tự add = thứ tự chạy trong cùng 1 tick: đọc nút -> logic nút -> 3 fsm -> LCD
 */

/* ===== Quy đổi thời gian -> tick ===== */
#define MS(x)           ((x) / SCH_TICK_MS)

/* Cờ chế độ mode_flag (global.h): chỉ Button_Logic ghi, các fsm đọc trực tiếp */

/* Đọc bỏ cờ nút còn sót từ chế độ cũ, tránh chế độ mới hiểu nhầm là vừa nhấn */
static void flush_button_flags(void)
{
    (void)isButtonPressed(BTN_1);
    (void)isButtonPressed(BTN_2);
    (void)isButtonPressed(BTN_3);
    (void)isButtonPressed(BTN_4);
}

/* ===== Task functions ===== */
void Button_Reading(void)
{
    button_reading();
}

void Button_Logic(void)
{
    if (isButtonPressed(BTN_USER)) {
        mode_flag = (SystemMode_t)((mode_flag + 1) % NUM_MODES);
        flush_button_flags();
    }
}

/* Dòng 0: thông tin theo chế độ | Dòng 1: nhiệt độ, độ ẩm
 * In số thực bằng phần nguyên + 1 chữ số thập phân (newlib-nano mặc định không hỗ trợ %f) */
void LCD_Display(void)
{
    char line0[17];
    char line1[17];

    switch (mode_flag) {
    case MODE_AUTO:
        snprintf(line0, sizeof(line0), "AUTO R1:%2u R2:%2u",
                 fsm_automatic_get_countdown(ROAD_1),
                 fsm_automatic_get_countdown(ROAD_2));
        break;

    case MODE_MANUAL: {
        const char *s;
        switch (fsm_manual_get_state()) {
        case MANUAL_RED1_GREEN2:   s = "1:R 2:G"; break;
        case MANUAL_RED1_AMBER2:   s = "1:R 2:A"; break;
        case MANUAL_GREEN1_RED2:   s = "1:G 2:R"; break;
        case MANUAL_AMBER1_RED2:   s = "1:A 2:R"; break;
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

void Sensor_Reading(void)
{
    float temp = 0.0f, hum = 0.0f;
    if (dht20_read(&temp, &hum)) {
        current_temperature = temp;
        current_humidity    = hum;
    }
}

/* ===== sTask control ===== */
sTask task_Button_Reading    = { .pTask = Button_Reading   , .Delay = 0,       .Period = MS(10)   };
sTask task_Button_Logic      = { .pTask = Button_Logic     , .Delay = 0,       .Period = MS(10)   };
sTask task_FSM_Automatic     = { .pTask = fsm_automatic_run, .Delay = 0,       .Period = MS(10)   };
sTask task_FSM_Manual        = { .pTask = fsm_manual_run   , .Delay = 0,       .Period = MS(10)   };
sTask task_FSM_Tuning        = { .pTask = fsm_tuning_run   , .Delay = 0,       .Period = MS(10)   };
sTask task_LCD_Display       = { .pTask = LCD_Display      , .Delay = 0,       .Period = MS(250)  };
sTask task_Sensor_Reading    = { .pTask = Sensor_Reading   , .Delay = MS(100), .Period = MS(2000) };

static void add(sTask *t)
{
    t->TaskID = SCH_Add_Task(t->pTask, t->Delay, t->Period);
}

void main_app_init(void)
{
    SCH_Init();
    button_init();
    traffic_init();
    lcd_init();
    dht20_init();

    mode_flag     = MODE_AUTO;
    traffic_phase = PHASE_OFF;

    add(&task_Button_Reading);
    add(&task_Button_Logic);
    add(&task_FSM_Automatic);
    add(&task_FSM_Manual);
    add(&task_FSM_Tuning);
    add(&task_LCD_Display);
    add(&task_Sensor_Reading);
}
