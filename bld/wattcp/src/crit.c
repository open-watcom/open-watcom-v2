/*
 * WatTCP critical error (int24h) handler for:
 *    WatcomC (real/prot mode)  !!to-do: finish it.
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


#if (DOSX & PHARLAP)
  #include <mw/exc.h>

  static REALPTR int24_old, rmcb;

#pragma off(check_stack)
  static void int24_isr (IREGS *regs)
  {
    regs->r_ax = 3;  /* simply fail the function */
  }                /* note: this only works on DOS 3.0 and above */
#pragma pop(check_stack)

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

#elif (DOSX & DOS4GW) || (DOSX == 0)
/* Watcom protected and real-mode target */
#pragma off(check_stack)
  static int __far int24_isr (unsigned dev_err, unsigned err_code, unsigned __far *devhdr)
  {
    ARGSUSED (dev_err); ARGSUSED (err_code); ARGSUSED (devhdr);

    return (_HARDERR_FAIL);
  }
#pragma pop(check_stack)

  void int24_init (void)
  {
    if (_osmajor >= 3)
      _harderr (int24_isr);
  }
#endif
