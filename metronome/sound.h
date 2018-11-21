#ifndef __sound_h__
#define __sound_h__

#include <avr/io.h>
#include "setup.h"
#include "util.h"

#ifdef SINE_WAVE
int8_t sine[256] = {
    0, 3, 6, 9, 12, 15, 18, 21,
    24, 27, 30, 34, 37, 39, 42, 45,
    48, 51, 54, 57, 60, 62, 65, 68,
    70, 73, 75, 78, 80, 83, 85, 87,
    90, 92, 94, 96, 98, 100, 102, 104,
    106, 107, 109, 110, 112, 113, 115, 116,
    117, 118, 120, 121, 122, 122, 123, 124,
    125, 125, 126, 126, 126, 127, 127, 127,
    127, 127, 127, 127, 126, 126, 126, 125,
    125, 124, 123, 122, 122, 121, 120, 118,
    117, 116, 115, 113, 112, 110, 109, 107,
    106, 104, 102, 100, 98, 96, 94, 92,
    90, 87, 85, 83, 80, 78, 75, 73,
    70, 68, 65, 62, 60, 57, 54, 51,
    48, 45, 42, 39, 37, 34, 30, 27,
    24, 21, 18, 15, 12, 9, 6, 3,
    0, -4, -7, -10, -13, -16, -19, -22,
    -25, -28, -31, -35, -38, -40, -43, -46,
    -49, -52, -55, -58, -61, -63, -66, -69,
    -71, -74, -76, -79, -81, -84, -86, -88,
    -91, -93, -95, -97, -99, -101, -103, -105,
    -107, -108, -110, -111, -113, -114, -116, -117,
    -118, -119, -121, -122, -123, -123, -124, -125,
    -126, -126, -127, -127, -127, -128, -128, -128,
    -128, -128, -128, -128, -127, -127, -127, -126,
    -126, -125, -124, -123, -123, -122, -121, -119,
    -118, -117, -116, -114, -113, -111, -110, -108,
    -107, -105, -103, -101, -99, -97, -95, -93,
    -91, -88, -86, -84, -81, -79, -76, -74,
    -71, -69, -66, -63, -61, -58, -55, -52,
    -49, -46, -43, -40, -38, -35, -31, -28,
    -25, -22, -19, -16, -13, -10, -7, -4};
#endif

extern volatile bool sound_locked;
extern volatile bool edit_active;

#ifdef SINE_WAVE
inline static void play_note(uint16_t frequency, uint8_t duration) {
    uint16_t accumulator;
    uint16_t accumulatorSteps = frequency << 2; 
    uint8_t waveStep;
    int8_t pwmValue;
    uint16_t t = duration;
    t <<= 5;  // divide by 32
    if (!sound_locked) {
        setBit(SPEAKER_DIR, SPEAKER);

        for (int i = 0; i < t; ++i) {
            accumulator += accumulatorSteps;
            waveStep = accumulator >> 8;     // position in lookup table
            pwmValue = sine[waveStep];      

            loop_until_bit_is_set(TIFR0, TOV0); // wait for PWM cycle
            OCR0A = 128 + pwmValue;             // calc new PWM value
            TIFR0 |= (1 << TOV0);
        }

        clearBit(SPEAKER_DIR, SPEAKER);
    }
}
#else
inline static void play_note(uint16_t wavelength, uint8_t duration) {
    OCR0A = wavelength;
    if (!sound_locked && !edit_active) {
        setBit(SPEAKER_DIR, SPEAKER);
        while (duration) {
            _delay_ms(1u);
            --duration;
        }
        clearBit(SPEAKER_DIR, SPEAKER);
    }
}
#endif

#endif //__sound_h__