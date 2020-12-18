/*
 * gbsplay is a Gameboy sound player
 *
 * 2004-2006,2008 (C) by Tobias Diedrich <ranma+gbsplay@tdiedrich.de>
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#ifndef _PLUGOUT_H_
#define _PLUGOUT_H_

#include <stdint.h>

#include "config.h"
#include "gbhw.h"

#if PLUGOUT_DSOUND == 1
#  define PLUGOUT_DEFAULT "dsound"
#elif PLUGOUT_PULSE == 1
#  define PLUGOUT_DEFAULT "pulse"
#elif PLUGOUT_ALSA == 1
#  define PLUGOUT_DEFAULT "alsa"
#elif PLUGOUT_SDL == 1
#  define PLUGOUT_DEFAULT "sdl"
#else
#  define PLUGOUT_DEFAULT "oss"
#endif

enum plugout_endian {
	PLUGOUT_ENDIAN_BIG,
	PLUGOUT_ENDIAN_LITTLE,
	PLUGOUT_ENDIAN_NATIVE
};

typedef long    (*plugout_open_fn )(enum plugout_endian endian, long rate, long *buffer_bytes);
typedef int     (*plugout_skip_fn )(int subsong);
typedef void    (*plugout_pause_fn)(int pause);
typedef int     (*plugout_io_fn   )(long cycles, uint32_t addr, uint8_t val);
typedef int     (*plugout_step_fn )(const long cycles, const struct gbhw_channel[]);
typedef ssize_t (*plugout_write_fn)(const void *buf, size_t count);
typedef void    (*plugout_close_fn)(void);

#define PLUGOUT_USES_STDOUT	1

struct output_plugin {
	char	*name;
	char	*description;
	long	flags;
	plugout_open_fn  open;
	plugout_skip_fn  skip;
	plugout_pause_fn pause;
	plugout_io_fn    io;
	plugout_step_fn  step;
	plugout_write_fn write;
	plugout_close_fn close;
};

void plugout_list_plugins(void);
const struct output_plugin* plugout_select_by_name(const char *name);

#endif
