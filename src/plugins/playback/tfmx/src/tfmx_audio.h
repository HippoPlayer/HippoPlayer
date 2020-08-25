#pragma once

struct TfmxState;

#ifdef __cplusplus__
extern "C" {
#endif

    void tfmx_calc_sizes(struct TfmxState* state);
    long tfmx_get_block_size(struct TfmxState* state);
    int  tfmx_get_block(struct TfmxState* state, void *buffer);
    int  tfmx_try_to_make_block(struct TfmxState* state);
    void TfmxTakedown(struct TfmxState* state);
    void TfmxResetBuffers(struct TfmxState* state);

#ifdef __cplusplus__
}
#endif

