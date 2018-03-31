#include "../../plugin_api/HippoPlugin.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <unistd.h>
#include <math.h>



#include <codecvt>
#include <locale>

extern "C"
{

#include "VGMPlay/chips/mamedef.h"
#include "stdbool.h"
#include "VGMPlay/VGMPlay.h"
#include "VGMPlay/VGMPlay_Intf.h"

extern VGM_HEADER VGMHead;
extern UINT32 SampleRate;
extern bool EndPlay;

extern UINT32 VGMMaxLoop;
extern UINT32 FadeTime;
extern GD3_TAG VGMTag;

}

#define FREQ 48000
#define FRAME_SIZE ((FREQ * 2) / 50)


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct VgmReplayerData {
	char title[4096];
	int has_data;
	int length;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static const char* vgm_track_info(void* userData) {
	struct VgmReplayerData* user_data = (struct VgmReplayerData*)userData;
	return user_data->title;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* vgm_supported_extensions() {
	return "vgm,vgz";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* vgm_create(HippoServiceAPI* service_api) {
	void* data = malloc(sizeof(struct VgmReplayerData));
	memset(data, 0, sizeof(struct VgmReplayerData));

	VGMPlay_Init();
	VGMPlay_Init2();

	VGMMaxLoop = 2;
	FadeTime = 5000;

	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vgm_destroy(void* user_data) {
	struct VgmReplayerData* data = (struct VgmReplayerData*)user_data;
	free(data);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vgm_open(void* userData, const char* buffer) {
	// TODO: Add reader functions etc to be used instead of fopen as file may come from zip, etc

	struct VgmReplayerData* replayerData = (struct VgmReplayerData*)userData;

	if (!OpenVGMFile(buffer)) {
		printf("Unable to open %s\n", buffer);
	}

	PlayVGM();

	if (VGMTag.strTrackNameE) {
	    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	    std::string utf8_name = utf8_conv.to_bytes(VGMTag.strTrackNameE);
        strcpy(replayerData->title, utf8_name.data());
	}

    replayerData->length = VGMHead.lngTotalSamples / (44100 / 2);
	replayerData->has_data = 1;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vgm_close(void* userData) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vgm_read_data(void* userData, void* dest, uint32_t max_samples) {
	WAVE_16BS temp_data[FRAME_SIZE * 2];

	struct VgmReplayerData* replayerData = (struct VgmReplayerData*)userData;

	if (!replayerData->has_data) {
		return 0 ;
	}

	float* newDest = (float*)dest;

	FillBuffer(temp_data, FRAME_SIZE / 2);

	const float scale = 1.0f / 32768.0f;

	for (int i = 0; i < FRAME_SIZE / 2; ++i) {
		newDest[(i * 2) + 0] = ((float)temp_data[i].Left) * scale;
		newDest[(i * 2) + 1] = ((float)temp_data[i].Right) * scale;
	}

	return FRAME_SIZE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Right now we assume gz files are vgm which of course isn't the case but will do for now until we a
// better implementation of this

HippoProbeResult vgm_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
	if ((data[0] == 'V') &&
		(data[1] == 'g') &&
		(data[2] == 'm')) {

		return HippoProbeResult_Supported;
	}

	// gzip header bytes

	if ((data[0] == 0x1f) &&    // id1
		(data[1] == 0x8b) &&    // id2
		(data[2] == 0x08)) {    // deflate method (only one supported

		return HippoProbeResult_Supported;
	}

	return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vgm_seek(void* userData, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static int vgm_length(void* userData) {
	struct VgmReplayerData* replayerData = (struct VgmReplayerData*)userData;
	return replayerData->length;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_vgm_plugin = {
	1,
	vgm_probe_can_play,
	vgm_supported_extensions,
	vgm_create,
	vgm_destroy,
	vgm_open,
	vgm_close,
	vgm_read_data,
	vgm_seek,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_vgm_plugin;
}



