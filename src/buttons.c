#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/sleep.h"
#include "hardware/watchdog.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"

#include "globals.h"
#include "audio.h"
#include "radio.h"
#include "led.h"

volatile absolute_time_t last_toggle = {0};
volatile absolute_time_t last_meow = {0};

volatile bool playing = false;

static audio_configuration_t* audio_config;

// https://github.com/raspberrypi/pico-playground/blob/master/sleep/hello_dormant/hello_dormant_gpio.c
void toggle() {
	if (g_sleeping) return;
	absolute_time_t now = get_absolute_time();

	if (absolute_time_diff_us(last_toggle, now) < 40000) return; // [1]
    last_toggle = now;
	g_sleeping = true;

	led_disable();
	radio_disable();

	busy_wait_us_32(300000); // since this is the first interrupt called[1], the consequent bounces will
				             // trigger the following interrupt[2], waking up the pico. therefore this debounces


	uart_default_tx_wait_blocking();
	sleep_run_from_xosc();
	uart_default_tx_wait_blocking();

	// [2]
	sleep_goto_dormant_until_edge_high(TOGGLE_PIN);

	sleep_power_up();

	watchdog_reboot(0, SRAM_END, 0);
    for (;;) {
        __wfi();
    }
}

void meow() {
	if (playing) return;
	absolute_time_t now = get_absolute_time();

	if (absolute_time_diff_us(last_meow, now) < 40000) return;
    last_meow = now;

	if (audio_config != NULL) audio_play(audio_config);
}

void irq_handler(uint gpio, uint32_t event_mask) {
	if (event_mask & GPIO_IRQ_EDGE_RISE != 1) return;

	if (gpio == TOGGLE_PIN) toggle();
	if (gpio == MEOW_PIN) meow();
}

void buttons_setup(audio_configuration_t* c) {
	gpio_init(MEOW_PIN);
	gpio_init(TOGGLE_PIN);
	gpio_set_dir(MEOW_PIN, GPIO_IN);
	gpio_set_dir(TOGGLE_PIN, GPIO_IN);
	gpio_set_pulls(MEOW_PIN, false, true);
	gpio_set_pulls(TOGGLE_PIN, false, true);

	// there's a possibility we just came back from a restart, and the button is still bouncing
	sleep_ms(300); // so this sleep should hopefully give enough time for that bounce to stop
	gpio_set_irq_enabled(TOGGLE_PIN, GPIO_IRQ_EDGE_RISE, true);

	audio_config = c;
	if (audio_config != NULL) gpio_set_irq_enabled(MEOW_PIN, GPIO_IRQ_EDGE_RISE, true);

	gpio_set_irq_callback(&irq_handler);
	irq_set_enabled(IO_IRQ_BANK0, true);
}
