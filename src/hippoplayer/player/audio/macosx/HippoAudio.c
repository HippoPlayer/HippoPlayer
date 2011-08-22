
#include "../HippoAudio.h"
#include "core/debug/Assert.h" 
#include <CoreAudio/CoreAudio.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HippoAudio_buildDeviceList(struct HippoAudioDevice* devices, size_t maxSize)
{
	OSStatus result = noErr;
	UInt32 thePropSize;
    AudioDeviceID *theDeviceList = NULL;
    UInt32 theNumDevices = 0;   

	AudioObjectPropertyAddress thePropertyAddress = { kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
	result = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &thePropertyAddress, 0, NULL, &thePropSize);
	if (result) { printf("Error in AudioObjectGetPropertyDataSize: %d\n", result); return 0; }
	
	// Find out how many devices are on the system
	theNumDevices = thePropSize / sizeof(AudioDeviceID);
	theDeviceList = (AudioDeviceID*)calloc(theNumDevices, sizeof(AudioDeviceID));
	
	result = AudioObjectGetPropertyData(kAudioObjectSystemObject, &thePropertyAddress, 0, NULL, &thePropSize, theDeviceList);
	if (result) { printf("Error in AudioObjectGetPropertyData: %d\n", result); return 0; }

	HIPPO_ASSERT(theNumDevices < maxSize);
	
	for (UInt32 i=0; i < theNumDevices; i++)
	{
		CFStringRef theDeviceName;      
		// get the device name
		thePropSize = sizeof(CFStringRef);
		thePropertyAddress.mSelector = kAudioObjectPropertyName;
		thePropertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
		thePropertyAddress.mElement = kAudioObjectPropertyElementMaster;
		
		result = AudioObjectGetPropertyData(theDeviceList[i], &thePropertyAddress, 0, NULL, &thePropSize, &theDeviceName);
		if (result) { printf("Error in AudioObjectGetPropertyData: %d\n", result); return 0; }
		
		CFStringGetCString(theDeviceName, devices[i].name, sizeof(devices[i].name), kCFStringEncodingUTF8);
		devices[i].deviceId = (void*)theDeviceList[i];

		CFRelease(theDeviceName);       
	}
	
	return theNumDevices;
}
