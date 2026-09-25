#include "../../hal/power_hal.h"
#include "board.h"
#include <Arduino.h>
#include <Wire.h>

// IP5306 power-bank IC (I2C variant, addr 0x75). It exposes only a 4-step
// fuel gauge and charger status — no voltage readout — so the percentage is
// quantized to 0/25/50/75/100. Register meanings follow M5Stack's own
// Power driver:
//   0x00 SYS_CTL0 bit1 — boost output stays on under light load
//   0x70 READ0    bit3 — charger input (VIN) present
//   0x71 READ1    bit3 — charge complete
//   0x78 READ3    [7:4] — fuel gauge (0x00 = full ... 0xF0 = empty)
// If the chip does not ACK (non-I2C IP5306 or no battery board) the battery
// reads -1 and the icon is hidden.
//
// Button B plays the PWR role (screens / brightness / hold-to-pair) with the
// same software edge synthesis as the LCD-1.54 port. Power off stays in
// hardware: the IP5306 cuts power on a double-press of the side button.

#define BATTERY_POLL_MS  2000
#define PWR_POLL_MS      50
#define PWR_LONG_MS      1500

static bool     ip5306_ok         = false;
static int      cached_pct        = -1;
static bool     cached_charging   = false;
static bool     cached_vbus       = false;
static bool     pwr_pressed_flag  = false;
static bool     pwr_long_flag     = false;
static bool     pwr_released_flag = false;
static bool     last_pwr_state    = false;
static uint32_t pwr_press_started_ms = 0;
static bool     pwr_long_fired    = false;
static uint32_t last_battery_ms   = 0;
static uint32_t last_pwr_ms       = 0;

static bool ip5306_read(uint8_t reg, uint8_t* out) {
    Wire.beginTransmission(IP5306_ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) return false;
    if (Wire.requestFrom((uint8_t)IP5306_ADDR, (uint8_t)1) != 1) return false;
    *out = Wire.read();
    return true;
}

static void ip5306_write(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(IP5306_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

static void sample_battery(void) {
    uint8_t r0, r1, r3;
    if (!ip5306_read(0x70, &r0) || !ip5306_read(0x71, &r1) || !ip5306_read(0x78, &r3)) {
        cached_pct = -1;
        cached_charging = cached_vbus = false;
        return;
    }
    cached_vbus     = r0 & 0x08;
    cached_charging = cached_vbus && !(r1 & 0x08);
    switch (r3 & 0xF0) {
    case 0x00: cached_pct = 100; break;
    case 0x80: cached_pct = 75;  break;
    case 0xC0: cached_pct = 50;  break;
    case 0xE0: cached_pct = 25;  break;
    default:   cached_pct = 0;   break;
    }
}

void power_hal_init(void) {
    pinMode(BTN_B_GPIO, INPUT);   // external pull-up, input-only pad

    uint8_t ctl0;
    ip5306_ok = ip5306_read(0x00, &ctl0);
    if (ip5306_ok) {
        // Keep the boost converter on at light load — otherwise the IP5306
        // auto-shuts the 5 V rail after ~30 s once the backlight dims.
        ip5306_write(0x00, ctl0 | 0x02);
        Serial.println("IP5306 OK");
    } else {
        Serial.println("IP5306 not found — battery gauge disabled");
    }
    sample_battery();
}

void power_hal_tick(void) {
    uint32_t now = millis();

    if (ip5306_ok && now - last_battery_ms >= BATTERY_POLL_MS) {
        last_battery_ms = now;
        sample_battery();
    }
    if (now - last_pwr_ms >= PWR_POLL_MS) {
        last_pwr_ms = now;
        bool pwr_now = (digitalRead(BTN_B_GPIO) == LOW);   // active LOW
        if (pwr_now && !last_pwr_state) {            // press edge — hold begins
            pwr_press_started_ms = now;
            pwr_long_fired = false;
        } else if (pwr_now && last_pwr_state) {      // held
            if (!pwr_long_fired && (now - pwr_press_started_ms >= PWR_LONG_MS)) {
                pwr_long_flag  = true;
                pwr_long_fired = true;
            }
        } else if (!pwr_now && last_pwr_state) {     // release edge
            pwr_released_flag = true;
            if (!pwr_long_fired) pwr_pressed_flag = true;  // short press
        }
        last_pwr_state = pwr_now;
    }
}

int  power_hal_battery_pct(void) { return cached_pct; }
bool power_hal_is_charging(void) { return cached_charging; }
bool power_hal_is_vbus_in(void)  { return cached_vbus; }

bool power_hal_pwr_pressed(void) {
    if (pwr_pressed_flag) { pwr_pressed_flag = false; return true; }
    return false;
}

bool power_hal_pwr_long_pressed(void) {
    if (pwr_long_flag) { pwr_long_flag = false; return true; }
    return false;
}

bool power_hal_pwr_released(void) {
    if (pwr_released_flag) { pwr_released_flag = false; return true; }
    return false;
}
