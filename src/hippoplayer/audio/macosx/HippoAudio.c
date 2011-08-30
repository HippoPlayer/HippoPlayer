
#include "../HippoAudio.h"
#include "core/debug/Assert.h" 
#include "plugin_api/HippoPlugin.h" 
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#include <CoreServices/CoreServices.h>

static AudioUnit gOutputUnit = 0;

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

uint32_t tempBuffer[8192];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static OSStatus renderCallback(void* inRefCon, AudioUnitRenderActionFlags* inActionFlags,
							   const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber,
							   UInt32 inNumFrames, AudioBufferList* ioData)

{
	//memset(tempBuffer, 0, sizeof(tempBuffer));

	HippoPlaybackPlugin* plugin = (HippoPlaybackPlugin*)inRefCon;
	plugin->readData(plugin->userData, tempBuffer, inNumFrames);

	for (UInt32 channel = 0; channel < ioData->mNumberBuffers; channel++)
		memcpy(ioData->mBuffers[channel].mData, tempBuffer, inNumFrames * sizeof(uint16_t));

	return noErr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoAudio_openDefaultOutput(HippoPlaybackPlugin* plugin)
{
	OSStatus err = noErr;

	// Open the default output unit
	ComponentDescription desc;
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_DefaultOutput;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;

	Component comp = FindNextComponent(NULL, &desc);
	if (comp == NULL) { printf ("FindNextComponent\n"); return; }

	err = OpenAComponent(comp, &gOutputUnit);
	if (comp == NULL) { printf ("OpenAComponent=%d\n", err); return; }

	// Set up a callback function to generate output to the output unit
	AURenderCallbackStruct input;
	input.inputProc = renderCallback;
	input.inputProcRefCon = plugin;

	err = AudioUnitSetProperty(gOutputUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Output,
							   0, &input, sizeof(input));
	if (err) { printf ("AudioUnitSetProperty-CB=%d\n", err); return; }

	Float64 sSampleRate = 44100;
	SInt32 sNumChannels = 2;
	UInt32 theFormatID = kAudioFormatLinearPCM;
	// 32 bit floats
	//int theFormatFlags =  kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved;
    //int theBytesPerFrame = 4, theBytesInAPacket = 4, theBitsPerChannel = 32;

	// 16 bits
	SInt32 theFormatFlags =  kLinearPCMFormatFlagIsSignedInteger | kAudioFormatFlagsNativeEndian | 
	                         kLinearPCMFormatFlagIsPacked | kAudioFormatFlagIsNonInterleaved;
    int theBytesPerFrame = 2, theBytesInAPacket = 2, theBitsPerChannel = 16;    
    
    
	AudioStreamBasicDescription streamFormat;
	streamFormat.mSampleRate = sSampleRate;     //  the sample rate of the audio stream
	streamFormat.mFormatID = theFormatID;           //  the specific encoding type of audio stream
	streamFormat.mFormatFlags = theFormatFlags;     //  flags specific to each format
	streamFormat.mBytesPerPacket = theBytesInAPacket;   
	streamFormat.mFramesPerPacket = 1; 
	streamFormat.mBytesPerFrame = theBytesPerFrame;     
	streamFormat.mChannelsPerFrame = sNumChannels;  
	streamFormat.mBitsPerChannel = theBitsPerChannel;   
 
    printf("Rendering source:\n\t");
    printf ("SampleRate=%f,", streamFormat.mSampleRate);
    printf ("BytesPerPacket=%d,", streamFormat.mBytesPerPacket);
    printf ("FramesPerPacket=%d,", streamFormat.mFramesPerPacket);
    printf ("BytesPerFrame=%d,", streamFormat.mBytesPerFrame);
    printf ("BitsPerChannel=%d,", streamFormat.mBitsPerChannel);
    printf ("ChannelsPerFrame=%d\n", streamFormat.mChannelsPerFrame);
    
    err = AudioUnitSetProperty (gOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamFormat, sizeof(AudioStreamBasicDescription));
    if (err) { printf ("AudioUnitSetProperty-SF=%4.4s, %d\n", (char*)&err, err); return; }
    
    // Initialize unit
    err = AudioUnitInitialize(gOutputUnit);
    if (err) { printf ("AudioUnitInitialize=%d\n", err); return; }
    
    Float64 outSampleRate;
    UInt32 size = sizeof(Float64);
    err = AudioUnitGetProperty (gOutputUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Input, 0, &outSampleRate, &size);
    if (err) { printf ("AudioUnitSetProperty-GF=%4.4s, %d\n", (char*)&err, err); return; }

    // Start the rendering
    // The DefaultOutputUnit will do any format conversions to the format of the default device
    err = AudioOutputUnitStart (gOutputUnit);
    if (err) { printf ("AudioOutputUnitStart=%d\n", err); return; }
            
    // we call the CFRunLoopRunInMode to service any notifications that the audio
    // system has to deal with
    // CFRunLoopRunInMode(kCFRunLoopDefaultMode, 2, false);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoAudio_close()
{
	OSStatus err = noErr;

	if (gOutputUnit == 0)
		return;
	
	AudioOutputUnitStop(gOutputUnit);
    
    err = AudioUnitUninitialize (gOutputUnit);
    if (err) { printf ("AudioUnitUninitialize=%d\n", err); return; }

    gOutputUnit = 0;
}

