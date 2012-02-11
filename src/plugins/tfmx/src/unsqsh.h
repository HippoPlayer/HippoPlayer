#ifndef __UNSQSH_H__
#define __UNSQSH_H__

//#include <glib.h>

#include <stdint.h>

uint32_t tfmx_sqsh_get_ulen (uint8_t *src, uint32_t srclen);
void tfmx_sqsh_unpack(uint8_t *src, uint8_t *dst, int32_t dstlen);

#endif /* __UNSQSH_H__ */
