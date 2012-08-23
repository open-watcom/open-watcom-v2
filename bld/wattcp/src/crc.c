/*
 *  These CRC functions are derived from code in chapter 19 of the book
 *  "C Programmer's Guide to Serial Communications", by Joe Campbell.
 *
 *  Only used by the language module (ref. language.l)
 */

#include <stdio.h>
#include <stdlib.h>

#include "wattcp.h"
#include "crc.h"

#define CRCBITS    32
#define CRCHIBIT   ((DWORD) (1L << (CRCBITS-1)))
#define CRCSHIFTS  (CRCBITS-8)
#define PRZCRC     0x864CFBL   /* PRZ's 24-bit CRC generator polynomial */

static DWORD *crctable;        /* Table for speeding up CRC's */

/*
 * mk_crctbl() derives a CRC lookup table from the CRC polynomial.
 * The table is used later by watt_crc_bytes() below.
 * mk_crctbl() only needs to be called once at the dawn of time.
 *
 * The theory behind mk_crctbl() is that table[i] is initialized
 * with the CRC of i, and this is related to the CRC of i>>1,
 * so the CRC of i>>1 (pointed to by p) can be used to derive
 * the CRC of i (pointed to by q).
 */
static void mk_crctbl (DWORD poly, DWORD *tab)
{
  int   i;
  DWORD *p = tab;
  DWORD *q = tab;

  *q++ = 0;
  *q++ = poly;
  for (i = 1; i < 128; i++)
  {
    DWORD t = *++p;

    if (t & CRCHIBIT)
    {
      t <<= 1;
      *q++ = t ^ poly;
      *q++ = t;
    }
    else
    {
      t <<= 1;
      *q++ = t;
      *q++ = t ^ poly;
    }
  }
}

/*
 * Calculate 32-bit CRC on buffer buf with length len
 */
DWORD watt_crc_bytes (const char *buf, unsigned len)
{
  DWORD accum = 0;

  if (!crctable)
     return (0L);

  while (len > 0)
  {
    accum = (accum << 8) ^ crctable[(BYTE)(accum >> CRCSHIFTS) ^ *buf++];
    len--;
  }
  return (accum);
}

int watt_crc_init (void)
{
  if (crctable)
     return (1);

  crctable = (DWORD*) calloc (sizeof(DWORD), 256);
  if (!crctable)
     return (0);

  mk_crctbl (PRZCRC, crctable);
  return (1);
}

