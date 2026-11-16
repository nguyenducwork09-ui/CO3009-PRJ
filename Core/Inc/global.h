#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <stdint.h>

/* System Operating Modes */
typedef enum {
    MODE_AUTO = 0,
    MODE_MANUAL,
    MODE_TUNING
} SystemMode_t;

#define NUM_MODES               3

/* Pha đèn hiện tại của 2 tuyến (tổ hợp màu tuyến 1 - tuyến 2) */
typedef enum {
    PHASE_OFF = 0,              /* tắt / không ở pha giao thông nào (vd: đang tuning) */
    PHASE_RED1_GREEN2,
    PHASE_RED1_AMBER2,
    PHASE_GREEN1_RED2,
    PHASE_AMBER1_RED2,
    PHASE_AMBER1_AMBER2
} TrafficPhase_t;

/* Traffic Light Colors */
typedef enum {
    LIGHT_OFF = 0,
    LIGHT_RED,
    LIGHT_AMBER,
    LIGHT_GREEN
} LightColor_t;

/* Road Identifiers */
#define ROAD_1                  1
#define ROAD_2                  2

/* Button IDs */
#define BTN_1                   0
#define BTN_2                   1
#define BTN_3                   2
#define BTN_4                   3
#define BTN_USER                4
#define NUM_BUTTONS             5

/* Default Duration Settings (seconds) */
#define DEFAULT_TIME_RED        5
#define DEFAULT_TIME_AMBER      2
#define DEFAULT_TIME_GREEN      3

/* Global Shared Variables */
extern SystemMode_t   mode_flag;      /* chế độ hiện tại: chỉ Button_Logic ghi, các fsm đọc */
extern TrafficPhase_t traffic_phase;  /* pha đèn hiện tại: fsm đang chạy ghi mỗi khi đổi đèn */

extern uint16_t time_red;
extern uint16_t time_amber;
extern uint16_t time_green;

extern float current_temperature;
extern float current_humidity;

#endif /* INC_GLOBAL_H_ */
