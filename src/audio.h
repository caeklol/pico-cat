typedef struct audio_configuration audio_configuration_t;

bool audio_is_playing(audio_configuration_t* config);
uint8_t audio_get_sample(audio_configuration_t* config);

audio_configuration_t* audio_setup();
void audio_play(audio_configuration_t* config);
