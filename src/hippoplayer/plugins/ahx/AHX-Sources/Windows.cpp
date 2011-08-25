
int fisk = 0;

#if 0

#include <windows.h>
#include <mmsystem.h>
#include <process.h>
#include <stdio.h>

#include "debug.h"
#include "ahx/windows.h"
#include "ahx/ahx.h"

AHXWaveOut::AHXWaveOut()
{
	NrBlocks = BlockLen = Frames = 0;
}

AHXWaveOut::~AHXWaveOut()
{
	Free();
}

int AHXWaveOut::Init(int Frequency, int Bits, int Frames, int NrBlocks, float Boost, int Hz)
{
	if(NrBlocks < 1 || NrBlocks > 32 || Frames < 1) return 0;
	
	Oversample = Playing = 0;
	this->Frames = Frames;
	this->NrBlocks = NrBlocks;
	if(!AHXOutput::Init(Frequency, Bits, Frames, Boost, Hz)) return 0;

	BlockLen = Frequency * Bits / 8 * Frames /Hz;

	// Set up the wave format
	WAVEFORMATEX format;
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = 1;
	format.nSamplesPerSec = Frequency;
	format.wBitsPerSample = Bits;
	format.nAvgBytesPerSec = format.nChannels * format.nSamplesPerSec * format.wBitsPerSample / 8;
	format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
	format.cbSize = 0;

	// Open the playback device
	if(waveOutOpen(&hwo, WAVE_MAPPER, &format, 0, 0, 0) != MMSYSERR_NOERROR) return 0;

	for(int i = 0; i < NrBlocks; i++) {
		// grab memory for data
		if(!(Blocks[i].DataHandle = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, BlockLen))) return 0;
		if(!(Blocks[i].Data = (LPBYTE)GlobalLock(Blocks[i].DataHandle))) return 0;
		// grab memory for headers
		if(!(Blocks[i].HeaderHandle = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, sizeof(WAVEHDR)))) return 0;
		if(!(Blocks[i].Header = (LPWAVEHDR)GlobalLock(Blocks[i].HeaderHandle))) return 0;
		Blocks[i].Header->lpData = (char*)Blocks[i].Data;
		Blocks[i].Header->dwBufferLength = BlockLen;
		Blocks[i].Header->dwFlags = WHDR_DONE; // mark the block is done
		Blocks[i].Header->dwLoops = 0;
		Blocks[i].Prepared = 0;
	}

	return 1;
}

int AHXWaveOut::Free()
{
	if(waveOutReset(hwo) != MMSYSERR_NOERROR) return 0;

	// wait for all blocks to finish
	for(;;) {
		int alldone = 1;
		for(int i = 0; i < NrBlocks; i++) if((Blocks[i].Header->dwFlags & WHDR_DONE) == 0) alldone = 0;
		if(alldone) break;
		Sleep(1000/Hz);
	}

	// unprepare all blocks
	for(int i = 0; i < NrBlocks; i++)
		if(Blocks[i].Prepared) if(waveOutUnprepareHeader(hwo, Blocks[i].Header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) return 0;

	// close wave output
	if(waveOutClose(hwo) != MMSYSERR_NOERROR) return 0;

	// kill all blocks
	for(    i = 0; i < NrBlocks; i++) {
		if((!GlobalUnlock(Blocks[i].DataHandle)) && (GetLastError() != NO_ERROR)) return 0;
		if(GlobalFree(Blocks[i].DataHandle)) return 0;
		if((!GlobalUnlock(Blocks[i].HeaderHandle)) && (GetLastError() != NO_ERROR)) return 0;
		if(GlobalFree(Blocks[i].HeaderHandle)) return 0;
	}

	return 1;
}

int AHXWaveOut::StartBackgroundPlay()
{
	StopPlaying = 0;
	return _beginthread(ThreadEntry, 0, this) != -1;
}

int AHXWaveOut::StopBackgroundPlay()
{
	StopPlaying = 1;
	while(StopPlaying) Sleep(Frames*(1000/Hz)/2);
	return 1;
}

int AHXWaveOut::Play(AHXPlayer* Player)
{
	if(!Stop() || !Player) return 0;
	this->Player = Player;
	Playing = 1;
	return 1;
}

int AHXWaveOut::Pause()
{
	Paused = 1;
	return waveOutPause(hwo);
}

int AHXWaveOut::Resume()
{
	Paused = 0;
	return waveOutRestart(hwo) == MMSYSERR_NOERROR;
}

int AHXWaveOut::Stop()
{
	Playing = 0;
	return waveOutReset(hwo) == MMSYSERR_NOERROR;
}

int AHXWaveOut::SetVolume(int Volume)
{
	Volume = Volume * 63 / 64;
	return waveOutSetVolume(hwo, ((Volume << 10) << 16) | (Volume << 10)) == MMSYSERR_NOERROR;
}

void AHXWaveOut::ThreadEntry(void* pArg)
{
	AHXWaveOut* This = (AHXWaveOut*)pArg;

	// Boost the thread priority
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

	This->EventLoop();
}

void AHXWaveOut::EventLoop()
{
	// Main message loop
	while(StopPlaying == 0) {
		if(Playing) PlayIt();
		Sleep(Frames*(1000/Hz));
	}
	StopPlaying = 0;
	_endthread();
}

void AHXWaveOut::PlayIt()
{
	for(int i = 0; i < NrBlocks; i++) {
		if((Blocks[i].Header->dwFlags & WHDR_DONE) == 0) continue;

		// unprepare header
		if(Blocks[i].Prepared) {
			waveOutUnprepareHeader(hwo, Blocks[i].Header, sizeof(WAVEHDR));
			Blocks[i].Prepared = 0;
		}

		// mix a new block (size: Frames frames = BlockLen)
		MixBuffer();

		// prepare it for waveOut
		OutputBuffer(i);
	}
}

void AHXWaveOut::OutputBuffer(int num)
{
#define LOW_CLIP16     -0x8000
#define HI_CLIP16       0x7FFF
#define LOW_CLIP8      -0x80
#define HI_CLIP8        0x7F

	void* lpChunkData = Blocks[num].Data;
	int thissample;

	if(Bits == 16) {
		for(int s = 0; s < BlockLen/(Bits/8); s++) {
			thissample = MixingBuffer[s] << 6; // 16 bit
			((short*)lpChunkData)[s] = thissample < LOW_CLIP16 ? LOW_CLIP16 : thissample > HI_CLIP16 ? HI_CLIP16 : thissample;
		}
	} else if(Bits == 8) {
		for(int s = 0; s < BlockLen/(Bits/8); s++) {
			thissample = MixingBuffer[s] >> 2; // 8 bit
			((char*)lpChunkData)[s] = (thissample < LOW_CLIP8 ? LOW_CLIP8 : thissample > HI_CLIP8 ? HI_CLIP8 : thissample)+128;
		}
	}
//	FILE*fi = fopen("dump.raw", "ab"); fwrite(lpChunkData, MixLen*Frequency/50*2, 1, fi); fclose(fi);
	Blocks[num].Header->lpData = (char*)lpChunkData;
	Blocks[num].Header->dwBufferLength = BlockLen;
	Blocks[num].Header->dwFlags = 0L;
	Blocks[num].Header->dwLoops = 0L;
	waveOutPrepareHeader(hwo, Blocks[num].Header, sizeof(WAVEHDR));
	Blocks[num].Prepared = 1;
	waveOutWrite(hwo, Blocks[num].Header, sizeof(WAVEHDR));
}

int AHXWaveOut::CopyBuffer(void* lpBuffer, int* lpValid)
{
	return 0;

}

#endif
