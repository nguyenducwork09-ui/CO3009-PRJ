#include "button.h"

#if __has_include("main.h")
#include "main.h"
#elif __has_include("stm32f1xx_hal.h")
#include "stm32f1xx_hal.h"
#endif

/* Fallback cấu hình chân GPIO nếu chưa sinh mã từ CubeIDE .ioc */
#ifndef BTN_1_Pin
#define BTN_1_Pin               GPIO_PIN_8
#define BTN_1_GPIO_Port         GPIOA
#endif

#ifndef BTN_2_Pin
#define BTN_2_Pin               GPIO_PIN_10
#define BTN_2_GPIO_Port         GPIOB
#endif

#ifndef BTN_3_Pin
#define BTN_3_Pin               GPIO_PIN_4
#define BTN_3_GPIO_Port         GPIOB
#endif

#ifndef BTN_4_Pin
#define BTN_4_Pin               GPIO_PIN_6
#define BTN_4_GPIO_Port         GPIOB
#endif

#ifndef BTN_USER_Pin
#define BTN_USER_Pin            GPIO_PIN_13
#define BTN_USER_GPIO_Port      GPIOC
#endif

/* Bộ đệm chống dội 3 mẫu liên tiếp cho từng nút (1: nhả, 0: nhấn) */
static uint8_t debounce_buf0[NUM_BUTTONS] = {1, 1, 1, 1, 1};
static uint8_t debounce_buf1[NUM_BUTTONS] = {1, 1, 1, 1, 1};
static uint8_t debounce_buf2[NUM_BUTTONS] = {1, 1, 1, 1, 1};
static uint8_t button_state[NUM_BUTTONS]  = {1, 1, 1, 1, 1};

static uint8_t flag_button_pressed[NUM_BUTTONS]      = {0, 0, 0, 0, 0};
static uint8_t flag_button_long_pressed[NUM_BUTTONS] = {0, 0, 0, 0, 0};
static uint16_t counter_long_press[NUM_BUTTONS]      = {0, 0, 0, 0, 0};

/**
 * @brief Đọc mức logic vật lý từ chân GPIO (Input Pull-up: nhấn = 0, nhả = 1)
 */
static uint8_t read_button_pin(uint8_t index)
{
#if defined(GPIO_PIN_RESET)
    switch (index) {
    case BTN_1:
        return (HAL_GPIO_ReadPin(BTN_1_GPIO_Port, BTN_1_Pin) == GPIO_PIN_RESET) ? 0 : 1;
    case BTN_2:
        return (HAL_GPIO_ReadPin(BTN_2_GPIO_Port, BTN_2_Pin) == GPIO_PIN_RESET) ? 0 : 1;
    case BTN_3:
        return (HAL_GPIO_ReadPin(BTN_3_GPIO_Port, BTN_3_Pin) == GPIO_PIN_RESET) ? 0 : 1;
    case BTN_4:
        return (HAL_GPIO_ReadPin(BTN_4_GPIO_Port, BTN_4_Pin) == GPIO_PIN_RESET) ? 0 : 1;
    case BTN_USER:
        return (HAL_GPIO_ReadPin(BTN_USER_GPIO_Port, BTN_USER_Pin) == GPIO_PIN_RESET) ? 0 : 1;
    default:
        return 1;
    }
#else
    (void)index;
    return 1;
#endif
}

/**
 * @brief Xử lý chống dội và bắt cạnh nhấn cho 1 nút
 */
static void process_single_button(uint8_t index)
{
    if (index >= NUM_BUTTONS) return;

    debounce_buf0[index] = debounce_buf1[index];
    debounce_buf1[index] = debounce_buf2[index];
    debounce_buf2[index] = read_button_pin(index);

    /* Khi 3 lần đọc liên tiếp cùng một mức logic -> tín hiệu đã ổn định */
    if ((debounce_buf0[index] == debounce_buf1[index]) && 
        (debounce_buf1[index] == debounce_buf2[index])) {
        
        /* Phát hiện thay đổi trạng thái */
        if (button_state[index] != debounce_buf2[index]) {
            button_state[index] = debounce_buf2[index];

            /* Cạnh xuống: Nhả (1) -> Nhấn (0) */
            if (button_state[index] == 0) {
                flag_button_pressed[index] = 1;
                counter_long_press[index] = 100; /* 100 * 10ms = 1 giây */
            }
        } else {
            /* Nút đang được giữ ở mức 0 */
            if (button_state[index] == 0) {
                if (counter_long_press[index] > 0) {
                    counter_long_press[index]--;
                    if (counter_long_press[index] == 0) {
                        flag_button_long_pressed[index] = 1;
                    }
                }
            }
        }
    }
}

void button_init(void)
{
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        debounce_buf0[i] = 1;
        debounce_buf1[i] = 1;
        debounce_buf2[i] = 1;
        button_state[i]  = 1;
        flag_button_pressed[i]      = 0;
        flag_button_long_pressed[i] = 0;
        counter_long_press[i]       = 0;
    }
}

void button_reading(void)
{
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        process_single_button(i);
    }
}

uint8_t isButtonPressed(uint8_t index)
{
    if (index >= NUM_BUTTONS) return 0;

    if (flag_button_pressed[index] == 1) {
        flag_button_pressed[index] = 0;
        return 1;
    }
    return 0;
}

uint8_t isButtonPressed1s(uint8_t index)
{
    if (index >= NUM_BUTTONS) return 0;

    if (flag_button_long_pressed[index] == 1) {
        flag_button_long_pressed[index] = 0;
        return 1;
    }
    return 0;
}

/* ===== Hiện thực các hàm btn_x() cho FSM ===== */

uint8_t btn_mode(void)
{
    process_single_button(BTN_1);
    return isButtonPressed(BTN_1);
}

uint8_t btn_red(void)
{
    process_single_button(BTN_2);
    return isButtonPressed(BTN_2);
}

uint8_t btn_yellow(void)
{
    process_single_button(BTN_3);
    return isButtonPressed(BTN_3);
}

uint8_t btn_green(void)
{
    process_single_button(BTN_4);
    return isButtonPressed(BTN_4);
}

uint8_t btn_up(void)
{
    process_single_button(BTN_2);
    return isButtonPressed(BTN_2);
}

uint8_t btn_down(void)
{
    process_single_button(BTN_3);
    return isButtonPressed(BTN_3);
}

uint8_t btn_set(void)
{
    process_single_button(BTN_4);
    return isButtonPressed(BTN_4);
}
