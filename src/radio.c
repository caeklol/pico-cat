#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "globals.h"
#include "audio.h"

bool radio_tick(audio_configuration_t* audio) {
	uint radio_slice = pwm_gpio_to_slice_num(RADIO_PIN);

	bool playing_audio = audio_is_playing(audio);
	pwm_set_enabled(radio_slice, playing_audio);

	if (playing_audio) {
		uint8_t current_sample = audio_get_sample(audio);
		pwm_set_clkdiv_int_frac4(radio_slice, 1, (current_sample / 8)); // todo: clock_gpio_init_int_frac16 conv
	}

	return playing_audio;
}

bool radio_setup() {
	uint slice_red = pwm_gpio_to_slice_num(RED_PIN);
    uint slice_green = pwm_gpio_to_slice_num(GREEN_PIN);
    uint slice_blue = pwm_gpio_to_slice_num(BLUE_PIN);

	uint chan_red = pwm_gpio_to_channel(RED_PIN);
    uint chan_green = pwm_gpio_to_channel(GREEN_PIN);
    uint chan_blue = pwm_gpio_to_channel(BLUE_PIN);

    uint slice_num = pwm_gpio_to_slice_num(RADIO_PIN);
    uint channel = pwm_gpio_to_channel(RADIO_PIN);

	if (((slice_num == slice_red) && (channel == chan_red)) ||
		((slice_num == slice_green) && (channel == chan_green)) ||
		((slice_num == slice_blue) && (channel == chan_blue))
   ) {
		return false;
	}

	gpio_set_function(RADIO_PIN, GPIO_FUNC_PWM);

	pwm_set_clkdiv_mode(slice_num, PWM_DIV_FREE_RUNNING);
    pwm_set_wrap(slice_num, 2);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 1);

	return true;
}

void radio_disable() {
    uint slice_num = pwm_gpio_to_slice_num(RADIO_PIN);
    uint channel = pwm_gpio_to_channel(RADIO_PIN);

    pwm_set_chan_level(slice_num, channel, 0);
	pwm_set_enabled(slice_num, false);
}
