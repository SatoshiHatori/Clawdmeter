#include "../../hal/input_hal.h"
#include "board.h"
#include <Arduino.h>

// GPIO 37/39 are input-only pads without internal pull-ups; the board has
// external 10k pull-ups, so plain INPUT is correct.

void input_hal_init(void) {
    pinMode(BTN_A_GPIO, INPUT);
    pinMode(BTN_C_GPIO, INPUT);
}

bool input_hal_is_held(InputButton btn) {
    switch (btn) {
    case INPUT_BTN_PRIMARY:   return digitalRead(BTN_A_GPIO) == LOW;
    case INPUT_BTN_SECONDARY: return digitalRead(BTN_C_GPIO) == LOW;
    }
    return false;
}
