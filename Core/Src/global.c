#include "global.h"

/* Global Shared Variables */
SystemMode_t   mode_flag     = MODE_AUTO;
TrafficPhase_t traffic_phase = PHASE_OFF;

uint16_t time_red  = DEFAULT_TIME_RED;
uint16_t time_amber = DEFAULT_TIME_AMBER;
uint16_t time_green = DEFAULT_TIME_GREEN;

float current_temperature = 0.0f;
float current_humidity    = 0.0f;
