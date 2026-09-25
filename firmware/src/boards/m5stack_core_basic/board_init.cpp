#include "board.h"
#include <Arduino.h>
#include <Wire.h>

// Bring up the internal I2C bus (IP5306 + Gray IMU) and silence the speaker.
// GPIO 25 floats at boot and the NS4148 amp turns that into an audible hiss /
// pop, so it is driven LOW before anything else runs. No IO expander, no
// reset gating — the panel reset is a direct GPIO handled by Arduino_GFX.
extern "C" void board_init(void) {
    pinMode(SPK_PIN, OUTPUT);
    digitalWrite(SPK_PIN, LOW);

    Wire.begin(IIC_SDA, IIC_SCL);
}
