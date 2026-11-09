#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <stdint.h>

/* System Operating Modes */
typedef enum {
    MODE_AUTO = 0,
    MODE_MANUAL,
    MODE_TUNING
} SystemMode_t;

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
extern SystemMode_t current_mode;
extern uint16_t time_red;
extern uint16_t time_amber;
extern uint16_t time_green;

extern float current_temperature;
extern float current_humidity;

#endif /* INC_GLOBAL_H_ */
