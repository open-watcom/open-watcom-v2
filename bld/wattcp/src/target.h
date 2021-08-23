#ifndef __WATT_TARGET_H
#define __WATT_TARGET_H

/*
 *  Definitions of targets and macros for Waterloo tcp/ip.
 *
 *  by G. Vanem <giva@bgnett.no> 1995
 */

#ifndef __WATTCP_H
#error TARGET.H must be included inside or after WATTCP.H
#endif

#define PHARLAP      1             /* PharLap 386|DosX extender target (1) */
#define DOS4GW       4             /* Tenberry's DOS extender        (1+2) */
#define WDOSX        16            /* WDOSX extender v0.96+            (4) */
#define PHARLAP_DLL (0x80|PHARLAP) /* PharLap DLL version target           */
#define DOS4GW_DLL  (0x80|DOS4GW)  /* DOS4GW DLL version target (possible?)*/
#define WDOSX_DLL   (0x80|WDOSX)   /* WDOSX DLL version target (possible?) */

/*
 * Notes:
 *
 * (1) Any 32-bit DOS compiler (Borland/Watcom/Microsoft/Symantec(?)/HighC)
 *     will work with this DOS extender. Some compilers support far
 *     pointers (48-bits), some don't. And even worse, some of those who
 *     do, have bugs in their segment register handling!
 *     Add `-DBUGGY_FARPTR=1' to your makefile's CFLAGS if you experience
 *     this (generate .asm listing to find out).
 *
 *     The problem is present in:
 *       - Metaware's HighC v3.1 at -O3 or above (for sure).
 *       - BCC32 v4, Some rumours say far-ptrs in combination with FPU-ops.
 *
 * (2) Several DOS-extenders supports Watcom-386. DOS4GW (from Tenberry)
 *     is a DPMI 0.9 host with limited API. Other compatible DOS-extenders
 *     can also be used without modifying Watt-32. These are:
 *     DOS4GW Pro, DOS4G, Pmode/W, CauseWay, EDOS and WDOSX.
 *
 * (3) Currently unsupported.
 *
 * (4) The WDOSX extender is backwards compatible with DOS4GW-style
 *     extenders, but includes more features that puts this extender
 *     in a class of it's own. Specially it supports Real-mode Callbacks
 *     (rmcb) which other "Watcom" extenders don't (except DOS4GW Pro?).
 *     Although untested, it's possible to use WDOSX with GNU-C.
 */

#ifndef BUGGY_FARPTR
#define BUGGY_FARPTR 0      /* Assume no compilers have fp-bugs, duh! */
#endif

#if defined(__TINY__) || defined(__MEDIUM__) || defined(__HUGE__)
  #error Unsupported memory model (tiny/medium/huge)
#endif

#if defined(M_I86TM) || defined(M_I86MM) || defined(M_I86HM)
  #error Unsupported memory model (tiny/medium/huge)
#endif

#if defined(_M_I86MM) || defined(_M_I86MH)
  #error Unsupported memory model (medium/huge)
#endif

#if defined(__386__)
  #ifndef  DOSX
    #define DOSX    DOS4GW      /* may also be WDOSX/PHARLAP */
  #endif
#endif

#if defined(__SMALL__) || defined(__COMPACT__) || defined(__LARGE__)
  #ifndef  DOSX
    #define DOSX    0           /* 16-bit real-mode */
  #endif
#endif

#ifndef DOSX
  #error DOSX target not defined
#endif

/*
 * Macros and hacks depending on target (DOS-extender)
 */

#if (DOSX & PHARLAP)
  #include <stdio.h>
  #include <pharlap.h>

  #undef _FP_OFF
  #include <dos.h>

  extern REALPTR          _watt_dosTbr;
  extern FARPTR           _watt_dosTbp;
  extern ULONG            _watt_dosTbSize;
  extern REALPTR          _watt_r2p_addr;

  #if (!BUGGY_FARPTR) &&         /* Trust the compiler to handle far-ptr ? */ \
      (__CMPLR_FEATURES__ & __FEATURE_FARPTR__) /* compilers with far-ptrs */
    #define HAS_FP                              /* i.e. HighC, Watcom386   */
    extern FARPTR _watt_dosFp;

    #define DOSMEM(s,o,t) *(t __far *)(_watt_dosFp + (DWORD)((o)|(s)<<4))
    #define peekb(s,o)    DOSMEM(s,o,BYTE)
    #define peekw(s,o)    DOSMEM(s,o,WORD)
    #define peekl(s,o)    DOSMEM(s,o,DWORD)
    #define pokeb(s,o,x)  DOSMEM(s,o,BYTE)  = (BYTE)(x)
    #define pokew(s,o,x)  DOSMEM(s,o,WORD)  = (WORD)(x)
    #define pokel(s,o,x)  DOSMEM(s,o,DWORD) = (DWORD)(x)
  #else
    #define peekb(s,o)    PeekRealByte (((s) << 16) + (o))
    #define peekw(s,o)    PeekRealWord (((s) << 16) + (o))
    #define peekl(s,o)    PeekRealDWord(((s) << 16) + (o))
    #define pokeb(s,o,x)  PokeRealByte (((s) << 16) + (o), (x))
    #define pokew(s,o,x)  PokeRealWord (((s) << 16) + (o), (x))
    #define pokel(s,o,x)  PokeRealDWord(((s) << 16) + (o), (x))
  #endif

#elif (DOSX & (DOS4GW|WDOSX))     /* DOS4GW or WDOSX extenders */
  #include <dos.h>
  extern unsigned short   _watt_dosTbSeg;
  extern unsigned short   _watt_dosTbSel;
  extern int              _watt_dosTbSize;

  #define DOSMEM(s,o,t)   *(volatile t *) (((s) << 4) | (o))
  #define peekb(s,o)      DOSMEM(s,o,BYTE)
  #define peekw(s,o)      DOSMEM(s,o,WORD)
  #define peekl(s,o)      DOSMEM(s,o,DWORD)
  #define pokeb(s,o,x)    DOSMEM(s,o,BYTE)  = (BYTE)(x)
  #define pokew(s,o,x)    DOSMEM(s,o,WORD)  = (WORD)(x)
  #define pokel(s,o,x)    DOSMEM(s,o,DWORD) = (DWORD)(x)
  #undef  BOOL
  #define BOOL int

#elif (DOSX == 0)       /* All 16-bit real-mode targets */
  #include <dos.h>
  #define BOOL            int

  #define peekb(s,o)      (*((BYTE  __far *)_MK_FP((s),(o))))
  #define peekw(s,o)      (*((WORD  __far *)_MK_FP((s),(o))))
  #define peekl(s,o)      (*((DWORD __far *)_MK_FP((s),(o))))
  #define pokeb(s,o,x)    (*((BYTE  __far *)_MK_FP((s),(o))) = (BYTE)(x))
  #define pokew(s,o,x)    (*((WORD  __far *)_MK_FP((s),(o))) = (WORD)(x))
  #define pokel(s,o,x)    (*((DWORD __far *)_MK_FP((s),(o))) = (DWORD)(x))
#endif


/*
 * Macros and hacks depending on vendor (compiler)
 */

#include <memory.h>
#include <malloc.h>

#include <i86.h>
#include <dos.h>
#include <string.h>
#pragma intrinsic(strcmp,memset)

#undef  cdecl
#undef  _cdecl
#define cdecl           __cdecl
#define _cdecl          __cdecl

#define movmem(s,d,n)   memmove(d,s,n)
#define getvect(a)      _dos_getvect(a)
#define setvect(a,b)    _dos_setvect(a,b)
#define ENABLE()        _enable()
#define DISABLE()       _disable()
#define BOOL            int

/*
 * Because kbhit() will pull in more conio function that we
 * really need, use the simple kbhit() variant (without ungetch
 * option). This also prevents multiple definition trouble when
 * linking e.g. PCcurses and Watt-32 library.
 */
#include <conio.h>

#endif  /* __WATT_TARGET_H */
