/*
 * gbsplay is a Gameboy sound player
 *
 * 2003-2006,2013 (C) by Tobias Diedrich <ranma+gbsplay@tdiedrich.de>
 *                       Christian Garbs <mitch@cgarbs.de>
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "gbhw.h"
#include "gbcpu.h"
#include "gbs.h"
#include "crc32.h"

#ifdef USE_ZLIB
#include <zlib.h>
#endif

/* Max GB rom size is 4MiB (mapper with 256 banks) */
#define GB_MAX_ROM_SIZE (256 * 0x4000)

#define HDR_LEN_GBS	0x70
#define HDR_LEN_GBR	0x20
#define HDR_LEN_GB	0x150
#define HDR_LEN_GZIP	10
#define HDR_LEN_VGM	0x100

#define GBS_MAGIC		"GBS"
#define GBS_EXTHDR_MAGIC	"GBSX"
#define GBR_MAGIC		"GBRF"
#define GD3_MAGIC		"Gd3 "
#define VGM_MAGIC		"Vgm "
#define GZIP_MAGIC		"\037\213\010"

const char *boot_rom_file = ".dmg_rom.bin";

long gbs_init(struct gbs *gbs, long subsong)
{
	gbhw_init(gbs->rom, gbs->romsize);

	if (subsong == -1) subsong = gbs->defaultsong - 1;
	if (subsong >= gbs->songs) {
		fprintf(stderr, _("Subsong number out of range (min=0, max=%d).\n"), (int)gbs->songs - 1);
		return 0;
	}

	if (gbs->defaultbank != 1) {
		gbcpu_mem_put(0x2000, gbs->defaultbank);
	}
	gbhw_io_put(0xff06, gbs->tma);
	gbhw_io_put(0xff07, gbs->tac);
	gbhw_io_put(0xffff, 0x05);

	REGS16_W(gbcpu_regs, SP, gbs->stack);

	/* put halt breakpoint PC on stack */
	gbcpu_halt_at_pc = 0xffff;
	REGS16_W(gbcpu_regs, PC, 0xff80);
	REGS16_W(gbcpu_regs, HL, gbcpu_halt_at_pc);
	gbcpu_mem_put(0xff80, 0xe5); /* push hl */
	gbcpu_step();
	/* clear regs/memory touched by stack etup */
	REGS16_W(gbcpu_regs, HL, 0x0000);
	gbcpu_mem_put(0xff80, 0x00);

	REGS16_W(gbcpu_regs, PC, gbs->init);
	gbcpu_regs.rn.a = subsong;

	gbs->ticks = 0;
	gbs->subsong = subsong;

	return 1;
}

void gbs_set_nextsubsong_cb(struct gbs *gbs, gbs_nextsubsong_cb cb, void *priv)
{
	gbs->nextsubsong_cb = cb;
	gbs->nextsubsong_cb_priv = priv;
}

static long gbs_nextsubsong(struct gbs *gbs)
{
	if (gbs->nextsubsong_cb != NULL) {
		return gbs->nextsubsong_cb(gbs, gbs->nextsubsong_cb_priv);
	} else {
		gbs->subsong++;
		if (gbs->subsong >= gbs->songs)
			return false;
		gbs_init(gbs, gbs->subsong);
	}
	return true;
}

long gbs_step(struct gbs *gbs, long time_to_work)
{
	long cycles = gbhw_step(time_to_work);
	long time;

	if (cycles < 0) {
		return false;
	}

	gbs->ticks += cycles;

	gbhw_getminmax(&gbs->lmin, &gbs->lmax, &gbs->rmin, &gbs->rmax);
	gbs->lvol = -gbs->lmin > gbs->lmax ? -gbs->lmin : gbs->lmax;
	gbs->rvol = -gbs->rmin > gbs->rmax ? -gbs->rmin : gbs->rmax;

	time = gbs->ticks / GBHW_CLOCK;
	if (gbs->silence_timeout) {
		if (gbs->lmin == gbs->lmax && gbs->rmin == gbs->rmax) {
			if (gbs->silence_start == 0)
				gbs->silence_start = gbs->ticks;
		} else gbs->silence_start = 0;
	}

	if (gbs->fadeout && gbs->subsong_timeout &&
	    time >= gbs->subsong_timeout - gbs->fadeout - gbs->gap)
		gbhw_master_fade(128/gbs->fadeout, 0);
	if (gbs->subsong_timeout &&
	    time >= gbs->subsong_timeout - gbs->gap)
		gbhw_master_fade(128*16, 0);

	if (gbs->silence_start &&
	    (gbs->ticks - gbs->silence_start) / GBHW_CLOCK >= gbs->silence_timeout) {
		if (gbs->subsong_info[gbs->subsong].len == 0) {
			gbs->subsong_info[gbs->subsong].len = gbs->ticks * GBS_LEN_DIV / GBHW_CLOCK;
		}
		return gbs_nextsubsong(gbs);
	}
	if (gbs->subsong_timeout && time >= gbs->subsong_timeout)
		return gbs_nextsubsong(gbs);

	return true;
}

void gbs_printinfo(struct gbs *gbs, long verbose)
{
	printf(_("GBSVersion:       %u\n"
	         "Title:            \"%s\"\n"
	         "Author:           \"%s\"\n"
	         "Copyright:        \"%s\"\n"
	         "Load address:     0x%04x\n"
	         "Init address:     0x%04x\n"
	         "Play address:     0x%04x\n"
	         "Stack pointer:    0x%04x\n"
	         "File size:        0x%08x\n"
	         "ROM size:         0x%08lx (%ld banks)\n"
	         "Subsongs:         %u\n"
	         "Default subsong:  %u\n"),
	       gbs->version,
	       gbs->title,
	       gbs->author,
	       gbs->copyright,
	       gbs->load,
	       gbs->init,
	       gbs->play,
	       gbs->stack,
	       (unsigned int)gbs->filesize,
	       gbs->romsize,
	       gbs->romsize/0x4000,
	       gbs->songs,
	       gbs->defaultsong);
	if (gbs->tac & 0x04) {
		long timertc = (256-gbs->tma) * (16 << (((gbs->tac+3) & 3) << 1));
		if (gbs->tac & 0x80)
			timertc /= 2;
		printf(_("Timing:           %2.2fHz timer%s\n"),
		       GBHW_CLOCK / (float)timertc,
		       (gbs->tac & 0x78) == 0x40 ? _(" + VBlank (ugetab)") : "");
	} else {
		printf(_("Timing:           %s\n"),
		       _("59.7Hz VBlank\n"));
	}
	if (gbs->defaultbank != 1) {
		printf(_("Bank @0x4000:     %d\n"), gbs->defaultbank);
	}
	if (gbs->version == 2) {
		printf(_("CRC32:            0x%08lx/0x%08lx (%s)\n"),
		       (unsigned long)gbs->crc, (unsigned long)gbs->crcnow,
		       gbs->crc == gbs->crcnow ? _("OK") : _("Failed"));
	} else {
		printf(_("CRC32:            0x%08lx\n"),
		       (unsigned long)gbs->crcnow);
	}
	if (verbose && gbs->version == 2) {
		long i;
		for (i=0; i<gbs->songs; i++) {
			printf(_("Subsong %03ld:	"), i);
			if (gbs->subsong_info[i].title) {
				printf("\"%s\" ", gbs->subsong_info[i].title);
			} else {
				printf("%s ", _("untitled"));
			}
			if (gbs->subsong_info[i].len) {
				printf(_("(%ld seconds)\n"),
				       (long)(gbs->subsong_info[i].len >> GBS_LEN_SHIFT));
			} else {
				printf("%s\n", _("no time limit"));
			}
		}
	}
}

static void gbs_free(struct gbs *gbs)
{
	if (gbs->buf)
		free(gbs->buf);
	if (gbs->subsong_info)
		free(gbs->subsong_info);
	free(gbs);
}

void gbs_close(struct gbs *gbs)
{
	gbs_free(gbs);
}

static void writeint(char *buf, uint32_t val, long bytes)
{
	long shift = 0;
	long i;

	for (i=0; i<bytes; i++) {
		buf[i] = (val >> shift) & 0xff;
		shift += 8;
	}
}

static uint32_t readint(char *buf, long bytes)
{
	long i;
	long shift = 0;
	uint32_t res = 0;

	for (i=0; i<bytes; i++) {
		res |= (unsigned char)buf[i] << shift;
		shift += 8;
	}

	return res;
}

long gbs_write(struct gbs *gbs, char *name, long version)
{
	long fd;
	long codelen = (gbs->codelen + 15) >> 4;
	char pad[16];
	char strings[65536];
	long stringofs = 0;
	long newlen = gbs->filesize;
	long namelen = strlen(name);
	char *tmpname = malloc(namelen + sizeof(".tmp\0"));

	memcpy(tmpname, name, namelen);
	sprintf(&tmpname[namelen], ".tmp");
	memset(pad, 0xff, sizeof(pad));

	if ((fd = open(tmpname, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
		fprintf(stderr, _("Could not open %s: %s\n"), name, strerror(errno));
		return 0;
	}

	if (version == 2) {
		long len,i;
		long ehdrlen = 32 + 8*gbs->songs;
		uint32_t hdrcrc;

		newlen = 0x70 + codelen*16 + ehdrlen;
		gbs->buf[3] = 1;
		gbs->buf = realloc(gbs->buf, newlen + 65536);
		gbs->code = gbs->buf + 0x70;
		gbs->exthdr = gbs->code + codelen*16;
		writeint(gbs->buf + 0x6e, codelen, 2);
		memset(&gbs->code[gbs->codelen], 0x00, codelen*16 - gbs->codelen);
		memset(gbs->exthdr, 0x00, ehdrlen + 65536);
		memcpy(gbs->exthdr, GBS_EXTHDR_MAGIC, strlen(GBS_EXTHDR_MAGIC));
		gbs->exthdr[0x1c] = gbs->songs;
		if ((len = strlen(gbs->title)) > 32) {
			memcpy(strings+stringofs, gbs->title, len+1);
			writeint(&gbs->exthdr[0x14], stringofs, 2);
			stringofs += len+1;
		} else writeint(&gbs->exthdr[0x14], 0xffff, 2);
		if ((len = strlen(gbs->author)) > 32) {
			memcpy(strings+stringofs, gbs->author, len+1);
			writeint(&gbs->exthdr[0x16], stringofs, 2);
			stringofs += len+1;
		} else writeint(&gbs->exthdr[0x16], 0xffff, 2);
		if ((len = strlen(gbs->copyright)) > 30) {
			memcpy(strings+stringofs, gbs->copyright, len+1);
			writeint(&gbs->exthdr[0x18], stringofs, 2);
			stringofs += len+1;
		} else writeint(&gbs->exthdr[0x18], 0xffff, 2);

		for (i=0; i<gbs->songs; i++) {
			writeint(&gbs->exthdr[0x20+8*i],
			         gbs->subsong_info[i].len, 4);
			if (gbs->subsong_info[i].title &&
			    strcmp(gbs->subsong_info[i].title, "") != 0) {
				len = strlen(gbs->subsong_info[i].title)+1;
				memcpy(strings+stringofs, gbs->subsong_info[i].title, len);
				writeint(&gbs->exthdr[0x20+8*i+4],
				         stringofs, 2);
				stringofs += len;
			} else writeint(&gbs->exthdr[0x20+8*i+4], 0xffff, 2);
		}
		memcpy(gbs->buf + newlen, strings, stringofs);
		newlen += stringofs;

		writeint(&gbs->exthdr[0x04], ehdrlen+stringofs-8, 4);
		writeint(&gbs->exthdr[0x0c], gbs->filesize, 4);
		gbs->crc = gbs_crc32(0, gbs->buf, gbs->filesize);
		writeint(&gbs->exthdr[0x10], gbs->crc, 4);
		hdrcrc = gbs_crc32(0, gbs->exthdr, ehdrlen+stringofs);
		writeint(&gbs->exthdr[0x08], hdrcrc, 4);

	} else {
		if (gbs->version == 2) {
			gbs->buf[3] = 1;
		}
	}
	if (write(fd, gbs->buf, newlen) == newlen) {
		int ret = 1;
		close(fd);
		if (rename(tmpname, name) == -1) {
			fprintf(stderr, _("Could not rename %s to %s: %s\n"), tmpname, name, strerror(errno));
			ret = 0;
		}
		return ret;
	}
	close(fd);

	return 1;
}

static struct gbs *gb_open(const char *name, char *buf, size_t size)
{
	int fd;
	long i, name_len;
	struct gbs *gbs = malloc(sizeof(struct gbs));
	char *bootname;
	uint8_t bootrom[256];
	char *na_str = _("gb / not available");

	memset(gbs, 0, sizeof(struct gbs));
	gbs->silence_timeout = 2*60;
	gbs->subsong_timeout = 2*60;
	gbs->gap = 2;
	gbs->fadeout = 3;
	gbs->version = 0;
	gbs->songs = 1;
	gbs->defaultsong = 1;
	gbs->defaultbank = 1;
	gbs->load  = 0;
	gbs->init  = 0x100;
	gbs->play = gbs->init;
	gbs->stack = 0xfffe;
	gbs->buf = buf;

	/* For accuracy testing purposes, support boot rom. */
	name_len = strlen(getenv("HOME")) + strlen(boot_rom_file) + 2;
	bootname = malloc(name_len);
	snprintf(bootname, name_len, "%s/%s", getenv("HOME"), boot_rom_file);
	if ((fd = open(bootname, O_RDONLY)) != -1) {
		if (read(fd, bootrom, sizeof(bootrom)) == sizeof(bootrom)) {
			gbhw_enable_bootrom(bootrom);
			gbs->init = 0;
		}
	}
	free(bootname);

	/* Test if this looks like a valid rom header title */
	for (i=0x0134; i<0x0143; i++) {
		if (!(isalnum(buf[i]) || isspace(buf[i])))
			break;
	}
	if (buf[i] == 0) {
		/* Title looks valid and is zero-terminated. */
		gbs->title = &buf[0x0134];
	} else {
		gbs->title = na_str;
	}
	gbs->author = na_str;
	gbs->copyright = na_str;
	gbs->code = buf;
	gbs->filesize = size;

	gbs->subsong_info = malloc(gbs->songs * sizeof(struct gbs_subsong_info));
	memset(gbs->subsong_info, 0, gbs->songs * sizeof(struct gbs_subsong_info));
	gbs->codelen = size - 0x20;
	gbs->crcnow = gbs_crc32(0, buf, gbs->filesize);
	gbs->romsize = (gbs->codelen + 0x3fff) & ~0x3fff;

	gbs->rom = calloc(1, gbs->romsize);
	memcpy(gbs->rom, buf, gbs->codelen);

	close(fd);

	return gbs;
}

static struct gbs *gbr_open(const char *name, char *buf, size_t size)
{
	long i;
	struct gbs *gbs = malloc(sizeof(struct gbs));
	char *na_str = _("gbr / not available");
	uint16_t vsync_addr;
	uint16_t timer_addr;

	memset(gbs, 0, sizeof(struct gbs));
	gbs->silence_timeout = 2;
	gbs->subsong_timeout = 2*60;
	gbs->gap = 2;
	gbs->fadeout = 3;
	if (strncmp(buf, GBR_MAGIC, 4) != 0) {
		fprintf(stderr, _("Not a GBR-File: %s\n"), name);
		gbs_free(gbs);
		return NULL;
	}
	if (buf[0x05] != 0) {
		fprintf(stderr, _("Unsupported default bank @0x0000: %d\n"), buf[0x05]);
		gbs_free(gbs);
		return NULL;
	}
	if (buf[0x07] < 1 || buf[0x07] > 3) {
		fprintf(stderr, _("Unsupported timerflag value: %d\n"), buf[0x07]);
		gbs_free(gbs);
		return NULL;
	}
	gbs->version = 0;
	gbs->songs = 255;
	gbs->defaultsong = 1;
	gbs->defaultbank = buf[0x06];
	gbs->load  = 0;
	gbs->init  = readint(&buf[0x08], 2);
	vsync_addr = readint(&buf[0x0a], 2);
	timer_addr = readint(&buf[0x0c], 2);

	if (buf[0x07] == 1) {
		gbs->play = vsync_addr;
	} else {
		gbs->play = timer_addr;
	}
	gbs->tma = buf[0x0e];
	gbs->tac = buf[0x0f];
	gbs->stack = 0xfffe;

	/* Test if this looks like a valid rom header title */
	for (i=0x0154; i<0x0163; i++) {
		if (!(isalnum(buf[i]) || isspace(buf[i])))
			break;
	}
	if (buf[i] == 0) {
		/* Title looks valid and is zero-terminated. */
		gbs->title = &buf[0x0154];
	} else {
		gbs->title = na_str;
	}
	gbs->author = na_str;
	gbs->copyright = na_str;
	gbs->code = &buf[0x20];
	gbs->filesize = size;

	gbs->subsong_info = malloc(gbs->songs * sizeof(struct gbs_subsong_info));
	memset(gbs->subsong_info, 0, gbs->songs * sizeof(struct gbs_subsong_info));
	gbs->codelen = size - 0x20;
	gbs->crcnow = gbs_crc32(0, buf, gbs->filesize);
	gbs->romsize = (gbs->codelen + 0x3fff) & ~0x3fff;

	gbs->rom = calloc(1, gbs->romsize);
	memcpy(gbs->rom, &buf[0x20], gbs->codelen);

	gbs->rom[0x40] = 0xd9; /* reti */
	gbs->rom[0x50] = 0xd9; /* reti */
	if (buf[0x07] & 1) {
		/* V-Blank */
		gbs->rom[0x40] = 0xc3; /* jp imm16 */
		gbs->rom[0x41] = vsync_addr & 0xff;
		gbs->rom[0x42] = vsync_addr >> 8;
	}
	if (buf[0x07] & 2) {
		/* Timer */
		gbs->rom[0x50] = 0xc3; /* jp imm16 */
		gbs->rom[0x51] = timer_addr & 0xff;
		gbs->rom[0x52] = timer_addr >> 8;
	}

	return gbs;
}

static uint32_t le32(const char *buf)
{
	const uint8_t *b = (const uint8_t*)buf;
	return b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
}

static uint16_t le16(const char *buf)
{
	const uint8_t *b = (const uint8_t*)buf;
	return b[0] | (b[1] << 8);
}

static void emit(struct gbs *gbs, long *code_used, uint8_t data, long reserve)
{
	long remain = gbs->codelen - *code_used;
	uint8_t *code = (uint8_t*) &gbs->code[*code_used];
	if (reserve + 1 > remain) {
		while (remain-- > 0) {
			*(code++) = 0xc9;  /* RET */
			(*code_used)++;
		}
		gbs->code = realloc(gbs->code, gbs->codelen + 0x4000);
		gbs->codelen += 0x4000;
		code = (uint8_t*) &gbs->code[*code_used];
	}
	*(code++) = data;
	(*code_used)++;
}

static void gd3_parse(struct gbs **gbs, const char *gd3, long gd3_len)
{
	char *buf;
	char *s;
	long ofs = 12;
	long idx = 0;
	if (gd3_len < 12 || gd3_len > 4096) {
		return;
	}
	if (strncmp(gd3, GD3_MAGIC, 4) != 0) {
		return;
	}
	if (le32(&gd3[4]) != 0x00000100) {
		return;
	}
	if (le32(&gd3[8]) != gd3_len - ofs) {
		return;
	}
	*gbs = realloc(*gbs, sizeof(struct gbs) + gd3_len);
	s = buf = (char *)&(*gbs)[1];
	while (ofs < gd3_len) {
		uint16_t val = le16(&gd3[ofs]);
		if (val == 0) {
			*(buf++) = 0;
			switch (idx) {
			case 0: (*gbs)->subsong_info[0].title = s; break;
			case 2: (*gbs)->title = s; break;
			case 6: (*gbs)->author = s; break;
			default: break;
			}
			s = buf;
			idx++;
		} else if (val < 256) {
			*(buf++) = val;
		} else {
			*(buf++) = '?';
		}
		ofs += 2;
	}
}

static struct gbs *vgm_open(const char *name, char *buf, size_t size)
{
	struct gbs *gbs = malloc(sizeof(struct gbs));
	char *na_str = _("vgm / not available");
	char *gd3 = NULL;
	char *data;
	long dmg_clock;
	long eof_ofs;
	long gd3_ofs;
	long gd3_len;
	long data_ofs;
	long data_len;
	long vgm_parsed = false;
	long total_wait;
	long total_clocks;
	long code_used;
	long addr;
	long jpaddr;

	memset(gbs, 0, sizeof(struct gbs));
	gbs->silence_timeout = 2;
	gbs->subsong_timeout = 2*60;
	gbs->gap = 2;
	gbs->fadeout = 3;
	gbs->buf = buf;
	if (strncmp(buf, VGM_MAGIC, 4) != 0) {
		fprintf(stderr, _("Not a VGM-File: %s\n"), name);
		gbs_free(gbs);
		return NULL;
	}
	if (buf[0x09] != 1 || buf[0x08] < 0x61) {
		fprintf(stderr, _("Unsupported VGM version: %d.%02x\n"), buf[0x09], buf[0x08]);
		gbs_free(gbs);
		return NULL;
	}
	dmg_clock = le32(&buf[0x80]);
	if (dmg_clock != 4194304) {
		fprintf(stderr, _("Unsupported DMG clock: %ldHz\n"), dmg_clock);
		gbs_free(gbs);
		return NULL;
	}
	eof_ofs = le32(&buf[0x4]) + 0x4;
	if (eof_ofs > size) {
		fprintf(stderr, _("Bad file size in header: %ld\n"), eof_ofs);
		gbs_free(gbs);
		return NULL;
	}
	gd3_ofs = le32(&buf[0x14]) + 0x14;
	if (gd3_ofs == 0x14) {
		gd3_ofs = eof_ofs;
		gd3_len = 0;
	} else {
		gd3_len = eof_ofs - gd3_ofs;
		gd3 = &buf[gd3_ofs];
		if (gd3_len < 4 || strncmp(gd3, GD3_MAGIC, 4) != 0) {
			fprintf(stderr, _("Bad GD3 offset: %08lx\n"), gd3_ofs);
			gbs_free(gbs);
			return NULL;
		}
	}
	data_ofs = le32(&buf[0x34]) + 0x34;
	data_len = gd3_ofs - data_ofs;
	data = &buf[data_ofs];
	if (data_len < 0) {
		fprintf(stderr, _("Bad data length: %ld\n"), data_len);
		gbs_free(gbs);
		return NULL;
	}

	gbs->codelen = 0x4000;
	gbs->code = calloc(1, gbs->codelen);
	code_used = 0;

	total_wait = total_clocks = 0;
	while (!vgm_parsed) {
		switch ((uint8_t)*data) {
		default:
			fprintf(stderr, _("Unsupported VGM opcode: 0x%02x\n"), *data);
			gbs_free(gbs);
			return NULL;
		case 0x61:  /* Wait n samples */
			total_wait += le16(&data[1]);
			data += 2;
			break;
		case 0x62:  /* Wait 735 (1/60s) */
			total_wait += 735;
			break;
		case 0x63:  /* Wait 882 (1/50s) */
			total_wait += 882;
			break;
		case 0x66:  /* End of sound data */
			vgm_parsed = true;
			break;
		case 0x70:
		case 0x71:
		case 0x72:
		case 0x73:
		case 0x74:
		case 0x75:
		case 0x76:
		case 0x77:
		case 0x78:
		case 0x79:
		case 0x7a:
		case 0x7b:
		case 0x7c:
		case 0x7d:
		case 0x7e:
		case 0x7f:
			/* Wait n+1 samples */
			total_wait += (*data & 0xf) + 1;
			break;
		case 0xb3:  /* DMG write */
			{
				long delay = (total_wait * 41943L / 441L) - total_clocks;
				long units = (delay + 31) / 64;
				uint8_t reg = 0x10 + ((uint8_t)data[1] & 0x7f);
				uint8_t val = (uint8_t)data[2];

				while (units > 0) {
					long d = units > 0x10000 ?  0x10000 : units;
					emit(gbs, &code_used, 0xcf, 3); // RST 0x08
					emit(gbs, &code_used, (d - 1) & 0xff, 0);
					emit(gbs, &code_used, (d - 1) >> 8, 0);
					units -= d;
					total_clocks += d * 64;
				}
				/* LD a, imm8 */
				emit(gbs, &code_used, 0x3e, 4);
				emit(gbs, &code_used, val, 0);
				/* LDH (a8), A */
				emit(gbs, &code_used, 0xe0, 0);
				emit(gbs, &code_used, reg, 0);
			}
			data += 2;
			break;
		}
		data++;
	}
	/* RST 0x38 */
	emit(gbs, &code_used, 0xff, 1);

	gbs->version = 0;
	gbs->songs = 1;
	gbs->defaultsong = 1;
	gbs->defaultbank = 1;
	gbs->load = 0x0400;
	gbs->init = 0x0440;
	gbs->play = 0x0404;
	gbs->tma = 0;
	gbs->tac = 0;
	gbs->stack = 0xfffe;
	gbs->title = na_str;
	gbs->author = na_str;
	gbs->copyright = na_str;
	gbs->filesize = size;
	gbs->crcnow = gbs_crc32(0, buf, gbs->filesize);

	gbs->subsong_info = malloc(gbs->songs * sizeof(struct gbs_subsong_info));
	memset(gbs->subsong_info, 0, gbs->songs * sizeof(struct gbs_subsong_info));
	gbs->subsong_info[0].len = total_clocks / 4096;

	if (gd3_len > 0) {
		gd3_parse(&gbs, gd3, gd3_len);
	}

	gbs->romsize = gbs->codelen + 0x4000;
	gbs->rom = calloc(1, gbs->romsize);
	memcpy(&gbs->rom[0x4000], gbs->code, gbs->codelen);

	/* 16 + 52 for RST + setup */
	addr = 0x8;
	gbs->rom[addr++] = 0xe1; /* 12: pop hl */
	gbs->rom[addr++] = 0x2a; /*  8: ld a, (hl+) */
	gbs->rom[addr++] = 0x5f; /*  4: ld e, a */
	gbs->rom[addr++] = 0x2a; /*  8: ld a, (hl+) */
	gbs->rom[addr++] = 0x57; /*  4: ld d, a */
	gbs->rom[addr++] = 0xe5; /* 16: push hl */

	/* 64 cycles for loop */
	jpaddr = addr;
	gbs->rom[addr++] = 0x7a; /*  4: ld a, d */
	gbs->rom[addr++] = 0xb3; /*  4: or e */
	gbs->rom[addr++] = 0xc8; /*  8: ret z */
	gbs->rom[addr++] = 0x1b; /*  8: dec de */
	gbs->rom[addr++] = 0x23; /*  8: inc hl */
	gbs->rom[addr++] = 0x23; /*  8: inc hl */
	gbs->rom[addr++] = 0x23; /*  8: inc hl */
	gbs->rom[addr++] = 0xc3; /* 16: jp @loop */
	gbs->rom[addr++] = jpaddr & 0xff;
	gbs->rom[addr++] = jpaddr >> 8;

	/* Trap opcode 0xff (rst 0x38) execution */
	jpaddr = addr = 0x38;
	gbs->rom[addr++] = 0xf3; /* di */
	gbs->rom[addr++] = 0x76; /* halt */
	gbs->rom[addr++] = 0xc3; /* jp $ */
	gbs->rom[addr++] = jpaddr & 0xff;
	gbs->rom[addr++] = jpaddr >> 8;

	gbs->rom[0x0040] = 0xd9; /* reti */
	gbs->rom[0x0050] = 0xd9; /* reti */

	addr = 0x440;
	gbs->rom[addr++] = 0xf3; /* di */
	gbs->rom[addr++] = 0x3e; /* ld a, 1 */
	gbs->rom[addr++] = 0x01;
	jpaddr = addr;
	gbs->rom[addr++] = 0xe0; /* ldh (0x80), a */
	gbs->rom[addr++] = 0x80;
	gbs->rom[addr++] = 0x21; /* ld hl, 0x2000 */
	gbs->rom[addr++] = 0x00;
	gbs->rom[addr++] = 0x20;
	gbs->rom[addr++] = 0x77; /* ld (hl), a */
	gbs->rom[addr++] = 0xcd; /* call 0x4000 */
	gbs->rom[addr++] = 0x00;
	gbs->rom[addr++] = 0x40;
	gbs->rom[addr++] = 0xf0; /* ldh a, (0x80) */
	gbs->rom[addr++] = 0x80;
	gbs->rom[addr++] = 0x3c; /* inc a */
	gbs->rom[addr++] = 0xc3; /* jp @loop */
	gbs->rom[addr++] = jpaddr & 0xff;
	gbs->rom[addr++] = jpaddr >> 8;

	return gbs;
}

static struct gbs *gbs_open_internal(const char *name, char *buf, size_t size)
{
	struct gbs *gbs = malloc(sizeof(struct gbs));
	long i;
	long have_ehdr = 0;
	char *buf2;

	memset(gbs, 0, sizeof(struct gbs));
	gbs->silence_timeout = 2;
	gbs->subsong_timeout = 2*60;
	gbs->gap = 2;
	gbs->fadeout = 3;
	gbs->defaultbank = 1;
	gbs->version = buf[0x03];
	if (gbs->version != 1) {
		fprintf(stderr, _("GBS Version %d unsupported.\n"), gbs->version);
		gbs_free(gbs);
		return NULL;
	}

	gbs->songs = buf[0x04];
	if (gbs->songs < 1) {
		fprintf(stderr, _("Number of subsongs = %d is unreasonable.\n"), gbs->songs);
		gbs_free(gbs);
		return NULL;
	}

	gbs->defaultsong = buf[0x05];
	if (gbs->defaultsong < 1 || gbs->defaultsong > gbs->songs) {
		fprintf(stderr, _("Default subsong %d is out of range [1..%d].\n"), gbs->defaultsong, gbs->songs);
		gbs_free(gbs);
		return NULL;
	}

	gbs->load  = readint(&buf[0x06], 2);
	gbs->init  = readint(&buf[0x08], 2);
	gbs->play  = readint(&buf[0x0a], 2);
	gbs->stack = readint(&buf[0x0c], 2);
	gbs->tma = buf[0x0e];
	gbs->tac = buf[0x0f];

	memcpy(gbs->v1strings, &buf[0x10], 32);
	memcpy(gbs->v1strings+33, &buf[0x30], 32);
	memcpy(gbs->v1strings+66, &buf[0x50], 30);
	gbs->title = gbs->v1strings;
	gbs->author = gbs->v1strings+33;
	gbs->copyright = gbs->v1strings+66;
	gbs->code = &buf[0x70];
	gbs->filesize = size;

	gbs->subsong_info = malloc(gbs->songs * sizeof(struct gbs_subsong_info));
	memset(gbs->subsong_info, 0, gbs->songs * sizeof(struct gbs_subsong_info));
	gbs->codelen = (buf[0x6e] + (buf[0x6f] << 8)) << 4;
	if ((0x70 + gbs->codelen) < (gbs->filesize - 8) &&
	    strncmp(&buf[0x70 + gbs->codelen], GBS_EXTHDR_MAGIC, 4) == 0) {
		uint32_t crc, realcrc, ehdrlen;

		gbs->exthdr = gbs->code + gbs->codelen;
		ehdrlen = readint(&gbs->exthdr[0x04], 4) + 8;
		crc = readint(&gbs->exthdr[0x08], 4);
		writeint(&gbs->exthdr[0x08], 0, 4);

		if ((realcrc=gbs_crc32(0, gbs->exthdr, ehdrlen)) == crc) {
			have_ehdr = 1;
		} else {
			fprintf(stderr, _("Warning: Extended header found, but CRC does not match (0x%08x != 0x%08x).\n"),
			        crc, realcrc);
		}
	}
	if (have_ehdr) {
		buf2 = gbs->exthdr;
		gbs->filesize = readint(&buf2[0x0c], 4);
		gbs->crc = readint(&buf2[0x10], 4);
		writeint(&buf2[0x10], 0, 4);
	} else {
		memcpy(gbs->v1strings+66, &buf[0x50], 32);
		gbs->codelen = size - 0x70;
	}
	gbs->crcnow = gbs_crc32(0, buf, gbs->filesize);
	if (have_ehdr) {
		long gbs_titleex;
		long gbs_authorex;
		long gbs_copyex;
		long entries;

		gbs->version = 2;
		entries = buf2[0x1c];
		gbs->strings = gbs->exthdr + 32 + 8*entries;

		gbs_titleex = readint(&buf2[0x14], 2);
		gbs_authorex = readint(&buf2[0x16], 2);
		gbs_copyex = readint(&buf2[0x18], 2);
		if (gbs_titleex != 0xffff)
			gbs->title = gbs->strings + gbs_titleex;
		if (gbs_authorex != 0xffff)
			gbs->author = gbs->strings + gbs_authorex;
		if (gbs_copyex != 0xffff)
			gbs->copyright = gbs->strings + gbs_copyex;

		for (i=0; i<entries; i++) {
			long ofs = readint(&buf2[32 + 8*i + 4], 2);
			gbs->subsong_info[i].len = readint(&buf2[32 + 8*i], 4);
			if (ofs == 0xffff)
				gbs->subsong_info[i].title = NULL;
			else gbs->subsong_info[i].title = gbs->strings + ofs;
		}

		if (gbs->crc != gbs->crcnow) {
			fprintf(stderr, _("Warning: File CRC does not match (0x%08x != 0x%08x).\n"),
			        gbs->crc, gbs->crcnow);
		}
	}

	gbs->romsize = (gbs->codelen + gbs->load + 0x3fff) & ~0x3fff;

	gbs->rom = calloc(1, gbs->romsize);
	memcpy(&gbs->rom[gbs->load], gbs->code, gbs->codelen);

	for (i=0; i<8; i++) {
		long addr = gbs->load + 8*i; /* jump address */
		gbs->rom[8*i]   = 0xc3; /* jp imm16 */
		gbs->rom[8*i+1] = addr & 0xff;
		gbs->rom[8*i+2] = addr >> 8;
	}
	if ((gbs->tac & 0x78) == 0x40) { /* ugetab int vector extension */
		/* V-Blank */
		gbs->rom[0x40] = 0xc3; /* jp imm16 */
		gbs->rom[0x41] = (gbs->load + 0x40) & 0xff;
		gbs->rom[0x42] = (gbs->load + 0x40) >> 8;
		/* Timer */
		gbs->rom[0x50] = 0xc3; /* jp imm16 */
		gbs->rom[0x51] = (gbs->load + 0x48) & 0xff;
		gbs->rom[0x52] = (gbs->load + 0x48) >> 8;
	} else if (gbs->tac & 0x04) { /* timer enabled */
		/* V-Blank */
		gbs->rom[0x40] = 0xd9; /* reti */
		/* Timer */
		gbs->rom[0x50] = 0xc3; /* jp imm16 */
		gbs->rom[0x51] = gbs->play & 0xff;
		gbs->rom[0x52] = gbs->play >> 8;
	} else {
		/* V-Blank */
		gbs->rom[0x40] = 0xc3; /* jp imm16 */
		gbs->rom[0x41] = gbs->play & 0xff;
		gbs->rom[0x42] = gbs->play >> 8;
		/* Timer */
		gbs->rom[0x50] = 0xd9; /* reti */
	}
	gbs->rom[0x48] = 0xd9; /* reti (LCD Stat) */
	gbs->rom[0x58] = 0xd9; /* reti (Serial) */
	gbs->rom[0x60] = 0xd9; /* reti (Joypad) */

	return gbs;
}

#ifdef USE_ZLIB
static struct gbs *gzip_open(const char *name, char *buf, size_t size)
{
	struct gbs *gbs;
	int ret;
	char *out = malloc(GB_MAX_ROM_SIZE);
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.next_in = (Bytef*)buf;
	strm.avail_in = size;
	strm.next_out = (Bytef*)out;
	strm.avail_out = GB_MAX_ROM_SIZE;

	/* inflate with gzip auto-detect */
	ret = inflateInit2(&strm, 15|32);
	if (ret != Z_OK) {
		fprintf(stderr, _("Could not open %s: inflateInit2: %d\n"), name, ret);
		return NULL;
	}

	ret = inflate(&strm, Z_FINISH);
	if (ret != Z_STREAM_END) {
		fprintf(stderr, _("Could not open %s: inflate: %d\n"), name, ret);
		return NULL;
	}
	inflateEnd(&strm);
	gbs = gbs_open_mem(name, out, GB_MAX_ROM_SIZE - strm.avail_out);
	if (gbs == NULL) {
		free(out);
	}

	return gbs;
}
#else
static struct gbs *gzip_open(const char *name, char *buf, size_t size)
{
	fprintf(stderr, _("Could not open %s: %s\n"), name, _("Not compiled with zlib support"));
	return NULL;
}
#endif

struct gbs *gbs_open_mem(const char *name, char *buf, size_t size)
{
	if (size > HDR_LEN_GZIP && strncmp(buf, GZIP_MAGIC, 3) == 0) {
		return gzip_open(name, buf, size);
	}
	if (size > HDR_LEN_GBR && strncmp(buf, GBR_MAGIC, 4) == 0) {
		return gbr_open(name, buf, size);
	}
	if (size > HDR_LEN_VGM && strncmp(buf, VGM_MAGIC, 4) == 0) {
		return vgm_open(name, buf, size);
	}
	if (size > HDR_LEN_GBS && strncmp(buf, GBS_MAGIC, 3) == 0) {
		return gbs_open_internal(name, buf, size);
	}
	if (size > HDR_LEN_GB && gbs_crc32(0, &buf[0x104], 48) == 0x46195417) {
		return gb_open(name, buf, size);
	}
	fprintf(stderr, _("Not a GBS-File: %s\n"), name);
	return NULL;
}

struct gbs *gbs_open(const char *name)
{
	struct gbs *gbs = NULL;
	FILE *f;
	struct stat st;
	char *buf;

	if ((f = fopen(name, "rb")) == NULL) {
		fprintf(stderr, _("Could not open %s: %s\n"), name, strerror(errno));
		return NULL;
	}
	if (fstat(fileno(f), &st) == -1) {
		fprintf(stderr, _("Could not stat %s: %s\n"), name, strerror(errno));
		return NULL;
	}
	if (st.st_size > GB_MAX_ROM_SIZE) {
		fprintf(stderr, _("Could not read %s: %s\n"), name, _("Bigger than allowed maximum (4MiB)"));
		return NULL;
	}
	buf = malloc(st.st_size);
	if (fread(buf, 1, st.st_size, f) != st.st_size) {
		fprintf(stderr, _("Could not read %s: %s\n"), name, strerror(errno));
		goto exit_free;
	}

	gbs = gbs_open_mem(name, buf, st.st_size);

exit_free:
	if (gbs == NULL || gbs->buf != buf)
		free(buf);
	fclose(f);
	return gbs;
}
