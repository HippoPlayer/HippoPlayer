/*
 * gbsplay is a Gameboy sound player
 *
 * 2003-2005 (C) by Tobias Diedrich <ranma+gbsplay@tdiedrich.de>
 * Licensed under GNU GPL v1 or, at your option, any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>
#include <string.h>

#include "gbcpu.h"

#if DEBUG == 1
static const char regnames[12] = "BCDEHLFASPPC";
static const char *regnamech16[6] = {
	"BC", "DE", "HL", "AF", "SP", "PC"
};
static const char *conds[4] = {
	"NZ", "Z", "NC", "C"
};
#endif

struct opinfo;

typedef void (*ex_fn)(uint32_t op, const struct opinfo *oi);

struct opinfo {
#if DEBUG == 1 || defined(S_SPLINT_S)
	char *name;
#endif
	ex_fn fn;
#if DEBUG == 1
	char cycles_1;
	char cycles_2;
#endif
};

gbcpu_regs_u gbcpu_regs;
long gbcpu_halted;
long gbcpu_stopped;
long gbcpu_if;
long gbcpu_halt_at_pc;
long gbcpu_cycles;

static uint32_t none_get(uint32_t addr)
{
	return 0xff;
}

static void none_put(uint32_t addr, uint8_t val)
{
}

static gbcpu_get_fn getlookup[256] = {
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get,
	&none_get
};

static gbcpu_put_fn putlookup[256] = {
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put,
	&none_put
};

static inline uint32_t mem_get(uint32_t addr)
{
	gbcpu_get_fn fn = getlookup[(addr >> 8) & 0xff];
	gbcpu_cycles += 4;
	return fn(addr);
}

static inline void mem_put(uint32_t addr, uint32_t val)
{
	gbcpu_put_fn fn = putlookup[(addr >> 8) & 0xff];
	gbcpu_cycles += 4;
	fn(addr, val);
}

uint8_t gbcpu_mem_get(uint16_t addr)
{
	return mem_get(addr);
}

void gbcpu_mem_put(uint16_t addr, uint8_t val)
{
	mem_put(addr, val);
}

static void push(uint32_t val)
{
	uint32_t sp = REGS16_R(gbcpu_regs, SP) - 2;
	REGS16_W(gbcpu_regs, SP, sp);
	mem_put(sp, val & 0xff);
	mem_put(sp+1, val >> 8);
}

static uint32_t pop(void)
{
	uint32_t res;
	uint32_t sp = REGS16_R(gbcpu_regs, SP);

	res  = mem_get(sp);
	res += mem_get(sp+1) << 8;
	REGS16_W(gbcpu_regs, SP, sp + 2);

	return res;
}

static uint32_t get_imm8(void)
{
	uint32_t pc = REGS16_R(gbcpu_regs, PC);
	uint32_t res;
	REGS16_W(gbcpu_regs, PC, pc + 1);
	res = mem_get(pc);
	DPRINTF("%02x", res);
	return res;
}

static uint32_t get_imm16(void)
{
	uint32_t pc = REGS16_R(gbcpu_regs, PC);
	uint32_t res;
	REGS16_W(gbcpu_regs, PC, pc + 2);
	res = mem_get(pc) + (mem_get(pc+1) << 8);
	DPRINTF("%04x", res);
	return res;
}

static inline void print_reg(long i)
{
	if (i == 6) DPRINTF("[HL]"); /* indirect memory access by [HL] */
	else DPRINTF("%c", regnames[i]);
}

static uint32_t get_reg(long i)
{
	if (i == 6) /* indirect memory access by [HL] */
		return mem_get(REGS16_R(gbcpu_regs, HL));
	return REGS8_R(gbcpu_regs, i);
}

static void put_reg(long i, uint32_t val)
{
	if (i == 6) /* indirect memory access by [HL] */
		mem_put(REGS16_R(gbcpu_regs, HL), val);
	else REGS8_W(gbcpu_regs, i, val);
}

static void op_unknown(uint32_t op, const struct opinfo *oi)
{
	fprintf(stderr, "\n\nUnknown opcode %02x.\n", (unsigned char)op);
	gbcpu_stopped = 1;
}

static void op_set(uint32_t op)
{
	long reg = op & 7;
	unsigned long bit = (op >> 3) & 7;

	DPRINTF("\tSET %ld, ", bit);
	print_reg(reg);
	put_reg(reg, get_reg(reg) | (1 << bit));
}

static void op_res(uint32_t op)
{
	long reg = op & 7;
	unsigned long bit = (op >> 3) & 7;

	DPRINTF("\tRES %ld, ", bit);
	print_reg(reg);
	put_reg(reg, get_reg(reg) & ~(1 << bit));
}

static void op_bit(uint32_t op)
{
	long reg = op & 7;
	unsigned long bit = (op >> 3) & 7;

	DPRINTF("\tBIT %ld, ", bit);
	print_reg(reg);
	gbcpu_regs.rn.f &= ~NF;
	gbcpu_regs.rn.f |= HF | ZF;
	gbcpu_regs.rn.f ^= ((get_reg(reg) << 8) >> (bit+1)) & ZF;
}

static void op_rl(uint32_t op, const struct opinfo *oi)
{
	/* C <- rrrrrrrr <-
	 * |              |
	 *  --------------
	 */
	long reg = op & 7;
	uint8_t res, val;

	DPRINTF(" \t%s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res << 1;
	res |= (gbcpu_regs.rn.f & CF) >> 4;
	gbcpu_regs.rn.f = (val >> 7) << 4;
	if (res == 0) gbcpu_regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_rla(uint32_t op, const struct opinfo *oi)
{
	/* C <- aaaaaaaa <-
	 * |              |
	 *  --------------
	 */
	uint8_t res;

	DPRINTF(" \t%s", oi->name);
	res  = gbcpu_regs.rn.a;
	res  = res << 1;
	res |= (gbcpu_regs.rn.f & CF) >> 4;
	gbcpu_regs.rn.f = (gbcpu_regs.rn.a >> 7) << 4;
	gbcpu_regs.rn.a = res;
}

static void op_rlc(uint32_t op, const struct opinfo *oi)
{
	/* C <- rrrrrrrr <-
	 *    |           |
	 *     -----------
	 */
	long reg = op & 7;
	uint8_t res, val;

	DPRINTF(" \t%s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res << 1;
	res |= val >> 7;
	gbcpu_regs.rn.f = (val >> 7) << 4;
	if (res == 0) gbcpu_regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_rlca(uint32_t op, const struct opinfo *oi)
{
	/* C <- aaaaaaaa <-
	 *    |           |
	 *     -----------
	 */
	uint8_t res;

	DPRINTF(" \t%s", oi->name);
	res  = gbcpu_regs.rn.a;
	res  = res << 1;
	res |= gbcpu_regs.rn.a >> 7;
	gbcpu_regs.rn.f = (gbcpu_regs.rn.a >> 7) << 4;
	gbcpu_regs.rn.a = res;
}

static void op_sla(uint32_t op, const struct opinfo *oi)
{
	long reg = op & 7;
	uint8_t res, val;

	DPRINTF(" \t%s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res << 1;
	gbcpu_regs.rn.f = (val >> 7) << 4;
	if (res == 0) gbcpu_regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_rr(uint32_t op, const struct opinfo *oi)
{
	long reg = op & 7;
	uint8_t res, val;

	DPRINTF(" \t%s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res >> 1;
	res |= (gbcpu_regs.rn.f & CF) << 3;
	gbcpu_regs.rn.f = (val & 1) << 4;
	if (res == 0) gbcpu_regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_rra(uint32_t op, const struct opinfo *oi)
{
	uint8_t res;

	DPRINTF(" \t%s", oi->name);
	res  = gbcpu_regs.rn.a;
	res  = res >> 1;
	res |= (gbcpu_regs.rn.f & CF) << 3;
	gbcpu_regs.rn.f = (gbcpu_regs.rn.a & 1) << 4;
	gbcpu_regs.rn.a = res;
}

static void op_rrc(uint32_t op, const struct opinfo *oi)
{
	long reg = op & 7;
	uint8_t res, val;

	DPRINTF(" \t%s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res >> 1;
	res |= val << 7;
	gbcpu_regs.rn.f = (val & 1) << 4;
	if (res == 0) gbcpu_regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_rrca(uint32_t op, const struct opinfo *oi)
{
	uint8_t res;

	DPRINTF(" \t%s", oi->name);
	res  = gbcpu_regs.rn.a;
	res  = res >> 1;
	res |= gbcpu_regs.rn.a << 7;
	gbcpu_regs.rn.f = (gbcpu_regs.rn.a & 1) << 4;
	gbcpu_regs.rn.a = res;
}

static void op_sra(uint32_t op, const struct opinfo *oi)
{
	long reg = op & 7;
	uint8_t res, val;

	DPRINTF(" \t%s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res >> 1;
	res |= val & 0x80;
	gbcpu_regs.rn.f = (val & 1) << 4;
	if (res == 0) gbcpu_regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_srl(uint32_t op, const struct opinfo *oi)
{
	long reg = op & 7;
	uint8_t res, val;

	DPRINTF(" \t%s ", oi->name);
	print_reg(reg);
	res  = val = get_reg(reg);
	res  = res >> 1;
	gbcpu_regs.rn.f = (val & 1) << 4;
	if (res == 0) gbcpu_regs.rn.f |= ZF;
	put_reg(reg, res);
}

static void op_swap(uint32_t op, const struct opinfo *oi)
{
	long reg = op & 7;
	uint32_t res;
	uint32_t val;

	DPRINTF(" \t%s ", oi->name);
	print_reg(reg);
	val = get_reg(reg);
	res = (val >> 4) |
	      (val << 4);
	gbcpu_regs.rn.f = 0;
	if (res == 0) gbcpu_regs.rn.f |= ZF;
	put_reg(reg, res);
}

static const struct opinfo cbops[8] = {
	OPINFO("RLC",  &op_rlc,  0, 0),		/* opcode cb00-cb07 */
	OPINFO("RRC",  &op_rrc,  0, 0),		/* opcode cb08-cb0f */
	OPINFO("RL",   &op_rl,   0, 0),		/* opcode cb10-cb17 */
	OPINFO("RR",   &op_rr,   0, 0),		/* opcode cb18-cb1f */
	OPINFO("SLA",  &op_sla,  0, 0),		/* opcode cb20-cb27 */
	OPINFO("SRA",  &op_sra,  0, 0),		/* opcode cb28-cb2f */
	OPINFO("SWAP", &op_swap, 0, 0),		/* opcode cb30-cb37 */
	OPINFO("SRL",  &op_srl,  0, 0),		/* opcode cb38-cb3f */
};

static void op_cbprefix(uint32_t op, const struct opinfo *oi)
{
	uint16_t pc = REGS16_R(gbcpu_regs, PC);

	REGS16_W(gbcpu_regs, PC, pc + 1);
	op = mem_get(pc);
	switch (op >> 6) {
		case 0: cbops[(op >> 3) & 7].fn(op, &cbops[(op >> 3) & 7]);
			return;
		case 1: op_bit(op); return;
		case 2: op_res(op); return;
		case 3: op_set(op); return;
	}
	fprintf(stderr, "\n\nUnknown CB subopcode %02x.\n", (unsigned char)op);
	gbcpu_stopped = 1;
}

static void op_ld(uint32_t op, const struct opinfo *oi)
{
	long src = op & 7;
	long dst = (op >> 3) & 7;

	DPRINTF(" \t%s  ", oi->name);
	print_reg(dst);
	DPRINTF(", ");
	print_reg(src);
	put_reg(dst, get_reg(src));
}

static void op_ld_imm(uint32_t op, const struct opinfo *oi)
{
	long ofs = get_imm16();

	DPRINTF(" \t%s  A, [0x%04lx]", oi->name, ofs);
	gbcpu_regs.rn.a = mem_get(ofs);
}

static void op_ld_ind16_a(uint32_t op, const struct opinfo *oi)
{
	long ofs = get_imm16();

	DPRINTF(" \t%s  [0x%04lx], A", oi->name, ofs);
	mem_put(ofs, gbcpu_regs.rn.a);
}

static void op_ld_ind16_sp(uint32_t op, const struct opinfo *oi)
{
	long ofs = get_imm16();
	long sp = REGS16_R(gbcpu_regs, SP);

	DPRINTF(" \t%s  [0x%04lx], SP", oi->name, ofs);
	mem_put(ofs, sp & 0xff);
	mem_put(ofs+1, sp >> 8);
}

static void op_ld_hlsp(uint32_t op, const struct opinfo *oi)
{
	int8_t ofs = get_imm8();
	uint16_t old = REGS16_R(gbcpu_regs, SP);
	uint16_t new = old + ofs;

	if (ofs>0) DPRINTF(" \t%s  HL, SP+0x%02x", oi->name, ofs);
	else DPRINTF(" \t%s  HL, SP-0x%02x", oi->name, -ofs);
	REGS16_W(gbcpu_regs, HL, new);
	gbcpu_regs.rn.f = 0;
	/* flags are based on LOW-BYTE */
	if ((old & 0xff) > (new & 0xff)) gbcpu_regs.rn.f |= CF;
	if ((old & 0xf) > (new & 0xf)) gbcpu_regs.rn.f |= HF;
	// 4 extra cycles.
	gbcpu_cycles += 4;
}

static void op_ld_sphl(uint32_t op, const struct opinfo *oi)
{
	DPRINTF(" \t%s  SP, HL", oi->name);
	REGS16_W(gbcpu_regs, SP, REGS16_R(gbcpu_regs, HL));
	// 4 extra cycles.
	gbcpu_cycles += 4;
}

static void op_ld_reg16_imm(uint32_t op, const struct opinfo *oi)
{
	long val = get_imm16();
	long reg = (op >> 4) & 3;

	reg += reg > 2; /* skip over AF */
	DPRINTF(" \t%s  %s, 0x%04lx", oi->name, regnamech16[reg], val);
	REGS16_W(gbcpu_regs, reg, val);
}

static void op_ld_reg16_a(uint32_t op, const struct opinfo *oi)
{
	long reg = (op >> 4) & 3;
	uint16_t r;

	reg -= reg > 2; /* for HL LDD LDI opcodes */
	if (op & 8) {
		DPRINTF(" \t%s  A, [%s]", oi->name, regnamech16[reg]);
		gbcpu_regs.rn.a = mem_get(r = REGS16_R(gbcpu_regs, reg));
	} else {
		DPRINTF(" \t%s  [%s], A", oi->name, regnamech16[reg]);
		mem_put(r = REGS16_R(gbcpu_regs, reg), gbcpu_regs.rn.a);
	}

	if (reg == 2) {
		r += (((op & 0x10) == 0) << 1)-1;
		REGS16_W(gbcpu_regs, reg, r);
	}
}

static void op_ld_reg8_imm(uint32_t op, const struct opinfo *oi)
{
	long val = get_imm8();
	long reg = (op >> 3) & 7;

	DPRINTF(" \t%s  ", oi->name);
	print_reg(reg);
	put_reg(reg, val);
	DPRINTF(", 0x%02lx", val);
}

static void op_ldh(uint32_t op, const struct opinfo *oi)
{
	long ofs = op & 2 ? 0 : get_imm8();

	if (op & 0x10) {
		DPRINTF(" \t%s  A, ", oi->name);
		if ((op & 2) == 0) {
			DPRINTF("[%02lx]", ofs);
		} else {
			ofs = gbcpu_regs.rn.c;
			DPRINTF("[C]");
		}
		gbcpu_regs.rn.a = mem_get(0xff00 + ofs);
	} else {
		if ((op & 2) == 0) {
			DPRINTF(" \t%s  [%02lx], A", oi->name, ofs);
		} else {
			ofs = gbcpu_regs.rn.c;
			DPRINTF(" \t%s  [C], A", oi->name);
		}
		mem_put(0xff00 + ofs, gbcpu_regs.rn.a);
	}
}

static void op_inc(uint32_t op, const struct opinfo *oi)
{
	long reg = (op >> 3) & 7;
	uint8_t res;
	uint8_t old;

	DPRINTF(" \t%s ", oi->name);
	print_reg(reg);
	old = res = get_reg(reg);
	res++;
	put_reg(reg, res);
	gbcpu_regs.rn.f &= ~(NF | ZF | HF);
	if (res == 0) gbcpu_regs.rn.f |= ZF;
	if ((old & 15) > (res & 15)) gbcpu_regs.rn.f |= HF;
}

static void op_inc16(uint32_t op, const struct opinfo *oi)
{
	long reg = (op >> 4) & 3;
	uint16_t res;
	reg += reg > 2; /* skip over AF */
	res = REGS16_R(gbcpu_regs, reg);

	DPRINTF(" \t%s %s\t", oi->name, regnamech16[reg]);
	res++;
	REGS16_W(gbcpu_regs, reg, res);
	// 16bit ALU op takes 4 extra cycles.
	gbcpu_cycles += 4;
}

static void op_dec(uint32_t op, const struct opinfo *oi)
{
	long reg = (op >> 3) & 7;
	uint8_t res;
	uint8_t old;

	DPRINTF(" \t%s ", oi->name);
	print_reg(reg);
	old = res = get_reg(reg);
	res--;
	put_reg(reg, res);
	gbcpu_regs.rn.f |= NF;
	gbcpu_regs.rn.f &= ~(ZF | HF);
	if (res == 0) gbcpu_regs.rn.f |= ZF;
	if ((old & 15) < (res & 15)) gbcpu_regs.rn.f |= HF;
}

static void op_dec16(uint32_t op, const struct opinfo *oi)
{
	long reg = (op >> 4) & 3;
	uint16_t res;
	reg += reg > 2; /* skip over AF */
	res = REGS16_R(gbcpu_regs, reg);

	DPRINTF(" \t%s %s", oi->name, regnamech16[reg]);
	res--;
	REGS16_W(gbcpu_regs, reg, res);
	// 16bit ALU op takes 4 extra cycles.
	gbcpu_cycles += 4;
}

static void op_add_sp_imm(uint32_t op, const struct opinfo *oi)
{
	int8_t imm = get_imm8();
	uint16_t old = REGS16_R(gbcpu_regs, SP);
	uint16_t new = old;

	DPRINTF(" \t%s SP, %02x", oi->name, imm);
	new += imm;
	REGS16_W(gbcpu_regs, SP, new);
	gbcpu_regs.rn.f = 0;
	/* flags are based on LOW-BYTE */
	if ((old & 0xff) > (new & 0xff)) gbcpu_regs.rn.f |= CF;
	if ((old & 0xf) > (new & 0xf)) gbcpu_regs.rn.f |= HF;
	// 8 extra cycles.
	gbcpu_cycles += 8;
}

static void op_add(uint32_t op, const struct opinfo *oi)
{
	uint8_t old = gbcpu_regs.rn.a;
	uint8_t new;

	DPRINTF(" \t%s A, ", oi->name);
	print_reg(op & 7);
	gbcpu_regs.rn.a += get_reg(op & 7);
	new = gbcpu_regs.rn.a;
	gbcpu_regs.rn.f = 0;
	if (old > new) gbcpu_regs.rn.f |= CF;
	if ((old & 15) > (new & 15)) gbcpu_regs.rn.f |= HF;
	if (new == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_add_imm(uint32_t op, const struct opinfo *oi)
{
	uint8_t imm = get_imm8();
	uint8_t old = gbcpu_regs.rn.a;
	uint8_t new = old;

	DPRINTF(" \t%s A, $0x%02x", oi->name, imm);
	new += imm;
	gbcpu_regs.rn.a = new;
	gbcpu_regs.rn.f = 0;
	if (old > new) gbcpu_regs.rn.f |= CF;
	if ((old & 15) > (new & 15)) gbcpu_regs.rn.f |= HF;
	if (new == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_add_hl(uint32_t op, const struct opinfo *oi)
{
	long reg = (op >> 4) & 3;
	uint16_t old = REGS16_R(gbcpu_regs, HL);
	uint16_t new = old;

	reg += reg > 2; /* skip over AF */
	DPRINTF(" \t%s HL, %s", oi->name, regnamech16[reg]);

	new += REGS16_R(gbcpu_regs, reg);
	REGS16_W(gbcpu_regs, HL, new);

	gbcpu_regs.rn.f &= ~(NF | CF | HF);
	if (old > new) gbcpu_regs.rn.f |= CF;
	if ((old & 0xfff) > (new & 0xfff)) gbcpu_regs.rn.f |= HF;

	// 4 extra cycles.
	gbcpu_cycles += 4;
}

static void op_adc(uint32_t op, const struct opinfo *oi)
{
	uint8_t reg = get_reg(op & 7);
	uint8_t old = gbcpu_regs.rn.a;
	long new = old;
	long c = (gbcpu_regs.rn.f & CF) > 0;

	DPRINTF(" \t%s A, ", oi->name);
	print_reg(op & 7);
	new += reg;
	new += c;
	gbcpu_regs.rn.f = 0;
	gbcpu_regs.rn.a = new;
	if (new > 0xff) gbcpu_regs.rn.f |= CF;
	if ((old & 15) + (reg & 15) + c > 15) gbcpu_regs.rn.f |= HF;
	if (gbcpu_regs.rn.a == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_adc_imm(uint32_t op, const struct opinfo *oi)
{
	uint8_t imm = get_imm8();
	uint8_t old = gbcpu_regs.rn.a;
	long new = old;
	long c = (gbcpu_regs.rn.f & CF) > 0;

	DPRINTF(" \t%s A, $0x%02x", oi->name, imm);
	new += imm;
	new += c;
	gbcpu_regs.rn.f = 0;
	gbcpu_regs.rn.a = new;
	if (new > 0xff) gbcpu_regs.rn.f |= CF;
	if ((old & 15) + (imm & 15) + c > 15) gbcpu_regs.rn.f |= HF;
	if (gbcpu_regs.rn.a == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_cp(uint32_t op, const struct opinfo *oi)
{
	uint8_t old = gbcpu_regs.rn.a;
	uint8_t new = old;

	DPRINTF(" \t%s A, ", oi->name);
	print_reg(op & 7);
	new -= get_reg(op & 7);
	gbcpu_regs.rn.f = NF;
	if (old < new) gbcpu_regs.rn.f |= CF;
	if ((old & 15) < (new & 15)) gbcpu_regs.rn.f |= HF;
	if (new == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_cp_imm(uint32_t op, const struct opinfo *oi)
{
	uint8_t imm = get_imm8();
	uint8_t old = gbcpu_regs.rn.a;
	uint8_t new = old;

	DPRINTF(" \t%s A, $0x%02x", oi->name, imm);
	new -= imm;
	gbcpu_regs.rn.f = NF;
	if (old < new) gbcpu_regs.rn.f |= CF;
	if ((old & 15) < (new & 15)) gbcpu_regs.rn.f |= HF;
	if (new == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_sub(uint32_t op, const struct opinfo *oi)
{
	uint8_t old = gbcpu_regs.rn.a;
	uint8_t new;

	DPRINTF(" \t%s A, ", oi->name);
	print_reg(op & 7);
	gbcpu_regs.rn.a -= get_reg(op & 7);
	new = gbcpu_regs.rn.a;
	gbcpu_regs.rn.f = NF;
	if (old < new) gbcpu_regs.rn.f |= CF;
	if ((old & 15) < (new & 15)) gbcpu_regs.rn.f |= HF;
	if (new == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_sub_imm(uint32_t op, const struct opinfo *oi)
{
	uint8_t imm = get_imm8();
	uint8_t old = gbcpu_regs.rn.a;
	uint8_t new = old;

	DPRINTF(" \t%s A, $0x%02x", oi->name, imm);
	new -= imm;
	gbcpu_regs.rn.a = new;
	gbcpu_regs.rn.f = NF;
	if (old < new) gbcpu_regs.rn.f |= CF;
	if ((old & 15) < (new & 15)) gbcpu_regs.rn.f |= HF;
	if (new == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_sbc(uint32_t op, const struct opinfo *oi)
{
	uint8_t reg = get_reg(op & 7);
	uint8_t old = gbcpu_regs.rn.a;
	long new = old + 0x100;
	long c = (gbcpu_regs.rn.f & CF) > 0;

	DPRINTF(" \t%s A, ", oi->name);
	print_reg(op & 7);
	new -= reg;
	new -= c;
	gbcpu_regs.rn.a = new;
	gbcpu_regs.rn.f = NF;
	if (new < 0x100) gbcpu_regs.rn.f |= CF;
	if ((old & 15) - (reg & 15) - c < 0) gbcpu_regs.rn.f |= HF;
	if (gbcpu_regs.rn.a == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_sbc_imm(uint32_t op, const struct opinfo *oi)
{
	uint8_t imm = get_imm8();
	uint8_t old = gbcpu_regs.rn.a;
	long new = old + 0x100;
	long c = (gbcpu_regs.rn.f & CF) > 0;

	DPRINTF(" \t%s A, $0x%02x", oi->name, imm);
	new -= imm;
	new -= c;
	gbcpu_regs.rn.a = new;
	gbcpu_regs.rn.f = NF;
	if (new < 0x100) gbcpu_regs.rn.f |= CF;
	if ((old & 15) - (imm & 15) - c < 0) gbcpu_regs.rn.f |= HF;
	if (gbcpu_regs.rn.a == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_and(uint32_t op, const struct opinfo *oi)
{
	DPRINTF(" \t%s A, ", oi->name);
	print_reg(op & 7);
	gbcpu_regs.rn.a &= get_reg(op & 7);
	gbcpu_regs.rn.f = HF;
	if (gbcpu_regs.rn.a == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_and_imm(uint32_t op, const struct opinfo *oi)
{
	uint8_t imm = get_imm8();

	DPRINTF(" \t%s A, $0x%02x", oi->name, imm);
	gbcpu_regs.rn.a &= imm;
	gbcpu_regs.rn.f = HF;
	if (gbcpu_regs.rn.a == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_or(uint32_t op, const struct opinfo *oi)
{
	DPRINTF(" \t%s A, ", oi->name);
	print_reg(op & 7);
	gbcpu_regs.rn.a |= get_reg(op & 7);
	gbcpu_regs.rn.f = 0;
	if (gbcpu_regs.rn.a == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_or_imm(uint32_t op, const struct opinfo *oi)
{
	uint8_t imm = get_imm8();

	DPRINTF(" \t%s A, $0x%02x", oi->name, imm);
	gbcpu_regs.rn.a |= imm;
	gbcpu_regs.rn.f = 0;
	if (gbcpu_regs.rn.a == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_xor(uint32_t op, const struct opinfo *oi)
{
	DPRINTF(" \t%s A, ", oi->name);
	print_reg(op & 7);
	gbcpu_regs.rn.a ^= get_reg(op & 7);
	gbcpu_regs.rn.f = 0;
	if (gbcpu_regs.rn.a == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_xor_imm(uint32_t op, const struct opinfo *oi)
{
	uint8_t imm = get_imm8();

	DPRINTF(" \t%s A, $0x%02x", oi->name, imm);
	gbcpu_regs.rn.a ^= imm;
	gbcpu_regs.rn.f = 0;
	if (gbcpu_regs.rn.a == 0) gbcpu_regs.rn.f |= ZF;
}

static void op_push(uint32_t op, const struct opinfo *oi)
{
	long reg = op >> 4 & 3;

	push(REGS16_R(gbcpu_regs, reg));
	// 4 extra cycles.
	gbcpu_cycles += 4;
	DPRINTF(" \t%s %s\t", oi->name, regnamech16[reg]);
}

static void op_push_af(uint32_t op, const struct opinfo *oi)
{
	uint16_t tmp = gbcpu_regs.rn.a << 8;

	tmp |= gbcpu_regs.rn.f;
	push(tmp);
	// 4 extra cycles.
	gbcpu_cycles += 4;
	DPRINTF(" \t%s %s\t", oi->name, regnamech16[op >> 4 & 3]);
}

static void op_pop(uint32_t op, const struct opinfo *oi)
{
	long reg = op >> 4 & 3;

	REGS16_W(gbcpu_regs, reg, pop());
	DPRINTF(" \t%s %s\t", oi->name, regnamech16[reg]);
}

static void op_pop_af(uint32_t op, const struct opinfo *oi)
{
	uint16_t tmp = pop();

	gbcpu_regs.rn.f = tmp & 0xf0;
	gbcpu_regs.rn.a = tmp >> 8;
	DPRINTF(" \t%s %s\t", oi->name, regnamech16[op >> 4 & 3]);
}

static void op_cpl(uint32_t op, const struct opinfo *oi)
{
	DPRINTF(" \t%s", oi->name);
	gbcpu_regs.rn.a = ~gbcpu_regs.rn.a;
	gbcpu_regs.rn.f |= NF | HF;
}

static void op_ccf(uint32_t op, const struct opinfo *oi)
{
	DPRINTF(" \t%s", oi->name);
	gbcpu_regs.rn.f ^= CF;
	gbcpu_regs.rn.f &= ~(NF | HF);
}

static void op_scf(uint32_t op, const struct opinfo *oi)
{
	DPRINTF(" \t%s", oi->name);
	gbcpu_regs.rn.f |= CF;
	gbcpu_regs.rn.f &= ~(NF | HF);
}

static void op_call(uint32_t op, const struct opinfo *oi)
{
	uint16_t ofs = get_imm16();

	DPRINTF(" \t%s 0x%04x", oi->name, ofs);
	push(REGS16_R(gbcpu_regs, PC));
	REGS16_W(gbcpu_regs, PC, ofs);
	// 4 extra cycles.
	gbcpu_cycles += 4;
}

static void op_call_cond(uint32_t op, const struct opinfo *oi)
{
	uint16_t ofs = get_imm16();
	long cond = (op >> 3) & 3;

	DPRINTF(" \t%s %s 0x%04x", oi->name, conds[cond], ofs);
	switch (cond) {
		case 0: if ((gbcpu_regs.rn.f & ZF) != 0) return; break;
		case 1: if ((gbcpu_regs.rn.f & ZF) == 0) return; break;
		case 2: if ((gbcpu_regs.rn.f & CF) != 0) return; break;
		case 3: if ((gbcpu_regs.rn.f & CF) == 0) return; break;
	}
	// A taken call is 4 extra cycles.
	gbcpu_cycles += 4;
	push(REGS16_R(gbcpu_regs, PC));
	REGS16_W(gbcpu_regs, PC, ofs);
}

static void op_ret(uint32_t op, const struct opinfo *oi)
{
	REGS16_W(gbcpu_regs, PC, pop());
	// 4 extra cycles.
	gbcpu_cycles += 4;
	DPRINTF(" \t%s", oi->name);
}

static void op_reti(uint32_t op, const struct opinfo *oi)
{
	gbcpu_if = 1;
	REGS16_W(gbcpu_regs, PC, pop());
	DPRINTF(" \t%s", oi->name);
	// 4 extra cycles.
	gbcpu_cycles += 4;
}

static void op_ret_cond(uint32_t op, const struct opinfo *oi)
{
	long cond = (op >> 3) & 3;

	// 4 extra cycles.
	gbcpu_cycles += 4;
	DPRINTF(" \t%s %s", oi->name, conds[cond]);
	switch (cond) {
		case 0: if ((gbcpu_regs.rn.f & ZF) != 0) return; break;
		case 1: if ((gbcpu_regs.rn.f & ZF) == 0) return; break;
		case 2: if ((gbcpu_regs.rn.f & CF) != 0) return; break;
		case 3: if ((gbcpu_regs.rn.f & CF) == 0) return; break;
	}
	// 4 extra cycles.
	gbcpu_cycles += 4;
	REGS16_W(gbcpu_regs, PC, pop());
}

static void op_halt(uint32_t op, const struct opinfo *oi)
{
	gbcpu_halted = 1;
	DPRINTF(" \t%s", oi->name);
}

static void op_stop(uint32_t op, const struct opinfo *oi)
{
	DPRINTF(" \t%s", oi->name);
}

static void op_di(uint32_t op, const struct opinfo *oi)
{
	gbcpu_if = 0;
	DPRINTF(" \t%s", oi->name);
}

static void op_ei(uint32_t op, const struct opinfo *oi)
{
	gbcpu_if = 1;
	DPRINTF(" \t%s", oi->name);
}

static void op_jr(uint32_t op, const struct opinfo *oi)
{
	int16_t ofs = (int8_t) get_imm8();

	if (ofs == -2 && gbcpu_if == 0) {
		gbcpu_halted = 1;
	}

	if (ofs < 0) DPRINTF(" \t%s $-0x%02x", oi->name, -ofs);
	else DPRINTF(" \t%s $+0x%02x", oi->name, ofs);
	// 4 extra cycles.
	gbcpu_cycles += 4;
	REGS16_W(gbcpu_regs, PC, REGS16_R(gbcpu_regs, PC) + ofs);
}

static void op_jr_cond(uint32_t op, const struct opinfo *oi)
{
	int16_t ofs = (int8_t) get_imm8();
	long cond = (op >> 3) & 3;

	if (ofs < 0) DPRINTF(" \t%s %s $-0x%02x", oi->name, conds[cond], -ofs);
	else DPRINTF(" \t%s %s $+0x%02x", oi->name, conds[cond], ofs);
	switch (cond) {
		case 0: if ((gbcpu_regs.rn.f & ZF) != 0) return; break;
		case 1: if ((gbcpu_regs.rn.f & ZF) == 0) return; break;
		case 2: if ((gbcpu_regs.rn.f & CF) != 0) return; break;
		case 3: if ((gbcpu_regs.rn.f & CF) == 0) return; break;
	}
	// A taken jump is 4 extra cycles.
	gbcpu_cycles += 4;
	REGS16_W(gbcpu_regs, PC, REGS16_R(gbcpu_regs, PC) + ofs);
}

static void op_jp(uint32_t op, const struct opinfo *oi)
{
	uint16_t ofs = get_imm16();

	DPRINTF(" \t%s 0x%04x", oi->name, ofs);
	// 4 extra cycles.
	gbcpu_cycles += 4;
	REGS16_W(gbcpu_regs, PC, ofs);
}

static void op_jp_hl(uint32_t op, const struct opinfo *oi)
{
	DPRINTF(" \t%s HL", oi->name);
	REGS16_W(gbcpu_regs, PC, REGS16_R(gbcpu_regs, HL));
}

static void op_jp_cond(uint32_t op, const struct opinfo *oi)
{
	uint16_t ofs = get_imm16();
	long cond = (op >> 3) & 3;

	DPRINTF(" \t%s %s 0x%04x", oi->name, conds[cond], ofs);
	switch (cond) {
		case 0: if ((gbcpu_regs.rn.f & ZF) != 0) return; break;
		case 1: if ((gbcpu_regs.rn.f & ZF) == 0) return; break;
		case 2: if ((gbcpu_regs.rn.f & CF) != 0) return; break;
		case 3: if ((gbcpu_regs.rn.f & CF) == 0) return; break;
	}
	// A taken jump is 4 extra cycles.
	gbcpu_cycles += 4;
	REGS16_W(gbcpu_regs, PC, ofs);
}

static void op_rst(uint32_t op, const struct opinfo *oi)
{
	int16_t ofs = op & 0x38;

	DPRINTF(" \t%s 0x%02x", oi->name, ofs);
	push(REGS16_R(gbcpu_regs, PC));
	REGS16_W(gbcpu_regs, PC, ofs);
	// 4 extra cycles.
	gbcpu_cycles += 4;
}

static void op_nop(uint32_t op, const struct opinfo *oi)
{
	DPRINTF(" \t%s", oi->name);
}

static void op_daa(uint32_t op, const struct opinfo *oi)
{
	long a = gbcpu_regs.rn.a;
	long f = gbcpu_regs.rn.f;

	if (f & NF) {
		if (f & HF) {
			a -= 0x06;
			a &= 0xff;
		}
		if (f & CF)
			a -= 0x60;
	} else {
		if (f & HF || (a & 0xf) > 9)
			a += 0x06;
		if (f & CF || a > 0x9f)
			a += 0x60;
	}
	f &= ~(HF | ZF);

	if (a > 0xff)
		f |= CF;
	a &= 0xff;
	if (a == 0)
		f |= ZF;

	gbcpu_regs.rn.a = (uint8_t)a;
	gbcpu_regs.rn.f = (uint8_t)f;
	DPRINTF(" \t%s", oi->name);
}

static const struct opinfo ops[256] = {
	OPINFO("NOP",  &op_nop         , 1, 1),		/* opcode 00 */
	OPINFO("LD",   &op_ld_reg16_imm, 3, 3),		/* opcode 01 */
	OPINFO("LD",   &op_ld_reg16_a  , 2, 2),		/* opcode 02 */
	OPINFO("INC",  &op_inc16       , 2, 2),		/* opcode 03 */
	OPINFO("INC",  &op_inc         , 1, 1),		/* opcode 04 */
	OPINFO("DEC",  &op_dec         , 1, 1),		/* opcode 05 */
	OPINFO("LD",   &op_ld_reg8_imm , 2, 2),		/* opcode 06 */
	OPINFO("RLCA", &op_rlca        , 1, 1),		/* opcode 07 */
	OPINFO("LD",   &op_ld_ind16_sp , 5, 5),		/* opcode 08 */
	OPINFO("ADD",  &op_add_hl      , 2, 2),		/* opcode 09 */
	OPINFO("LD",   &op_ld_reg16_a  , 2, 2),		/* opcode 0a */
	OPINFO("DEC",  &op_dec16       , 2, 2),		/* opcode 0b */
	OPINFO("INC",  &op_inc         , 1, 1),		/* opcode 0c */
	OPINFO("DEC",  &op_dec         , 1, 1),		/* opcode 0d */
	OPINFO("LD",   &op_ld_reg8_imm , 2, 2),		/* opcode 0e */
	OPINFO("RRCA", &op_rrca        , 1, 1),		/* opcode 0f */
	OPINFO("STOP", &op_stop        , 0, 0),		/* opcode 10 */
	OPINFO("LD",   &op_ld_reg16_imm, 3, 3),		/* opcode 11 */
	OPINFO("LD",   &op_ld_reg16_a  , 2, 2),		/* opcode 12 */
	OPINFO("INC",  &op_inc16       , 2, 2),		/* opcode 13 */
	OPINFO("INC",  &op_inc         , 1, 1),		/* opcode 14 */
	OPINFO("DEC",  &op_dec         , 1, 1),		/* opcode 15 */
	OPINFO("LD",   &op_ld_reg8_imm , 2, 2),		/* opcode 16 */
	OPINFO("RLA",  &op_rla         , 1, 1),		/* opcode 17 */
	OPINFO("JR",   &op_jr          , 3, 3),		/* opcode 18 */
	OPINFO("ADD",  &op_add_hl      , 2, 2),		/* opcode 19 */
	OPINFO("LD",   &op_ld_reg16_a  , 2, 2),		/* opcode 1a */
	OPINFO("DEC",  &op_dec16       , 2, 2),		/* opcode 1b */
	OPINFO("INC",  &op_inc         , 1, 1),		/* opcode 1c */
	OPINFO("DEC",  &op_dec         , 1, 1),		/* opcode 1d */
	OPINFO("LD",   &op_ld_reg8_imm , 2, 2),		/* opcode 1e */
	OPINFO("RRA",  &op_rra         , 1, 1),		/* opcode 1f */
	OPINFO("JR",   &op_jr_cond     , 2, 3),		/* opcode 20 */
	OPINFO("LD",   &op_ld_reg16_imm, 3, 3),		/* opcode 21 */
	OPINFO("LDI",  &op_ld_reg16_a  , 2, 2),		/* opcode 22 */
	OPINFO("INC",  &op_inc16       , 2, 2),		/* opcode 23 */
	OPINFO("INC",  &op_inc         , 1, 1),		/* opcode 24 */
	OPINFO("DEC",  &op_dec         , 1, 1),		/* opcode 25 */
	OPINFO("LD",   &op_ld_reg8_imm , 2, 2),		/* opcode 26 */
	OPINFO("DAA",  &op_daa         , 1, 1),		/* opcode 27 */
	OPINFO("JR",   &op_jr_cond     , 2, 3),		/* opcode 28 */
	OPINFO("ADD",  &op_add_hl      , 2, 2),		/* opcode 29 */
	OPINFO("LDI",  &op_ld_reg16_a  , 2, 2),		/* opcode 2a */
	OPINFO("DEC",  &op_dec16       , 2, 2),		/* opcode 2b */
	OPINFO("INC",  &op_inc         , 1, 1),		/* opcode 2c */
	OPINFO("DEC",  &op_dec         , 1, 1),		/* opcode 2d */
	OPINFO("LD",   &op_ld_reg8_imm , 2, 2),		/* opcode 2e */
	OPINFO("CPL",  &op_cpl         , 1, 1),		/* opcode 2f */
	OPINFO("JR",   &op_jr_cond     , 2, 3),		/* opcode 30 */
	OPINFO("LD",   &op_ld_reg16_imm, 3, 3),		/* opcode 31 */
	OPINFO("LDD",  &op_ld_reg16_a  , 2, 2),		/* opcode 32 */
	OPINFO("INC",  &op_inc16       , 2, 2),		/* opcode 33 */
	OPINFO("INC",  &op_inc         , 3, 3),		/* opcode 34 */
	OPINFO("DEC",  &op_dec         , 3, 3),		/* opcode 35 */
	OPINFO("LD",   &op_ld_reg8_imm , 3, 3),		/* opcode 36 */
	OPINFO("SCF",  &op_scf         , 1, 1),		/* opcode 37 */
	OPINFO("JR",   &op_jr_cond     , 2, 3),		/* opcode 38 */
	OPINFO("ADD",  &op_add_hl      , 2, 2),		/* opcode 39 */
	OPINFO("LDD",  &op_ld_reg16_a  , 2, 2),		/* opcode 3a */
	OPINFO("DEC",  &op_dec16       , 2, 2),		/* opcode 3b */
	OPINFO("INC",  &op_inc         , 1, 1),		/* opcode 3c */
	OPINFO("DEC",  &op_dec         , 1, 1),		/* opcode 3d */
	OPINFO("LD",   &op_ld_reg8_imm , 2, 2),		/* opcode 3e */
	OPINFO("CCF",  &op_ccf         , 1, 1),		/* opcode 3f */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 40 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 41 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 42 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 43 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 44 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 45 */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 46 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 47 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 48 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 49 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 4a */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 4b */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 4c */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 4d */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 4e */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 4f */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 50 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 51 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 52 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 53 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 54 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 55 */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 56 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 57 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 58 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 59 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 5a */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 5b */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 5c */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 5d */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 5e */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 5f */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 60 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 61 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 62 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 63 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 64 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 65 */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 66 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 67 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 68 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 69 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 6a */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 6b */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 6c */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 6d */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 6e */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 6f */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 70 */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 71 */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 72 */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 73 */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 74 */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 75 */
	OPINFO("HALT", &op_halt        , 0, 0),		/* opcode 76 */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 77 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 78 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 79 */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 7a */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 7b */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 7c */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 7d */
	OPINFO("LD",   &op_ld          , 2, 2),		/* opcode 7e */
	OPINFO("LD",   &op_ld          , 1, 1),		/* opcode 7f */
	OPINFO("ADD",  &op_add         , 1, 1),		/* opcode 80 */
	OPINFO("ADD",  &op_add         , 1, 1),		/* opcode 81 */
	OPINFO("ADD",  &op_add         , 1, 1),		/* opcode 82 */
	OPINFO("ADD",  &op_add         , 1, 1),		/* opcode 83 */
	OPINFO("ADD",  &op_add         , 1, 1),		/* opcode 84 */
	OPINFO("ADD",  &op_add         , 1, 1),		/* opcode 85 */
	OPINFO("ADD",  &op_add         , 2, 2),		/* opcode 86 */
	OPINFO("ADD",  &op_add         , 1, 1),		/* opcode 87 */
	OPINFO("ADC",  &op_adc         , 1, 1),		/* opcode 88 */
	OPINFO("ADC",  &op_adc         , 1, 1),		/* opcode 89 */
	OPINFO("ADC",  &op_adc         , 1, 1),		/* opcode 8a */
	OPINFO("ADC",  &op_adc         , 1, 1),		/* opcode 8b */
	OPINFO("ADC",  &op_adc         , 1, 1),		/* opcode 8c */
	OPINFO("ADC",  &op_adc         , 1, 1),		/* opcode 8d */
	OPINFO("ADC",  &op_adc         , 2, 2),		/* opcode 8e */
	OPINFO("ADC",  &op_adc         , 1, 1),		/* opcode 8f */
	OPINFO("SUB",  &op_sub         , 1, 1),		/* opcode 90 */
	OPINFO("SUB",  &op_sub         , 1, 1),		/* opcode 91 */
	OPINFO("SUB",  &op_sub         , 1, 1),		/* opcode 92 */
	OPINFO("SUB",  &op_sub         , 1, 1),		/* opcode 93 */
	OPINFO("SUB",  &op_sub         , 1, 1),		/* opcode 94 */
	OPINFO("SUB",  &op_sub         , 1, 1),		/* opcode 95 */
	OPINFO("SUB",  &op_sub         , 2, 2),		/* opcode 96 */
	OPINFO("SUB",  &op_sub         , 1, 1),		/* opcode 97 */
	OPINFO("SBC",  &op_sbc         , 1, 1),		/* opcode 98 */
	OPINFO("SBC",  &op_sbc         , 1, 1),		/* opcode 99 */
	OPINFO("SBC",  &op_sbc         , 1, 1),		/* opcode 9a */
	OPINFO("SBC",  &op_sbc         , 1, 1),		/* opcode 9b */
	OPINFO("SBC",  &op_sbc         , 1, 1),		/* opcode 9c */
	OPINFO("SBC",  &op_sbc         , 1, 1),		/* opcode 9d */
	OPINFO("SBC",  &op_sbc         , 2, 2),		/* opcode 9e */
	OPINFO("SBC",  &op_sbc         , 1, 1),		/* opcode 9f */
	OPINFO("AND",  &op_and         , 1, 1),		/* opcode a0 */
	OPINFO("AND",  &op_and         , 1, 1),		/* opcode a1 */
	OPINFO("AND",  &op_and         , 1, 1),		/* opcode a2 */
	OPINFO("AND",  &op_and         , 1, 1),		/* opcode a3 */
	OPINFO("AND",  &op_and         , 1, 1),		/* opcode a4 */
	OPINFO("AND",  &op_and         , 1, 1),		/* opcode a5 */
	OPINFO("AND",  &op_and         , 2, 2),		/* opcode a6 */
	OPINFO("AND",  &op_and         , 1, 1),		/* opcode a7 */
	OPINFO("XOR",  &op_xor         , 1, 1),		/* opcode a8 */
	OPINFO("XOR",  &op_xor         , 1, 1),		/* opcode a9 */
	OPINFO("XOR",  &op_xor         , 1, 1),		/* opcode aa */
	OPINFO("XOR",  &op_xor         , 1, 1),		/* opcode ab */
	OPINFO("XOR",  &op_xor         , 1, 1),		/* opcode ac */
	OPINFO("XOR",  &op_xor         , 1, 1),		/* opcode ad */
	OPINFO("XOR",  &op_xor         , 2, 2),		/* opcode ae */
	OPINFO("XOR",  &op_xor         , 1, 1),		/* opcode af */
	OPINFO("OR",   &op_or          , 1, 1),		/* opcode b0 */
	OPINFO("OR",   &op_or          , 1, 1),		/* opcode b1 */
	OPINFO("OR",   &op_or          , 1, 1),		/* opcode b2 */
	OPINFO("OR",   &op_or          , 1, 1),		/* opcode b3 */
	OPINFO("OR",   &op_or          , 1, 1),		/* opcode b4 */
	OPINFO("OR",   &op_or          , 1, 1),		/* opcode b5 */
	OPINFO("OR",   &op_or          , 2, 2),		/* opcode b6 */
	OPINFO("OR",   &op_or          , 1, 1),		/* opcode b7 */
	OPINFO("CP",   &op_cp          , 1, 1),		/* opcode b8 */
	OPINFO("CP",   &op_cp          , 1, 1),		/* opcode b9 */
	OPINFO("CP",   &op_cp          , 1, 1),		/* opcode ba */
	OPINFO("CP",   &op_cp          , 1, 1),		/* opcode bb */
	OPINFO("CP",   &op_cp          , 1, 1),		/* opcode bc */
	OPINFO("CP",   &op_cp          , 1, 1),		/* opcode bd */
	OPINFO("CP",   &op_cp          , 2, 2),		/* opcode be */
	OPINFO("CP",   &op_cp          , 1, 1),		/* opcode bf */
	OPINFO("RET",  &op_ret_cond    , 2, 5),		/* opcode c0 */
	OPINFO("POP",  &op_pop         , 3, 3),		/* opcode c1 */
	OPINFO("JP",   &op_jp_cond     , 3, 4),		/* opcode c2 */
	OPINFO("JP",   &op_jp          , 4, 4),		/* opcode c3 */
	OPINFO("CALL", &op_call_cond   , 3, 6),		/* opcode c4 */
	OPINFO("PUSH", &op_push        , 4, 4),		/* opcode c5 */
	OPINFO("ADD",  &op_add_imm     , 2, 2),		/* opcode c6 */
	OPINFO("RST",  &op_rst         , 4, 4),		/* opcode c7 */
	OPINFO("RET",  &op_ret_cond    , 2, 5),		/* opcode c8 */
	OPINFO("RET",  &op_ret         , 4, 4),		/* opcode c9 */
	OPINFO("JP",   &op_jp_cond     , 3, 4),		/* opcode ca */
	OPINFO("CBPREFIX", &op_cbprefix, 0, 0),		/* opcode cb */
	OPINFO("CALL", &op_call_cond   , 3, 6),		/* opcode cc */
	OPINFO("CALL", &op_call        , 6, 6),		/* opcode cd */
	OPINFO("ADC",  &op_adc_imm     , 2, 2),		/* opcode ce */
	OPINFO("RST",  &op_rst         , 4, 4),		/* opcode cf */
	OPINFO("RET",  &op_ret_cond    , 2, 5),		/* opcode d0 */
	OPINFO("POP",  &op_pop         , 3, 3),		/* opcode d1 */
	OPINFO("JP",   &op_jp_cond     , 3, 4),		/* opcode d2 */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode d3 */
	OPINFO("CALL", &op_call_cond   , 3, 6),		/* opcode d4 */
	OPINFO("PUSH", &op_push        , 4, 4),		/* opcode d5 */
	OPINFO("SUB",  &op_sub_imm     , 2, 2),		/* opcode d6 */
	OPINFO("RST",  &op_rst         , 4, 4),		/* opcode d7 */
	OPINFO("RET",  &op_ret_cond    , 2, 5),		/* opcode d8 */
	OPINFO("RETI", &op_reti        , 4, 4),		/* opcode d9 */
	OPINFO("JP",   &op_jp_cond     , 3, 4),		/* opcode da */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode db */
	OPINFO("CALL", &op_call_cond   , 3, 6),		/* opcode dc */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode dd */
	OPINFO("SBC",  &op_sbc_imm     , 2, 2),		/* opcode de */
	OPINFO("RST",  &op_rst         , 4, 4),		/* opcode df */
	OPINFO("LDH",  &op_ldh         , 3, 3),		/* opcode e0 */
	OPINFO("POP",  &op_pop         , 3, 3),		/* opcode e1 */
	OPINFO("LDH",  &op_ldh         , 2, 2),		/* opcode e2 */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode e3 */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode e4 */
	OPINFO("PUSH", &op_push        , 4, 4),		/* opcode e5 */
	OPINFO("AND",  &op_and_imm     , 2, 2),		/* opcode e6 */
	OPINFO("RST",  &op_rst         , 4, 4),		/* opcode e7 */
	OPINFO("ADD",  &op_add_sp_imm  , 4, 4),		/* opcode e8 */
	OPINFO("JP",   &op_jp_hl       , 1, 1),		/* opcode e9 */
	OPINFO("LD",   &op_ld_ind16_a  , 4, 4),		/* opcode ea */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode eb */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode ec */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode ed */
	OPINFO("XOR",  &op_xor_imm     , 2, 2),		/* opcode ee */
	OPINFO("RST",  &op_rst         , 4, 4),		/* opcode ef */
	OPINFO("LDH",  &op_ldh         , 3, 3),		/* opcode f0 */
	OPINFO("POP",  &op_pop_af      , 3, 3),		/* opcode f1 */
	OPINFO("LDH",  &op_ldh         , 2, 2),		/* opcode f2 */
	OPINFO("DI",   &op_di          , 1, 1),		/* opcode f3 */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode f4 */
	OPINFO("PUSH", &op_push_af     , 4, 4),		/* opcode f5 */
	OPINFO("OR",   &op_or_imm      , 2, 2),		/* opcode f6 */
	OPINFO("RST",  &op_rst         , 4, 4),		/* opcode f7 */
	OPINFO("LD",   &op_ld_hlsp     , 3, 3),		/* opcode f8 */
	OPINFO("LD",   &op_ld_sphl     , 2, 2),		/* opcode f9 */
	OPINFO("LD",   &op_ld_imm      , 4, 4),		/* opcode fa */
	OPINFO("EI",   &op_ei          , 1, 1),		/* opcode fb */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode fc */
	OPINFO("UNKN", &op_unknown     , 0, 0),		/* opcode fd */
	OPINFO("CP", &op_cp_imm        , 2, 2),		/* opcode fe */
	OPINFO("RST", &op_rst          , 4, 4),		/* opcode ff */
};

#if DEBUG == 1
static gbcpu_regs_u oldregs;

static void dump_regs(void)
{
	long i;

	DPRINTF("; ");
	for (i=0; i<8; i++) {
		DPRINTF("%c=%02x ", regnames[i], REGS8_R(gbcpu_regs, i));
	}
	for (i=5; i<6; i++) {
		DPRINTF("%s=%04x ", regnamech16[i], REGS16_R(gbcpu_regs, i));
	}
	DPRINTF("\n");
	oldregs = gbcpu_regs;
}

static void show_reg_diffs(const struct opinfo *oi)
{
	long i;

	DPRINTF("\t\t; ");
	for (i=0; i<3; i++) {
		if (REGS16_R(gbcpu_regs, i) != REGS16_R(oldregs, i)) {
			DPRINTF("%s=%04x ", regnamech16[i], REGS16_R(gbcpu_regs, i));
			REGS16_W(oldregs, i, REGS16_R(gbcpu_regs, i));
		}
	}
	for (i=6; i<8; i++) {
		if (REGS8_R(gbcpu_regs, i) != REGS8_R(oldregs, i)) {
			if (i == 6) { /* Flags */
				if (gbcpu_regs.rn.f & ZF) DPRINTF("Z");
				else DPRINTF("z");
				if (gbcpu_regs.rn.f & NF) DPRINTF("N");
				else DPRINTF("n");
				if (gbcpu_regs.rn.f & HF) DPRINTF("H");
				else DPRINTF("h");
				if (gbcpu_regs.rn.f & CF) DPRINTF("C");
				else DPRINTF("c");
				DPRINTF(" ");
			} else {
				DPRINTF("%c=%02x ", regnames[i], REGS8_R(gbcpu_regs,i));
			}
			REGS8_W(oldregs, i, REGS8_R(gbcpu_regs, i));
		}
	}
	for (i=4; i<5; i++) {
		if (REGS16_R(gbcpu_regs, i) != REGS16_R(oldregs, i)) {
			DPRINTF("%s=%04x ", regnamech16[i], REGS16_R(gbcpu_regs, i));
			REGS16_W(oldregs, i, REGS16_R(gbcpu_regs, i));
		}
	}
	DPRINTF(" %ld cycles", gbcpu_cycles);
	if (!CYCLES_OK(oi, gbcpu_cycles/4)) {
		DPRINTF(", but should be %d or %d!\n", 4*CYCLES1(oi), 4*CYCLES2(oi));
	}
	DPRINTF("\n");
}
#endif

void gbcpu_addmem(uint32_t start, uint32_t end, gbcpu_put_fn putfn, gbcpu_get_fn getfn)
{
	uint32_t i;

	for (i=start; i<=end; i++) {
		putlookup[i] = putfn;
		getlookup[i] = getfn;
	}
}

void gbcpu_init(void)
{
	memset(&gbcpu_regs, 0, sizeof(gbcpu_regs));
	gbcpu_halted = 0;
	gbcpu_stopped = 0;
	gbcpu_if = 0;
	gbcpu_halt_at_pc = -1;
	DEB(dump_regs());
}

void gbcpu_intr(long vec)
{
	DPRINTF("gbcpu_intr(%04lx)\n", vec);
	gbcpu_halted = 0;
	gbcpu_if = 0;
	push(REGS16_R(gbcpu_regs, PC));
	REGS16_W(gbcpu_regs, PC, vec);
}

long gbcpu_step(void)
{
	uint8_t op;

	if (!gbcpu_halted) {
		op = mem_get(gbcpu_regs.rn.pc++);
		gbcpu_cycles = 4;
		DPRINTF("%04x: %02x", gbcpu_regs.rn.pc - 1, op);
		ops[op].fn(op, &ops[op]);

		DEB(show_reg_diffs(&ops[op]));

		if (gbcpu_halt_at_pc != -1 &&
		    REGS16_R(gbcpu_regs, PC) == gbcpu_halt_at_pc) {
			DPRINTF("halted at PC %04lx\n", gbcpu_halt_at_pc);
			gbcpu_halted = 1;
			gbcpu_if = 1;
		}
		return gbcpu_cycles;
	}
	if (gbcpu_stopped) return -1;
	return 16;
}
