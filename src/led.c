#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "globals.h"
#include "audio.h"

uint8_t hue = 0;
int mode = 0;

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

void hsv_to_rgb(uint8_t hue, uint8_t* r, uint8_t* g, uint8_t* b) {
  uint8_t region = hue / 43;
  uint8_t remainder = (hue - (region * 43)) * 6;

  uint8_t p = 0;
  uint8_t q = (255 * (255 - remainder)) / 255;
  uint8_t t = (255 * remainder) / 255;

  switch (region) {
    case 0:
      *r = 255; *g = t; *b = p;
      break;
    case 1:
      *r = q; *g = 255; *b = p;
      break;
    case 2:
      *r = p; *g = 255; *b = t;
      break;
    case 3:
      *r = p; *g = q; *b = 255;
      break;
    case 4:
      *r = t; *g = p; *b = 255;
      break;
    default:
      *r = 255; *g = p; *b = q;
      break;
  }
}

void led_tick() {
	if (mode == 0) {
		uint8_t r, g, b;
		hsv_to_rgb(hue, &r, &g, &b);

		set_color(r, g, b);

		hue++;
	} else if (mode == 1) {
		set_color(255, 195, 77);
	} else {
		set_color(255, 255, 255);
	}
}

void led_interact() {
	mode = (mode + 1) % 4;
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
