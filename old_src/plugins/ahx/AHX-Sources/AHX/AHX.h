#ifndef __AHX_H
#define __AHX_H

#define Period2Freq(period) (3579545.25f / (period))

class AHXWaveOut;
class AHXWaves;
class AHXPlayer;
class AHXPList;
class AHXPListEntry;
class AHXInstrument;
class AHXSong;
class AHXPosition;
class AHXStep;
class AHXBuffer;
class AHXEnvelope;

class  AHXPListEntry {
public:
	int Note;
	int Fixed;
	int Waveform;
	int FX[2], FXParam[2];
};

class  AHXPList {
public:
	int Speed, Length;
	AHXPListEntry* Entries;
};

class  AHXEnvelope {
public:
	int aFrames, aVolume;
	int dFrames, dVolume;
	int sFrames;
	int rFrames, rVolume;
};

class  AHXInstrument {
public:
	char* Name;
	int Volume;			// 0..64
	int WaveLength;		// 0..5 (shifts)
	AHXEnvelope Envelope;
	int FilterLowerLimit, FilterUpperLimit, FilterSpeed;
	int SquareLowerLimit, SquareUpperLimit, SquareSpeed;
	int VibratoDelay, VibratoDepth, VibratoSpeed;
	int HardCutRelease, HardCutReleaseFrames;
	AHXPList PList;
};

class  AHXSong {
public:
	 AHXSong();
	~AHXSong();

	char* Name;
	int Restart, PositionNr, TrackLength, TrackNr, InstrumentNr, SubsongNr;
	int Revision, SpeedMultiplier;
	AHXPosition* Positions;
	AHXStep** Tracks;
	AHXInstrument* Instruments;
	int* Subsongs;
};

class  AHXPosition {
public:
	int Track[4], Transpose[4];
};

class  AHXStep {
public:
	int Note, Instrument, FX, FXParam;
};

class  AHXVoice {
public:
	// Read those variables for mixing!
	int VoiceVolume, VoicePeriod;
	char VoiceBuffer[0x281]; // for oversampling optimization!

protected:
	AHXVoice();
	void Init();

	void CalcADSR();

	int Track, Transpose;
	int NextTrack, NextTranspose;
	int ADSRVolume; // fixed point 8:8
	AHXEnvelope ADSR; // frames/delta fixed 8:8
	AHXInstrument* Instrument; // current instrument
	int InstrPeriod, TrackPeriod, VibratoPeriod;
	int NoteMaxVolume, PerfSubVolume, TrackMasterVolume;
	int NewWaveform, Waveform, PlantSquare, PlantPeriod, IgnoreSquare;
	int TrackOn, FixedNote;
	int VolumeSlideUp, VolumeSlideDown;
	int HardCut, HardCutRelease, HardCutReleaseF;
	int PeriodSlideSpeed, PeriodSlidePeriod, PeriodSlideLimit, PeriodSlideOn, PeriodSlideWithLimit;
	int PeriodPerfSlideSpeed, PeriodPerfSlidePeriod, PeriodPerfSlideOn;
	int VibratoDelay, VibratoCurrent, VibratoDepth, VibratoSpeed;
	int SquareOn, SquareInit, SquareWait, SquareLowerLimit, SquareUpperLimit, SquarePos, SquareSign, SquareSlidingIn, SquareReverse;
	int FilterOn, FilterInit, FilterWait, FilterLowerLimit, FilterUpperLimit, FilterPos, FilterSign, FilterSpeed, FilterSlidingIn, IgnoreFilter;
	int PerfCurrent, PerfSpeed, PerfWait;
	int WaveLength;
	AHXPList* PerfList;
	int NoteDelayWait, NoteDelayOn, NoteCutWait, NoteCutOn;
	char* AudioPointer, * AudioSource;
	int AudioPeriod, AudioVolume;
	char SquareTempBuffer[0x80];

	friend class AHXPlayer;
};

class  AHXWaves {
public:
	AHXWaves();

protected:	
	void Generate();
	void GenerateTriangle(char* Buffer, int Len);
	void GenerateSquare(char* Buffer);
	void GenerateSawtooth(char* Buffer, int Len);
	void GenerateWhiteNoise(char* Buffer, int Len);
	void GenerateFilterWaveforms(char* Buffer, char* Low, char* High);

public:
	struct {
		char LowPasses[(0xfc+0xfc+0x80*0x1f+0x80+3*0x280)*31];
		char Triangle04[0x04], Triangle08[0x08], Triangle10[0x10], Triangle20[0x20], Triangle40[0x40], Triangle80[0x80];
		char Sawtooth04[0x04], Sawtooth08[0x08], Sawtooth10[0x10], Sawtooth20[0x20], Sawtooth40[0x40], Sawtooth80[0x80];
		char Squares[0x80*0x20];
		char WhiteNoiseBig[0x280*3];
		char HighPasses[(0xfc+0xfc+0x80*0x1f+0x80+3*0x280)*31];
	};
};

class  AHXPlayer {
public:
	void Init(AHXWaves* Waves = 0);

	int LoadSong(char* Filename);
	int LoadSong(void* Buffer, int Len);
	int InitSubsong(int Nr);
	void PlayIRQ();

	void VoiceOnOff(int Voice, int OnOff);
	void NextPosition();
	void PrevPosition();

	int PlayingTime;
	AHXSong Song;

//protected:
	void ProcessStep(int v);
	void ProcessFrame(int v);
	void SetAudio(int v);
	void PListCommandParse(int v, int FX, int FXParam);

	AHXWaves* Waves; int OurWaves;
	AHXVoice Voices[4];

	static int VibratoTable[];
	static int PeriodTable[];
//	static char* Notes[];
	int StepWaitFrames, GetNewPosition, SongEndReached, TimingValue;
	int PatternBreak;
	int MainVolume;
	int Playing, Tempo;
	int PosNr, PosJump;
	int NoteNr, PosJumpNote;
	char* WaveformTab[4];
	int WNRandom;

	friend class AHXOutput;
	friend class AHXWaveOut;
	friend class AHXDisplay;
};

#define AHXOF_BOOST        0
#define AHXOI_OVERSAMPLING 1

class  AHXOutput {
public:
	 AHXOutput();
	~AHXOutput();

	virtual int Init(int Frequency, int Bits, int MixLen, float Boost, int Hz);
	virtual int Free();
	virtual int SetOption(int Option, int Value);
	virtual int SetOption(int Option, float Value);
	virtual int GetOption(int Option, int* pValue);
	virtual int GetOption(int Option, float* pValue);
	virtual int StartBackgroundPlay() = 0;
	virtual int StopBackgroundPlay() = 0;
	virtual int Play(AHXPlayer* Player) = 0;
	virtual int Stop() = 0;
	virtual int Pause() = 0;
	virtual int Resume() = 0;
	virtual int SetVolume(int Volume) = 0;
	virtual int CopyBuffer(void* lpBuffer, int* lpValid) = 0;

	int Bits, Frequency, MixLen;
	int Hz;
	int Playing, Paused;

	AHXPlayer* Player;

protected:
	virtual void MixChunk(int NrSamples, int** mb);
	virtual void MixBuffer();

	//Options
	int Oversampling;
	float Boost;

	int* MixingBuffer;
	int VolumeTable[65][256];
};

#endif //__AHX_H
