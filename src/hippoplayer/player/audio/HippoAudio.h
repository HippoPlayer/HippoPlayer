
#ifndef HIPPOAUDIO_H
#define HIPPOAUDIO_H

#include "core/Types.h"

typedef struct HippoAudioDevice
{
	char name[512];
	void* deviceId;

} HippoAudioDevice;

int HippoAudio_buildDeviceList(HippoAudioDevice* devices, size_t maxSize);

#endif

