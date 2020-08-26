/* Miscellaneous functions for using & implementing the general
 * interface to TFMXPlay
 */

#ifndef TFMXIFACE_H
#define TFMXIFACE_H

struct TfmxData;

/* Error function definition */
#define TFMXERR(state, x) TFMXError(state, x)
void TFMXError(TfmxState* state, char *err);

/* TFMX file & playing management */
char IsTFMXFilename(char* fName);
int LoadTFMXFile(TfmxState* state, struct TfmxData* mdat, struct TfmxData* samp);
void TFMXRewind(TfmxState* state);
void TFMXStop(TfmxState* state);
void TFMXQuit(TfmxState* state);

/* Per-module info */
void tfmx_fill_module_info(TfmxState* state, char *textBuffer);
void tfmx_fill_text_info(TfmxState* state, char* text_buffer);
int TFMXGetSubSongs(TfmxState* state);

/* TFMX player settings read/write */
int TFMXGetSubSong(TfmxState* state);
void TFMXSetSubSong(TfmxState* state, int num);
void SetFilterState(TfmxState* g_state, char state);
void SetBlendState(TfmxState* g_state, char state);
void SetLoopState(TfmxState* g_state, char state);
char GetFilterState(TfmxState* state);
char GetBlendState(TfmxState* state);
char GetLoopState(TfmxState* state);

/* Sample buffer handling */
long tfmx_get_block_size(TfmxState* state);
int tfmx_get_block(TfmxState* state, void *buffer);
int tfmx_try_to_make_block(TfmxState* state);


void player_TfmxInit(TfmxState* state);
void player_StartSong(int, int);
void TfmxTakedown(TfmxState* state);
void tfmxIrqIn(TfmxState* state);


#endif
