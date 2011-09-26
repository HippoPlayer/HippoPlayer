
#ifndef HIPPOAUDIO_H
#define HIPPOAUDIO_H

#include "core/Types.h"

typedef struct HippoAudioDevice
{
	char name[512];
	void* deviceId;

} HippoAudioDevice;

struct HippoPlaybackPlugin;

int HippoAudio_buildDeviceList(HippoAudioDevice* devices, size_t maxSize);
void HippoAudio_openDefaultOutput();
void HippoAudio_preparePlayback(struct HippoPlaybackPlugin* plugin);
void HippoAudio_close();

#endif

