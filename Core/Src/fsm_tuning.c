//#include "fsm_tuning.h"
#include "../Inc/fsm_tuning.h"
#include "../Inc/scheduler.h"
#include "../Inc/traffic_light.h"
#include "../Inc/button.h"

/*
 * Chế độ chỉnh thời gian: 2 task
 *   - Tuning_Button_Handler (10ms): đọc cờ nút từ driver
 *       BTN_1 (Up)  : tăng giá trị thời gian của đèn đang chỉnh
 *       BTN_2 (Down): giảm giá trị
 *       BTN_3 (Set) : lưu giá trị rồi chuyển sang đèn kế tiếp (RED -> AMBER -> GREEN -> RED)
 *   - Tuning_Blink (500ms): chớp đèn đang chỉnh ở cả 2 tuyến
 * Không ghi LCD: task LCD trong fsm_mode đọc qua fsm_tuning_get_state()/get_value().
 *
 * Ràng buộc để 2 tuyến đồng bộ ở chế độ tự động: time_red = time_green + time_amber
 *   - Lưu RED         -> tính lại time_green = time_red - time_amber
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

/* Lưu giá trị đang chỉnh + chuyển sang đèn kế tiếp */
static void tuning_save_and_next(void)
{
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

/* ===== Task functions ===== */
void Tuning_Button_Handler(void)
{
    uint16_t min, max;
    tuning_limits(&min, &max);

    if (isButtonPressed(BTN_1)) {           /* Up: vượt max thì quay về min */
        tuning_value = (tuning_value >= max) ? min : tuning_value + 1;
    }
    if (isButtonPressed(BTN_2)) {           /* Down: nhỏ hơn min thì quay về max */
        tuning_value = (tuning_value <= min) ? max : tuning_value - 1;
    }
    if (isButtonPressed(BTN_3)) {           /* Set */
        tuning_save_and_next();
    }
}

/* Chớp đèn đang chỉnh: 500ms sáng / 500ms tắt */
void Tuning_Blink(void)
{
    LightColor_t color;

    switch (tuning_state) {
    case TUNING_RED:   color = LIGHT_RED;   break;
    case TUNING_AMBER: color = LIGHT_AMBER; break;
    case TUNING_GREEN: color = LIGHT_GREEN; break;
    default:           color = LIGHT_OFF;   break;
    }

    blink_on = !blink_on;
    if (blink_on) {
        traffic_set(ROAD_1, color);
        traffic_set(ROAD_2, color);
    } else {
        traffic_turn_off_all();
    }
}

/* ===== sTask control ===== */
/*                                   pTask                  Delay  Period   */
sTask task_Tuning_Button_Handler = { Tuning_Button_Handler, 0,     MS(10)  };
sTask task_Tuning_Blink          = { Tuning_Blink,          0,     MS(500) };

void fsm_tuning_start(void){
    tuning_state = TUNING_RED;
    tuning_load();
    blink_on = 0;

    task_Tuning_Button_Handler.TaskID = SCH_Add_Task(task_Tuning_Button_Handler.pTask,
                                                     task_Tuning_Button_Handler.Delay,
                                                     task_Tuning_Button_Handler.Period);
    task_Tuning_Blink.TaskID          = SCH_Add_Task(task_Tuning_Blink.pTask,
                                                     task_Tuning_Blink.Delay,
                                                     task_Tuning_Blink.Period);
}

void fsm_tuning_end(void){
    SCH_Delete_Task(task_Tuning_Button_Handler.TaskID);
    SCH_Delete_Task(task_Tuning_Blink.TaskID);
}

TuningState_t fsm_tuning_get_state(void){
    return tuning_state;
}

uint16_t fsm_tuning_get_value(void){
    return tuning_value;
}
