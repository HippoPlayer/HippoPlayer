/*

  portable code to decrunch XPK-SQSH files
  written from "xpksqsh.library 1.10 (6.3.94)"

  (24.12.97) jah@fh-zwickau.de
  
  tested with sas6.51(m68k) gcc2.7.2(i386) gcc2.7.2(sparc)

  modified a bit by David Le Corfec
*/

#include "unsqsh.h"

static int bfextu(uint8_t *p,int bo,int bc);
static int bfexts(uint8_t *p,int bo,int bc);

/* Recognize and return uncrunched length of the given buffer
 * Return 0 if not squashed.
 */

uint32_t tfmx_sqsh_get_ulen (uint8_t *buf, uint32_t buflen)
{
  uint32_t dstlen = 0; /* decrunched length of buffer */

  if (buflen < 32)
      return 0;

  if (('X' != buf[0]) ||
      ('P' != buf[1]) ||
      ('K' != buf[2]) ||
      ('F' != buf[3]))
    return dstlen;

  if (('S' != buf[8]) ||
      ('Q' != buf[9]) ||
      ('S' != buf[10]) ||
      ('H' != buf[11]))
    return dstlen;

  dstlen  = buf[12]; dstlen <<= 8;
  dstlen |= buf[13]; dstlen <<= 8;
  dstlen |= buf[14]; dstlen <<= 8; 
  dstlen |= buf[15];

  return dstlen;
}

/*
;    0	LONG	"XPKF"
;    4	LONG	file size - 8
;    8	LONG	"SQSH"
;   12	LONG	unpacked size
;  $10	QUAD	original file header
;  $20	WORD
;  $22	WORD
;  $24
;
;    0	WORD	ratio?
;    2	WORD	?
;    4	WORD	packed chunk size starting from offset 6, long aligned
;    6	WORD	unpacked chunk size
;    8	WORD	unpacked chunk size
;   10	LABEL	bitstream...
*/

/*
 * src must point after XPK header (16 bytes), else the Hell on you !
 */
void tfmx_sqsh_unpack(uint8_t *src, uint8_t *dst, int32_t dstlen) {
  int d0,d1,d2,d3,d4,d5,d6,a2,a5;
  int rat,u,cp,cup1,cup2;
  uint8_t *a4,*a6,*c;
  uint8_t a3[] = { 2,3,4,5,6,7,8,0,3,2,4,5,6,7,8,0,4,3,5,2,6,7,8,0,5,4,
        6,2,3,7,8,0,6,5,7,2,3,4,8,0,7,6,8,2,3,4,5,0,8,7,6,2,3,4,5,0 };
   
  c = src + 20;
  while (dstlen) {
    rat = *c++; rat <<= 8; rat |= *c++;		/* ratio ? */
    u = *c++; u <<= 8; u |= *c++;		/* ? */
    cp = *c++; cp <<= 8; cp |= *c++;		/* packed */
    cup1 = *c++; cup1 <<= 8; cup1 |= *c++;	/* unpacked */
    cup2 = *c++; cup2 <<= 8; cup2 |= *c++;	/* unpacked */
    src = c;
/*
fprintf(stderr,"c=%p rat=%04x u=%04x cp=%04x cup1=%04x cup2=%04x\n",c,rat,u,cp,cup1,cup2);
*/
    if (cup1 != cup2) {
      c -= 8;
      continue;
    }

    dstlen -= cup2;
    if (dstlen < 0)
    {
	break;
    }
    cp = (cp + 3) & 0xfffc;
    c += cp - 2;

	a6 = dst + cup2;
	d0 = d1 = d2 = d3 = a2 = 0;
  
	d3 = *(src++);
	*(dst++) = d3;
l6c6:	if (d1 >= 8) goto l6dc;
	if (bfextu(src,d0,1)) goto l75a;
	d0 += 1;
	d5 = 0;
	d6 = 8;
	goto l734;
  
l6dc:	if (bfextu(src,d0,1)) goto l726;
	d0 += 1;
	if (! bfextu(src,d0,1)) goto l75a;
	d0 += 1;
	if (bfextu(src,d0,1)) goto l6f6;
	d6 = 2;
	goto l708;

l6f6:	d0 += 1;
	if (! bfextu(src,d0,1)) goto l706;
	d6 = bfextu(src,d0,3);
	d0 += 3;
	goto l70a;
  
l706:	d6 = 3;
l708:	d0 += 1;
l70a:	d6 = *(a3 + (8*a2) + d6 -17);
	if (d6 != 8) goto l730;
l718:	if (d2 < 20) goto l722;
	d5 = 1;
	goto l732;
  
l722:	d5 = 0;
	goto l734;

l726:	d0 += 1;
	d6 = 8;
	if (d6 == a2) goto l718;
	d6 = a2;
l730:	d5 = 4;
l732:	d2 += 8;
l734:	d4 = bfexts(src,d0,d6);
	d0 += d6;
	d3 -= d4;
	*dst++ = d3;
	d5--;
	if (d5 != -1) goto l734;
	if (d1 == 31) goto l74a;
	d1 += 1;
l74a:	a2 = d6;
l74c:	d6 = d2;
	d6 >>= 3;
	d2 -= d6;
	if (dst < a6) goto l6c6;
    /*
    if (dst != a6) {
      fprintf(stderr,"fatal dst=%p a6=%p\n",dst,a6);
      exit(1);
    }
    */
    dst = a6;
  }
  return;

l75a:	d0 += 1;
	if (bfextu(src,d0,1)) goto l766;
	d4 = 2;
	goto l79e;
  
l766:	d0 += 1;
	if (bfextu(src,d0,1)) goto l772;
	d4 = 4;
	goto l79e;

l772:	d0 += 1;
	if (bfextu(src,d0,1)) goto l77e;
	d4 = 6;
	goto l79e;

l77e:	d0 += 1;
	if (bfextu(src,d0,1)) goto l792;
	d0 += 1;
	d6 = bfextu(src,d0,3);
	d0 += 3;
	d6 += 8;
	goto l7a8;
  
l792:	d0 += 1;
	d6 = bfextu(src,d0,5);
	d0 += 5;
	d4 = 16;
	goto l7a6;
  
l79e:	d0 += 1;
	d6 = bfextu(src,d0,1);
	d0 += 1;
l7a6:	d6 += d4;
l7a8:	if (bfextu(src,d0,1)) goto l7c4;
	d0 += 1;
	if (bfextu(src,d0,1)) goto l7bc;
	d5 = 8;
	a5 = 0;
	goto l7ca;

l7bc:	d5 = 14;
	a5 = -0x1100;
	goto l7ca;

l7c4:	d5 = 12;
	a5 = -0x100;
l7ca:	d0 += 1;
	d4 = bfextu(src,d0,d5);
	d0 += d5;
	d6 -= 3;
	if (d6 < 0) goto l7e0;
	if (d6 == 0) goto l7da;
	d1 -= 1;
l7da:	d1 -= 1;
	if (d1 >= 0) goto l7e0;
	d1 = 0;
l7e0:	d6 += 2;
	a4 = -1 + dst + a5 - d4;
l7ex:	*dst++ = *a4++;
	d6--;
	if (d6 != -1) goto l7ex;
	d3 = *(--a4);
	goto l74c;
}


int bfextu(uint8_t *p,int bo,int bc) {
  int r;
  
  p += bo / 8;
  r = *(p++);
  r <<= 8;
  r |= *(p++);
  r <<= 8;
  r |= *p;
  r <<= bo % 8;
  r &= 0xffffff;
  r >>= 24 - bc;

  return r;
}

int bfexts(uint8_t *p,int bo,int bc) {
  int r;
  
  p += bo / 8;
  r = *(p++);
  r <<= 8;
  r |= *(p++);
  r <<= 8;
  r |= *p;
  r <<= (bo % 8) + 8;
  r >>= 32 - bc;

  return r;
}

