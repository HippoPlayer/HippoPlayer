#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ahx/AHX.h"
//#include "Debug.h"

// AHXSong /////////////////////////////////////////////////////////////////////////////////////////////////

AHXSong::AHXSong()
{
	Restart = PositionNr = TrackLength = TrackNr = InstrumentNr = SubsongNr = 0;
	Name = NULL;
	Positions = NULL;
	Tracks = NULL;
	Instruments = NULL;
	Subsongs = NULL;
}

AHXSong::~AHXSong()
{
	delete Name;
	delete[] Positions;
	delete[] Tracks;
	delete[] Instruments;
	delete[] Subsongs;
}

// AHXPlayer ///////////////////////////////////////////////////////////////////////////////////////////////

int AHXPlayer::VibratoTable[] = { 
	0,24,49,74,97,120,141,161,180,197,212,224,235,244,250,253,255,
	253,250,244,235,224,212,197,180,161,141,120,97,74,49,24,
	0,-24,-49,-74,-97,-120,-141,-161,-180,-197,-212,-224,-235,-244,-250,-253,-255,
	-253,-250,-244,-235,-224,-212,-197,-180,-161,-141,-120,-97,-74,-49,-24
};

int AHXPlayer::PeriodTable[] = {
	0x0000, 0x0D60, 0x0CA0, 0x0BE8, 0x0B40, 0x0A98, 0x0A00, 0x0970,
	0x08E8, 0x0868, 0x07F0, 0x0780, 0x0714, 0x06B0, 0x0650, 0x05F4,
	0x05A0, 0x054C, 0x0500, 0x04B8, 0x0474, 0x0434, 0x03F8, 0x03C0,
	0x038A, 0x0358, 0x0328, 0x02FA, 0x02D0, 0x02A6, 0x0280, 0x025C,
	0x023A, 0x021A, 0x01FC, 0x01E0, 0x01C5, 0x01AC, 0x0194, 0x017D,
	0x0168, 0x0153, 0x0140, 0x012E, 0x011D, 0x010D, 0x00FE, 0x00F0,
	0x00E2, 0x00D6, 0x00CA, 0x00BE, 0x00B4, 0x00AA, 0x00A0, 0x0097,
	0x008F, 0x0087, 0x007F, 0x0078, 0x0071
};
/*
char *AHXPlayer::Notes[] = {
	"---",
	"C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
	"C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
	"C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
	"C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
	"C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5"
};
*/
#pragma warning(disable:4309 4305)


void AHXPlayer::Init(AHXWaves* Waves /*= NULL*/)
{
	if(Waves) {
		OurWaves = 0;
		this->Waves = Waves;
	} else {
		OurWaves = 1;
		this->Waves = new AHXWaves;
	}
	WaveformTab[0] = this->Waves->Triangle04;
	WaveformTab[1] = this->Waves->Sawtooth04;
	WaveformTab[3] = this->Waves->WhiteNoiseBig;
}

int AHXPlayer::LoadSong(char* Filename)
{
	unsigned char SongBuffer[65536];
	FILE* f = fopen(Filename, "rb"); if(!f) return 0;
	int SongLength = fread(SongBuffer, 1, 65536, f);
	fclose(f);

	return LoadSong(SongBuffer, SongLength);
}

int AHXPlayer::LoadSong(void* Buffer, int Len)
{
	unsigned char* SongBuffer = (unsigned char*)Buffer;
	unsigned char* SBPtr = &SongBuffer[14];
	int SongLength = Len;
	if(SongLength < 14 || SongLength == 65536) return 0;

	if(SongBuffer[0] != 'T' && SongBuffer[1] != 'H' && SongBuffer[2] != 'X') return 0;
	Song.Revision = SongBuffer[3];
	if(Song.Revision > 1) return 0;

	// Header ////////////////////////////////////////////
	// Songname
	char* NamePtr = (char*)&SongBuffer[(SongBuffer[4]<<8) | SongBuffer[5]];
	Song.Name = new char[strlen(NamePtr)+1];
	strcpy(Song.Name, NamePtr); NamePtr += strlen(NamePtr)+1;
	Song.SpeedMultiplier = ((SongBuffer[6]>>5)&3)+1;

	Song.PositionNr = ((SongBuffer[6]&0xf)<<8) | SongBuffer[7];
	Song.Restart = (SongBuffer[8]<<8) | SongBuffer[9];
	Song.TrackLength = SongBuffer[10];
	Song.TrackNr = SongBuffer[11];
	Song.InstrumentNr = SongBuffer[12];
	Song.SubsongNr = SongBuffer[13];

	int i;

	// Subsongs //////////////////////////////////////////
	Song.Subsongs = new int[Song.SubsongNr];
	for(i = 0; i < Song.SubsongNr; i++) {
		if(SBPtr - SongBuffer > SongLength) return 0;
		Song.Subsongs[i] = (SBPtr[0]<<8)|SBPtr[1];
		SBPtr += 2;
	}

	// Position List /////////////////////////////////////
	Song.Positions = new AHXPosition[Song.PositionNr];
	for(i = 0; i < Song.PositionNr; i++) {
		for(int j = 0; j < 4; j++) {
			if(SBPtr - SongBuffer > SongLength) return 0;
			Song.Positions[i].Track[j] = *SBPtr++;
			Song.Positions[i].Transpose[j] = *(signed char*)SBPtr++;
		}
	}

	// Tracks ////////////////////////////////////////////
	int MaxTrack = Song.TrackNr;
	Song.Tracks = new AHXStep*[MaxTrack+1];
	for(i = 0; i < MaxTrack+1; i++) {
		Song.Tracks[i] = new AHXStep[Song.TrackLength];
		if((SongBuffer[6]&0x80)==0x80 && i==0) {
			memset(Song.Tracks[i], 0, Song.TrackLength*sizeof(AHXStep));
			continue;
		}
		for(int j = 0; j < Song.TrackLength; j++) {
			if(SBPtr - SongBuffer > SongLength) return 0;
			Song.Tracks[i][j].Note = (SBPtr[0]>>2)&0x3f;
			Song.Tracks[i][j].Instrument = ((SBPtr[0]&0x3)<<4) | (SBPtr[1]>>4);
			Song.Tracks[i][j].FX = SBPtr[1]&0xf;
			Song.Tracks[i][j].FXParam = SBPtr[2];
			SBPtr += 3;
		}
	}

	// Instruments ///////////////////////////////////////
	Song.Instruments = new AHXInstrument[Song.InstrumentNr+1];
	for(i = 1; i < Song.InstrumentNr+1; i++) {
		Song.Instruments[i].Name = new char[strlen(NamePtr)+1];
		strcpy(Song.Instruments[i].Name, NamePtr); NamePtr += strlen(NamePtr)+1;
		if(SBPtr - SongBuffer > SongLength) return 0;
		Song.Instruments[i].Volume = SBPtr[0];
		Song.Instruments[i].FilterSpeed = ((SBPtr[1]>>3)&0x1f) | ((SBPtr[12]>>2)&0x20);
		Song.Instruments[i].WaveLength = SBPtr[1]&0x7;
		Song.Instruments[i].Envelope.aFrames = SBPtr[2];
		Song.Instruments[i].Envelope.aVolume = SBPtr[3];
		Song.Instruments[i].Envelope.dFrames = SBPtr[4]; //4
		Song.Instruments[i].Envelope.dVolume = SBPtr[5];
		Song.Instruments[i].Envelope.sFrames = SBPtr[6];
		Song.Instruments[i].Envelope.rFrames = SBPtr[7]; //7
		Song.Instruments[i].Envelope.rVolume = SBPtr[8];
		Song.Instruments[i].FilterLowerLimit = SBPtr[12]&0x7f;
		Song.Instruments[i].VibratoDelay = SBPtr[13]; //13
		Song.Instruments[i].HardCutReleaseFrames = (SBPtr[14]>>4)&7;
		Song.Instruments[i].HardCutRelease = SBPtr[14]&0x80?1:0;
		Song.Instruments[i].VibratoDepth = SBPtr[14]&0xf; //14
		Song.Instruments[i].VibratoSpeed = SBPtr[15];
		Song.Instruments[i].SquareLowerLimit = SBPtr[16];
		Song.Instruments[i].SquareUpperLimit = SBPtr[17]; //17
		Song.Instruments[i].SquareSpeed = SBPtr[18];
		Song.Instruments[i].FilterUpperLimit = SBPtr[19]&0x3f; //19
		Song.Instruments[i].PList.Speed = SBPtr[20];
		Song.Instruments[i].PList.Length= SBPtr[21];
		SBPtr += 22;
		Song.Instruments[i].PList.Entries=new AHXPListEntry[Song.Instruments[i].PList.Length];
		for(int j = 0; j < Song.Instruments[i].PList.Length; j++) {
			if(SBPtr - SongBuffer > SongLength) return 0;
			Song.Instruments[i].PList.Entries[j].FX[1] = (SBPtr[0]>>5)&7;
			Song.Instruments[i].PList.Entries[j].FX[0] = (SBPtr[0]>>2)&7;
			Song.Instruments[i].PList.Entries[j].Waveform = ((SBPtr[0]<<1)&6) | (SBPtr[1]>>7);
			Song.Instruments[i].PList.Entries[j].Fixed = (SBPtr[1]>>6)&1;
			Song.Instruments[i].PList.Entries[j].Note = SBPtr[1]&0x3f;
			Song.Instruments[i].PList.Entries[j].FXParam[0] = SBPtr[2];
			Song.Instruments[i].PList.Entries[j].FXParam[1] = SBPtr[3];
			SBPtr += 4;
		}
	}

	InitSubsong(0);

	return 1;
}

int AHXPlayer::InitSubsong(int Nr)
{
	if(Nr > Song.SubsongNr) return 0;

	if(Nr == 0) PosNr = 0;
	       else PosNr = Song.Subsongs[Nr-1];

	PosJump = 0;
	PatternBreak = 0;
	MainVolume = 0x40;
	Playing = 1;
	NoteNr = PosJumpNote = 0;
	Tempo = 6;
	StepWaitFrames = 0;
	GetNewPosition = 1;
	SongEndReached = 0;
	TimingValue = PlayingTime = 0;

	for(int v = 0; v < 4; v++) Voices[v].Init();

	return 1;
}

void AHXPlayer::PlayIRQ()
{
	if(StepWaitFrames <= 0) {
		if(GetNewPosition) {
			int NextPos = (PosNr+1==Song.PositionNr)?0:(PosNr+1);
			for(int i = 0; i < 4; i++) {
				Voices[i].Track = Song.Positions[PosNr].Track[i];
				Voices[i].Transpose = Song.Positions[PosNr].Transpose[i];
				Voices[i].NextTrack = Song.Positions[NextPos].Track[i];
				Voices[i].NextTranspose = Song.Positions[NextPos].Transpose[i];
			}
			GetNewPosition = 0;
		}
		for(int i = 0; i < 4; i++) ProcessStep(i);
		StepWaitFrames = Tempo;
	}
	//DoFrameStuff
	for(int i = 0; i < 4; i++) ProcessFrame(i);
	PlayingTime++;
	if(Tempo > 0 && --StepWaitFrames <= 0) {
		if(!PatternBreak) {
			NoteNr++;
			if(NoteNr >= Song.TrackLength) {
				PosJump = PosNr+1;
				PosJumpNote = 0;
				PatternBreak = 1;
			}
		}
		if(PatternBreak) {
			PatternBreak = 0;
			NoteNr = PosJumpNote;
			PosJumpNote = 0;
			PosNr = PosJump;
			PosJump = 0;
			if(PosNr == Song.PositionNr) {
				SongEndReached = 1;
				PosNr = Song.Restart;
			}
			GetNewPosition = 1;
		}
	}
	//RemainPosition
	for(int a = 0; a < 4; a++) SetAudio(a);
}

void AHXPlayer::NextPosition()
{
	PosNr++;
	if(PosNr == Song.PositionNr) PosNr = 0;
	StepWaitFrames = 0;
	GetNewPosition = 1;
}

void AHXPlayer::PrevPosition()
{
	PosNr--;
	if(PosNr < 0) PosNr = 0;
	StepWaitFrames = 0;
	GetNewPosition = 1;
}

void AHXPlayer::ProcessStep(int v)
{
	if(!Voices[v].TrackOn) return;
	Voices[v].VolumeSlideUp = Voices[v].VolumeSlideDown = 0;

	int Note = Song.Tracks[Song.Positions[PosNr].Track[v]][NoteNr].Note;
	int Instrument = Song.Tracks[Song.Positions[PosNr].Track[v]][NoteNr].Instrument;
	int FX = Song.Tracks[Song.Positions[PosNr].Track[v]][NoteNr].FX;
	int FXParam = Song.Tracks[Song.Positions[PosNr].Track[v]][NoteNr].FXParam;

	switch(FX) {
		case 0x0: // Position Jump HI
			if((FXParam & 0xf) > 0 && (FXParam & 0xf) <= 9)
				PosJump = FXParam & 0xf;
			break;
		case 0x5: // Volume Slide + Tone Portamento
		case 0xa: // Volume Slide
			Voices[v].VolumeSlideDown = FXParam & 0x0f;
			Voices[v].VolumeSlideUp   = FXParam >> 4;
			break;
		case 0xb: // Position Jump
			PosJump = PosJump*100 + (FXParam & 0x0f) + (FXParam >> 4)*10;
			PatternBreak = 1;
			break;
		case 0xd: // Patternbreak
			PosJump = PosNr + 1;
			PosJumpNote = (FXParam & 0x0f) + (FXParam >> 4)*10;
			if(PosJumpNote > Song.TrackLength) PosJumpNote = 0;
			PatternBreak = 1;
			break;
		case 0xe: // Enhanced commands
			switch(FXParam >> 4) {
				case 0xc: // Note Cut
					if((FXParam & 0x0f) < Tempo) {
						Voices[v].NoteCutWait = FXParam & 0x0f;
						if(Voices[v].NoteCutWait) {
							Voices[v].NoteCutOn = 1;
							Voices[v].HardCutRelease = 0;
						}
					}
					break;
				case 0xd: // Note Delay
					if(Voices[v].NoteDelayOn) {
						Voices[v].NoteDelayOn = 0;
					} else {
						if((FXParam & 0x0f) < Tempo) {
							Voices[v].NoteDelayWait = FXParam & 0x0f;
							if(Voices[v].NoteDelayWait) {
								Voices[v].NoteDelayOn = 1;
								return;
							}
						}
					}
					break;
			}
			break;
		case 0xf: // Speed
			Tempo = FXParam;
			break;
	}
	if(Instrument) {
		Voices[v].PerfSubVolume = 0x40;
		Voices[v].PeriodSlideSpeed = Voices[v].PeriodSlidePeriod = Voices[v].PeriodSlideLimit = 0;
		Voices[v].ADSRVolume = 0;
		Voices[v].Instrument = &Song.Instruments[Instrument];
		Voices[v].CalcADSR();
		//InitOnInstrument
		Voices[v].WaveLength = Voices[v].Instrument->WaveLength;
		Voices[v].NoteMaxVolume = Voices[v].Instrument->Volume;
		//InitVibrato
		Voices[v].VibratoCurrent = 0;
		Voices[v].VibratoDelay = Voices[v].Instrument->VibratoDelay;
		Voices[v].VibratoDepth = Voices[v].Instrument->VibratoDepth;
		Voices[v].VibratoSpeed = Voices[v].Instrument->VibratoSpeed;
		Voices[v].VibratoPeriod = 0;
		//InitHardCut
		Voices[v].HardCutRelease = Voices[v].Instrument->HardCutRelease;
		Voices[v].HardCut = Voices[v].Instrument->HardCutReleaseFrames;
		//InitSquare
		Voices[v].IgnoreSquare = Voices[v].SquareSlidingIn = 0;
		Voices[v].SquareWait = Voices[v].SquareOn = 0;
		int SquareLower = Voices[v].Instrument->SquareLowerLimit >> (5-Voices[v].WaveLength);
		int SquareUpper = Voices[v].Instrument->SquareUpperLimit >> (5-Voices[v].WaveLength);
		if(SquareUpper < SquareLower) { int t = SquareUpper; SquareUpper = SquareLower; SquareLower = t; }
		Voices[v].SquareUpperLimit = SquareUpper;
		Voices[v].SquareLowerLimit = SquareLower;
		//InitFilter
		Voices[v].IgnoreFilter = Voices[v].FilterWait = Voices[v].FilterOn = 0;
		Voices[v].FilterSlidingIn = 0;
		int d6 = Voices[v].Instrument->FilterSpeed;
		int d3 = Voices[v].Instrument->FilterLowerLimit;
		int d4 = Voices[v].Instrument->FilterUpperLimit;
		if(d3 & 0x80) d6 |= 0x20;
		if(d4 & 0x80) d6 |= 0x40;
		Voices[v].FilterSpeed = d6;
		d3 &= ~0x80;
		d4 &= ~0x80;
		if(d3 > d4) { int t = d3; d3 = d4; d4 = t; }
		Voices[v].FilterUpperLimit = d4;
		Voices[v].FilterLowerLimit = d3;
		Voices[v].FilterPos = 32;
		//Init PerfList
		Voices[v].PerfWait  =  Voices[v].PerfCurrent = 0;
		Voices[v].PerfSpeed =  Voices[v].Instrument->PList.Speed;
		Voices[v].PerfList  = &Voices[v].Instrument->PList;
	}
	//NoInstrument
	Voices[v].PeriodSlideOn = 0;

	switch(FX) {
		case 0x4: // Override filter
			break;
		case 0x9: // Set Squarewave-Offset
			Voices[v].SquarePos = FXParam >> (5 - Voices[v].WaveLength);
			Voices[v].PlantSquare = 1;
			Voices[v].IgnoreSquare = 1;
			break;
		case 0x5: // Tone Portamento + Volume Slide
		case 0x3: // Tone Portamento (Period Slide Up/Down w/ Limit)
			if(FXParam != 0) Voices[v].PeriodSlideSpeed = FXParam;
			if(Note) {
				int Neue = PeriodTable[Note];
				int Alte = PeriodTable[Voices[v].TrackPeriod];
				Alte -= Neue;
				Neue = Alte + Voices[v].PeriodSlidePeriod;
				if(Neue) Voices[v].PeriodSlideLimit = -Alte;
			}
			Voices[v].PeriodSlideOn = 1;
			Voices[v].PeriodSlideWithLimit = 1;
			goto NoNote;
	}

	// Note anschlagen
	if(Note) {
		Voices[v].TrackPeriod = Note;
		Voices[v].PlantPeriod = 1;
	}
NoNote:
	switch(FX) {
		case 0x1: // Portamento up (Period slide down)
			Voices[v].PeriodSlideSpeed = -FXParam;
			Voices[v].PeriodSlideOn = 1;
			Voices[v].PeriodSlideWithLimit = 0;
			break;
		case 0x2: // Portamento down (Period slide up)
			Voices[v].PeriodSlideSpeed = FXParam;
			Voices[v].PeriodSlideOn = 1;
			Voices[v].PeriodSlideWithLimit = 0;
			break;
		case 0xc: // Volume
			if(FXParam <= 0x40) 
				Voices[v].NoteMaxVolume = FXParam;
			else {
				FXParam -= 0x50;
				if(FXParam <= 0x40)
					for(int i = 0; i < 4; i++) Voices[i].TrackMasterVolume = FXParam;
				else {
					FXParam -= 0xa0 - 0x50;
					if(FXParam <= 0x40)
						Voices[v].TrackMasterVolume = FXParam;
				}
			}
			break;
		case 0xe: // Enhanced commands
			switch(FXParam >> 4) {
				case 0x1: // Fineslide up (Period fineslide down)
					Voices[v].PeriodSlidePeriod = -(FXParam & 0x0f);
					Voices[v].PlantPeriod = 1;
					break;
				case 0x2: // Fineslide down (Period fineslide up)
					Voices[v].PeriodSlidePeriod = FXParam & 0x0f;
					Voices[v].PlantPeriod = 1;
					break;
				case 0x4: // Vibrato control
					Voices[v].VibratoDepth = FXParam & 0x0f;
					break;
				case 0xa: // Finevolume up
					Voices[v].NoteMaxVolume += FXParam & 0x0f;
					if(Voices[v].NoteMaxVolume > 0x40) Voices[v].NoteMaxVolume = 0x40;
					break;
				case 0xb: // Finevolume down
					Voices[v].NoteMaxVolume -= FXParam & 0x0f;
					if(Voices[v].NoteMaxVolume < 0) Voices[v].NoteMaxVolume = 0;
					break;
			}
			break;
	}
}

void AHXPlayer::ProcessFrame(int v)
{
	if(!Voices[v].TrackOn) return;

	if(Voices[v].NoteDelayOn) {
		if(Voices[v].NoteDelayWait <= 0) ProcessStep(v);
		                            else Voices[v].NoteDelayWait--;
	}
	if(Voices[v].HardCut) {
		int NextInstrument;
		if(NoteNr+1 < Song.TrackLength) NextInstrument = Song.Tracks[Voices[v].Track][NoteNr+1].Instrument;
						           else NextInstrument = Song.Tracks[Voices[v].NextTrack][0].Instrument;
		if(NextInstrument) {
			int d1 = Tempo - Voices[v].HardCut;
			if(d1 < 0) d1 = 0;
			if(!Voices[v].NoteCutOn) {
				Voices[v].NoteCutOn = 1;
				Voices[v].NoteCutWait = d1;
				Voices[v].HardCutReleaseF = -(d1 - Tempo);
			} else Voices[v].HardCut = 0;
		}
	}
	if(Voices[v].NoteCutOn) {
		if(Voices[v].NoteCutWait <= 0) {
			Voices[v].NoteCutOn = 0;
			if(Voices[v].HardCutRelease) {
				Voices[v].ADSR.rVolume = -(Voices[v].ADSRVolume - (Voices[v].Instrument->Envelope.rVolume << 8))/Voices[v].HardCutReleaseF;
				Voices[v].ADSR.rFrames = Voices[v].HardCutReleaseF;
				Voices[v].ADSR.aFrames = Voices[v].ADSR.dFrames = Voices[v].ADSR.sFrames = 0;
			} else Voices[v].NoteMaxVolume = 0;
		} else Voices[v].NoteCutWait--;
	}
	//adsrEnvelope
	if(Voices[v].ADSR.aFrames) {
		Voices[v].ADSRVolume += Voices[v].ADSR.aVolume; // Delta
		if(--Voices[v].ADSR.aFrames <= 0) Voices[v].ADSRVolume = Voices[v].Instrument->Envelope.aVolume << 8;
	} else if(Voices[v].ADSR.dFrames) {
		Voices[v].ADSRVolume += Voices[v].ADSR.dVolume; // Delta
		if(--Voices[v].ADSR.dFrames <= 0) Voices[v].ADSRVolume = Voices[v].Instrument->Envelope.dVolume << 8;
	} else if(Voices[v].ADSR.sFrames) {
		Voices[v].ADSR.sFrames--;
	} else if(Voices[v].ADSR.rFrames) {
		Voices[v].ADSRVolume += Voices[v].ADSR.rVolume; // Delta
		if(--Voices[v].ADSR.rFrames <= 0) Voices[v].ADSRVolume = Voices[v].Instrument->Envelope.rVolume << 8;
	}
	//VolumeSlide
	Voices[v].NoteMaxVolume = Voices[v].NoteMaxVolume + Voices[v].VolumeSlideUp - Voices[v].VolumeSlideDown;
	if(Voices[v].NoteMaxVolume < 0) Voices[v].NoteMaxVolume = 0;
	if(Voices[v].NoteMaxVolume > 0x40) Voices[v].NoteMaxVolume = 0x40;
	//Portamento
	if(Voices[v].PeriodSlideOn) {
		if(Voices[v].PeriodSlideWithLimit) {
			int d0 = Voices[v].PeriodSlidePeriod - Voices[v].PeriodSlideLimit;
			int d2 = Voices[v].PeriodSlideSpeed;
			if(d0 > 0) d2 = -d2;
			if(d0) {
				int d3 = (d0 + d2) ^ d0;
				if(d3 >= 0) d0 = Voices[v].PeriodSlidePeriod + d2;
				       else d0 = Voices[v].PeriodSlideLimit;
				Voices[v].PeriodSlidePeriod = d0;
				Voices[v].PlantPeriod = 1;
			}
		} else {
			Voices[v].PeriodSlidePeriod += Voices[v].PeriodSlideSpeed;
			Voices[v].PlantPeriod = 1;
		}
	}
	//Vibrato
	if(Voices[v].VibratoDepth) {
		if(Voices[v].VibratoDelay <= 0) {
			Voices[v].VibratoPeriod = (VibratoTable[Voices[v].VibratoCurrent] * Voices[v].VibratoDepth) >> 7;
			Voices[v].PlantPeriod = 1;
			Voices[v].VibratoCurrent = (Voices[v].VibratoCurrent + Voices[v].VibratoSpeed) & 0x3f;
		} else Voices[v].VibratoDelay--;
	}
	//PList
	if(Voices[v].Instrument && Voices[v].PerfCurrent < Voices[v].Instrument->PList.Length) {
		if(--Voices[v].PerfWait <= 0) {
			int Cur = Voices[v].PerfCurrent++;
			Voices[v].PerfWait = Voices[v].PerfSpeed;
			if(Voices[v].PerfList->Entries[Cur].Waveform) {
				Voices[v].Waveform = Voices[v].PerfList->Entries[Cur].Waveform-1;
				Voices[v].NewWaveform = 1;
				Voices[v].PeriodPerfSlideSpeed = Voices[v].PeriodPerfSlidePeriod = 0;
			}
			//Holdwave
			Voices[v].PeriodPerfSlideOn = 0;
			for(int i = 0; i < 2; i++) PListCommandParse(v, Voices[v].PerfList->Entries[Cur].FX[i], Voices[v].PerfList->Entries[Cur].FXParam[i]);
			//GetNote
			if(Voices[v].PerfList->Entries[Cur].Note) {
				Voices[v].InstrPeriod = Voices[v].PerfList->Entries[Cur].Note;
				Voices[v].PlantPeriod = 1;
				Voices[v].FixedNote = Voices[v].PerfList->Entries[Cur].Fixed;
			}
		}
	} else {
		if(Voices[v].PerfWait) Voices[v].PerfWait--;
		                  else Voices[v].PeriodPerfSlideSpeed = 0;
	}
	//PerfPortamento
	if(Voices[v].PeriodPerfSlideOn) {
		Voices[v].PeriodPerfSlidePeriod -= Voices[v].PeriodPerfSlideSpeed;
		if(Voices[v].PeriodPerfSlidePeriod) Voices[v].PlantPeriod = 1;
	}
	if(Voices[v].Waveform == 3-1 && Voices[v].SquareOn) {
		if(--Voices[v].SquareWait <= 0) {
			int d1 = Voices[v].SquareLowerLimit;
			int	d2 = Voices[v].SquareUpperLimit;
			int d3 = Voices[v].SquarePos;
			if(Voices[v].SquareInit) {
				Voices[v].SquareInit = 0;
				if(d3 <= d1) { 
					Voices[v].SquareSlidingIn = 1;
					Voices[v].SquareSign = 1;
				} else if(d3 >= d2) {
					Voices[v].SquareSlidingIn = 1;
					Voices[v].SquareSign = -1;
				}
			}
			//NoSquareInit
			if(d1 == d3 || d2 == d3) {
				if(Voices[v].SquareSlidingIn) {
					Voices[v].SquareSlidingIn = 0;
				} else {
					Voices[v].SquareSign = -Voices[v].SquareSign;
				}
			}
			d3 += Voices[v].SquareSign;
			Voices[v].SquarePos = d3;
			Voices[v].PlantSquare = 1;
			Voices[v].SquareWait = Voices[v].Instrument->SquareSpeed;
		}
	}
	if(Voices[v].FilterOn && --Voices[v].FilterWait <= 0) {
		int d1 = Voices[v].FilterLowerLimit;
		int d2 = Voices[v].FilterUpperLimit;
		int d3 = Voices[v].FilterPos;
		if(Voices[v].FilterInit) {
			Voices[v].FilterInit = 0;
			if(d3 <= d1) {
				Voices[v].FilterSlidingIn = 1;
				Voices[v].FilterSign = 1;
			} else if(d3 >= d2) {
				Voices[v].FilterSlidingIn = 1;
				Voices[v].FilterSign = -1;
			}
		}
		//NoFilterInit
		int FMax = (Voices[v].FilterSpeed < 3)?(5-Voices[v].FilterSpeed):1;
		for(int i = 0; i < FMax; i++) {
			if(d1 == d3 || d2 == d3) {
				if(Voices[v].FilterSlidingIn) {
					Voices[v].FilterSlidingIn = 0;
				} else {
					Voices[v].FilterSign = -Voices[v].FilterSign;
				}
			}
			d3 += Voices[v].FilterSign;
		}
		Voices[v].FilterPos = d3;
		Voices[v].NewWaveform = 1;
		Voices[v].FilterWait = Voices[v].FilterSpeed - 3;
		if(Voices[v].FilterWait < 1) Voices[v].FilterWait = 1;
	}
	if(Voices[v].Waveform == 3-1 || Voices[v].PlantSquare) {
		//CalcSquare
		char* SquarePtr = &Waves->Squares[(Voices[v].FilterPos-0x20)*(0xfc+0xfc+0x80*0x1f+0x80+0x280*3)];
		int X = Voices[v].SquarePos << (5 - Voices[v].WaveLength);
		if(X > 0x20) {
			X = 0x40 - X;
			Voices[v].SquareReverse = 1;
		}
		//OkDownSquare
		if(--X) SquarePtr += X << 7;
		int Delta = 32 >> Voices[v].WaveLength;
		WaveformTab[2] = Voices[v].SquareTempBuffer;
		for(int i = 0; i < (1 << Voices[v].WaveLength)*4; i++) {
			Voices[v].SquareTempBuffer[i] = *SquarePtr;
			SquarePtr += Delta;
		}
		Voices[v].NewWaveform = 1;
		Voices[v].Waveform = 3-1;
		Voices[v].PlantSquare = 0;
	}
	if(Voices[v].Waveform == 4-1) Voices[v].NewWaveform = 1;

	if(Voices[v].NewWaveform) {
		char* AudioSource = WaveformTab[Voices[v].Waveform];
		if(Voices[v].Waveform != 3-1) {
			AudioSource += (Voices[v].FilterPos-0x20)*(0xfc+0xfc+0x80*0x1f+0x80+0x280*3);
		}
		if(Voices[v].Waveform < 3-1) {
			//GetWLWaveformlor2
			static int Offsets[] = {0x00,0x04,0x04+0x08,0x04+0x08+0x10,0x04+0x08+0x10+0x20,0x04+0x08+0x10+0x20+0x40};
			AudioSource += Offsets[Voices[v].WaveLength];
		}
		if(Voices[v].Waveform == 4-1) {
			//AddRandomMoving
			AudioSource += (WNRandom & (2*0x280-1)) & ~1;
			//GoOnRandom
			WNRandom += 2239384;
			WNRandom = ((((WNRandom >> 8) | (WNRandom << 24)) + 782323) ^ 75) - 6735;
		}
		Voices[v].AudioSource = AudioSource;
	}
	//StillHoldWaveform
	//AudioInitPeriod
	Voices[v].AudioPeriod = Voices[v].InstrPeriod;
	if(!Voices[v].FixedNote) Voices[v].AudioPeriod += Voices[v].Transpose + Voices[v].TrackPeriod-1;
	if(Voices[v].AudioPeriod > 5*12) Voices[v].AudioPeriod = 5*12;
	if(Voices[v].AudioPeriod < 0)    Voices[v].AudioPeriod = 0;
	Voices[v].AudioPeriod = PeriodTable[Voices[v].AudioPeriod];
	if(!Voices[v].FixedNote) Voices[v].AudioPeriod += Voices[v].PeriodSlidePeriod;
	Voices[v].AudioPeriod += Voices[v].PeriodPerfSlidePeriod + Voices[v].VibratoPeriod;
	if(Voices[v].AudioPeriod > 0x0d60) Voices[v].AudioPeriod = 0x0d60;
	if(Voices[v].AudioPeriod < 0x0071) Voices[v].AudioPeriod = 0x0071;
	//AudioInitVolume
	Voices[v].AudioVolume = ((((((((Voices[v].ADSRVolume >> 8) * Voices[v].NoteMaxVolume) >> 6) * Voices[v].PerfSubVolume) >> 6) * Voices[v].TrackMasterVolume) >> 6) * MainVolume) >> 6;
}

void AHXPlayer::SetAudio(int v)
{
	if(!Voices[v].TrackOn) {
		Voices[v].VoiceVolume = 0;
		return;
	}

	Voices[v].VoiceVolume = Voices[v].AudioVolume;
	if(Voices[v].PlantPeriod) {
		Voices[v].PlantPeriod = 0;
		Voices[v].VoicePeriod = Voices[v].AudioPeriod;
	}
	if(Voices[v].NewWaveform) {
		if(Voices[v].Waveform == 4-1) {
			memcpy(Voices[v].VoiceBuffer, Voices[v].AudioSource, 0x280);
		} else {
			int WaveLoops = (1 << (5-Voices[v].WaveLength))*5;
			for(int i = 0; i < WaveLoops; i++) memcpy(&Voices[v].VoiceBuffer[i*4*(1 << Voices[v].WaveLength)], Voices[v].AudioSource, 4*(1 << Voices[v].WaveLength));
		}
		Voices[v].VoiceBuffer[0x280] = Voices[v].VoiceBuffer[0];
	}
}

void AHXPlayer::PListCommandParse(int v, int FX, int FXParam)
{
	switch(FX) {
		case 0: 
			if(Song.Revision > 0 && FXParam != 0) {
				if(Voices[v].IgnoreFilter) {
					Voices[v].FilterPos = Voices[v].IgnoreFilter;
					Voices[v].IgnoreFilter = 0;
				} else Voices[v].FilterPos = FXParam;
				Voices[v].NewWaveform = 1;
			}
			break;
		case 1:
			Voices[v].PeriodPerfSlideSpeed = FXParam;
			Voices[v].PeriodPerfSlideOn = 1;
			break;
		case 2:
			Voices[v].PeriodPerfSlideSpeed = -FXParam;
			Voices[v].PeriodPerfSlideOn = 1;
			break;
		case 3: // Init Square Modulation
			if(!Voices[v].IgnoreSquare) {
				Voices[v].SquarePos = FXParam >> (5-Voices[v].WaveLength);
			} else Voices[v].IgnoreSquare = 0;
			break;
		case 4: // Start/Stop Modulation
			if(Song.Revision == 0 || FXParam == 0) {
				Voices[v].SquareInit = (Voices[v].SquareOn ^= 1);
				Voices[v].SquareSign = 1;
			} else {
				if(FXParam & 0x0f) {
					Voices[v].SquareInit = (Voices[v].SquareOn ^= 1);
					Voices[v].SquareSign = 1;
					if((FXParam & 0x0f) == 0x0f) Voices[v].SquareSign = -1;
				}
				if(FXParam & 0xf0) {
					Voices[v].FilterInit = (Voices[v].FilterOn ^= 1);
					Voices[v].FilterSign = 1;
					if((FXParam & 0xf0) == 0xf0) Voices[v].FilterSign = -1;
				}
			}
			break;
		case 5: // Jump to Step [xx]
			Voices[v].PerfCurrent = FXParam;
			break;
		case 6: // Set Volume
			if(FXParam > 0x40) {
				if((FXParam -= 0x50) >= 0) {
					if(FXParam <= 0x40) Voices[v].PerfSubVolume = FXParam;
					else 
						if((FXParam -= 0xa0-0x50) >= 0) 
							if(FXParam <= 0x40) Voices[v].TrackMasterVolume = FXParam;
				}
			} else Voices[v].NoteMaxVolume = FXParam;
			break;
		case 7: // set speed
			Voices[v].PerfSpeed = Voices[v].PerfWait = FXParam;
			break;
	}
}

void AHXPlayer::VoiceOnOff(int Voice, int OnOff)
{
	if(Voice < 0 || Voice > 3) return;
	Voices[Voice].TrackOn = OnOff;
}

// AHXVoice ////////////////////////////////////////////////////////////////////////////////////////////////

AHXVoice::AHXVoice()
{
	Init();
}

void AHXVoice::Init()
{
	memset(this, 0, sizeof(AHXVoice));
	memset(VoiceBuffer, 0, 0x281);
	TrackOn = 1;
	TrackMasterVolume = 0x40;
}

void AHXVoice::CalcADSR()
{
	ADSR.aFrames = Instrument->Envelope.aFrames;
	ADSR.aVolume = Instrument->Envelope.aVolume*256/ADSR.aFrames;
	ADSR.dFrames = Instrument->Envelope.dFrames;
	ADSR.dVolume = (Instrument->Envelope.dVolume-Instrument->Envelope.aVolume)*256/ADSR.dFrames;
	ADSR.sFrames = Instrument->Envelope.sFrames;
	ADSR.rFrames = Instrument->Envelope.rFrames;
	ADSR.rVolume = (Instrument->Envelope.rVolume-Instrument->Envelope.dVolume)*256/ADSR.rFrames;
}


// AHXWaves ////////////////////////////////////////////////////////////////////////////////////////////////

AHXWaves::AHXWaves()
{
	Generate();
}

void AHXWaves::Generate()
{
	GenerateSawtooth(Sawtooth04, 0x04);
	GenerateSawtooth(Sawtooth08, 0x08);
	GenerateSawtooth(Sawtooth10, 0x10);
	GenerateSawtooth(Sawtooth20, 0x20);
	GenerateSawtooth(Sawtooth40, 0x40);
	GenerateSawtooth(Sawtooth80, 0x80);
	GenerateTriangle(Triangle04, 0x04);
	GenerateTriangle(Triangle08, 0x08);
	GenerateTriangle(Triangle10, 0x10);
	GenerateTriangle(Triangle20, 0x20);
	GenerateTriangle(Triangle40, 0x40);
	GenerateTriangle(Triangle80, 0x80);
	GenerateSquare(Squares);
	GenerateWhiteNoise(WhiteNoiseBig, 0x280*3);
	GenerateFilterWaveforms(Triangle04, LowPasses, HighPasses);
}

static inline void clip(float* x)
{
	if(*x > 127.f) { *x = 127.f; return; }
	if(*x < -128.f) { *x = -128.f; return; }
}

void AHXWaves::GenerateFilterWaveforms(char* Buffer, char* Low, char* High)
{
	int LengthTable[] = { 3, 7, 0xf, 0x1f, 0x3f, 0x7f, 3, 7, 0xf, 0x1f, 0x3f, 0x7f,
		0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
		0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,
		(0x280*3)-1
	};

	for(int temp = 0, freq = 8; temp < 31; temp++, freq += 3) {
		char* a0 = Buffer;
		for(int waves = 0; waves < 6+6+0x20+1; waves++) {
			float fre = (float)freq*1.25f/100.f;
			float high, mid = 0.f, low = 0.f;
			int i;
			for(i = 0; i <= LengthTable[waves]; i++) {
				high = a0[i] - mid - low; clip(&high);
				mid += high*fre; clip(&mid);
				low += mid*fre; clip(&low);
			}
			for(i = 0; i <= LengthTable[waves]; i++) {
				high = a0[i] - mid - low; clip(&high);
				mid += high*fre; clip(&mid);
				low += mid*fre; clip(&low);
				*Low++ = (char)low;
				*High++ = (char)high;
			}
			a0 += LengthTable[waves]+1;
		}
	}
}

void AHXWaves::GenerateTriangle(char* Buffer, int Len)
{
	int d2 = Len;
	int d5 = d2 >> 2;
	int d1 = 128/d5;
	int d4 = -(d2 >> 1);
	char* edi = Buffer;
	int eax = 0;
	int ecx;
	for(ecx = 0; ecx < d5; ecx++) {
		*edi++ = eax;
		eax += d1;
	}
	*edi++ = 0x7f;
	if(d5 != 1) {
		eax = 128;
		for(ecx = 0; ecx < d5-1; ecx++) {
			eax -= d1;
			*edi++ = eax;
		}
	}
	char* esi = edi + d4;
	for(ecx = 0; ecx < d5*2; ecx++) {
		*edi++ = *esi++;
		if(edi[-1] == 0x7f) edi[-1] = 0x80;
		               else edi[-1] = -edi[-1];
	}
}

void AHXWaves::GenerateSquare(char* Buffer)
{
	char* edi = Buffer;
	int ebx = 0;
	for(int ebx = 1; ebx <= 0x20; ebx++) {
		int ecx;
		for(ecx = 0; ecx < (0x40-ebx)*2; ecx++) *edi++ = 0x80;
		for(ecx = 0; ecx <       ebx *2; ecx++) *edi++ = 0x7f;
	}
}

void AHXWaves::GenerateSawtooth(char* Buffer, int Len)
{
	char* edi = Buffer;
	int ebx = 256/(Len-1), eax = -128;
	for(int ecx = 0; ecx < Len; ecx++) {
		*edi++ = eax;
		eax += ebx;
	}
}

void AHXWaves::GenerateWhiteNoise(char* Buffer, int Len)
{
	// todo: Rewrite in C

/*
	__asm {
		mov edi, Buffer
		mov ecx, Len
		mov eax, 0x41595321 // AYS!
loop0:	test eax, 0x100
		je lower
		cmp ax, 0
		jl mi
		mov byte ptr [edi], 0x7f
		jmp weida
mi:		mov byte ptr [edi], 0x80
		jmp weida
lower:	mov byte ptr [edi], al
weida:	inc edi
		ror eax, 5
		xor al, 10011010b
		mov bx, ax
		rol eax, 2
		add bx, ax
		xor ax, bx
		ror eax, 3
		dec ecx
		jnz loop0
	}
*/

	/* Setup constants */
	const static int q = 15;
	const static float c1 = (1 << q) - 1;
	const static float c2 = ((int)(c1 / 3)) + 1;
	const static float c3 = 1.f / c1;

	/* random number in range 0 - 1 not including 1 */
	float random = 0.f;

	/* the white noise */
	float noise = 0.f;

	for (int i = 0; i < Len; i++)
	{
		random = ((float)rand() / (float)(RAND_MAX + 1));
		noise = (2.f * ((random * c2) + (random * c2) + (random * c2)) - 3.f * (c2 - 1.f)) * c3;
		Buffer[i] = (char)(noise * 255.0f);
	}
}

// AHXOutput ///////////////////////////////////////////////////////////////////////////////////////////////

AHXOutput::AHXOutput()
{
	Player = NULL;
	MixingBuffer = NULL;
	Playing = Paused = 0;
}

AHXOutput::~AHXOutput()
{
//	Free();
}

int AHXOutput::Init(int Frequency, int Bits, int MixLen, float Boost, int Hz)
{
	this->MixLen = MixLen;
	this->Frequency = Frequency;
	this->Bits = Bits;
	this->Hz = Hz;
	this->MixingBuffer = new int [MixLen*Frequency/Hz];
	return SetOption(AHXOF_BOOST, Boost);
}

int AHXOutput::Free()
{
	delete MixingBuffer; MixingBuffer = 0;
	return 1;
}

int AHXOutput::SetOption(int Option, int Value)
{
	switch(Option) {
		case AHXOI_OVERSAMPLING: Oversampling = Value; return 1;
		default: return 0;
	}
}

int AHXOutput::SetOption(int Option, float Value)
{
	switch(Option) {
		case AHXOF_BOOST: {
			// Initialize volume table
			for (int i = 0; i < 65; i++)
				for (int j = -128; j < 128; j++)
					VolumeTable[i][j+128] = (int)(i * j * Value) / 64;
			Boost = Value;
		} return 1;
		default: return 0;
	}
}

int AHXOutput::GetOption(int Option, int* pValue)
{
	switch(Option) {
		case AHXOI_OVERSAMPLING: *pValue = Oversampling; return 1;
		default: return 0;
	}
}

int AHXOutput::GetOption(int Option, float* pValue)
{
	switch(Option) {
		case AHXOF_BOOST: *pValue = Boost; return 1;
		default: return 0;
	}
}

#define min(a,b)  (((a) < (b)) ? (a) : (b))

void AHXOutput::MixChunk(int NrSamples, int** mb)
{
	static int pos[4] = { 0, 0, 0, 0 };

	for(int v = 0; v < 4; v++) {
		if(Player->Voices[v].VoiceVolume == 0) continue;
		float freq = Period2Freq(Player->Voices[v].VoicePeriod);
		int delta = (int)(freq * (1 << 16) / Frequency);
		int samples_to_mix = NrSamples;
		int mixpos = 0;
		while(samples_to_mix) {
			if(pos[v] > (0x280 << 16)) pos[v] -= 0x280 << 16;
			int thiscount = min(samples_to_mix, ((0x280 << 16)-pos[v]-1) / delta + 1);
			samples_to_mix -= thiscount;
			int* VolTab = &VolumeTable[Player->Voices[v].VoiceVolume][128];
			//INNER LOOP
			if(Oversampling) {
				for(int i = 0; i < thiscount; i++) {
					int offset = pos[v] >> 16;
					int sample1 = VolTab[Player->Voices[v].VoiceBuffer[offset]];
					int sample2 = VolTab[Player->Voices[v].VoiceBuffer[offset+1]];
					int frac1 = pos[v] & ((1 << 16) - 1);
					int frac2 = (1 << 16) - frac1;
					(*mb)[mixpos++] += ((sample1 * frac2) + (sample2 * frac1)) >> 16;
					pos[v] += delta;
				}
			} else {
				for(int i = 0; i < thiscount; i++) {
					(*mb)[mixpos++] += VolTab[Player->Voices[v].VoiceBuffer[pos[v] >> 16]];
					pos[v] += delta;
				}
			}
		} // while
	} // v = 0-3
	*mb += NrSamples;
}

void AHXOutput::MixBuffer()
{
	int NrSamples = Frequency / Hz / Player->Song.SpeedMultiplier;
	int* mb = MixingBuffer;
	
	memset(MixingBuffer, 0, MixLen*Frequency/Hz*sizeof(int));
	for(int f = 0; f < MixLen*Player->Song.SpeedMultiplier /* MixLen = # frames */; f++) {
		Player->PlayIRQ();
		MixChunk(NrSamples, &mb);
	} // frames
}
