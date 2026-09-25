#include "../../hal/sound_hal.h"
#include "board.h"
#include <Arduino.h>

// Core Basic: no I2S codec — GPIO 25 feeds an NS4148 amp directly. Instead of
// the shared ES8311 chime engine, play a short three-note bell as an LEDC
// square wave. Low duty keeps the volume tolerable (the amp is loud and a
// 50% square is harsh). Non-blocking: sound_hal_tick() advances the notes.
// Between chimes the pin is detached and held LOW so the amp stays silent.

#define SPK_RES_BITS 10
#define SPK_DUTY     24          // out of 1023 — volume

struct Note { uint16_t hz; uint16_t ms; };
static const Note chime[] = {
    { 1319, 120 },   // E6
    { 1568, 120 },   // G6
    { 2093, 260 },   // C7
};
static const int CHIME_LEN = sizeof(chime) / sizeof(chime[0]);

static int      note_idx = -1;   // -1 = idle
static uint32_t note_started_ms = 0;

static void speaker_off(void) {
    ledcDetach(SPK_PIN);
    pinMode(SPK_PIN, OUTPUT);
    digitalWrite(SPK_PIN, LOW);
}

static void start_note(int i) {
    note_idx = i;
    note_started_ms = millis();
    ledcChangeFrequency(SPK_PIN, chime[i].hz, SPK_RES_BITS);
    ledcWrite(SPK_PIN, SPK_DUTY);
}

void sound_hal_init(void) {
    speaker_off();
}

void sound_hal_play_reset(void) {
    if (note_idx < 0) ledcAttach(SPK_PIN, chime[0].hz, SPK_RES_BITS);
    start_note(0);
}

void sound_hal_tick(void) {
    if (note_idx < 0) return;
    if (millis() - note_started_ms < chime[note_idx].ms) return;
    if (note_idx + 1 < CHIME_LEN) {
        start_note(note_idx + 1);
    } else {
        note_idx = -1;
        speaker_off();
    }
}
