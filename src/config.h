#ifndef CONFIG_H
#define CONFIG_H

// sample rate of the PCM file in Hz
#define SAMPLING_RATE 48000

// debounce for both edges in microseconds
#define DEBOUNCE_US 10000

// pin definitions. since this is Pico SDK, this corresponds to the RP2040 pins
#define RADIO_PIN      8
#define INTERACT_PIN   4
#define TOGGLE_PIN     10

#define RED_PIN        1
#define GREEN_PIN      2
#define BLUE_PIN       3

// 0 (0V) -> 255 (3.3V)
// https://cdn-shop.adafruit.com/datasheets/FD-5WSRGB-A.pdf
// 3.6V max for green and blue, which is not in range (3.3V), and 2.4V for red
// 3.55V for tolerance
//
// (3.3 / 3.55) ~= 0.9295
// 0.9295... * 2.35 = 2.18450
// (2.18450 / 3.3) * 255 ~= 168.8022
#define RED_MAX        168
#define GREEN_MAX      255
#define BLUE_MAX       255

// for DEBUG_PRINT and DEBUG_INIT
#define ENABLE_DEBUG

#endif
