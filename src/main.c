#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include "pcm_data.h"

#define SAMPLING_RATE 48000

unsigned int radio_pin = 0;
volatile uint8_t amplitude = 0;
volatile uint32_t index = 0;

bool play_sample(struct repeating_timer *t) {
	amplitude = audio_pcm[index];
	index = (index + 1) % audio_pcm_size;
    return true;
}

int main() {
    stdio_init_all();

	printf("hello, world!\n");
	set_sys_clock_khz(220000, false);
	printf("overclocked!\n");

	//gpio_init(radio_pin);
    //gpio_set_dir(radio_pin, GPIO_OUT);
    gpio_set_function(radio_pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(radio_pin);

	pwm_set_clkdiv_mode(slice_num, PWM_DIV_FREE_RUNNING);
    pwm_set_wrap(slice_num, 2);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 1);
    pwm_set_enabled(slice_num, true);

	//for (int i = 0; i < BUFFER_SIZE; i++) {
    //    // Calculate the sine wave value for each position
    //    float angle = 2.0 * PI * (i / (float)BUFFER_SIZE);
    //    float sine_value = sin(angle);
    //    
    //    // Convert to uint8_t value (0 to 255)
    //    uint8_t amplitude = (uint8_t)((sine_value + 1.0) * 64);
    //    sin_table[i] = amplitude; // 0 -> 128 (clkdiv weirdnesses);
    //}

	struct repeating_timer timer;
	add_repeating_timer_us(-1e6 / SAMPLING_RATE, play_sample, NULL, &timer);

	while(1) {
		pwm_set_clkdiv_int_frac4(slice_num, 1, amplitude); // todo: frac8 conv
		sleep_us(1);
	}
}
