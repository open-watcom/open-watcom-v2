/*
 * WatTCP critical error (int24h) handler for:
 *    Metaware HighC / PharLap
 *    WatcomC (real/prot mode)  !!to-do: finish it.
 *    GNU C / djgpp2
 *    real-mode DOS targets
 *
 *  by G. Vanem 4-Apr-97
 */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#include "wattcp.h"
#include "strings.h"
#include "language.h"
#include "crit.h"

#if defined(__HIGHC__)       /* disable stack-checking here */
#pragma off(check_stack)
#pragma off(call_trace)
#pragma off(prolog_trace)
#pragma off(epilog_trace)
#endif

#if defined(__WATCOMC__)
#pragma off(check_stack)
#endif

#if (defined(__TURBOC__) || defined(__BORLANDC__)) && !defined(OLD_TURBOC)
#pragma option -N-
#endif


#if (DOSX & PHARLAP)
  #include <mw/exc.h>

  static REALPTR int24_old, rmcb;

  static void int24_isr (SWI_REGS *reg)
  {
    reg->eax = 3;  /* simply fail the function */
  }                /* note: this only works on DOS 3.0 and above */

  static void int24_restore (void)
  {
    _dx_rmiv_set (CRIT_VECT,int24_old);
    _dx_free_rmode_wrapper (rmcb);
  }

  void int24_init (void)
  {
    if (_osmajor < 3)
       return;

    _dx_rmiv_get (CRIT_VECT, &int24_old);
    rmcb = _dx_alloc_rmode_wrapper_iret (int24_isr, 256);

    if (!rmcb)
    {
      outsnl (_LANG("Failed to allocate INT24 callback.\7"));
      return;
    }
    _dx_rmiv_set (CRIT_VECT, rmcb);
    atexit (int24_restore);
  }

/*---------------------------------------------------------------------*/

#elif (DOSX & DOS4GW)    /* Watcom prot-mode target */
  #pragma argused
  static int int24_isr (unsigned dev_err, unsigned err_code,
                        unsigned *devhdr)
  {
    return (_HARDERR_FAIL);
  }

  void int24_init (void)
  {
  #if 0  /* doesn't compile/work */
    if (_osmajor >= 3)
      _harderr (int24_isr);
  #endif
  }

/*---------------------------------------------------------------------*/

#elif defined (__WATCOMC__) && (DOSX == 0)  /* Watcom real-mode target */
  #pragma argsused
  static int _far int24_isr (unsigned dev_err, unsigned err_code,
                             unsigned _far *devhdr)
  {
    return (_HARDERR_FAIL);
  }

  void int24_init (void)
  {
    if (_osmajor >= 3)
      _harderr (int24_isr);
  }

/*---------------------------------------------------------------------*/

#elif (DOSX & DJGPP)
  static _go32_dpmi_seginfo rm_cb, int24_old;
  static __dpmi_regs        rm_reg;

  static void int24_isr (void)
  {
    rm_reg.x.ax = 3;
  }

  static void int24_restore (void)
  {
    _go32_dpmi_set_real_mode_interrupt_vector (CRIT_VECT, &int24_old);
    _go32_dpmi_free_real_mode_callback (&rm_cb);
  }

  void int24_init (void)
  {
    _get_dos_version (0);  /* bug; _osmajor/_osminor not set in crt0 */
    if (_osmajor < 3)
       return;

    _go32_dpmi_get_real_mode_interrupt_vector (CRIT_VECT, &int24_old);
    rm_cb.pm_offset = (DWORD) &int24_isr;
    if (_go32_dpmi_allocate_real_mode_callback_iret(&rm_cb,&rm_reg) ||
        _go32_dpmi_lock_data(&rm_reg,sizeof(rm_reg)))
    {
      outsnl (_LANG("Failed to allocate INT24 callback.\7"));
      return;
    }
    _go32_dpmi_set_real_mode_interrupt_vector (CRIT_VECT, &rm_cb);
    atexit (int24_restore);
  }

/*---------------------------------------------------------------------*/

#elif DOSX == 0      /* real-mode targets */
  #ifdef __TURBOC__
    static void interrupt (*int24_old)(void);
  #else
    static void (interrupt *int24_old)();
  #endif

  static void interrupt int24_isr (bp,di,si,ds,es,dx,cx,bx,ax,ip,cs,flags)
  {
    ax = 3;
    ARGSUSED (bp); ARGSUSED (di); ARGSUSED (si); ARGSUSED (ds);
    ARGSUSED (es); ARGSUSED (dx); ARGSUSED (cx); ARGSUSED (bx);
    ARGSUSED (ax); ARGSUSED (ip); ARGSUSED (cs); ARGSUSED (flags);
  }

  static void int24_restore (void)
  {
    setvect (CRIT_VECT, int24_old);
  }

  void int24_init (void)
  {
    if (_osmajor >= 3)
    {
      int24_old = getvect (CRIT_VECT);
      setvect (CRIT_VECT, int24_isr);
      atexit (int24_restore);
    }
  }
#endif

