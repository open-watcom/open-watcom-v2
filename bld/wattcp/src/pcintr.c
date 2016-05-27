/*
 * pcintr - add interrupt based processing, improve performance
 *          during disk slowdowns
 *
 * wintr_init()     - call once
 * wintr_shutdown() - called automatically
 * wintr_enable()   - enable interrupt based calls
 * wintr_disable()  - diable interrupt based calls (default)
 * (*wintr_chain)() - a place to chain in your own calls, must live
 *                    within something like 1K stack
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dos.h>

#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "misc.h"
#include "pcintr.h"
#include "pctcp.h"
#include "wdpmi.h"

#if (DOSX & PHARLAP)
#include <mw/exc.h>  /* _dx_alloc_rmode_wrapper_iret() */
#endif

#define TIMER_INTR 0x08
#define STK_SIZE   1024

void (*wintr_chain)(void) = NULL;
int  on_isr8;
int  inside_isr8 = 0;

void wintr_enable (void) { on_isr8 = 1; }
void wintr_disable(void) { on_isr8 = 0; }

#if (DOSX & PHARLAP)
  static REALPTR oldint, timer_cb;
  static RMC_BLK rmRegs;

  static void NewTimer (void)
  {
    _dx_call_real (oldint, &rmRegs, 1);

    if (!inside_isr8)
    {
      if (on_isr8)
      {
        if (wintr_chain)
          (*wintr_chain)();
        tcp_tick (NULL);
      }
      inside_isr8 = 0;
    }
  }

  void wintr_shutdown (void)
  {
    if (oldint)
    {
      _dx_rmiv_set (TIMER_INTR, oldint);
      _dx_free_rmode_wrapper (timer_cb);
      oldint = 0;
    }
  }

  void wintr_init (void)
  {
    timer_cb = _dx_alloc_rmode_wrapper_iret ((pmodeHook)NewTimer, 40000);
    if (!timer_cb)
    {
      outsnl (_LANG("Cannot allocate real-mode timer callback"));
      exit (1);
    }
    _dx_rmiv_get (TIMER_INTR, &oldint);
    _dx_rmiv_set (TIMER_INTR, timer_cb);
    atexit (wintr_shutdown);
  }

#elif (DOSX & DJGPP)
  #include <sys/time.h>

  static void NewTimer (void)
  {
    if (!inside_isr8)
    {
      if (on_isr8)
      {
        if (wintr_chain)
          (*wintr_chain)();
        tcp_tick (NULL);
      }
      inside_isr8 = 0;
    }
  }

  void wintr_shutdown (void)
  {
    struct itimerval tim;

    tim.it_interval.tv_sec  = 0;
    tim.it_interval.tv_usec = 0;
    signal (SIGALRM, SIG_DFL);
    setitimer (ITIMER_REAL, &tim, NULL);
  }

  void wintr_init (void)
  {
    struct itimerval tim;

    tim.it_interval.tv_usec = 54945;  /* 1000000/18.2 */
    tim.it_interval.tv_sec  = 0;
    tim.it_value = tim.it_interval;
    signal (SIGALRM, (void(*)(int))NewTimer);
    setitimer (ITIMER_REAL, &tim, NULL);
  }

#elif defined (__WATCOM386__) && (DOSX & (DOS4GW|WDOSX))
  static void (__interrupt __far *oldint)();

  static void __interrupt __far NewTimer (void)
  {
    if (!inside_isr8)
    {
      if (on_isr8)
      {
        static UINT locstack [STK_SIZE];
        DISABLE();
        stackset (&locstack[STK_SIZE-1]);

        if (wintr_chain)
          (*wintr_chain)();
        tcp_tick (NULL);

        stackrestore();
        ENABLE();
      }
      inside_isr8 = 0;
    }
    _chain_intr (oldint); /* !! does this work? Maybe need to use DPMI func. */
  }

  void wintr_shutdown (void)
  {
    if (oldint)
    {
      _dos_setvect (TIMER_INTR, oldint);
      oldint = NULL;
    }
  }

  void wintr_init (void)
  {
    atexit (wintr_shutdown);
    oldint = _dos_getvect (TIMER_INTR);
    _dos_setvect (TIMER_INTR, NewTimer);
  }

#elif defined (__BORLAND386__) && (DOSX == WDOSX)
  void wintr_shutdown (void)
  {
    UNFINISHED();
  }
  void wintr_init (void)
  {
    UNFINISHED();
  }

#elif (DOSX & POWERPAK)
  void wintr_shutdown (void)
  {
    UNFINISHED();
  }
  void wintr_init (void)
  {
    UNFINISHED();
  }

#elif (DOSX == 0) && !defined(NO_INLINE_ASM)

  #ifdef __TURBOC__
    void interrupt (*oldint)(void);
  #else
    void (interrupt *oldint)();
  #endif

  static void interrupt NewTimer(void)
  {
    (*oldint)();    /* chain now */

    if (!inside_isr8)
    {
      if (on_isr8)
      {
        static UINT locstack [STK_SIZE];
  #ifdef __WATCOMC__
        DISABLE();
        stackset (&locstack[STK_SIZE-1]);
  #else
        static UINT old_SP;
        static WORD old_SS;
        asm  pushf
        asm  cli
        asm  mov ax,ss
        asm  mov old_SS,ax
        asm  mov ax,sp
        asm  mov old_SP,ax
        asm  mov ax,ds
        asm  mov ss,ax
        asm  lea sp,locstack[STK_SIZE-1]
  #endif
        ENABLE();

        if (wintr_chain)
          (*wintr_chain)();
        tcp_tick (NULL);

        DISABLE();
  #ifdef __WATCOMC__
        stackrestore();
  #else
        asm  mov ax,old_SS
        asm  mov ss,ax
        asm  mov ax,old_SP
        asm  mov sp,ax
  #endif
        ENABLE();
      }
      inside_isr8 = 0;
    }
  }

  void wintr_shutdown (void)
  {
    if (oldint)
    {
      setvect (TIMER_INTR, oldint);
      oldint = NULL;
    }
  }

  void wintr_init (void)
  {
    atexit (wintr_shutdown);
    oldint = getvect (TIMER_INTR);
    setvect (TIMER_INTR, NewTimer);
  }
#endif

