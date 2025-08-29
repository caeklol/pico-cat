#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "pico/sleep.h"
#include "pico/time.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/watchdog.h"
#include "hardware/pwm.h"
#include "pcm_data.h"

#define SAMPLING_RATE 48000

unsigned int radio_pin = 8;

static uint red = 1;
static uint green = 2;
static uint blue = 3;

static uint meow_pin = 4;
static uint toggle_pin = 10;

volatile uint8_t amplitude = 0;
volatile uint32_t index = 0;

volatile absolute_time_t last_toggle = {0};
volatile absolute_time_t last_meow = {0};
volatile bool asleep = false;
volatile bool playing = false;

static struct repeating_timer timer;

// https://github.com/raspberrypi/pico-playground/blob/master/sleep/hello_dormant/hello_dormant_gpio.c
void toggle() {
	if (asleep) return;
	absolute_time_t now = get_absolute_time();

	if (absolute_time_diff_us(last_toggle, now) < 40000) return; // [1]
    last_toggle = now;
	asleep = true;

	busy_wait_us_32(100000); // since this is the first interrupt called[1], the consequent bounces will
				             // trigger the following interrupt[2]. therefore this debounces

	printf("going dormant until GPIO %d goes edge high\n", toggle_pin);

	uart_default_tx_wait_blocking();
	sleep_run_from_xosc();
	uart_default_tx_wait_blocking();

	// [2]
	sleep_goto_dormant_until_edge_high(toggle_pin);

	sleep_power_up();

	gpio_put(25, false);

	// reboot
	watchdog_enable(10, false);
	sleep_ms(100);
}

bool play_sample(struct repeating_timer *t) {
	if (index > audio_pcm_size) {
		playing = false;
		return false;
	}

	amplitude = audio_pcm[index];
	index = index + 1;
    return true;
}

void meow() {
	if (playing) return;
	absolute_time_t now = get_absolute_time();

	if (absolute_time_diff_us(last_meow, now) < 40000) return;
    last_meow = now;

	playing = true;
	index = 0;
	add_repeating_timer_us(-1e6 / (SAMPLING_RATE * 2), play_sample, NULL, &timer);
}

void irq_handler(uint gpio, uint32_t event_mask) {
	if (event_mask & GPIO_IRQ_EDGE_RISE != 1) return;

	if (gpio == toggle_pin) toggle();
	if (gpio == meow_pin) meow();
}

void setup_buttons() {
	gpio_init(meow_pin);
	gpio_init(toggle_pin);
	gpio_set_dir(meow_pin, GPIO_IN);
	gpio_set_dir(toggle_pin, GPIO_IN);
	gpio_set_pulls(meow_pin, false, true);
	gpio_set_pulls(toggle_pin, false, true);

	// there's a possibility we just came back from a restart, and the button is still bouncing
	sleep_ms(100); // so this sleep should hopefully give enough time for that bounce to stop
	gpio_set_irq_enabled(toggle_pin, GPIO_IRQ_EDGE_RISE, true);
	gpio_set_irq_enabled(meow_pin, GPIO_IRQ_EDGE_RISE, true);
	gpio_set_irq_callback(&irq_handler);
	irq_set_enabled(IO_IRQ_BANK0, true);
}

void setup_radio() {
	gpio_set_function(radio_pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(radio_pin);

	pwm_set_clkdiv_mode(slice_num, PWM_DIV_FREE_RUNNING);
    pwm_set_wrap(slice_num, 2);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 1);
}

int main() {
	stdio_init_all();

	printf("stdio initialized\n");

	set_sys_clock_khz(220000, false); // 220MHz, giving fs*2 harmonic. yes, square waves
									  // are supposed to only be odd harmonics. but in
									  // testing for some reason 220MHz gives a louder
									  // 440MHz signal than 88MHz. maybe it's some RF
									  // thing, or maybe is a clkdiv thing, but whatever
									  // works i guess
	printf("overclocked!\n");
	setup_buttons();
	setup_radio();
	printf("setup done\n");

	uint radio_slice = pwm_gpio_to_slice_num(radio_pin);

	int n = 0;
	uint64_t last_tick = 0;

	while(1) {
		if (asleep) continue;

		pwm_set_enabled(radio_slice, playing);
		if (playing) {
			pwm_set_clkdiv_int_frac4(radio_slice, 1, (amplitude >> 3)); // todo: clock_gpio_init_int_frac16 conv
																	  // / 2 to get in range (nibble required, as per docs)
																	  // / 4 to get bandwidth into NFM ranges
																	  // / 8 = 2^3
			sleep_us(2);
		} else {
			sleep_ms(20);
		}

		uint64_t now = to_us_since_boot(get_absolute_time());

		if (now - last_tick >= 20000) {
			n += 1;
			last_tick = now;
			printf("iter %d\n", n);
		}
	}
}
