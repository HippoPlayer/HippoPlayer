
// Woohoo!
#define MAX_CHANNELS 16
#define Period2Freq(period) ((3546897.f * 65536.f) / (period))

#if defined(HIPPO_MACOSX)
#include <stdint.h>
#include <stdbool.h>
#else
// temp
#include "../../../hippoplayer/core/Types.h"
#endif

struct hvl_envelope
{
  int16_t aFrames, aVolume;
  int16_t dFrames, dVolume;
  int16_t sFrames;
  int16_t rFrames, rVolume;
  int16_t pad;
};

struct hvl_plsentry
{
  uint8_t ple_Note;
  uint8_t ple_Waveform;
  int16_t ple_Fixed;
  int8_t  ple_FX[2];
  int8_t  ple_FXParam[2];
};

struct hvl_plist
{
  int16_t                pls_Speed;
  int16_t                pls_Length;
  struct hvl_plsentry *pls_Entries;
};

struct hvl_instrument
{
  char                ins_Name[128];
  uint8_t               ins_Volume;
  uint8_t               ins_WaveLength;
  uint8_t               ins_FilterLowerLimit;
  uint8_t               ins_FilterUpperLimit;
  uint8_t               ins_FilterSpeed;
  uint8_t               ins_SquareLowerLimit;
  uint8_t               ins_SquareUpperLimit;
  uint8_t               ins_SquareSpeed;
  uint8_t               ins_VibratoDelay;
  uint8_t               ins_VibratoSpeed;
  uint8_t               ins_VibratoDepth;
  uint8_t               ins_HardCutRelease;
  uint8_t               ins_HardCutReleaseFrames;
  struct hvl_envelope ins_Envelope;
  struct hvl_plist    ins_PList;
};

struct hvl_position
{
  uint8_t pos_Track[MAX_CHANNELS];
  int8_t  pos_Transpose[MAX_CHANNELS];
};

struct hvl_step
{
  uint8_t stp_Note;
  uint8_t stp_Instrument;
  uint8_t stp_FX;
  uint8_t stp_FXParam;
  uint8_t stp_FXb;
  uint8_t stp_FXbParam;
};

struct hvl_voice
{
  int16_t                  vc_Track;
  int16_t                  vc_NextTrack;
  int16_t                  vc_Transpose;
  int16_t                  vc_NextTranspose;
  int16_t                  vc_OverrideTranspose; // 1.5
  int32_t                  vc_ADSRVolume;
  struct hvl_envelope    vc_ADSR;
  struct hvl_instrument *vc_Instrument;
  uint32_t                 vc_SamplePos;
  uint32_t                 vc_Delta;
  uint16_t                 vc_InstrPeriod;
  uint16_t                 vc_TrackPeriod;
  uint16_t                 vc_VibratoPeriod;
  uint16_t                 vc_WaveLength;
  int16_t                  vc_NoteMaxVolume;
  uint16_t                 vc_PerfSubVolume;
  uint8_t                  vc_NewWaveform;
  uint8_t                  vc_Waveform;
  uint8_t                  vc_PlantPeriod;
  uint8_t                  vc_VoiceVolume;
  uint8_t                  vc_PlantSquare;
  uint8_t                  vc_IgnoreSquare;
  uint8_t                  vc_FixedNote;
  int16_t                  vc_VolumeSlideUp;
  int16_t                  vc_VolumeSlideDown;
  int16_t                  vc_HardCut;
  uint8_t                  vc_HardCutRelease;
  int16_t                  vc_HardCutReleaseF;
  uint8_t                  vc_PeriodSlideOn;
  int16_t                  vc_PeriodSlideSpeed;
  int16_t                  vc_PeriodSlidePeriod;
  int16_t                  vc_PeriodSlideLimit;
  int16_t                  vc_PeriodSlideWithLimit;
  int16_t                  vc_PeriodPerfSlideSpeed;
  int16_t                  vc_PeriodPerfSlidePeriod;
  uint8_t                  vc_PeriodPerfSlideOn;
  int16_t                  vc_VibratoDelay;
  int16_t                  vc_VibratoSpeed;
  int16_t                  vc_VibratoCurrent;
  int16_t                  vc_VibratoDepth;
  int16_t                  vc_SquareOn;
  int16_t                  vc_SquareInit;
  int16_t                  vc_SquareWait;
  int16_t                  vc_SquareLowerLimit;
  int16_t                  vc_SquareUpperLimit;
  int16_t                  vc_SquarePos;
  int16_t                  vc_SquareSign;
  int16_t                  vc_SquareSlidingIn;
  int16_t                  vc_SquareReverse;
  uint8_t                  vc_FilterOn;
  uint8_t                  vc_FilterInit;
  int16_t                  vc_FilterWait;
  int16_t                  vc_FilterSpeed;
  int16_t                  vc_FilterUpperLimit;
  int16_t                  vc_FilterLowerLimit;
  int16_t                  vc_FilterPos;
  int16_t                  vc_FilterSign;
  int16_t                  vc_FilterSlidingIn;
  int16_t                  vc_IgnoreFilter;
  int16_t                  vc_PerfCurrent;
  int16_t                  vc_PerfSpeed;
  int16_t                  vc_PerfWait;
  struct hvl_plist      *vc_PerfList;
  int8_t                  *vc_AudioPointer;
  int8_t                  *vc_AudioSource;
  uint8_t                  vc_NoteDelayOn;
  uint8_t                  vc_NoteCutOn;
  int16_t                  vc_NoteDelayWait;
  int16_t                  vc_NoteCutWait;
  int16_t                  vc_AudioPeriod;
  int16_t                  vc_AudioVolume;
  int32_t                  vc_WNRandom;
  int8_t                  *vc_MixSource;
  int8_t                   vc_SquareTempBuffer[0x80];
  int8_t                   vc_VoiceBuffer[0x282*4];
  uint8_t                  vc_VoiceNum;
  uint8_t                  vc_TrackMasterVolume;
  uint8_t                  vc_TrackOn;
  int16_t                  vc_VoicePeriod;
  uint32_t                 vc_Pan;
  uint32_t                 vc_SetPan;   // New for 1.4
  uint32_t                 vc_PanMultLeft;
  uint32_t                 vc_PanMultRight;
  uint32_t                 vc_RingSamplePos;
  uint32_t                 vc_RingDelta;
  int8_t                  *vc_RingMixSource;
  uint8_t                  vc_RingPlantPeriod;
  int16_t                  vc_RingInstrPeriod;
  int16_t                  vc_RingBasePeriod;
  int16_t                  vc_RingAudioPeriod;
  int8_t                  *vc_RingAudioSource;
  uint8_t                  vc_RingNewWaveform;
  uint8_t                  vc_RingWaveform;
  uint8_t                  vc_RingFixedPeriod;
  int8_t                   vc_RingVoiceBuffer[0x282*4];
};

struct hvl_tune
{
  char                   ht_Name[128];
  uint16_t                 ht_SongNum;
  uint32_t                 ht_Frequency;
  double                ht_FreqF;
  int8_t                  *ht_WaveformTab[MAX_CHANNELS];
  uint16_t                 ht_Restart;
  uint16_t                 ht_PositionNr;
  uint8_t                  ht_SpeedMultiplier;
  uint8_t                  ht_TrackLength;
  uint8_t                  ht_TrackNr;
  uint8_t                  ht_InstrumentNr;
  uint8_t                  ht_SubsongNr;
  uint16_t                 ht_PosJump;
  uint32_t                 ht_PlayingTime;
  int16_t                  ht_Tempo;
  int16_t                  ht_PosNr;
  int16_t                  ht_StepWaitFrames;
  int16_t                  ht_NoteNr;
  uint16_t                 ht_PosJumpNote;
  uint8_t                  ht_GetNewPosition;
  uint8_t                  ht_PatternBreak;
  uint8_t                  ht_SongEndReached;
  uint8_t                  ht_Stereo;
  uint16_t                *ht_Subsongs;
  uint16_t                 ht_Channels;
  struct hvl_position   *ht_Positions;
  struct hvl_step        ht_Tracks[256][64];
  struct hvl_instrument *ht_Instruments;
  struct hvl_voice       ht_Voices[MAX_CHANNELS];
  int32_t                  ht_defstereo;
  int32_t                  ht_defpanleft;
  int32_t                  ht_defpanright;
  int32_t                  ht_mixgain;
  uint8_t                  ht_Version;
};

int hvl_DecodeFrame( struct hvl_tune *ht, int8_t *buf1, int8_t *buf2, int32_t bufmod );
void hvl_InitReplayer( void );
int hvl_InitSubsong( struct hvl_tune *ht, uint32_t nr );
struct hvl_tune *hvl_LoadTune( char *name, uint32_t freq, uint32_t defstereo );
struct hvl_tune *hvl_load_ahx( uint8_t *buf, uint32_t buflen, uint32_t defstereo, uint32_t freq );
void hvl_FreeTune( struct hvl_tune *ht );

