#ifndef __TFMX_AUDIO_H__
#define __TFMX_AUDIO_H__

#ifdef __cplusplus__
extern "C" {
#endif

    void tfmx_calc_sizes(void);
    long tfmx_get_block_size(void);
    int  tfmx_get_block(void *buffer);
    int  tfmx_try_to_make_block(void);
    void TfmxTakedown(void);
    void TfmxResetBuffers(void);

#ifdef __cplusplus__
}
#endif

#endif /* __TFMX_AUDIO_H__ */

