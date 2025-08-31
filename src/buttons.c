#include "pico/stdlib.h"
#include "pico/sleep.h"

#include "hardware/watchdog.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"

#include "globals.h"
#include "audio.h"
#include "radio.h"
#include "led.h"

#include <inttypes.h>

typedef struct {
    bool current_state;
    absolute_time_t last_time;
    bool debounced;
} debounce_t;

volatile debounce_t toggle_debounce = {0};
volatile debounce_t interact_debounce = {0};
volatile absolute_time_t time_interact_high = {0};

volatile bool playing = false;

static audio_configuration_t* audio_config;

// https://electronics.stackexchange.com/questions/547074/how-can-i-debounce-a-switch-on-both-positive-and-negative-going-edges
bool debounced(volatile debounce_t* debounce, bool edge) {
	absolute_time_t now = get_absolute_time();
	int64_t diff = absolute_time_diff_us(debounce->last_time, now);

	if (edge == debounce->current_state) {
		debounce->last_time = now;
		debounce->debounced = false;
		return false;
	} else {
		if (diff >= DEBOUNCE_US) {
			debounce->current_state = edge;
			debounce->last_time = now;
			debounce->debounced = true;
			return true;
		} else {
			debounce->debounced = false;
			return false;
		}
	}
}

// https://github.com/raspberrypi/pico-playground/blob/master/sleep/hello_dormant/hello_dormant_gpio.c
void toggle(bool edge) {
	if (g_sleeping) return;
	if (edge) return; // force falling edge
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

void interact(bool edge) {
	absolute_time_t now = get_absolute_time();
	int64_t diff = absolute_time_diff_us(time_interact_high, now);
	time_interact_high = now;

	if (edge) return;
#ifdef ENABLE_RADIO
	if (diff >= 600000) {
		if (audio_config == NULL) return;
		if (audio_is_playing(audio_config)) return;
		audio_play(audio_config);
	} else {
#endif

		led_interact();

#ifdef ENABLE_RADIO
	}
#endif
}

// edge == true means rising edge, vice versa
void irq_handler(uint gpio, uint32_t event_mask) {
	bool rise = event_mask & GPIO_IRQ_EDGE_RISE;

	if (gpio == TOGGLE_PIN) {
		if (!debounced(&toggle_debounce, rise)) return;
		toggle(rise);
	}

	if (gpio == INTERACT_PIN) {
		if (!debounced(&interact_debounce, rise)) return;
		interact(rise);
	}

	DEBUG_PRINT("IRQ for %d\n", gpio);
}

void buttons_setup(audio_configuration_t* c) {
	gpio_init(INTERACT_PIN);
	gpio_init(TOGGLE_PIN);
	gpio_set_dir(INTERACT_PIN, GPIO_IN);
	gpio_set_dir(TOGGLE_PIN, GPIO_IN);
	gpio_set_pulls(INTERACT_PIN, false, true);
	gpio_set_pulls(TOGGLE_PIN, false, true);

	// there's a possibility we just came back from a restart, and the button is still bouncing
	sleep_ms(300); // so this sleep should hopefully give enough time for that bounce to stop
	gpio_set_irq_enabled(TOGGLE_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

	audio_config = c;
	gpio_set_irq_enabled(INTERACT_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

	gpio_set_irq_callback(&irq_handler);
	irq_set_enabled(IO_IRQ_BANK0, true);
}
