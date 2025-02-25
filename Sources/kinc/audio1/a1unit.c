#include "audio.h"

#include <stdbool.h>

typedef struct kinc_a1_channel {
	kinc_a1_sound_t *sound;
	float position;
	bool loop;
	volatile float volume;
	float pitch;
} kinc_a1_channel_t;

typedef struct kinc_a1_stream_channel {
	kinc_a1_sound_stream_t *stream;
	int position;
} kinc_a1_stream_channel_t;

typedef struct kinc_internal_video_channel {
	struct kinc_internal_video_sound_stream *stream;
	int position;
} kinc_internal_video_channel_t;

#include "audio.c.h"
#include "sound.c.h"
#include "soundstream.c.h"
