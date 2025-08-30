typedef struct audio_configuration audio_configuration_t;

/// returns true if radio is currently playing audio
bool radio_tick(audio_configuration_t* audio_config);
bool radio_setup();
void radio_disable();
