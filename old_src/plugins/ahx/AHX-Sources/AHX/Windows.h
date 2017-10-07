#ifndef __WINDOWS_H
#define __WINDOWS_H

#include <mmsystem.h>

#include "ahx.h"

class AHXWaveOut : public AHXOutput {
public:
	 AHXWaveOut();
	~AHXWaveOut();

	int Init(int Frequency = 44100, int Bits = 16, int Frames = 2, int NrBlocks = 16, float Boost = 1.f, int Hz = 50);
	int Free();
	int StartBackgroundPlay();
	int StopBackgroundPlay();
	int Play(AHXPlayer* Player);
	int Pause();
	int Resume();
	int Stop();
	int SetVolume(int Volume);
	int CopyBuffer(void* lpBuffer, int* lpValid);

protected:
//	void MixChunk(int NrSamples, int** mb);
	void PlayIt();
	void OutputBuffer(int num);
	void EventLoop();

	HWAVEOUT hwo;

	struct {
		HGLOBAL   DataHandle;
		LPBYTE    Data;
		HGLOBAL   HeaderHandle;
		LPWAVEHDR Header;
		int       Prepared;
	} Blocks[32];
	int NrBlocks, Frames, BlockLen;
	volatile int StopPlaying;

	int Oversample;

	static void ThreadEntry(void* pArg);

	friend class AHXDisplay;
};

#endif