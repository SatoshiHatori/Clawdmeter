#pragma once

// M5Stack Core Basic / Gray — the original 2018-era M5Stack Core.
// Classic ESP32 (dual-core Xtensa LX6, no PSRAM), 2.0" ILI9342C 320x240 IPS
// over plain 4-wire SPI, no touch, three front buttons (A/B/C), IP5306
// power-bank IC on I2C (4-step battery gauge), speaker on GPIO 25 via an
// NS4148 amp. The Gray adds an MPU6886/MPU9250 + BMM150 — unused here
// (fixed orientation), so one binary drives both.
//
// Pins from M5Stack's official Core Basic docs / M5Unified board table.

#define BOARD_NAME           "M5Stack Core Basic"

// ---- Display geometry (landscape — the panel's native orientation) ----
#define LCD_WIDTH            320
#define LCD_HEIGHT           240

// ---- SPI display pins (ILI9342C, 4-wire SPI; shares VSPI with the SD slot) ----
#define LCD_CS               14
#define LCD_SCLK             18
#define LCD_MOSI             23
#define LCD_MISO             19
#define LCD_DC               27
#define LCD_RST              33
#define LCD_BL               32    // backlight, LEDC PWM (panel has no brightness cmd)
// IPS panels (every Core Basic / Gray shipped since ~2019) need colour
// inversion. Set to 0 if an early TN-panel unit shows inverted colours.
#define LCD_INVERT           1

// ---- I2C bus (internal: IP5306, plus the IMU on the Gray) ----
#define IIC_SDA              21
#define IIC_SCL              22
#define IP5306_ADDR          0x75

// ---- Buttons (active-LOW, external pull-ups; GPIO 34-39 are input-only) ----
// Mapped to mirror the AMOLED-2.16's left/middle/right layout:
#define BTN_A_GPIO           39    // left   — primary, Space (PTT)
#define BTN_B_GPIO           38    // middle — PWR role: cycle screens / hold to pair
#define BTN_C_GPIO           37    // right  — secondary, Shift+Tab (mode toggle)

// ---- Speaker (NS4148 amp fed from GPIO 25, driven as LEDC square wave) ----
#define SPK_PIN              25

// ---- Capability flags ----
#define BOARD_HAS_SECONDARY_BUTTON 1
#define BOARD_HAS_ROTATION         0
#define BOARD_HAS_IMU              0    // MPU6886 on the Gray only; unused
#define BOARD_HAS_BATTERY          1
#define BOARD_HAS_IO_EXPANDER      0
#define BOARD_HAS_SOUND            1
