/*
 * gbsplay is a Gameboy sound player
 *
 * 2003-2005 (C) by Tobias Diedrich <ranma+gbsplay@tdiedrich.de>
 *                  Christian Garbs <mitch@cgarbs.de>
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#ifndef _CFGPARSER_H_
#define _CFGPARSER_H_

typedef void (*cfg_parse_fn)(void *ptr);

struct cfg_option {
	char *name;
	void *ptr;
	cfg_parse_fn parse_fn;
};

void  cfg_string(void *ptr);
void  cfg_long(void *ptr);
void  cfg_endian(void *ptr);
void  cfg_parse(const char *fname, const struct cfg_option *options);
char* get_userconfig(const char* cfgfile);

#endif
