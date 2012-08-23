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
#define DJGPP        2             /* GNU C/C++ and djgpp 2.0 target       */
#define DOS4GW       4             /* Tenberry's DOS extender        (1+2) */
#define POWERPAK     8             /* Borland's PowerPak DOS extender  (3) */
#define WDOSX        16            /* WDOSX extender v0.96+            (4) */
#define PHARLAP_DLL (0x80|PHARLAP) /* PharLap DLL version target           */
#define DOS4GW_DLL  (0x80|DOS4GW)  /* DOS4GW DLL version target (possible?)*/
#define WDOSX_DLL   (0x80|WDOSX)   /* WDOSX DLL version target (possible?) */
#define DJGPP_DXE   (0x80|DJGPP)   /* djgpp DXE target                     */

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

#if defined(_MSC_VER) && defined(M_I86SM)   /* Microsoft doesn't have */
  #define __SMALL__                         /* __SMALL__,  __LARGE__  */
#endif

#if defined(_MSC_VER) && defined(M_I86LM)
  #define __LARGE__
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

#if defined(__SMALL__) || defined(__COMPACT__) || defined(__LARGE__)
  #undef  DOSX
  #define DOSX 0
#endif

#if defined(__DJGPP__) && defined(__GNUC__)
  #undef  DOSX
  #define DOSX      DJGPP      /* WDOSX is possible? */
#endif

#if defined(__WATCOMC__) && defined(__386__)
  #undef  DOSX
  #define DOSX      DOS4GW     /* may also be WDOSX/PHARLAP */
  #define __WATCOM386__
#endif

#if defined(_MSC_VER) && defined(__386__)
  #undef  DOSX
  #define DOSX      PHARLAP
#endif

#if defined(__HIGHC__)
  #undef  DOSX
  #define DOSX      PHARLAP /* DOS4GW is possible? */
  #undef  BUGGY_FARPTR
  #define BUGGY_FARPTR 0    /* set to 1 for HighC 3.1 at opt-lvl >=3 */
#endif                      /* It generates buggy code for far-ptrs */

#if defined(__BORLANDC__) && defined(__FLAT__) && defined(__DPMI32__)
  #undef  DOSX
  #define DOSX  WDOSX       /* may use WDOSX,POWERPAK */
  #define __BORLAND386__
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

  #ifdef __WATCOM386__
    #undef FP_OFF
    #include <i86.h>
    #include <dos.h>
  #endif

  extern REALPTR  _watt_dosTbr;
  extern FARPTR   _watt_dosTbp;
  extern ULONG    _watt_dosTbSize;
  extern REALPTR  _watt_r2p_addr;

  #if (!BUGGY_FARPTR) &&         /* Trust the compiler to handle far-ptr ? */ \
      (__CMPLR_FEATURES__ & __FEATURE_FARPTR__) /* compilers with far-ptrs */
    #define HAS_FP                              /* i.e. HighC, Watcom386   */
    extern FARPTR _watt_dosFp;

    #define DOSMEM(s,o,t) *(t _far*)(_watt_dosFp + (DWORD)((o)|(s)<<4))
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

#elif (DOSX & DJGPP)
  #include <dpmi.h>
  #include <go32.h>
  #include <sys/farptr.h>
  #define peekb(s,o)      _farpeekb (_dos_ds, (o)+((s)<<4))
  #define peekw(s,o)      _farpeekw (_dos_ds, (o)+((s)<<4))
  #define peekl(s,o)      _farpeekl (_dos_ds, (o)+((s)<<4))
  #define pokeb(s,o,x)    _farpokeb (_dos_ds, (o)+((s)<<4), x)
  #define pokew(s,o,x)    _farpokew (_dos_ds, (o)+((s)<<4), x)
  #define pokel(s,o,x)    _farpokel (_dos_ds, (o)+((s)<<4), x)
  #define write           _write
  #define read            _read   /* no '\n' -> '\r\n' translation */
  #define close           _close
  #define BOOL            int

#elif (DOSX & (DOS4GW|WDOSX))     /* DOS4GW or WDOSX extenders */
  #undef DJGPP
  #undef __DJGPP__                /* for use with djgpp's gcc */
  #include <dos.h>
  extern unsigned short _watt_dosTbSeg, _watt_dosTbSel;
  extern int            _watt_dosTbSize;

  #define DOSMEM(s,o,t)   *(volatile t *) (((s) << 4) | (o))
  #define peekb(s,o)      DOSMEM(s,o,BYTE)
  #define peekw(s,o)      DOSMEM(s,o,WORD)
  #define peekl(s,o)      DOSMEM(s,o,DWORD)
  #define pokeb(s,o,x)    DOSMEM(s,o,BYTE)  = (BYTE)(x)
  #define pokew(s,o,x)    DOSMEM(s,o,WORD)  = (WORD)(x)
  #define pokel(s,o,x)    DOSMEM(s,o,DWORD) = (DWORD)(x)
  #undef  BOOL
  #define BOOL int

#elif (DOSX & POWERPAK)   /* to-do !! */
  #include <dos.h>
  extern unsigned long _to_do1 (unsigned long);
  extern void          _to_do2 (unsigned long, unsigned long);
  extern int int86 (int, union REGS*, union REGS*);

  #define peekb(s,o)      (BYTE) _to_do1 (((s)<<4)+(o))
  #define peekw(s,o)      (WORD) _to_do1 (((s)<<4)+(o))
  #define peekl(s,o)      (DWORD)_to_do1 (((s)<<4)+(o))
  #define pokeb(s,o,x)    _to_do2 (((s)<<4)+(o), (BYTE)x)
  #define pokew(s,o,x)    _to_do2 (((s)<<4)+(o), (WORD)x)
  #define pokel(s,o,x)    _to_do2 (((s)<<4)+(o), (DWORD)x)
  #undef  BOOL
  #define BOOL int

#else     /* All real-mode targets */
  #include <dos.h>
  #define  BOOL           int

  #if defined(__WATCOMC__)
    #define peekb(s,o)    (*((BYTE  __far*)MK_FP((s),(o))))
    #define peekw(s,o)    (*((WORD  __far*)MK_FP((s),(o))))
    #define peekl(s,o)    (*((DWORD __far*)MK_FP((s),(o))))
    #define pokeb(s,o,x)  (*((BYTE  __far*)MK_FP((s),(o))) = (BYTE)(x))
    #define pokew(s,o,x)  (*((WORD  __far*)MK_FP((s),(o))) = (WORD)(x))
    #define pokel(s,o,x)  (*((DWORD __far*)MK_FP((s),(o))) = (DWORD)(x))
  #else
    #define peekw(s,o)    (WORD)peek(s,o)
    #define pokew(s,o,x)  poke (s,o,x)
    #define peekl(s,o)    (*((DWORD far*)MK_FP((s),(o))))
  #endif
#endif


/*
 * Macros and hacks depending on vendor (compiler)
 */

#if defined(__TURBOC__) && (__TURBOC__ <= 0x301)
  #include <mem.h>
  #include <alloc.h>
  #define OLD_TURBOC   /* TCC <= 2.01 doesn't have <malloc.h> and <memory.h> */
  #define _cdecl _Cdecl
  #define _far   far
#else
  #include <memory.h>
  #include <malloc.h>
#endif

#if defined(__HIGHC__)
  #include <string.h>
  #define max(a,b)        _max(a,b)           /* intrinsic functions */
  #define min(a,b)        _min(a,b)
  #define movmem(s,d,n)   memmove(d,s,n)
  #define ENABLE()        _inline(0xFB)       /* sti */
  #define DISABLE()       _inline(0xFA)       /* cli */
#endif

#if defined(_MSC_VER)
  #if (DOSX) && (DOSX != PHARLAP)
    #error MSC and non-Pharlap targets not supported
  #endif

  #if (_MSC_VER <= 600)   /* A few exceptions for MSC <=6.0 */
    #define NO_INLINE_ASM /* doesn't have built-in assembler */
    #define ENABLE()      _enable()
    #define DISABLE()     _disable()
  #else
    #pragma warning (disable:4103 4113)
    #pragma warning (disable:4024 4047)
    #define ENABLE()      __asm sti
    #define DISABLE()     __asm cli
    #define asm           __asm
  #endif

  #define interrupt       _interrupt far
  #define getvect(a)      _dos_getvect(a)
  #define setvect(a,b)    _dos_setvect(a,b)
  #define movmem(s,d,n)   memmove(d,s,n)
  #define MK_FP(s,o)      (void _far*) (((DWORD)(s) << 16) + (DWORD)(o))
  #define peek(s,o)       (*((WORD _far*)MK_FP((s),(o))))
  #define poke(s,o,x)     (*((WORD _far*)MK_FP((s),(o))) = (DWORD)(x))
#endif

#if defined(__TURBOC__) || defined(__BORLANDC__)
  #include <string.h>
  #define ENABLE()        __emit__(0xFB)
  #define DISABLE()       __emit__(0xFA)
  #pragma warn -bbf-   /* "Bitfields must be signed or unsigned int" warning */
  #pragma warn -sig-   /* "Conversion may loose significant didgits" warning */
  #pragma warn -cln-   /* "Constant is long" warning */

  #if defined(__BORLANDC__) /* make string/memory functions inline */
    #define strlen        __strlen__
    #define strncpy       __strncpy__
    #define strrchr       __strrchr__
    #define strncmp       __strncmp__
    #define memset        __memset__
    #define memcpy        __memcpy__
    #define memcmp        __memcmp__
    #define memchr        __memchr__
  #endif
#endif

#if defined(__GNUC__)
  #if (__GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 7))
    #error I need GCC 2.7.x or later
  #endif
  #include <string.h>
  #define max(a,b)        (((a) > (b)) ? (a) : (b))
  #define min(a,b)        (((a) < (b)) ? (a) : (b))
  #define movmem(s,d,n)   memmove(d,s,n)
  #define ENABLE()        __asm__ __volatile__ ("sti")
  #define DISABLE()       __asm__ __volatile__ ("cli")
  #define outp(p,x)       outportb(p,x)
  #define inp(p)          inportb(p)
#endif

#if defined(__WATCOMC__)
  #include <i86.h>
  #include <dos.h>
  #include <string.h>
  #pragma intrinsic(strcmp,memset)
  #pragma warning(disable:120)

  #define movmem(s,d,n)   memmove(d,s,n)
  #define getvect(a)      _dos_getvect(a)
  #define setvect(a,b)    _dos_setvect(a,b)
  #define ENABLE()        _enable()
  #define DISABLE()       _disable()
  #define BOOL            int
  #if (__WATCOMC__ < 1100)
  #define OLD_WATCOMC     /* < v11.0 is "old" */
  #endif
#endif



/*
 * Because kbhit() will pull in more conio function that we
 * really need, use the simple kbhit() variant (without ungetch
 * option). This also prevents multiple definition trouble when
 * linking e.g. PCcurses and Watt-32 library.
 */

#if defined (__DJGPP__)
  #ifdef __dj_include_conio_h_
    #error "Don't include <conio.h>"
  #endif
  #include <pc.h>     /* simple kbhit() */

#elif defined (__HIGHC__)
  #ifdef __metaware_conio_h_
    #error "Don't include <mw/conio.h>"
  #endif
  #include <conio.h>  /* simple kbhit() */

#elif defined(_MSC_VER) && defined(__386__)
  /* problems including <conio.h> from Visual C 4.0
   */
  int __cdecl kbhit (void);

#else                 /* no other option */
  #include <conio.h>
#endif

#endif  /* __WATT_TARGET_H */

