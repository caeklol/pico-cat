#include <math.h>

#include "pico/time.h"

#include "hardware/clocks.h"
#include "hardware/pwm.h"

#include "globals.h"
#include "buttons.h"
#include "radio.h"
#include "led.h"

int main() {
	DEBUG_INIT();
	DEBUG_PRINT("debug initialized\n");

	set_sys_clock_khz(220000, false); // 220MHz, giving fs*2 harmonic. yes, square waves
									  // are supposed to only be odd harmonics. but in
									  // testing for some reason 220MHz gives a louder
									  // 440MHz signal than 88MHz. maybe it's some RF
									  // thing, or maybe is a clkdiv thing, but whatever
									  // works i guess
	DEBUG_PRINT("overclocked!\n");
	bool radio_enabled = radio_setup();

	audio_configuration_t* audio = NULL;

	if (radio_enabled) {
		audio = audio_setup();
		buttons_setup(audio);
	} else {
		buttons_setup(NULL);
	}

	led_setup();
	DEBUG_PRINT("setup done\n");

	uint64_t last_tick = 0;

	while(1) {
		if (g_sleeping) tight_loop_contents(); // i don't think this is ever triggered, but just in case

		bool radio_playing = false;

		if (radio_enabled) radio_playing = radio_tick(audio);

		if (radio_playing) {
			sleep_us(2);
		} else {
			uint64_t now = to_us_since_boot(get_absolute_time());
			uint64_t diff = now - last_tick;
			if (diff < 20000) sleep_us(diff);
		}

		uint64_t now = to_us_since_boot(get_absolute_time());

		if (now - last_tick >= 20000) {
			last_tick = now;
			led_tick();
		}
	}
}
