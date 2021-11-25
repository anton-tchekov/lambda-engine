#include "audio.h"
#include "log.h"
#include "debug.h"
#include "internal.h"

/* #define AUDIO_DEBUG */

#define AAF_SAMPLE_RATE_MASK      ((1UL << 30) - 1)
#define AAF_BITS_PER_SAMPLE_BIT   (1UL << 30)
#define AAF_CHANNELS_BIT          (1UL << 31)

static ALenum _aaf_openal_format(AAF *aaf);

#ifdef AUDIO_DEBUG

static const char *_al_format_string(ALenum format)
{
	static const char *format_strings[4] =
	{
		"AL_FORMAT_MONO8",
		"AL_FORMAT_MONO16",
		"AL_FORMAT_STEREO8",
		"AL_FORMAT_STEREO16"
	};

	switch(format)
	{
		case AL_FORMAT_MONO8:
			return format_strings[0];

		case AL_FORMAT_MONO16:
			return format_strings[1];

		case AL_FORMAT_STEREO8:
			return format_strings[2];

		case AL_FORMAT_STEREO16:
			return format_strings[3];

		default:
			break;
	}

	return "Unknown";
}

#endif

int audio_init(void)
{
	ALCboolean rv;
	log_debug("Initializing audio");
	log_debug("Initializing audio device");
	if(!(GameEngine.Audio.Device = alcOpenDevice(NULL)))
	{
		log_error("Failed to initialize audio device");
		GameEngine.Audio.Context = NULL;
		return -1;
	}

	log_debug("Initializing audio context");
	ALC_CHECK(GameEngine.Audio.Context = alcCreateContext(GameEngine.Audio.Device, NULL));
	if(!GameEngine.Audio.Context)
	{
		log_error("Failed to initialize audio context");
		ALC_CHECK(alcCloseDevice(GameEngine.Audio.Device));
		GameEngine.Audio.Device = NULL;
		return -1;
	}

	log_debug("Enabling audio context");
	ALC_CHECK(rv = alcMakeContextCurrent(GameEngine.Audio.Context));
	if(!rv)
	{
		log_error("Failed to enable audio context");
		ALC_CHECK(alcDestroyContext(GameEngine.Audio.Context));
		ALC_CHECK(alcCloseDevice(GameEngine.Audio.Device));
		GameEngine.Audio.Device = NULL;
		GameEngine.Audio.Context = NULL;
		return -1;
	}

	return 0;
}

void audio_destroy(void)
{
	if(!GameEngine.Audio.Device)
	{
		return;
	}

	log_debug("Destroying audio");
	ALC_CHECK(alcMakeContextCurrent(NULL));
	ALC_CHECK(alcDestroyContext(GameEngine.Audio.Context));
	ALC_CHECK(alcCloseDevice(GameEngine.Audio.Device));
}

int audio_enabled(void)
{
	return GameEngine.Audio.Device != NULL;
}

void audio_listener_update(Camera *camera)
{
	static ALfloat _orientation[] = { 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	AL_CHECK(alListener3f(AL_POSITION, camera->Position[X], camera->Position[Y], camera->Position[Z]));
	_orientation[X] = camera->Front[X];
	_orientation[Z] = camera->Front[Y];
	AL_CHECK(alListenerfv(AL_ORIENTATION, _orientation));
}

AudioBuffer audio_buffer_aaf_memory(u8 *buffer)
{
	AAF aaf;
	if(aaf_load_memory(buffer, &aaf))
	{
		return 0;
	}

	return audio_buffer_aaf(&aaf);
}

AudioBuffer audio_buffer_aaf(AAF *aaf)
{
	ALenum al_format;
	u32 sample_rate;
	ALuint audio_buffer;
	if(!audio_enabled())
	{
		return 1;
	}

#ifdef AUDIO_DEBUG
	log_debug("Buffering audio data");
#endif
	AL_CHECK(alGenBuffers(1, &audio_buffer));
	if(alGetError() != AL_NO_ERROR)
	{
		log_error("Failed to initialize audio buffer");
		return 0;
	}

	al_format = _aaf_openal_format(aaf);
	sample_rate = aaf->Format & AAF_SAMPLE_RATE_MASK;

#ifdef AUDIO_DEBUG
	log_debug("Audio information:\n"
		"Sample rate: %d\n"
		"Format: %s", sample_rate, _al_format_string(al_format));
#endif

	AL_CHECK(alBufferData(audio_buffer, al_format,
			aaf->Data, (ALsizei)aaf->Length, (ALsizei)sample_rate));
	if(alGetError() != AL_NO_ERROR)
	{
		AL_CHECK(alDeleteBuffers(1, &audio_buffer));
		log_error("Failed to buffer audio data");
		return 0;
	}

	return audio_buffer;
}

void audio_buffer_destroy(AudioBuffer buffer)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alDeleteBuffers(1, &buffer));
}

void audio_volume(float volume)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alListenerf(AL_GAIN, volume));
}

AudioSource audio_source_new(void)
{
	AudioSource source = 1;
	if(!audio_enabled())
	{
		return source;
	}

	alGenSources(1, &source);
	if(alGetError() != AL_NO_ERROR)
	{
		AL_CHECK(alDeleteBuffers(1, &source));
		log_error("Failed to create audio source");
		return 0;
	}

	AL_CHECK(alSourcef(source, AL_PITCH, 1.0f));
	AL_CHECK(alSourcef(source, AL_GAIN, 1.0f));
	AL_CHECK(alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f));
	AL_CHECK(alSourcei(source, AL_LOOPING, AL_FALSE));
	AL_CHECK(alSourcei(source, AL_BUFFER, 0));
	return source;
}

void audio_source_play(AudioSource source)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alSourcePlay(source));
}

int audio_source_playing(AudioSource source)
{
	ALint state;
	if(!audio_enabled())
	{
		return 1;
	}

	alGetSourcei(source, AL_SOURCE_STATE, &state);
	return state == AL_PLAYING;
}

void audio_source_reset(AudioSource source)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alSourceRewind(source));
}

void audio_source_pause(AudioSource source)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alSourcePause(source));
}

void audio_source_destroy(AudioSource source)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alDeleteSources(1, &source));
}

void audio_source_relative(AudioSource source, int relative)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alSourcei(source, AL_SOURCE_RELATIVE, relative));
}

void audio_source_position_v3(AudioSource source, vec3 position)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alSource3f(source, AL_POSITION, position[X], position[Y], position[Z]));
}

void audio_source_position(AudioSource source, float x, float y, float z)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alSource3f(source, AL_POSITION, x, y, z));
}

void audio_source_looping(AudioSource source, int looping)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alSourcei(source, AL_LOOPING, looping));
}

void audio_source_buffer(AudioSource source, AudioBuffer buffer)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alSourcei(source, AL_BUFFER, (ALint)buffer));
}

void audio_source_volume(AudioSource source, float volume)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alSourcef(source, AL_GAIN, volume));
}

void audio_source_pitch(AudioSource source, float pitch)
{
	if(!audio_enabled())
	{
		return;
	}

	AL_CHECK(alSourcef(source, AL_PITCH, pitch));
}

static ALenum _aaf_openal_format(AAF *aaf)
{
	static const ALenum formats[4] =
	{
		/* OPENAL FORMAT       CHNL_BIT  BPS_BIT */
		AL_FORMAT_MONO8,    /*     0        0    */
		AL_FORMAT_MONO16,   /*     1        0    */
		AL_FORMAT_STEREO8,  /*     0        1    */
		AL_FORMAT_STEREO16  /*     1        1    */
	};

	/* Extract bits 30, 31 */
	return formats[aaf->Format >> 30];
}

