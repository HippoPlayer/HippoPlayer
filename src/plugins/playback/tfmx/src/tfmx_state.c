#include "tfmx.h"
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void TfmxState_init(TfmxState* state) {
	memset(state, 0, sizeof(TfmxState));
	state->plugin_cfg.filt = 2;
	state->plugin_cfg.blend = 1;
	state->plugin_cfg.over = 1;
	memset(state->active_voice, 1, 8);
	state->output_chans = 2;
#ifndef NO_SCHED
    state->sched = 1;
#endif
    state->bytes_per_sample = 1; /* range : 1 to 4 (16 bits stereo) */
    state->eClocks = 14318;
    state->outRate = 48000;
    state->loops = 1;
}
