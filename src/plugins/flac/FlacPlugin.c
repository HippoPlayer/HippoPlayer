#include "../../hippoplayer/plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct FlacReplayerData
{
	uint64_t length;
	FLAC__StreamDecoder* decoder;
	HippoPlaybackBuffer* playbackBuffer;
	FILE* file;
	int format;
	float totalTime;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks needed by the FLAC code

static d_read_status_t readCallback(const decoder_t* decoder, FLAC__byte buffer[], size_t* bytes, void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;

	// TODO: Use file-api
	size_t readSize = fread((uint8_t*)buffer, 1, *bytes, replayer->file);

	if (retval == 0)
	{
		*bytes = 0;
		return D_READ_END_OF_STREAM;
	} 

	*bytes = readSize;

	return D_READ_CONTINUE;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static d_write_status_t write_callback(const decoder_t* decoder, const FLAC__Frame* frame, 
									   const FLAC__int32* const buffer[], void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;
	HippoPlaybackBuffer* playbackBuffer = replayer->playbackBuffer;

    playbackBuffer->frameSize = frame->header.channels * frame->header.blocksize * frame->header.bits_per_sample / 8;

    if (playbackBuffer->frameSize > playbackBuffer->frameMaxSize)
    {
		// TODO: Use allocator 
		free(playbackBuffer->data);
		playbackBuffer->data = (uint8_t*)mallc(playbackBuffer->frameSize);
		playbackBuffer->frameMaxSize = playbackBuffer->frameSize;
    }

    uint8_t* dst = f->sample->buffer;

	/* If the sample is neither exactly 8-bit nor 16-bit, it will have to
	 * be converted. Unfortunately the buffer is read-only, so we either
	 * have to check for each sample, or make a copy of the buffer. I'm
	 * not sure which way is best, so I've arbitrarily picked the former.
	 */

	// TODO: Rewrite this using SSE2

	if (replayer->format == AUDIO_S8)
	{
		for (uint32_t i = 0, blockSize = frame->header.blocksize; i < blockSize; i++)
		{
			for (uint32_t j = 0, channelCount = frame->header.channels; j < channelCount; j++)
			{
				uint32_t sample = buffer[j][i];

				if (frame->header.bits_per_sample < 8)
					sample <<= (8 - frame->header.bits_per_sample);
				*dst++ = sample & 0x00ff;
			} 
		}
	} 
    else
    {
		for (uint32_t i = 0, blockSize = frame->header.blocksize; i < blockSize; i++)
		{
			for (uint32_t j = 0, channelCount = frame->header.channels; j < channelCount; j++)
			{
				sample = buffer[j][i];

				if (frame->header.bits_per_sample < 16)
					sample <<= (16 - frame->header.bits_per_sample);
				else if (frame->header.bits_per_sample > 16)
					sample >>= (frame->header.bits_per_sample - 16);

				*dst++ = (sample & 0xff00) >> 8;
				*dst++ = sample & 0x00ff;
			} 
		}
	} 

    return D_WRITE_CONTINUE;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static d_seek_status_t seekCallback(const decoder_t* decoder, FLAC__uint64 offset, void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;

	// TODO: Use file-api
    if (fseek(replayer->file, offset, SEEK_SET) >= 0)
        return D_SEEK_STATUS_OK;

    return D_SEEK_STATUS_ERROR;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static d_tell_status_t tellCallback(const decoder_t* decoder, FLAC__uint64* offset, void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;

    long int pos = ftell(replayer->file);

	if (pos < 0)
		return D_TELL_STATUS_ERROR;

	*offset = pos;

	return D_TELL_STATUS_OK;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static d_length_status_t lengthCallback(const decoder_t* decoder, FLAC__uint64* length, void * clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;

	*length = replayer->length;

	return D_LENGTH_STATUS_OK;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static FLAC__bool eofRallback(const decoder_t* decoder, void *clientData) 
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;

	// TODO: Use buffer api
    long int pos = ftell(replayer->file);
    
    if (pos >= 0 && pos >= replayer->length)
        return true;

    return false;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void errorCallback( const decoder_t* decoder, d_error_status_t status, void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;
	printf("flac error %s\n", d_error_status_string[status]);
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void metadataCallback(const decoder_t* decoder, const d_metadata_t* metaData, void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;

	if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
	{
		if (metadata->data.stream_info.sample_rate == 0 || metadata->data.stream_info.total_samples == 0)
		{
			replayer->totalTime = -1;
		}
		else
		{
			replayer->totalTime = (metadata->data.stream_info.total_samples)
				/ metadata->data.stream_info.sample_rate * 1000;
			replayer->totalTime += (metadata->data.stream_info.total_samples
				% metadata->data.stream_info.sample_rate) * 1000
				/ metadata->data.stream_info.sample_rate;
		}

		if (metadata->data.stream_info.bits_per_sample > 8)
			sample->actual.format = AUDIO_S16MSB;
		else
			sample->actual.format = AUDIO_S8;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* flacInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* flacTrackInfo(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char** flacSupportedExtensions(void* userData)
{
	static const char* supportedFomats[] =
	{
		"flac",
		0,
	};

	return supportedFomats;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* flacCreate()
{
	// TODO: supply custom allocator

	void* replayerData = malloc(sizeof(struct FlacReplayerData));
	memset(replayerData, 0, sizeof(struct FlacReplayerData));

	return replayerData;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flacDestroy(void* userData)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flacOpen(void* userData, const char* buffer)
{
	// TODO: Use allocator
	FlacReplayerData* replayer = (FlacReplayerData*)malloc(sizeof(FlacReplayer));
	memset(replayer, 0, sizeof(FlacReplayerData));

    decoder_t* decoder = FLAC__seekable_stream_decoder_new();

    int ret = FLAC__stream_decoder_init_stream(decoder, readCallback, seekCallback, 
											   tellCallback, lengthCallback, eofCallback, writeCallback,
											   metadataCallback, errorCallback, replayer);

	if (!ret)
	{
		printf("FLAC init failed\n");
		return -1;
	}

	// Get metadata
	FLAC__stream_decoder_process_until_end_of_metadata(decoder)

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flacClose(void* userData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) userData;

	if (replayer->file)
	{
		fclose(replayer->file);
		replayer->file = 0;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flacReadData(void* userData, HippoPlayerBuffer* dest)
{
	FlacReplayerData* replayer = (FlacReplayerData*) userData;
	replayer->playbackBuffer = dest;
	
	if (!FLAC__stream_decoder_process_single(replayer->decoder))
	{
		printf("FLAC: Unable to decode frame\n");
	}

	if (FLAC__stream_decoder_get_state(replayer->decoder) == D_END_OF_STREAM)
	{
		// TODO: Set flags in replayer to have proper clean up
		return 0;
	}

	return dest->frameSize;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flacSeek(void* userData, int ms)
{
	// TODO: Implement
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flacFrameSize(void* userData)
{
	// Temp size, the decoder will set this on a per frame basis
	return 44100 * sizeof(uint16_t) * 2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_flacPlugin = 
{
	1,
	flacInfo,
	flacTrackInfo,
	flacSupportedExtensions,
	flacCreate,
	flacDestroy,
	flacOpen,
	flacClose,
	flacReadData,
	flacSeek,
	flacFrameSize,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoPlaybackPlugin* getPlugin()
{
	return &g_flacPlugin;
}

