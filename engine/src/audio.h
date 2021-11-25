#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "openal.h"
#include "types.h"
#include "camera.h"
#include "aaf.h"

typedef ALuint AudioBuffer;
typedef ALuint AudioSource;

int audio_init(void);
int audio_enabled(void);
void audio_destroy(void);
void audio_buffer_destroy(AudioBuffer buffer);
void audio_listener_update(Camera *camera);
AudioBuffer audio_buffer_aaf_memory(u8 *buffer);
AudioBuffer audio_buffer_aaf(AAF *aaf);
void audio_volume(float volume);

AudioSource audio_source_new(void);
void audio_source_play(AudioSource source);
int audio_source_playing(AudioSource source);
void audio_source_reset(AudioSource source);
void audio_source_pause(AudioSource source);
void audio_source_destroy(AudioSource source);
void audio_source_relative(AudioSource source, int relative);
void audio_source_position_v3(AudioSource source, vec3 position);
void audio_source_position(AudioSource source, float x, float y, float z);
void audio_source_looping(AudioSource source, int looping);
void audio_source_buffer(AudioSource source, AudioBuffer buffer);
void audio_source_volume(AudioSource source, float volume);
void audio_source_pitch(AudioSource source, float pitch);

#endif

