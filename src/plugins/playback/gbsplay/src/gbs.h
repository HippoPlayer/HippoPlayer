/*
 * gbsplay is a Gameboy sound player
 *
 * 2003-2005,2013 (C) by Tobias Diedrich <ranma+gbsplay@tdiedrich.de>
 *                       Christian Garbs <mitch@cgarbs.de>
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#ifndef _GBS_H_
#define _GBS_H_

#include <inttypes.h>
#include "common.h"

#define GBS_LEN_SHIFT	10
#define GBS_LEN_DIV	(1 << GBS_LEN_SHIFT)

struct gbs;

typedef long (*gbs_nextsubsong_cb)(struct gbs *gbs, void *priv);

struct gbs_subsong_info {
	uint32_t len;
	char *title;
};

struct gbs {
	char *buf;
	uint8_t version;
	uint8_t songs;
	uint8_t defaultsong;
	uint8_t defaultbank;
	uint16_t load;
	uint16_t init;
	uint16_t play;
	uint16_t stack;
	uint8_t tma;
	uint8_t tac;
	char *title;
	char *author;
	char *copyright;
	unsigned long codelen;
	char *code;
	char *exthdr;
	size_t filesize;
	uint32_t crc;
	uint32_t crcnow;
	struct gbs_subsong_info *subsong_info;
	char *strings;
	char v1strings[33*3];
	uint8_t *rom;
	unsigned long romsize;

	long long ticks;
	int16_t lmin, lmax, lvol, rmin, rmax, rvol;
	long subsong_timeout, silence_timeout, fadeout, gap;
	long long silence_start;
	int subsong;
	gbs_nextsubsong_cb nextsubsong_cb;
	void *nextsubsong_cb_priv;
};

struct gbs *gbs_open(const char *name);
struct gbs *gbs_open_mem(const char *name, char *buf, size_t size);
long gbs_init(struct gbs *gbs, long subsong);
long gbs_step(struct gbs *gbs, long time_to_work);
void gbs_set_nextsubsong_cb(struct gbs *gbs, gbs_nextsubsong_cb cb, void *priv);
void gbs_printinfo(struct gbs *gbs, long verbose);
void gbs_close(struct gbs *gbs);
long gbs_write(struct gbs *gbs, char *name, long version);

#endif
