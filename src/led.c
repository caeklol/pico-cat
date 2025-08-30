#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "globals.h"
#include "audio.h"

// integer-math only `map`. taken from https://docs.arduino.cc/language-reference/en/functions/math/map/
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void set_color(uint8_t red, uint8_t green, uint8_t blue) {
	long red_constrained = map(red, 0, 255, 0, RED_MAX);
	long green_constrained = map(green, 0, 255, 0, GREEN_MAX);
	long blue_constrained = map(blue, 0, 255, 0, BLUE_MAX);

	uint slice_red = pwm_gpio_to_slice_num(RED_PIN);
    uint slice_green = pwm_gpio_to_slice_num(GREEN_PIN);
    uint slice_blue = pwm_gpio_to_slice_num(BLUE_PIN);

	uint chan_red = pwm_gpio_to_channel(RED_PIN);
    uint chan_green = pwm_gpio_to_channel(GREEN_PIN);
    uint chan_blue = pwm_gpio_to_channel(BLUE_PIN);

	pwm_set_chan_level(slice_red, chan_red, red_constrained);
	pwm_set_chan_level(slice_green, chan_green, green_constrained);
	pwm_set_chan_level(slice_blue, chan_blue, blue_constrained);
}

void led_tick() {
	set_color(255, 255, 255);
}

void led_interact() {
	printf(">///////<\n");
}

void led_setup() {
	gpio_set_function(RED_PIN, GPIO_FUNC_PWM);
	gpio_set_function(GREEN_PIN, GPIO_FUNC_PWM);
	gpio_set_function(BLUE_PIN, GPIO_FUNC_PWM);

	uint slice_red = pwm_gpio_to_slice_num(RED_PIN);
    uint slice_green = pwm_gpio_to_slice_num(GREEN_PIN);
    uint slice_blue = pwm_gpio_to_slice_num(BLUE_PIN);

	uint chan_red = pwm_gpio_to_channel(RED_PIN);
    uint chan_green = pwm_gpio_to_channel(GREEN_PIN);
    uint chan_blue = pwm_gpio_to_channel(BLUE_PIN);

	uint16_t wrap = 255;
    pwm_set_wrap(slice_red, wrap);
    pwm_set_wrap(slice_green, wrap);
    pwm_set_wrap(slice_blue, wrap);

	pwm_set_enabled(slice_red, true);
    pwm_set_enabled(slice_green, true);
    pwm_set_enabled(slice_blue, true);

	pwm_set_chan_level(slice_red, chan_red, 128);
	pwm_set_chan_level(slice_green, chan_green, 64);
	pwm_set_chan_level(slice_blue, chan_blue, 128);
}

void led_disable() {
	uint slice_red = pwm_gpio_to_slice_num(RED_PIN);
    uint slice_green = pwm_gpio_to_slice_num(GREEN_PIN);
    uint slice_blue = pwm_gpio_to_slice_num(BLUE_PIN);

	uint chan_red = pwm_gpio_to_channel(RED_PIN);
    uint chan_green = pwm_gpio_to_channel(GREEN_PIN);
    uint chan_blue = pwm_gpio_to_channel(BLUE_PIN);

    pwm_set_chan_level(slice_red, chan_red, 0);
    pwm_set_chan_level(slice_green, chan_green, 0);
    pwm_set_chan_level(slice_blue, chan_blue, 0);

	pwm_set_enabled(slice_red, false);
	pwm_set_enabled(slice_green, false);
	pwm_set_enabled(slice_blue, false);
}
