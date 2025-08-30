#include <stdlib.h>

#include "pico/stdlib.h"

#include "pcm_data.h"
#include "globals.h"

volatile uint32_t pcm_index = 0;
static struct repeating_timer timer;

typedef struct audio_configuration {
	bool playing;
	uint8_t current_sample;
} audio_configuration_t;

bool play_sample(struct repeating_timer *rt) {
	audio_configuration_t* config = (audio_configuration_t*)(rt->user_data);
	if (pcm_index > audio_pcm_size) {
		config->playing = false;
		return false;
	}

	config->current_sample = audio_pcm[pcm_index];
	pcm_index = pcm_index + 1;
    return true;
}

bool audio_is_playing(audio_configuration_t* config) {
	return config->playing;
}

uint8_t audio_get_sample(audio_configuration_t* config) {
	return config->current_sample;
}

void audio_play(audio_configuration_t* config) {
	if (config->playing) return;
	config->playing = true;
	pcm_index = 0;
	config->current_sample = audio_pcm[pcm_index];
	add_repeating_timer_us(-1e6 / (SAMPLING_RATE * 2), play_sample, (void*)(config), &timer);
}

audio_configuration_t* audio_setup() {
	audio_configuration_t* audio = malloc(sizeof(audio_configuration_t));
	if (audio) {
		audio->playing = false;
		audio->current_sample = 127;
	}
	return audio;
}
