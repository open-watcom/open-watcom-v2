/*
 *  Module for various things:
 *   - host/network order (little/big endian) swapping of bytes.
 *   - initialise peek/poke macros.
 *   - allocate transfer buffer for DOSX targets.
 *   - address validation for DOSX targets.
 *   - simple range limited random routine.
 *   - ffs routine to find the first bit set.
 *   - stack checker exit routine for Watcom.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dos.h>

#ifdef __HIGHC__
#include <init.h>  /* _mwlsl(), _msgetcs() */
#endif
#ifdef __DJGPP__
#include <unistd.h>
#endif

#include "wattcp.h"
#include "wdpmi.h"
#include "strings.h"
#include "pcsed.h"
#include "misc.h"

/* These arrays are used several places (save some space)
 */
char hex_chars[] = "0123456789abcdef";
char hex_CHARS[] = "0123456789ABCDEF";

#if (DOSX == 0)
int cpu_type = 86;      /* Detected CPU type; 86,386, ... */
#else
int cpu_type = 386;
#endif

#if !defined(__DJGPP__)
int __bss_count = 0;    /* detect program restarts (!!to-do) */
#endif

#ifdef HAS_FP
FARPTR _watt_dosFp = (FARPTR)0; /* Put in _DATA segment */
#endif

#if (DOSX) && !defined(__DJGPP__)
static void setup_dos_xfer_buf (void);
#endif

#if defined(__WATCOMC__)
extern char cdecl __begtext;    /* label at TEXT start */
extern UINT cdecl _x386_stacklow;

#if defined(__SW_3S)            /* wcc386 -3s, wcc */
  void cdecl _fatal_runtime_error (UINT stk);
  #define FATAL_HANDLER _fatal_runtime_error
#else
  void cdecl _fatal_runtime_error_ (UINT stk);
  #define FATAL_HANDLER _fatal_runtime_error_
#endif

/* Prevent linker (with 'option eliminate') to drop our
 * '_fatal_runtime_error()' function from .exe-image.
 */
char *dummy_fatal_rte = (char*)&FATAL_HANDLER;
#endif /* __WATCOMC__ */


/*
 * Turn off stack-checking to avoid destroying assumptions
 * made in bswap patch code and ffs() below. And also to make
 * this run a bit faster.
 */
#if defined(__HIGHC__) || defined(__WATCOMC__)
#pragma Off(check_stack)
#endif

#if defined(__BORLANDC__)
#pragma option -N-
#endif

#if !defined(BIG_ENDIAN_MACHINE)

#define swap16(p)  ( *p = (*p << 8) | (*p >> 8))
#define swap32(p)  ( swap16 (&((unsigned short*)p)[0]), \
                     swap16 (&((unsigned short*)p)[1]), \
                     *p = (*p << 16) | (*p >> 16) )

/*
 * Convert 32-bit big-endian (network order) to intel (host order) format.
 * Or vice-versa
 */
unsigned long _w32_intel (unsigned long val)
{
  return swap32 (&val);
}

/*
 * Convert 16-bit big-endian (network order) to intel (host order) format.
 * Or vice-versa
 */
unsigned short _w32_intel16 (unsigned short val)
{
  return swap16 (&val);
}

#if (DOSX) && !defined(__WATCOMC__) /* suspect trouble with Watcom */
static BYTE bswap[] = {
            0x8B,0x44,0x24,0x04,       /* mov eax,[esp+4] */
            0x0F,0xC8,                 /* bswap eax       */
            0xC3                       /* ret             */
          };

static BYTE bswap16[] = {
            0x8B,0x44,0x24,0x04,       /* mov eax,[esp+4] */
            0x0F,0xC8,                 /* bswap eax       */
            0xC1,0xE8,0x10,            /* shr eax,16      */
            0xC3                       /* ret             */
          };


/*
 * Modify functions intel/intel16 (htonl/htons) to use the
 * BSWAP instruction on 80486+ CPUs. We don't bother with real-mode
 * targets on a 80486+ CPU. Hope that size of overwritten functions
 * are big enough.
 */
static void patch_bswap (void)
{
  /* For Watcom's register calling convention. Argument is in EAX.
   */
#if defined(__WATCOM386__) && defined(__SW_3R) /* -3r/-4r/-5r used */
  memcpy ((void*)_w32_intel,  4+(BYTE*)&bswap,  sizeof(bswap)-4);
  memcpy ((void*)_w32_intel16,4+(BYTE*)&bswap16,sizeof(bswap16)-4);
#else
  memcpy ((void*)_w32_intel,  (const void*)&bswap,  sizeof(bswap));
  memcpy ((void*)_w32_intel16,(const void*)&bswap16,sizeof(bswap16));
#endif
}
#endif  /* DOSX */
#endif  /* BIG_ENDIAN_MACHINE */


void init_misc (void)
{
#if defined(HAS_FP) && (DOSX & PHARLAP)
  /*
   * For 32-bit compilers with 48-bit far-pointers.
   * `init_misc' MUST be called before `peekx()' functions are used.
   */
  FP_SET (_watt_dosFp, 0, SS_DOSMEM);

#elif defined(HAS_FP) /* MSVC + WDOSX */
  UNFINISHED();
#endif

  /* Check if we're running a 80486+ CPU.
   */
#if (DOSX & PHARLAP)
  {
    CONFIG_INF cnf;
    _dx_config_inf (&cnf, (UCHAR*)&cnf);
    if (cnf.c_processor >= 4)
       cpu_type = 486;
  }
#elif (DOSX & DJGPP)
  {
    __dpmi_version_ret cnf;
    __dpmi_get_version (&cnf);
    if (cnf.cpu >= 4)
       cpu_type = 486;
  }
#elif (DOSX & (DOS4GW|WDOSX))
  if (dpmi_cpu_type() >= 4)
     cpu_type = 486;

#elif (DOSX & POWERPAK)
  UNFINISHED();
#endif

#if (DOSX) && !defined(BIG_ENDIAN_MACHINE) && !defined(__WATCOMC__)
  if (cpu_type >= 486)                      /* ^suspect trouble with Watcom */
     patch_bswap();
#endif

#if (DOSX) && !defined(__DJGPP__)
  setup_dos_xfer_buf();
#endif
 
  srand (peekw(0,0x46C)); /* initialize rand using BIOS clock */
  init_timers();
}

/*
 * returns a random integer in range [a..b]
 */
int Random (unsigned a, unsigned b)
{
  if (a == b)
     return (a);

  if (a > b)
  {
    unsigned x = b;
    b = a;
    a = x;
  }
  return (a + (unsigned)(rand() % (b-a+1)));
}

/*
 * Wait for a random period in range [a..b] millisec
 */
void RandomWait (unsigned a, unsigned b)
{
  DWORD t = set_timeout (Random(a, b));

  while (!chk_timeout(t))
        ;
}

#if defined(USE_DEBUG) && defined(NOT_USED)
/*
 * dword_str() - return nicely formatted string " xx,xxx,xxx"
 * with thousand separators.
 */
const char *dword_str (DWORD val)
{
  static char buf[20];
  char   tmp[20];

  if (val < 1000UL)
  {
    sprintf (buf, "%lu", val);
    return (buf);
  }
  if (val < 1000000UL)       /* 1E6 */
  {
    sprintf (buf, "%lu.%03lu", val/1000UL, val % 1000UL);
    return (buf);
  }
  if (val < 1000000000UL)    /* 1E9 */
  {
    sprintf (tmp, "%9lu", val);
    sprintf (buf, "%.3s,%.3s,%.3s", tmp, tmp+3, tmp+6);
    return (buf);
  }
  sprintf (tmp, "%12lu", val);
  sprintf (buf, "%.3s,%.3s,%.3s,%.3s", tmp, tmp+3, tmp+6, tmp+9);
  return (buf);
}
#endif /* USE_DEBUG && NOT_USED */
 

#if (DOSX)
#if defined(__DJGPP__)
static inline DWORD get_limit (WORD seg)
{
  DWORD lim;
  __asm__ ("lsll %1,%0"
           : "=r" (lim) : "r" (seg));
  return (lim+1);
}
#endif

/*
 * Test for valid read/write data address.
 * We assume linear address 'addr' is both readable and writeable.
 */
BOOL valid_addr (DWORD addr, int len)
{
  if (addr < 0x1000 || (addr >= 0xFFFFFFFFL - len))
     return (FALSE);

#if defined (__DJGPP__)
  if (addr + len > __dpmi_get_segment_limit(_my_ds()))
//if (addr + len > get_limit(_my_ds())) /* 'as 2.8' doesn't understand 'lsll' */
     return (FALSE);

#elif defined (__HIGHC__)
  if (addr + len > _mwlsl(_mwgetcs())) /* DS & CS are aliases */
     return (FALSE);

#elif defined (__WATCOMC__)
  if (addr + len > _get_limit(My_DS()))
     return (FALSE);
#endif

  return (TRUE);
}
#endif  /* DOSX */


/*
 * Pharlap targets:      Determine location of DOS-transfer buffer.
 * DOS4GW/WDOSX targets: Allocate a small (1kB) DOS-transfer buffer.
 * PowerPak targets:     ??
 */
#if (DOSX & PHARLAP)
  REALPTR _watt_dosTbr;          /* rmode-address of transfer buffer */
  FARPTR  _watt_dosTbp;          /* pmode-address of transfer buffer */
  ULONG   _watt_dosTbSize = 0;   /* size of transfer buffer          */
  REALPTR _watt_r2p_addr  = 0;   /* address for real to pmode switch */

  static void setup_dos_xfer_buf (void)
  {
    _dx_rmlink_get (&_watt_r2p_addr,  &_watt_dosTbr,
                    &_watt_dosTbSize, &_watt_dosTbp);
  }

#elif (DOSX & (DOS4GW|WDOSX))
  unsigned short _watt_dosTbSeg = 0; /* paragraph address of xfer buffer */
  unsigned short _watt_dosTbSel = 0; /* selector for transfer buffer     */
  int _watt_dosTbSize = 0;           /* size of transfer buffer          */

  static void free_tb_sel (void)
  {
    if (_watt_dosTbSel)
       dpmi_real_free (_watt_dosTbSel);
    _watt_dosTbSel = 0;
  }
  static void setup_dos_xfer_buf (void)
  {
    _watt_dosTbSize = 1024;
    _watt_dosTbSeg  = dpmi_real_malloc (_watt_dosTbSize, &_watt_dosTbSel);
    if (_watt_dosTbSeg)
         _watt_dosTbSize = 0;
    else atexit (free_tb_sel);
  }

#elif (DOSX & POWERPAK)
  static void setup_dos_xfer_buf (void)
  {
    UNFINISHED();
  }
#endif

#if defined(USE_DEBUG)
  void unfinished (const char *file, unsigned line)
  {
    fprintf (stderr, "%s (%u):\7Help! Unfinished code.\n", file, line);
    _exit (-1);
  }
#endif

/*
 * ffs() isn't needed yet, but should be used in select_s()
 */
#if defined(USE_BSD_FUNC)
#if (DOSX == 0) || defined(OLD_WATCOMC)
/*
 * Copyright (C) 1991, 1992 Free Software Foundation, Inc.
 * Contributed by Torbjorn Granlund (tege@sics.se).
 *
 * The GNU C Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the GNU C Library; see the file COPYING.LIB.  If
 * not, write to the Free Software Foundation, Inc., 675 Mass Ave,
 * Cambridge, MA 02139, USA.
 */

/*
 * Find the first bit set in 'i'.
 */
static int ffs_86 (int i)
{
  static BYTE table[] = {
    0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
  };
  DWORD a, x = i & -i;

  a = x <= 0xFFFFUL ? (x <= 0xFF ? 0 : 8) : (x <= 0xFFFFFF ? 16 : 24);
  return (table[x >> a] + a);
}
#endif  /* DOSX == 0 || OLD_WATCOMC */

#ifndef __cdecl
#define __cdecl  /* prevent register calls */
#endif

int __cdecl ffs (int val)
{
 /* Calling code in data-segment, but this is flat model remember.
  * The "data-array" code crashes the BCC32 and Watcom 10.6 compilers!
  */
#if defined(__BORLAND386__)
  __asm bsf eax, val
  __asm jz zero

    val = _EAX;
    return (++val);
  zero:
    return (0);

#elif (DOSX) && !defined(OLD_WATCOMC)
  __asm {
         bsf eax, val
         jnz short L1
         or eax,-1
     L1: inc eax
         mov val,eax
  }
  return(val);
#else
  return ffs_86 (val);
#endif
}
#endif  /* USE_BSD_FUNC */


/*
 * Checks for bugs when compiling in large model C compiler
 *
 * Borland C uses a 4K stack by default. In all memory models the
 * stack grows down toward the heap.
 *
 * If you accidentally place tcp_Socket onto the stack, then you
 * will have already used up that whole 4K and then some!
 *
 * In large model, this will mess up the data space in a major way
 * because the stack starts at SS:_stklen, or SS:1000, so you will
 * wrap the SP pointer back around to FFFE and start writing over
 * the far heap.  Yuck.
 *
 * In small model it usually doesn't kill your application because
 * you would have to be down to your last 4K of memory and this is
 * not as common.
 *
 * The solutions: declare your sockets as static, or put them on the
 * heap, or bump up your stack size by using the global special variable:
 *
 * unsigned _stklen = 16536;
 */

#if defined(__LARGE__)
void watt_largecheck (void *s, int size, char *file, unsigned line)
{
  if ((unsigned)(FP_OFF(s)) > (unsigned)(-size))
  {
    printf ("%s (%d): user stack size error", file, line);
    exit (3);
  }
}
#endif


#if defined(__WATCOMC__) && defined(USE_DEBUG)

/*
 * For tracking down stack overflow bugs.
 * Stack checker __CHK is pascal-style with stack-size at [esp+4].
 * __CHK calls __STK which in turn may call _fatal_runtime_error()
 *
 * Compiling with stack-checking on, this prologue is in every function:
 *  (*) push <stack size needed>   <- 68h, dword size at EIP-9
 *      call __CHK                 <- 5 bytes
 *      ...                        <- extracted EIP of return
 */
static void stk_overflow (WORD cs, UINT eip)
{
  UINT size = *(UINT*)(eip-9);
  static char buf[12];
  eip -= (UINT)&__begtext - 9; /* print .map-file address of (*) */

#if 1
  outs ("Stack overflow (");
  itoa (size, buf, 10);
  outs (buf);
  outs (" bytes) detected at ");
  itoa (cs, buf, 16);
  outs (buf);
  buf[0] = ':';
  itoa (eip, buf+1, 16);
  outsnl (buf);
#else
  fprintf (stderr, "Stack overflow (%u bytes) detected at %X:%08lXh\n",
           size, cs, (DWORD)eip);
#endif

  _eth_release();
  _exit (1);     /* do minimal work, no atexit() functions */
}

void FATAL_HANDLER (UINT stk)
{
#if defined(__SMALL__)
  _x386_stacklow = stk + 2;
  stk_overflow (My_CS(), *(UINT*)(&stk+3));

#elif defined(__LARGE__)
  _x386_stacklow = stk + 4;
  stk_overflow (*(WORD*)(&stk+5), *(UINT*)(&stk+3)); /* far-call */

#else     /* wcc386 */
  _x386_stacklow = stk + 4;
  stk_overflow (My_CS(), *(UINT*)(&stk+3));
#endif
}

#endif  /* __WATCOMC__ && USE_DEBUG */
