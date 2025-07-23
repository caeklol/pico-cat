#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

unsigned int radio_pin = 0;
volatile uint8_t amplitude = 0;
volatile uint32_t index = 0;

#define SAMPLING_RATE 22000   // Sampling rate in Hz
#define FREQUENCY 500        // Frequency of the tone in Hz
#define BUFFER_SIZE 256       // Size of the sine wave table (uint8_t values)
#define PI 3.14159265358979323846
volatile uint8_t sin_table[BUFFER_SIZE];

bool play_sample(__unused struct repeating_timer *t) {
	amplitude = sin_table[index];
	index = (index + 1) % BUFFER_SIZE;
    return true;
}

int main() {
    stdio_init_all();

	printf("hello, world!\n");
	set_sys_clock_khz(220000, false);
	printf("overclocked!\n");

	gpio_init(radio_pin);
    gpio_set_dir(radio_pin, GPIO_OUT);

    gpio_set_function(radio_pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(radio_pin);

    pwm_set_wrap(slice_num, 2);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 1);
    pwm_set_enabled(slice_num, true);

	struct repeating_timer timer;
	double sample_delay_seconds = 1.0 / SAMPLING_RATE;
	double sample_delay_microseconds = sample_delay_seconds * 1e6;

	add_repeating_timer_us((uint32_t)sample_delay_microseconds, play_sample, NULL, &timer);
	for (int i = 0; i < BUFFER_SIZE; i++) {
        // Calculate the sine wave value for each position
        float angle = 2.0 * PI * (i / (float)BUFFER_SIZE);
        float sine_value = sin(angle);
        
        // Convert to uint8_t value (0 to 255)
        uint8_t amplitude = (uint8_t)((sine_value + 1.0) * 64); // Normalize to 0-255
        sin_table[i] = amplitude; // 0 -> 128 (clkdiv weirdnesses);
    }

	while(1) {
		pwm_set_clkdiv_int_frac4(slice_num, 2, amplitude);
		sleep_us(1);
	}
}
