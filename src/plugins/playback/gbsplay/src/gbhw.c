/*
 * gbsplay is a Gameboy sound player
 *
 * 2003-2006,2008,2018 (C) by Tobias Diedrich <ranma+gbsplay@tdiedrich.de>
 *                            Christian Garbs <mitch@cgarbs.de>
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

#include "gbcpu.h"
#include "gbhw.h"
#include "impulse.h"

#define REG_TIMA 0x05
#define REG_TMA  0x06
#define REG_TAC  0x07
#define REG_IF   0x0f
#define REG_IE   0x7f /* Nominally 0xff, but we remap it to 0x7f internally. */

static uint8_t *rom;
static uint8_t intram[0x2000];
static uint8_t extram[0x2000];
static uint8_t ioregs[0x80];
static uint8_t hiram[0x80];
static long rombank = 1;
static long lastbank;
static long apu_on = 1;
static long io_written = 0;

static const uint8_t ioregs_ormask[sizeof(ioregs)] = {
	/* 0x00 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/* 0x10 */ 0x80, 0x3f, 0x00, 0xff, 0xbf,
	/* 0x15 */ 0xff, 0x3f, 0x00, 0xff, 0xbf,
	/* 0x1a */ 0x7f, 0xff, 0x9f, 0xff, 0xbf,
	/* 0x1f */ 0xff, 0xff, 0x00, 0x00, 0xbf,
	/* 0x24 */ 0x00, 0x00, 0x70, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};
static const uint8_t ioregs_initdata[sizeof(ioregs)] = {
	/* 0x00 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* sound registers */
	/* 0x10 */ 0x80, 0xbf, 0x00, 0x00, 0xbf,
	/* 0x15 */ 0x00, 0x3f, 0x00, 0x00, 0xbf,
	/* 0x1a */ 0x7f, 0xff, 0x9f, 0x00, 0xbf,
	/* 0x1f */ 0x00, 0xff, 0x00, 0x00, 0xbf,
	/* 0x24 */ 0x77, 0xf3, 0xf1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* wave pattern memory, taken from gbsound.txt v0.99.19 (12/31/2002) */
	/* 0x30 */ 0xac, 0xdd, 0xda, 0x48, 0x36, 0x02, 0xcf, 0x16, 0x2c, 0x04, 0xe5, 0x2c, 0xac, 0xdd, 0xda, 0x48,
};

static uint8_t boot_rom[256];

static const char dutylookup[4] = {
	1, 2, 4, 6
};
static const long len_mask[4] = {
	0x3f, 0x3f, 0xff, 0x3f
};

struct gbhw_channel gbhw_ch[4];

static long lminval, lmaxval, rminval, rmaxval;
static double filter_constant = GBHW_FILTER_CONST_DMG;
static int filter_enabled = 1;
static long cap_factor = 0x10000;

#define MASTER_VOL_MIN	0
#define MASTER_VOL_MAX	(256*256)
static long master_volume;
static long master_fade;
static long master_dstvol;
static long sample_rate;
static long update_level = 0;
static long sequence_ctr = 0;
static long halted_noirq_cycles = 0;

static const long vblanktc = 70224; /* ~59.73 Hz (vblankctr)*/
static const long vblankclocks = 4560;
static long vblankctr;
static long timertc = 16;
static long timerctr;

static const long msec_cycles = GBHW_CLOCK/1000;

static long sum_cycles;

static long pause_output = 0;
static long rom_lockout = 1;

static gbhw_callback_fn callback;
static void *callbackpriv;
static struct gbhw_buffer *soundbuf = NULL; /* externally visible output buffer */
static struct gbhw_buffer *impbuf = NULL;   /* internal impulse output buffer */

static gbhw_iocallback_fn iocallback;
static void *iocallback_priv;

static gbhw_stepcallback_fn stepcallback;
static void *stepcallback_priv;

#define TAP1_15		0x4000;
#define TAP2_15		0x2000;
#define TAP1_7		0x0040;
#define TAP2_7		0x0020;

static uint32_t tap1 = TAP1_15;
static uint32_t tap2 = TAP2_15;
static uint32_t lfsr = 0xffffffff;

#define SOUND_DIV_MULT 0x10000LL

static long long sound_div_tc = 0;
static const long main_div_tc = 32;
static long main_div;
static const long sweep_div_tc = 256;
static long sweep_div;

static long ch3pos;
static long last_l_value = 0, last_r_value = 0;
static long ch3_next_nibble = 0;

#define IMPULSE_WIDTH (1 << IMPULSE_W_SHIFT)
#define IMPULSE_N (1 << IMPULSE_N_SHIFT)
#define IMPULSE_N_MASK (IMPULSE_N - 1)

static uint32_t rom_get(uint32_t addr)
{
	if ((addr >> 8) == 0 && rom_lockout == 0) {
		return boot_rom[addr & 0xff];
	}
//	DPRINTF("rom_get(%04x)\n", addr);
	return rom[addr & 0x3fff];
}

static uint32_t rombank_get(uint32_t addr)
{
//	DPRINTF("rombank_get(%04x)\n", addr);
	return rom[(addr & 0x3fff) + 0x4000*rombank];
}

static uint32_t io_get(uint32_t addr)
{
	if (addr >= 0xff80 && addr <= 0xfffe) {
		return hiram[addr & 0x7f];
	}
	if (addr >= 0xff10 &&
	           addr <= 0xff3f) {
		uint8_t val = ioregs[addr & 0x7f];
		if (addr == 0xff26) {
			long i;
			val &= 0xf0;
			for (i=0; i<4; i++) {
				if (gbhw_ch[i].running) {
					val |= (1 << i);
				}
			}
		}
		val |= ioregs_ormask[addr & 0x7f];
		DPRINTF("io_get(%04x): %02x\n", addr, val);
		return val;
	}
	switch (addr) {
	case 0xff00:  // P1
		return 0;
	case 0xff05:  // TIMA
	case 0xff06:  // TMA
	case 0xff07:  // TAC
	case 0xff0f:  // IF
		return ioregs[addr & 0x7f];
	case 0xff41: /* LCDC Status */
		if (vblankctr > vblanktc - vblankclocks) {
			return 0x01;  /* vblank */
		} else {
			/* ~108.7uS per line */
			long t = (2 * vblanktc - vblankctr) % 456;
			if (t < 204) {
				/* 48.6uS in hblank (201-207 clks) */
				return 0x00;
			} else if (t < 284) {
				/* 19uS in OAM scan (77-83 clks) */
				return 0x02;
			}
		}
		return 0x03;  /* both OAM and display RAM busy */
	case 0xff44: /* LCD Y-coordinate */
		return ((2 * vblanktc - vblankclocks - vblankctr) / 456) % 154;
	case 0xff70:  // CGB ram bank switch
		WARN_ONCE("ioread from SVBK (CGB mode) ignored.\n");
		return 0xff;
	case 0xffff:
		return ioregs[0x7f];
	default:
		WARN_ONCE("ioread from 0x%04x unimplemented.\n", (unsigned int)addr);
		DPRINTF("io_get(%04x)\n", addr);
		return 0xff;
	}
}

static uint32_t intram_get(uint32_t addr)
{
//	DPRINTF("intram_get(%04x)\n", addr);
	return intram[addr & 0x1fff];
}

static uint32_t extram_get(uint32_t addr)
{
//	DPRINTF("extram_get(%04x)\n", addr);
	return extram[addr & 0x1fff];
}

static void rom_put(uint32_t addr, uint8_t val)
{
	if (addr >= 0x2000 && addr <= 0x3fff) {
		val &= 0x1f;
		rombank = val + (val == 0);
		if (rombank > lastbank) {
			WARN_ONCE("Bank %ld out of range (0-%ld)!\n", rombank, lastbank);
			rombank = lastbank;
		}
	} else {
		WARN_ONCE("rom write of %02x to %04x ignored\n", val, addr);
	}
}

static void apu_reset(void)
{
	long i;
	int mute_tmp[4];

	for (i = 0; i < 4; i++) {
		mute_tmp[i] = gbhw_ch[i].mute;
	}
	memset(gbhw_ch, 0, sizeof(gbhw_ch));
	for (i = 0xff10; i < 0xff26; i++) {
		ioregs[i & 0x7f] = 0;
	}
	for (i = 0; i < 4; i++) {
		gbhw_ch[i].len = 0;
		gbhw_ch[i].len_gate = 0;
		gbhw_ch[i].volume = 0;
		gbhw_ch[i].duty_ctr = 4;
		gbhw_ch[i].div_tc = 1;
		gbhw_ch[i].master = 1;
		gbhw_ch[i].running = 0;
		gbhw_ch[i].mute = mute_tmp[i];
	}
	sequence_ctr = 0;
}

static void linkport_atexit(void);

static void linkport_write(long c)
{
	static char buf[256];
	static long idx = 0;
	static long exit_handler_set = 0;
	static long enabled = 1;

	if (!enabled) {
		return;
	}
	if (!(c == -1 || c == '\r' || c == '\n' || (c >= 0x20 && c <= 0x7f))) {
		enabled = 0;
		fprintf(stderr, "Link port output %02lx ignored.\n", c);
		return;
	}
	if (c != -1 && idx < (sizeof(buf) - 1)) {
		buf[idx++] = c;
		buf[idx] = 0;
	}
	if (c == '\n' || (c == -1 && idx > 0)) {
		fprintf(stderr, "Link port text: %s", buf);
		idx = 0;
		if (!exit_handler_set) {
			atexit(linkport_atexit);
		}
	}
}

static void linkport_atexit(void)
{
	linkport_write(-1);
}

static void sequencer_update_len(long chn)
{
	if (gbhw_ch[chn].len_enable && gbhw_ch[chn].len_gate) {
		gbhw_ch[chn].len++;
		gbhw_ch[chn].len &= len_mask[chn];
		if (gbhw_ch[chn].len == 0) {
			gbhw_ch[chn].volume = 0;
			gbhw_ch[chn].env_tc = 0;
			gbhw_ch[chn].running = 0;
			gbhw_ch[chn].len_gate = 0;
		}
	}
}

static long sweep_check_overflow(void)
{
	long val = (2048 - gbhw_ch[0].div_tc_shadow) >> gbhw_ch[0].sweep_shift;

	if (gbhw_ch[0].sweep_shift == 0) {
		return 1;
	}

	if (!gbhw_ch[0].sweep_dir) {
		if (gbhw_ch[0].div_tc_shadow <= val) {
			gbhw_ch[0].running = 0;
			return 0;
		}
	}
	return 1;
}

static void io_put(uint32_t addr, uint8_t val)
{
	long chn = (addr - 0xff10)/5;

	if (addr >= 0xff80 && addr <= 0xfffe) {
		hiram[addr & 0x7f] = val;
		return;
	}

	io_written = 1;

	if (iocallback)
		iocallback(sum_cycles, addr, val, iocallback_priv);

	if (apu_on == 0 && addr >= 0xff10 && addr < 0xff26) {
		return;
	}
	ioregs[addr & 0x7f] = val;
	DPRINTF(" ([0x%04x]=%02x) ", addr, val);
	switch (addr) {
		case 0xff02:
			if (val & 0x80) {
				linkport_write(ioregs[1]);
			}
			break;
		case 0xff05:  // TIMA
		case 0xff06:  // TMA
			break;
		case 0xff07:  // TAC
			timertc = 16 << (((val+3) & 3) << 1);
			if ((val & 0xf0) == 0x80) timertc /= 2;
			if (timerctr > timertc) {
				timerctr = 0;
			}
			break;
		case 0xff0f:  // IF
			break;
		case 0xff10:
			gbhw_ch[0].sweep_ctr = gbhw_ch[0].sweep_tc = ((val >> 4) & 7);
			gbhw_ch[0].sweep_dir = (val >> 3) & 1;
			gbhw_ch[0].sweep_shift = val & 7;

			break;
		case 0xff11:
		case 0xff16:
		case 0xff20:
			{
				long duty_ctr = val >> 6;
				long len = val & 0x3f;

				gbhw_ch[chn].duty_ctr = dutylookup[duty_ctr];
				gbhw_ch[chn].duty_tc = gbhw_ch[chn].div_tc*gbhw_ch[chn].duty_ctr/8;
				gbhw_ch[chn].len = len;
				gbhw_ch[chn].len_gate = 1;

				break;
			}
		case 0xff12:
		case 0xff17:
		case 0xff21:
			{
				long vol = val >> 4;
				long envdir = (val >> 3) & 1;
				long envspd = val & 7;

				gbhw_ch[chn].volume = vol;
				gbhw_ch[chn].env_dir = envdir;
				gbhw_ch[chn].env_ctr = gbhw_ch[chn].env_tc = envspd;

				gbhw_ch[chn].master = (val & 0xf8) != 0;
				if (!gbhw_ch[chn].master) {
					gbhw_ch[chn].running = 0;
				}
			}
			break;
		case 0xff13:
		case 0xff14:
		case 0xff18:
		case 0xff19:
		case 0xff1d:
		case 0xff1e:
			{
				long div = ioregs[0x13 + 5*chn];
				long old_len_enable = gbhw_ch[chn].len_enable;

				div |= ((long)ioregs[0x14 + 5*chn] & 7) << 8;
				gbhw_ch[chn].div_tc = 2048 - div;
				gbhw_ch[chn].duty_tc = gbhw_ch[chn].div_tc*gbhw_ch[chn].duty_ctr/8;

				if (addr == 0xff13 ||
				    addr == 0xff18 ||
				    addr == 0xff1d) break;

				gbhw_ch[chn].len_enable = (ioregs[0x14 + 5*chn] & 0x40) > 0;
				if ((val & 0x80) == 0x80) {
					if (!gbhw_ch[chn].len_gate) {
						gbhw_ch[chn].len_gate = 1;
						if (old_len_enable == 1 &&
						    gbhw_ch[chn].len_enable == 1 &&
						    (sequence_ctr & 1) == 1) {
							// Trigger that un-freezes enabled length should clock it
							sequencer_update_len(chn);
						}
					}
					if (gbhw_ch[chn].master) {
						gbhw_ch[chn].running = 1;
					}
					if (addr == 0xff1e) {
						ch3pos = 0;
					}
					if (addr == 0xff14) {
						gbhw_ch[0].div_tc_shadow = gbhw_ch[0].div_tc;
						sweep_check_overflow();
					}
				}
				if (old_len_enable == 0 &&
				    gbhw_ch[chn].len_enable == 1 &&
				    (sequence_ctr & 1) == 1) {
					// Enabling in first half of length period should clock length
					sequencer_update_len(chn);
				}
			}

//			printf(" ch%ld: vol=%02d envd=%ld envspd=%ld duty_ctr=%ld len=%03d len_en=%ld key=%04d gate=%ld%ld\n", chn, gbhw_ch[chn].volume, gbhw_ch[chn].env_dir, gbhw_ch[chn].env_tc, gbhw_ch[chn].duty_ctr, gbhw_ch[chn].len, gbhw_ch[chn].len_enable, gbhw_ch[chn].div_tc, gbhw_ch[chn].leftgate, gbhw_ch[chn].rightgate);
			break;
		case 0xff15:
			break;
		case 0xff1a:
			gbhw_ch[2].master = (ioregs[0x1a] & 0x80) > 0;
			if (!gbhw_ch[2].master) {
				gbhw_ch[2].running = 0;
			}
			break;
		case 0xff1b:
			gbhw_ch[2].len = val;
			gbhw_ch[2].len_gate = 1;
			break;
		case 0xff1c:
			{
				long vol = (ioregs[0x1c] >> 5) & 3;
				gbhw_ch[2].volume = vol;
				break;
			}
		case 0xff1f:
			break;
		case 0xff22:
		case 0xff23:
			{
				long reg = ioregs[0x22];
				long shift = reg >> 4;
				long rate = reg & 7;
				long old_len_enable = gbhw_ch[chn].len_enable;
				gbhw_ch[3].div_ctr = 0;
				gbhw_ch[3].div_tc = 16 << shift;
				if (reg & 8) {
					tap1 = TAP1_7;
					tap2 = TAP2_7;
				} else {
					tap1 = TAP1_15;
					tap2 = TAP2_15;
				}
				if (rate) gbhw_ch[3].div_tc *= rate;
				else gbhw_ch[3].div_tc /= 2;
				gbhw_ch[chn].len_enable = (ioregs[0x23] & 0x40) > 0;
				if (addr == 0xff22) break;

				if (val & 0x80) {  /* trigger */
					lfsr = 0xffffffff;
					if (!gbhw_ch[chn].len_gate) {
						gbhw_ch[chn].len_gate = 1;
						if (old_len_enable == 1 &&
						    gbhw_ch[chn].len_enable == 1 &&
						    (sequence_ctr & 1) == 1) {
							// Trigger that un-freezes enabled length should clock it
							sequencer_update_len(chn);
						}
					}
					if (gbhw_ch[3].master) {
						gbhw_ch[3].running = 1;
					}
				}
				if (old_len_enable == 0 &&
				    gbhw_ch[chn].len_enable == 1 &&
				    (sequence_ctr & 1) == 1) {
					// Enabling in first half of length period should clock length
					sequencer_update_len(chn);
				}
//				printf(" ch4: vol=%02d envd=%ld envspd=%ld duty_ctr=%ld len=%03d len_en=%ld key=%04d gate=%ld%ld\n", gbhw_ch[3].volume, gbhw_ch[3].env_dir, gbhw_ch[3].env_ctr, gbhw_ch[3].duty_ctr, gbhw_ch[3].len, gbhw_ch[3].len_enable, gbhw_ch[3].div_tc, gbhw_ch[3].leftgate, gbhw_ch[3].rightgate);
			}
			break;
		case 0xff25:
			gbhw_ch[0].leftgate = (val & 0x10) > 0;
			gbhw_ch[0].rightgate = (val & 0x01) > 0;
			gbhw_ch[1].leftgate = (val & 0x20) > 0;
			gbhw_ch[1].rightgate = (val & 0x02) > 0;
			gbhw_ch[2].leftgate = (val & 0x40) > 0;
			gbhw_ch[2].rightgate = (val & 0x04) > 0;
			gbhw_ch[3].leftgate = (val & 0x80) > 0;
			gbhw_ch[3].rightgate = (val & 0x08) > 0;
			update_level = 1;
			break;
		case 0xff26:
			if (val & 0x80) {
				ioregs[0x26] = 0x80;
				apu_on = 1;
			} else {
				apu_reset();
				apu_on = 0;
			}
			break;
		case 0xff70:
			WARN_ONCE("iowrite to SVBK (CGB mode) ignored.\n");
			break;
		case 0xff00:
		case 0xff24:
		case 0xff27:
		case 0xff28:
		case 0xff29:
		case 0xff2a:
		case 0xff2b:
		case 0xff2c:
		case 0xff2d:
		case 0xff2e:
		case 0xff2f:
		case 0xff30:
		case 0xff31:
		case 0xff32:
		case 0xff33:
		case 0xff34:
		case 0xff35:
		case 0xff36:
		case 0xff37:
		case 0xff38:
		case 0xff39:
		case 0xff3a:
		case 0xff3b:
		case 0xff3c:
		case 0xff3d:
		case 0xff3e:
		case 0xff3f:
		case 0xff50: /* bootrom lockout reg */
			if (val == 0x01) {
				rom_lockout = 1;
			}
			break;
		case 0xffff:
			break;
		default:
			WARN_ONCE("iowrite to 0x%04x unimplemented (val=%02x).\n", addr, val);
			break;
	}
}

static void intram_put(uint32_t addr, uint8_t val)
{
	intram[addr & 0x1fff] = val;
}

static void extram_put(uint32_t addr, uint8_t val)
{
	extram[addr & 0x1fff] = val;
}

static void sequencer_step(void)
{
	long i;
	long clock_len = 0;
	long clock_env = 0;
	long clock_sweep = 0;

	switch (sequence_ctr & 7) {
	case 0: clock_len = 1; break;
	case 1: break;
	case 2: clock_len = 1; clock_sweep = 1; break;
	case 3: break;
	case 4: clock_len = 1; break;
	case 5: break;
	case 6: clock_len = 1; clock_sweep = 1; break;
	case 7: clock_env = 1; break;
	}

	sequence_ctr++;

	if (clock_sweep && gbhw_ch[0].sweep_tc) {
		gbhw_ch[0].sweep_ctr--;
		if (gbhw_ch[0].sweep_ctr < 0) {
			long val = (2048 - gbhw_ch[0].div_tc_shadow) >> gbhw_ch[0].sweep_shift;

			gbhw_ch[0].sweep_ctr = gbhw_ch[0].sweep_tc;
			if (sweep_check_overflow()) {
				if (gbhw_ch[0].sweep_dir) {
					gbhw_ch[0].div_tc_shadow += val;
				} else {
					gbhw_ch[0].div_tc_shadow -= val;
				}
				gbhw_ch[0].div_tc = gbhw_ch[0].div_tc_shadow;
			}
			gbhw_ch[0].duty_tc = gbhw_ch[0].div_tc*gbhw_ch[0].duty_ctr/8;
		}
	}
	for (i=0; clock_len && i<4; i++) {
		sequencer_update_len(i);
	}
	for (i=0; clock_env && i<4; i++) {
		if (gbhw_ch[i].env_tc) {
			gbhw_ch[i].env_ctr--;
			if (gbhw_ch[i].env_ctr <=0 ) {
				gbhw_ch[i].env_ctr = gbhw_ch[i].env_tc;
				if (gbhw_ch[i].running) {
					if (!gbhw_ch[i].env_dir) {
						if (gbhw_ch[i].volume > 0)
							gbhw_ch[i].volume--;
					} else {
						if (gbhw_ch[i].volume < 15)
						gbhw_ch[i].volume++;
					}
				}
			}
		}
	}
	if (master_fade) {
		master_volume += master_fade;
		if ((master_fade > 0 &&
		     master_volume >= master_dstvol) ||
		    (master_fade < 0 &&
		     master_volume <= master_dstvol)) {
			master_fade = 0;
			master_volume = master_dstvol;
		}
	}
}

void gbhw_master_fade(long speed, long dstvol)
{
	if (dstvol < MASTER_VOL_MIN) dstvol = MASTER_VOL_MIN;
	if (dstvol > MASTER_VOL_MAX) dstvol = MASTER_VOL_MAX;
	master_dstvol = dstvol;
	if (dstvol > master_volume)
		master_fade = speed;
	else master_fade = -speed;
}

#define GET_NIBBLE(p, n) ({ \
	long index = ((n) >> 1) & 0xf; \
	long shift = (~(n) & 1) << 2; \
	(((p)[index] >> shift) & 0xf); })

static void gb_flush_buffer(void)
{
	long i;
	long overlap;
	long l_smpl, r_smpl;
	long l_cap, r_cap;

	assert(soundbuf != NULL);
	assert(impbuf != NULL);

	/* integrate buffer */
	l_smpl = soundbuf->l_lvl;
	r_smpl = soundbuf->r_lvl;
	l_cap = soundbuf->l_cap;
	r_cap = soundbuf->r_cap;
	for (i=0; i<soundbuf->samples; i++) {
		long l_out, r_out;
		l_smpl = l_smpl + impbuf->data[i*2  ];
		r_smpl = r_smpl + impbuf->data[i*2+1];
		if (filter_enabled && cap_factor <= 0x10000) {
			/*
			 * RC High-pass & DC decoupling filter. Gameboy
			 * Classic uses 1uF and 510 Ohms in series,
			 * followed by 10K Ohms pot to ground between
			 * CPU output and amplifier input, which gives a
			 * cutoff frequency of 15.14Hz.
			 */
			l_out = l_smpl - (l_cap >> 16);
			r_out = r_smpl - (r_cap >> 16);
			/* cap factor is 0x10000 for a factor of 1.0 */
			l_cap = (l_smpl << 16) - l_out * cap_factor;
			r_cap = (r_smpl << 16) - r_out * cap_factor;
		} else {
			l_out = l_smpl;
			r_out = r_smpl;
		}
		soundbuf->data[i*2  ] = l_out * master_volume / MASTER_VOL_MAX;
		soundbuf->data[i*2+1] = r_out * master_volume / MASTER_VOL_MAX;
		if (l_out > lmaxval) lmaxval = l_out;
		if (l_out < lminval) lminval = l_out;
		if (r_out > rmaxval) rmaxval = r_out;
		if (r_out < rminval) rminval = r_out;
	}
	soundbuf->pos = soundbuf->samples;
	soundbuf->l_lvl = l_smpl;
	soundbuf->r_lvl = r_smpl;
	soundbuf->l_cap = l_cap;
	soundbuf->r_cap = r_cap;

	if (callback != NULL) callback(soundbuf, callbackpriv);

	overlap = impbuf->samples - soundbuf->samples;
	memmove(impbuf->data, impbuf->data+(2*soundbuf->samples), 4*overlap);
	memset(impbuf->data + 2*overlap, 0, impbuf->bytes - 4*overlap);
	assert(impbuf->bytes == impbuf->samples*4);
	assert(soundbuf->bytes == soundbuf->samples*4);
	memset(soundbuf->data, 0, soundbuf->bytes);
	soundbuf->pos = 0;

	impbuf->cycles -= (sound_div_tc * soundbuf->samples) / SOUND_DIV_MULT;
}

static void gb_change_level(long l_ofs, long r_ofs)
{
	long pos;
	long imp_idx;
	long imp_l = -IMPULSE_WIDTH/2;
	long imp_r = IMPULSE_WIDTH/2;
	long i;
	const short *ptr = base_impulse;

	assert(impbuf != NULL);
	pos = (long)(impbuf->cycles * SOUND_DIV_MULT / sound_div_tc);
	imp_idx = (long)((impbuf->cycles << IMPULSE_N_SHIFT)*SOUND_DIV_MULT / sound_div_tc) & IMPULSE_N_MASK;
	assert(pos + imp_r < impbuf->samples);
	assert(pos + imp_l >= 0);

	ptr += imp_idx * IMPULSE_WIDTH;

	for (i=imp_l; i<imp_r; i++) {
		long bufi = pos + i;
		long impi = i + IMPULSE_WIDTH/2;
		impbuf->data[bufi*2  ] += ptr[impi] * l_ofs;
		impbuf->data[bufi*2+1] += ptr[impi] * r_ofs;
	}

	impbuf->l_lvl += l_ofs*256;
	impbuf->r_lvl += r_ofs*256;
}

static void gb_sound(long cycles)
{
	long i, j;
	long l_lvl = 0, r_lvl = 0;

	assert(impbuf != NULL);

	for (j=0; j<cycles; j++) {
		main_div++;
		impbuf->cycles++;
		if (impbuf->cycles*SOUND_DIV_MULT >= sound_div_tc*(impbuf->samples - IMPULSE_WIDTH/2))
			gb_flush_buffer();

		if (gbhw_ch[2].running) {
			gbhw_ch[2].div_ctr--;
			if (gbhw_ch[2].div_ctr <= 0) {
				long val = ch3_next_nibble;
				long pos = ch3pos++;
				ch3_next_nibble = GET_NIBBLE(&ioregs[0x30], pos) * 2;
				gbhw_ch[2].div_ctr = gbhw_ch[2].div_tc*2;
				if (gbhw_ch[2].volume) {
					val = val >> (gbhw_ch[2].volume-1);
				} else val = 0;
				gbhw_ch[2].lvl = val - 15;
				update_level = 1;
			}
		}

		if (gbhw_ch[3].running) {
			gbhw_ch[3].div_ctr--;
			if (gbhw_ch[3].div_ctr <= 0) {
				long val;
				gbhw_ch[3].div_ctr = gbhw_ch[3].div_tc;
				lfsr = (lfsr << 1) | (((lfsr & tap1) > 0) ^ ((lfsr & tap2) > 0));
				val = gbhw_ch[3].volume * 2 * (!(lfsr & tap1));
				gbhw_ch[3].lvl = val - 15;
				update_level = 1;
			}
		}

		if (main_div > main_div_tc) {
			main_div -= main_div_tc;

			for (i=0; i<2; i++) if (gbhw_ch[i].running) {
				long val = 2 * gbhw_ch[i].volume;
				if (gbhw_ch[i].div_ctr > gbhw_ch[i].duty_tc) {
					val = 0;
				}
				gbhw_ch[i].lvl = val - 15;
				gbhw_ch[i].div_ctr--;
				if (gbhw_ch[i].div_ctr <= 0) {
					gbhw_ch[i].div_ctr = gbhw_ch[i].div_tc;
				}
			}

			sweep_div += 1;
			if (sweep_div >= sweep_div_tc) {
				sweep_div = 0;
				sequencer_step();
			}
			update_level = 1;
		}

		if (update_level) {
			update_level = 0;
			l_lvl = 0;
			r_lvl = 0;
			for (i=0; i<4; i++) {
				if (gbhw_ch[i].mute)
					continue;
				if (gbhw_ch[i].leftgate)
					l_lvl += gbhw_ch[i].lvl;
				if (gbhw_ch[i].rightgate)
					r_lvl += gbhw_ch[i].lvl;
			}

			if (l_lvl != last_l_value || r_lvl != last_r_value) {
				gb_change_level(l_lvl - last_l_value, r_lvl - last_r_value);
				last_l_value = l_lvl;
				last_r_value = r_lvl;
			}
		}
	}
}

void gbhw_setcallback(gbhw_callback_fn fn, void *priv)
{
	callback = fn;
	callbackpriv = priv;
}

void gbhw_setiocallback(gbhw_iocallback_fn fn, void *priv)
{
	iocallback = fn;
	iocallback_priv = priv;
}

void gbhw_setstepcallback(gbhw_stepcallback_fn fn, void *priv)
{
	stepcallback = fn;
	stepcallback_priv = priv;
}

static void gbhw_impbuf_reset(struct gbhw_buffer *impbuf)
{
	assert(sound_div_tc != 0);
	impbuf->cycles = (long)(sound_div_tc * IMPULSE_WIDTH/2 / SOUND_DIV_MULT);
	impbuf->l_lvl = 0;
	impbuf->r_lvl = 0;
	memset(impbuf->data, 0, impbuf->bytes);
}

void gbhw_setbuffer(struct gbhw_buffer *buffer)
{
	soundbuf = buffer;
	soundbuf->samples = soundbuf->bytes / 4;

	if (impbuf) free(impbuf);
	impbuf = malloc(sizeof(*impbuf) + (soundbuf->samples + IMPULSE_WIDTH + 1) * 4);
	if (impbuf == NULL) {
		fprintf(stderr, "%s", _("Memory allocation failed!\n"));
		return;
	}
	memset(impbuf, 0, sizeof(*impbuf));
	impbuf->data = (void*)(impbuf+1);
	impbuf->samples = soundbuf->samples + IMPULSE_WIDTH + 1;
	impbuf->bytes = impbuf->samples * 4;
	gbhw_impbuf_reset(impbuf);
}

static void gbhw_update_filter()
{
	double cap_constant = pow(filter_constant, (double)GBHW_CLOCK / sample_rate);
	cap_factor = round(65536.0 * cap_constant);
}

long gbhw_setfilter(const char *type)
{
	if (strcasecmp(type, GBHW_CFG_FILTER_OFF) == 0) {
		filter_enabled = 0;
		filter_constant = GBHW_FILTER_CONST_OFF;
	} else if (strcasecmp(type, GBHW_CFG_FILTER_DMG) == 0) {
		filter_enabled = 1;
		filter_constant = GBHW_FILTER_CONST_DMG;
	} else if (strcasecmp(type, GBHW_CFG_FILTER_CGB) == 0) {
		filter_enabled = 1;
		filter_constant = GBHW_FILTER_CONST_CGB;
	} else {
		return 0;
	}

	gbhw_update_filter();

	return 1;
}

void gbhw_setrate(long rate)
{
	sample_rate = rate;
	sound_div_tc = GBHW_CLOCK*SOUND_DIV_MULT/rate;
	gbhw_update_filter();
}

void gbhw_getminmax(int16_t *lmin, int16_t *lmax, int16_t *rmin, int16_t *rmax)
{
	if (lminval == INT_MAX) return;
	*lmin = lminval;
	*lmax = lmaxval;
	*rmin = rminval;
	*rmax = rmaxval;
	lminval = rminval = INT_MAX;
	lmaxval = rmaxval = INT_MIN;
}

/*
 * Initialize Gameboy hardware emulation.
 * The size should be a multiple of 0x4000,
 * so we don't need range checking in rom_get and
 * rombank_get.
 */
void gbhw_init(uint8_t *rombuf, uint32_t size)
{
	long i;

	vblankctr = vblanktc;
	timerctr = 0;

	if (impbuf)
		gbhw_impbuf_reset(impbuf);
	rom = rombuf;
	lastbank = ((size + 0x3fff) / 0x4000) - 1;
	rombank = 1;
	master_volume = MASTER_VOL_MAX;
	master_fade = 0;
	apu_on = 1;
	if (soundbuf) {
		soundbuf->pos = 0;
		soundbuf->l_lvl = 0;
		soundbuf->r_lvl = 0;
		soundbuf->l_cap = 0;
		soundbuf->r_cap = 0;
	}
	lminval = rminval = INT_MAX;
	lmaxval = rmaxval = INT_MIN;
	apu_reset();
	memset(extram, 0, sizeof(extram));
	memset(intram, 0, sizeof(intram));
	memset(hiram, 0, sizeof(hiram));
	memset(ioregs, 0, sizeof(ioregs));
	for (i=0x10; i<0x40; i++) {
		io_put(0xff00 + i, ioregs_initdata[i]);
	}

	sum_cycles = 0;
	halted_noirq_cycles = 0;
	ch3pos = 0;
	ch3_next_nibble = 0;
	last_l_value = 0;
	last_r_value = 0;

	gbcpu_init();
	gbcpu_addmem(0x00, 0x3f, rom_put, rom_get);
	gbcpu_addmem(0x40, 0x7f, rom_put, rombank_get);
	gbcpu_addmem(0xa0, 0xbf, extram_put, extram_get);
	gbcpu_addmem(0xc0, 0xfe, intram_put, intram_get);
	gbcpu_addmem(0xff, 0xff, io_put, io_get);
}

void gbhw_enable_bootrom(const uint8_t *rombuf)
{
	memcpy(boot_rom, rombuf, sizeof(boot_rom));
	rom_lockout = 0;
}

/* internal for gbs.c, not exported from libgbs */
void gbhw_io_put(uint16_t addr, uint8_t val) {
	if (addr != 0xffff && (addr < 0xff00 || addr > 0xff7f))
		return;
	io_put(addr, val);
}

/* unmasked peek used by gbsplay.c to print regs */
uint8_t gbhw_io_peek(uint16_t addr)
{
	if (addr >= 0xff10 && addr <= 0xff3f) {
		return ioregs[addr & 0x7f];
	}
	return 0xff;
}


void gbhw_check_if(void)
{
	uint8_t mask = 0x01; /* lowest bit is highest priority irq */
	uint8_t vec = 0x40;
	if (!gbcpu_if) {
		/* interrupts disabled */
		if (ioregs[REG_IF] & ioregs[REG_IE]) {
			/* but will still exit halt */
			gbcpu_halted = 0;
		}
		return;
	}
	while (mask <= 0x10) {
		if (ioregs[REG_IF] & ioregs[REG_IE] & mask) {
			ioregs[REG_IF] &= ~mask;
			gbcpu_halted = 0;
			gbcpu_intr(vec);
			break;
		}
		vec += 0x08;
		mask <<= 1;
	}
}

static void blargg_debug(void)
{
	long i;

	/* Blargg GB debug output signature. */
	if (gbcpu_mem_get(0xa001) != 0xde ||
	    gbcpu_mem_get(0xa002) != 0xb0 ||
	    gbcpu_mem_get(0xa003) != 0x61) {
		return;
	}

	fprintf(stderr, "\nBlargg debug output:\n");

	for (i = 0xa004; i < 0xb000; i++) {
		uint8_t c = gbcpu_mem_get(i);
		if (c == 0 || c >= 128) {
			return;
		}
		if (c < 32 && c != 10 && c != 13) {
			return;
		}
		fputc(c, stderr);
	}
}

/**
 * @param time_to_work  emulated time in milliseconds
 * @return  elapsed cpu cycles
 */
long gbhw_step(long time_to_work)
{
	long cycles_total = 0;

	if (pause_output) {
		struct timespec waittime = {
			.tv_sec = 0,
			.tv_nsec = time_to_work*1000
		};
		nanosleep(&waittime, NULL);
		return 0;
	}

	time_to_work *= msec_cycles;
	
	while (cycles_total < time_to_work) {
		long maxcycles = time_to_work - cycles_total;
		long cycles = 0;

		if (vblankctr > 0 && vblankctr < maxcycles) maxcycles = vblankctr;
		if (timerctr > 0 && timerctr < maxcycles) maxcycles = timerctr;

		io_written = 0;
		while (cycles < maxcycles && !io_written) {
			long step;
			gbhw_check_if();
			step = gbcpu_step();
			if (gbcpu_halted) {
				halted_noirq_cycles += step;
				if (gbcpu_if == 0 &&
				    (ioregs[REG_IE] == 0 ||
				     halted_noirq_cycles > GBHW_CLOCK/10)) {
					fprintf(stderr, "CPU locked up (halt with interrupts disabled).\n");
					blargg_debug();
					return -1;
				}
			} else {
				halted_noirq_cycles = 0;
			}
			if (step < 0) return step;
			cycles += step;
			sum_cycles += step;
			vblankctr -= step;
			if (vblankctr <= 0) {
				vblankctr += vblanktc;
				ioregs[REG_IF] |= 0x01;
				DPRINTF("vblank_interrupt\n");
			}
			gb_sound(step);
			if (stepcallback)
			   stepcallback(sum_cycles, gbhw_ch, stepcallback_priv);
		}

		if (ioregs[REG_TAC] & 4) {
			if (timerctr > 0) timerctr -= cycles;
			while (timerctr <= 0) {
				timerctr += timertc;
				ioregs[REG_TIMA]++;
				//DPRINTF("TIMA=%02x\n", ioregs[REG_TIMA]);
				if (ioregs[REG_TIMA] == 0) {
					ioregs[REG_TIMA] = ioregs[REG_TMA];
					ioregs[REG_IF] |= 0x04;
					DPRINTF("timer_interrupt\n");
				}
			}
		}
		cycles_total += cycles;
	}

	return cycles_total;
}

void gbhw_pause(long new_pause)
{
	pause_output = new_pause != 0;
}
