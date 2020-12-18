/*
 * gbsplay is a Gameboy sound player
 *
 * 2003-2005,2018 (C) by Tobias Diedrich <ranma+gbsplay@tdiedrich.de>
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#ifndef _GBHW_H_
#define _GBHW_H_

#include <inttypes.h>
#include "common.h"

#define GBHW_CLOCK 4194304

#define GBHW_CFG_FILTER_OFF "off"
#define GBHW_CFG_FILTER_DMG "dmg"
#define GBHW_CFG_FILTER_CGB "cgb"

#define GBHW_FILTER_CONST_OFF 1.0
/* From blargg's "Game Boy Sound Operation" doc */
#define GBHW_FILTER_CONST_DMG 0.999958
#define GBHW_FILTER_CONST_CGB 0.998943

struct gbhw_buffer {
	int16_t *data;
	long pos;
	long l_lvl;
	long r_lvl;
	long l_cap;
	long r_cap;
	long bytes;
	long samples;
	long cycles;
};

struct gbhw_channel {
	long mute;
	long running;
	long master;
	long leftgate;
	long rightgate;
	long lvl;
	long volume;
	long env_dir;
	long env_tc;
	long env_ctr;
	long sweep_dir;
	long sweep_tc;
	long sweep_ctr;
	long sweep_shift;
	long len;
	long len_enable;
	long len_gate;
	long div_tc;
	long div_tc_shadow;
	long div_ctr;
	long duty_tc;
	long duty_ctr;
};

extern struct gbhw_channel gbhw_ch[4];

typedef void (*gbhw_callback_fn)(struct gbhw_buffer *buf, void *priv);
typedef void (*gbhw_iocallback_fn)(long cycles, uint32_t addr, uint8_t valu, void *priv);
typedef void (*gbhw_stepcallback_fn)(const long cycles, const struct gbhw_channel[], void *priv);

void gbhw_setcallback(gbhw_callback_fn fn, void *priv);
void gbhw_setiocallback(gbhw_iocallback_fn fn, void *priv);
void gbhw_setstepcallback(gbhw_stepcallback_fn fn, void *priv);
long gbhw_setfilter(const char *type);
void gbhw_setrate(long rate);
void gbhw_setbuffer(struct gbhw_buffer *buffer);
void gbhw_init(uint8_t *rombuf, uint32_t size);
void gbhw_enable_bootrom(const uint8_t *rombuf);
void gbhw_pause(long new_pause);
void gbhw_master_fade(long speed, long dstvol);
void gbhw_getminmax(int16_t *lmin, int16_t *lmax, int16_t *rmin, int16_t *rmax);
long gbhw_step(long time_to_work);
uint8_t gbhw_io_peek(uint16_t addr);  /* unmasked peek */
void gbhw_io_put(uint16_t addr, uint8_t val);

#endif
