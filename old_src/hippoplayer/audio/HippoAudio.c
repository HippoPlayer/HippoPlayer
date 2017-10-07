
#include "HippoAudio.h"
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static HippoPlaybackPlugin* s_currentPlugin = 0;
static HippoPlaybackPlugin* s_lastPlugin = 0;

static struct HippoPlaybackBuffer s_decodeBuffers[2];
static uint32_t s_pluginFrameSize = 0;
static uint32_t s_currentBuffer = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoAudio_preparePlayback(HippoPlaybackPlugin* plugin) 
{
	uint32_t pluginFrameSize = plugin->frameSize(plugin->privateData);
	uint32_t frameMaxSize = 1024 * 1024;

	// TODO: Remove malloc here and use custom allocator

	for (uint32_t i = 0; i < 2; ++i)
	{
		free(s_decodeBuffers[i].data);
		s_decodeBuffers[i].data = malloc(frameMaxSize);
		s_decodeBuffers[i].readOffset = 0;
		s_decodeBuffers[i].frameSize = pluginFrameSize;
		s_decodeBuffers[i].frameMaxSize = frameMaxSize;
	}

	// Decode the first bit of audio here so it's prepared when starting the playback. Potentiall race here if
	// we get a callback to the rederCallback, needs to be investigated
	
	plugin->readData(plugin->privateData, &s_decodeBuffers[0]);
	plugin->readData(plugin->privateData, &s_decodeBuffers[1]);
	s_pluginFrameSize = pluginFrameSize;
	s_currentBuffer = 0;
	s_currentPlugin = plugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoAudio_pausePlayback()
{
	s_lastPlugin = s_currentPlugin;
	s_currentPlugin = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoAudio_resumePlayback()
{
	s_currentPlugin = s_lastPlugin;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void writeAudioData(AudioBufferList* ioData, const uint8_t* data, uint32_t offset, uint32_t size)
{
	uint32_t copySize = size / 4; // 2 channels 2 bytes for each sample
	const uint16_t* srcData = (uint16_t*)data;
	uint16_t* dest0 = (uint16_t*)ioData->mBuffers[0].mData;
	uint16_t* dest1 = (uint16_t*)ioData->mBuffers[1].mData;

	dest0 += offset;
	dest1 += offset;
	
	// TODO: we know that we have 2 channels here so we assume that for now

	for (uint32 i = 0; i < copySize; ++i)
	{
		*dest0++ = srcData[0];
		*dest1++ = srcData[1];
		srcData += 2;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void clearOutputBuffers(AudioBufferList* ioData, uint32_t size)
{
	memset(ioData->mBuffers[0].mData, 0, size * sizeof(uint16_t));
	memset(ioData->mBuffers[1].mData, 0, size * sizeof(uint16_t));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static OSStatus renderCallback(void* inRefCon, AudioUnitRenderActionFlags* inActionFlags,
							   const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber,
							   UInt32 frameSize, AudioBufferList* ioData)

{
	HippoPlaybackPlugin* plugin = s_currentPlugin;

	if (!plugin)
	{
		clearOutputBuffers(ioData, frameSize);
		return noErr;
	}

	frameSize *= 4; // 2 channels with 2 bytes for each sample (hard-coded for now)

	const uint32_t pluginFrameSize = s_pluginFrameSize;

	// This has to be true for now. That is that the frame decoded by the plugin needs to be bigger than then the read
	// frame by the audio playback. This may not always be the case but should always be changed so it's true esp as
	// the decode jobs will be kicked using GDC and decoding very little amount each time will make things slower anyway
	// and would complicate the code more even if we could support it

	if (frameSize > pluginFrameSize) 
	{
		printf("frameSize too small (%d) needs to be at least (%d)\n",  plugin->frameSize(plugin->privateData), frameSize);
		return -1;
	}

	uint32_t currentBuffer = s_currentBuffer;
	struct HippoPlaybackBuffer* decodeBuffer = &s_decodeBuffers[currentBuffer];

	uint32_t decodeFrameSize = decodeBuffer->frameSize;

	// if readOffset + frameSize is larger than the pluginFrameSize it means that we are crossing a buffer boundry
	// and we need to copy the first part from the currentBuffer and the remaing part from the second one
	// When we are done with the first buffer we kick of the the next decode job 

	if (decodeBuffer->readOffset + frameSize > decodeFrameSize)
	{
		uint32_t bufferSize = decodeFrameSize - decodeBuffer->readOffset;

		// write the data remaining data in the first buffer
		writeAudioData(ioData, decodeBuffer->data + decodeBuffer->readOffset, 0, bufferSize);

		// Here we are done with the buffer, we reset the readOffset to 0 and kick of the new decode
		decodeBuffer->readOffset = 0;
		plugin->readData(plugin->privateData, decodeBuffer);

		currentBuffer = !currentBuffer;
		decodeBuffer = &s_decodeBuffers[currentBuffer];

		uint32_t restSize = frameSize - bufferSize; 
		writeAudioData(ioData, decodeBuffer->data, bufferSize / 4, restSize); // And the rest int the second buffer 
		decodeBuffer->readOffset += restSize;
	}
	else
	{
		// Still data left in the buffer so just copy it and move along

		writeAudioData(ioData, decodeBuffer->data + decodeBuffer->readOffset, 0, frameSize);
		decodeBuffer->readOffset += frameSize;
	}

	s_currentBuffer = currentBuffer;

	return noErr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoAudio_openDefaultOutput()
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
	input.inputProcRefCon = 0;

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

