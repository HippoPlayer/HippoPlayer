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

HippoLogAPI* g_hp_log = NULL;

#define FREQ 48000
#define FRAME_SIZE 1024

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void get_file_stem(char* dest, const char* path) {
    const char* end_path = nullptr;

	for(size_t i = strlen(path) - 1;  i > 0; i--) {
		if (path[i] == '/') {
            end_path = &path[i+1];
            break;
		}
    }

    strcpy(dest, end_path);

    for(size_t i = strlen(dest) - 1;  i > 0; i--) {
        if (dest[i] == '.') {
            dest[i] = 0;
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool has_init = false;

static void init() {
    /*
    if (has_init) {
        return;
    }
    */

	VGMPlay_Init();
	VGMPlay_Init2();

	VGMMaxLoop = 2;
	FadeTime = 5000;

    has_init = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct VgmReplayerData {
	char title[4096];
	int has_data;
	int length;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* vgm_supported_extensions() {
	return "vgm,vgz";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void* vgm_create(const HippoServiceAPI* service_api) {
	void* data = malloc(sizeof(struct VgmReplayerData));
	memset(data, 0, sizeof(struct VgmReplayerData));

	init();

	return data;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vgm_destroy(void* user_data) {
	struct VgmReplayerData* data = (struct VgmReplayerData*)user_data;
	free(data);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vgm_open(void* user_data, const char* buffer, int subsong) {
	// TODO: Add reader functions etc to be used instead of fopen as file may come from zip, etc

	struct VgmReplayerData* replayer_data = (struct VgmReplayerData*)user_data;

	if (!OpenVGMFile(buffer)) {
		hp_error("Unable to open %s\n", buffer);
		return -1;
	}

	PlayVGM();

    replayer_data->length = VGMHead.lngTotalSamples / (44100 / 2);
	replayer_data->has_data = 1;

	hp_info("Starting to play %s (subsong %d)", buffer, subsong);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vgm_close(void* user_data) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoReadInfo vgm_read_data(void* user_data, void* dest, uint32_t max_output_bytes,
                                   uint32_t native_sample_rate) {
	//struct VgmReplayerData* replayer_data = (struct VgmReplayerData*)user_data;
	uint16_t samples_to_read = hippo_min(max_output_bytes / 4, FRAME_SIZE);

	FillBuffer((WAVE_16BS*)dest, samples_to_read);

    return HippoReadInfo {
        FREQ,
        samples_to_read,
        2,
        HippoOutputType_s16
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Right now we assume gz files are vgm which of course isn't the case but will do for now until we a
// better implementation of this

HippoProbeResult vgm_probe_can_play(const uint8_t* data, uint32_t data_size, const char* filename, uint64_t total_size) {
	if ((data[0] == 'V') &&
		(data[1] == 'g') &&
		(data[2] == 'm')) {
		hp_info("Supported %s", filename);
		return HippoProbeResult_Supported;
	}

	// gzip header bytes

	if ((data[0] == 0x1f) &&    // id1
		(data[1] == 0x8b) &&    // id2
		(data[2] == 0x08)) {    // deflate method (only one supported
		hp_info("Supported %s", filename);
		return HippoProbeResult_Supported;
	}

    hp_debug("Unsupported: %s", filename);
	return HippoProbeResult_Unsupported;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vgm_seek(void* user_data, int ms) {
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vgm_event(void* user_data, const unsigned char* data, int len) {
    (void)user_data;
    (void)data;
    (void)len;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string get_string(wchar_t* text) {
    if (!text) {
        return std::string();
    }

    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.to_bytes(text);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string get_tag(wchar_t* eng_text, wchar_t* jp_text) {
    if (eng_text) {
        return get_string(eng_text);
    } else if (jp_text) {
        return get_string(jp_text);
    } else {
        return std::string();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void append_to_message(std::string& dest, const std::string& data, const char* tag) {
    if (data == "") {
        return;
    }

    dest += tag;
    dest += data;
    dest += "\n";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int vgm_metadata(const char* filename, const HippoServiceAPI* service_api) {
    init();

	if (!OpenVGMFile(filename)) {
		hp_error("Unable to metadata open %s\n", filename);
		return -1;
	}

	PlayVGM();

    hp_info("Updating metadata for %s", filename);

	auto track_name = get_tag(VGMTag.strTrackNameE, VGMTag.strTrackNameJ);
	auto game_name = get_tag(VGMTag.strGameNameE, VGMTag.strGameNameJ);
	auto system_name = get_tag(VGMTag.strSystemNameE, VGMTag.strSystemNameJ);
	auto author_name = get_tag(VGMTag.strAuthorNameE, VGMTag.strAuthorNameJ);
	auto creator = get_string(VGMTag.strCreator);
	auto notes = get_string(VGMTag.strNotes);

    const HippoMetadataAPI* metadata_api = HippoServiceAPI_get_metadata_api(service_api, HIPPO_METADATA_API_VERSION);
    HippoMetadataId index = HippoMetadata_create_url(metadata_api, filename);

    // TODO: Verify that this calculation is correct
    HippoMetadata_set_tag_f64(metadata_api, index, HippoMetadata_LengthTag, VGMHead.lngTotalSamples / (44100 / 2));
    HippoMetadata_set_tag(metadata_api, index, HippoMetadata_SongTypeTag, "VGM (Video Game Music)");

    if (track_name != "") {
        HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, track_name.c_str());
    } else {
        char buffer[1024] = { 0 };
        get_file_stem(buffer, filename);
        HippoMetadata_set_tag(metadata_api, index, HippoMetadata_TitleTag, buffer);
    }

    if (author_name != "") {
        HippoMetadata_set_tag(metadata_api, index, HippoMetadata_ArtistTag, author_name.c_str());
    }

    std::string message;
    append_to_message(message, game_name, "Game: ");
    append_to_message(message, system_name, "System: ");
    append_to_message(message, creator, "VGM Creator: ");
    append_to_message(message, notes, "Notes:\n");

    if (message != "") {
        HippoMetadata_set_tag(metadata_api, index, HippoMetadata_MessageTag, message.c_str());
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void vgm_set_log(struct HippoLogAPI* log, const HippoServiceAPI*) { g_hp_log = log; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin g_vgm_plugin = {
	HIPPO_PLAYBACK_PLUGIN_API_VERSION,
	"VGM",
	"0.0.1",
    "VGMPlay 0.40.8",
	vgm_probe_can_play,
	vgm_supported_extensions,
	vgm_create,
	vgm_destroy,
	vgm_event,
	vgm_open,
	vgm_close,
	vgm_read_data,
	vgm_seek,
	vgm_metadata,
	vgm_set_log,
	NULL,
	NULL,
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HIPPO_EXPORT HippoPlaybackPlugin* hippo_playback_plugin() {
	return &g_vgm_plugin;
}

