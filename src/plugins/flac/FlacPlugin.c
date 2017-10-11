#include "../../hippoplayer/plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <FLAC/stream_decoder.h> 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct FlacReplayerData
{
	long long length;
	FLAC__StreamDecoder* decoder;
	HippoPlaybackBuffer* playbackBuffer;
	FILE* file;
	int format;
	float totalTime;
} FlacReplayerData;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callbacks needed by the FLAC code

/*
static FLAC__StreamDecoderReadStatus readCallback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;
	(void)decoder;

	printf("readCallback %d\n", (int)*bytes);

	// TODO: Use file-api
	size_t readSize = fread((uint8_t*)buffer, 1, *bytes, replayer->file);

	if (readSize == 0)
	{
		*bytes = 0;
		return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
	} 

	*bytes = readSize;

	return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
} 
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static FLAC__StreamDecoderWriteStatus writeCallback(const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame, 
									   				const FLAC__int32* const buffer[], void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;
	HippoPlaybackBuffer* playbackBuffer = replayer->playbackBuffer;
	(void)decoder;

    playbackBuffer->frameSize = frame->header.channels * frame->header.blocksize * frame->header.bits_per_sample / 8;

    if (playbackBuffer->frameSize > playbackBuffer->frameMaxSize)
    {
		// TODO: Use allocator 
		free(playbackBuffer->data);
		playbackBuffer->data = (uint8_t*)malloc(playbackBuffer->frameSize);
		playbackBuffer->frameMaxSize = playbackBuffer->frameSize;
    }

    uint16_t* dst = (uint16_t*)playbackBuffer->data;

	/* If the sample is neither exactly 8-bit nor 16-bit, it will have to
	 * be converted. Unfortunately the buffer is read-only, so we either
	 * have to check for each sample, or make a copy of the buffer. I'm
	 * not sure which way is best, so I've arbitrarily picked the former.
	 */

	// TODO: Rewrite this using SSE2

	/*
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
    */
    {
		for (uint32_t i = 0, blockSize = frame->header.blocksize; i < blockSize; i++)
		{
			for (uint32_t j = 0, channelCount = frame->header.channels; j < channelCount; j++)
			{
				FLAC__int32 sample = buffer[j][i];

				/*
				if (frame->header.bits_per_sample < 16)
					sample <<= (16 - frame->header.bits_per_sample);
				else if (frame->header.bits_per_sample > 16)
					sample >>= (frame->header.bits_per_sample - 16);
				*dst++ = (sample & 0xff00) >> 8;
				*dst++ = sample & 0x00ff;
				*/

				*dst++ = (uint16_t)sample;
			} 
		}
	} 

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static FLAC__StreamDecoderSeekStatus seekCallback(const FLAC__StreamDecoder* decoder, FLAC__uint64 offset, void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;
	(void)decoder;

    printf("%d\n", __LINE__);

	// TODO: Use file-api
    if (fseek(replayer->file, offset, SEEK_SET) >= 0)
        return FLAC__STREAM_DECODER_SEEK_STATUS_OK;

    return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
} 
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static FLAC__StreamDecoderTellStatus tellCallback(const FLAC__StreamDecoder* decoder, FLAC__uint64* offset, void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;
	(void)decoder;

    long int pos = ftell(replayer->file);

    printf("%d\n", __LINE__);

	if (pos < 0)
		return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;

	*offset = pos;

	return FLAC__STREAM_DECODER_TELL_STATUS_OK;
} 
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static FLAC__StreamDecoderLengthStatus lengthCallback(const FLAC__StreamDecoder* decoder, FLAC__uint64* length, void * clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;
	(void)decoder;

	printf("lengthCallback %d\n", (int)replayer->length);

	*length = replayer->length;

	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
} 
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static FLAC__bool eofCallback(const FLAC__StreamDecoder* decoder, void *clientData) 
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;
	(void)decoder;

	// TODO: Use buffer api
    long int pos = ftell(replayer->file);

    printf("pos = %d\n", (int)pos);
    
    if (pos >= 0 && pos >= replayer->length)
        return true;

    return false;
} 
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void errorCallback(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status, void* clientData)
{
	(void)decoder;
	(void)clientData;
	//FlacReplayerData* replayer = (FlacReplayerData*) clientData;
	printf("flac error %s\n", FLAC__StreamDecoderErrorStatusString[status]);
} 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void metadataCallback(const FLAC__StreamDecoder* decoder, const FLAC__StreamMetadata* metadata, void* clientData)
{
	FlacReplayerData* replayer = (FlacReplayerData*) clientData;
	(void)decoder;

	if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
	{
		FLAC__uint64 total_samples = 0;
		unsigned sample_rate = 0;
		unsigned channels = 0;
		unsigned bps = 0;
		sample_rate = metadata->data.stream_info.sample_rate;
		channels = metadata->data.stream_info.channels;
		bps = metadata->data.stream_info.bits_per_sample;

		printf("sample rate    : %u Hz\n", sample_rate);
		printf("channels       : %u\n", channels);
		printf("bits per sample: %u\n", bps);
		printf("total samples  : %llu\n", total_samples);

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

		/*
		if (metadata->data.stream_info.bits_per_sample > 8)
			sample->actual.format = AUDIO_S16MSB;
		else
			sample->actual.format = AUDIO_S8;
		*/
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* flacInfo(void* userData)
{
	(void)userData;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* flacTrackInfo(void* userData)
{
	(void)userData;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char** flacSupportedExtensions(void* userData)
{
	(void)userData;
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
	(void)userData;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flacOpen(void* userData, const char* buffer)
{
	// TODO: Use allocator
	FlacReplayerData* replayer = (FlacReplayerData*)userData;

    FLAC__StreamDecoder* decoder = FLAC__stream_decoder_new();

	(void)FLAC__stream_decoder_set_md5_checking(decoder, true);

	/*
    FLAC__StreamDecoderInitStatus ret = FLAC__stream_decoder_init_stream(decoder, readCallback, seekCallback, 
											   tellCallback, lengthCallback, eofCallback, writeCallback,
											   metadataCallback, errorCallback, replayer);
	*/	
    FLAC__StreamDecoderInitStatus ret = FLAC__stream_decoder_init_file(decoder, buffer, writeCallback, metadataCallback, 
    																   errorCallback, replayer);
	if (ret != FLAC__STREAM_DECODER_INIT_STATUS_OK )
	{
		printf("FLAC init failed. Status: %s\n", FLAC__StreamDecoderInitStatusString[ret]);
		return -1;
	}

	printf("%s\n", buffer);

	//replayer->file = fopen(buffer, "rb");
	replayer->decoder = decoder;

	// Get metadata
	//FLAC__stream_decoder_process_until_end_of_metadata(decoder);

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

static int flacReadData(void* userData, HippoPlaybackBuffer* dest)
{
	FlacReplayerData* replayer = (FlacReplayerData*) userData;
	replayer->playbackBuffer = dest;

	if (!FLAC__stream_decoder_process_single(replayer->decoder))
	{
		//printf("FLAC: Unable to decode frame\n");
	}

	if (FLAC__stream_decoder_get_state(replayer->decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
	{
		// TODO: Set flags in replayer to have proper clean up
		return 0;
	}

	return dest->frameSize;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flacSeek(void* userData, int ms)
{
	(void)ms;
	(void)userData;
	// TODO: Implement
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int flacFrameSize(void* userData)
{
	(void)userData;
	// Temp size, the decoder will set this on a per frame basis
	return 2048;
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
	0
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoPlaybackPlugin* getPlugin()
{
	return &g_flacPlugin;
}

