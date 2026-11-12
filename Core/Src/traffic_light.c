#include "traffic_light.h"
#include "main.h"

/*
 * Bảng tổ hợp 2 chân tín hiệu điều khiển đèn (A, B)
 * Chuẩn module đèn 2 tín hiệu (CO3009):
 * - TẮT   (LIGHT_OFF)  : A = 0, B = 0
 * - XANH  (LIGHT_GREEN): A = 0, B = 1
 * - VÀNG  (LIGHT_AMBER): A = 1, B = 0
 * - ĐỎ    (LIGHT_RED)  : A = 1, B = 1
 */
#define SIG_OFF_A       GPIO_PIN_RESET
#define SIG_OFF_B       GPIO_PIN_RESET
/*
 * Bảng tổ hợp 2 chân tín hiệu điều khiển đèn (A, B)
 * Chuẩn module đèn 2 tín hiệu (CO3009):
 * - TẮT   (LIGHT_OFF)  : A = 0, B = 0
 * - XANH  (LIGHT_GREEN): A = 0, B = 1
 * - VÀNG  (LIGHT_AMBER): A = 1, B = 0
 * - ĐỎ    (LIGHT_RED)  : A = 1, B = 1
 * (Khi kiểm thử thực tế trên mạch, nếu màu bị lệch chỉ cần đổi lại giá trị 4 macro dưới đây)
 */
#define SIG_OFF_A       GPIO_PIN_RESET
#define SIG_OFF_B       GPIO_PIN_RESET

#define SIG_GREEN_A     GPIO_PIN_RESET
#define SIG_GREEN_B     GPIO_PIN_SET

#define SIG_AMBER_A     GPIO_PIN_SET
#define SIG_AMBER_B     GPIO_PIN_RESET

#define SIG_RED_A       GPIO_PIN_SET
#define SIG_RED_B       GPIO_PIN_SET

void traffic_init(void)
{
    traffic_turn_off_all();
}

void traffic_set(uint8_t road, LightColor_t color)
{
    GPIO_PinState val_a = SIG_OFF_A;
    GPIO_PinState val_b = SIG_OFF_B;

    switch (color) {
    case LIGHT_RED:
        val_a = SIG_RED_A;
        val_b = SIG_RED_B;
        break;
    case LIGHT_AMBER:
        val_a = SIG_AMBER_A;
        val_b = SIG_AMBER_B;
        break;
    case LIGHT_GREEN:
        val_a = SIG_GREEN_A;
        val_b = SIG_GREEN_B;
        break;
    case LIGHT_OFF:
    default:
        val_a = SIG_OFF_A;
        val_b = SIG_OFF_B;
        break;
    }

    if (road == ROAD_1) {
        HAL_GPIO_WritePin(TL1_A_GPIO_Port, TL1_A_Pin, val_a);
        HAL_GPIO_WritePin(TL1_B_GPIO_Port, TL1_B_Pin, val_b);
    } else if (road == ROAD_2) {
        HAL_GPIO_WritePin(TL2_A_GPIO_Port, TL2_A_Pin, val_a);
        HAL_GPIO_WritePin(TL2_B_GPIO_Port, TL2_B_Pin, val_b);
    }
}

void traffic_turn_off_all(void)
{
    traffic_set(ROAD_1, LIGHT_OFF);
    traffic_set(ROAD_2, LIGHT_OFF);
}
