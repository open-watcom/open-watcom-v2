/*
 *  DOS-extender/DPMI interface for Watcom-386.
 *  (possibly useable with gcc and WDOSX also)
 *
 *  Supports Tenberry's DOS4GW, Michael Tippach's WDOSX,
 *  Tran's PMODE/W and Pharlap DOS extenders.
 *
 *  From udplib 1.111 by
 *    John Snagel  <jslagel@volition-inc.com> and
 *    Freek Brysse <frbrysse@vub.ac.be>
 *
 *  11 november, 1997
 *  Ref. http://igweb.vub.ac.be/knights/udplib.html
 *
 *  Heavily changed by G. Vanem <giva@bgnett.no>  August 1998
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wattcp.h"
#include "wdpmi.h"
#include "strings.h"
#include "language.h"
#include "pcsed.h"
#include "sock_ini.h"
#include "iregs.h"


#if defined(__386__)
  /*
   * Values for '_Extender'
   */
  #define DOSX_ERGO     0
  #define DOSX_RATIONAL 1  /* DOS4Gxx, WDOSX, Pmode/W, CauseWay, EDOS */
  #define DOSX_PHAR_V2  2
  #define DOSX_PHAR_V3  3
  #define DOSX_PHAR_V4  4
  #define DOSX_PHAR_V5  5
  #define DOSX_PHAR_V6  6
  #define DOSX_PHAR_V7  7
  #define DOSX_PHAR_V8  8
  #define DOSX_INTEL    9  /* Intel C-Builder ? */
  #define DOSX_WIN386  10  /* Windows 3.11+ ?   */

  /*
   * Values for '_ExtenderSubtype'
   */
  #define XS_NONE                  0
  #define XS_RATIONAL_ZEROBASE     0
  #define XS_RATIONAL_NONZEROBASE  1  /* Only in DOS4G Pro */

  extern void cdecl cstart_();
  extern char        _Extender;
  extern char        _ExtenderSubtype;

#endif

/*
 * DPMI functions for DOS4GW Pro, DOS4G, Pmode/W, CauseWay, EDOS, WDOSX
 */
#if defined(USES_DPMI_API)
void *dpmi_get_real_vector (int intr)
{
  union REGS r;

  r.x.eax = 0x200;
  r.x.ebx = (DWORD) intr;
  int386 (0x31, &r, &r);
  return SEG_OFS_TO_LIN (r.w.cx, r.w.dx);
}

WORD dpmi_real_malloc (WORD size, WORD *selector)
{
  union REGS r;

  r.x.eax = 0x0100;             /* DPMI allocate DOS memory */
  r.x.ebx = (size + 15) / 16;   /* Number of paragraphs requested */
  int386 (0x31, &r, &r);
  if (r.w.cflag & CARRY_BIT)
     return (0);

  *selector = r.w.dx;
  return (r.w.ax);              /* Return segment address */
}

void dpmi_real_free (WORD selector)
{
  union REGS r;

  r.x.eax = 0x101;              /* DPMI free DOS memory */
  r.x.ebx = selector;           /* Selector to free */
  int386 (0x31, &r, &r);
}

int dpmi_lock_region (void *address, unsigned length)
{
  union REGS r;
  DWORD linear = (DWORD)address;

  r.x.eax = 0x600;                /* DPMI Lock Linear Region */
  r.x.ebx = (linear >> 16);       /* Linear address in BX:CX */
  r.x.ecx = (linear & 0xFFFF);
  r.x.esi = (length >> 16);       /* Length in SI:DI */
  r.x.edi = (length & 0xFFFF);
  int386 (0x31, &r, &r);
  return ((r.w.cflag & CARRY_BIT) == 0);
}

int dpmi_unlock_region (void *address, unsigned length)
{
  union REGS r;
  DWORD linear = (DWORD)address;

  r.x.eax = 0x601;                /* DPMI Unlock Linear Region */
  r.x.ebx = (linear >> 16);       /* Linear address in BX:CX */
  r.x.ecx = (linear & 0xFFFF);
  r.x.esi = (length >> 16);       /* Length in SI:DI */
  r.x.edi = (length & 0xFFFF);
  int386 (0x31, &r, &r);
  return ((r.w.cflag & CARRY_BIT) == 0);  /* Return 0 if failed */
}

int dpmi_real_interrupt (int intr, IREGS *regs)
{
  union  REGS  r;
  struct SREGS s;

  memset (&r, 0, sizeof(r));
  segread (&s);
  r.w.ax  = 0x300;
  r.x.ebx = intr;
  r.w.cx  = 0;
  s.es    = _FP_SEG(regs);
  r.x.edi = _FP_OFF(regs);
  regs->r_flags = 0;
  regs->r_ss = regs->r_sp = 0;      /* DPMI host provides stack */

  int386x (0x31, &r, &r, &s);
  return ((r.w.cflag & CARRY_BIT) == 0);  /* Return 0 if failed */
}

int dpmi_alloc_callback (void (*callback)(void), struct DPMI_callback *cb)
{
  union  REGS  r;
  struct SREGS s;

  memset (&r, 0, sizeof(r));
  segread (&s);
  r.w.ax  = 0x303;
  s.ds    = s.ds;
  r.x.esi = (DWORD) callback;
  s.es    = s.ds;
  r.x.edi = (DWORD) &cb->cb_regs;
  int386x (0x31, &r, &r, &s);
  if (r.w.cflag & CARRY_BIT)
     return (0);

  cb->cb_segment = r.w.cx;
  cb->cb_offset  = r.w.dx;
  return (1);
}

int dpmi_cpu_type (void)
{
  union REGS r;

  r.x.eax = 0x400;              /* Get DPMI Version */
  int386 (0x31, &r, &r);
  if (r.w.cflag & CARRY_BIT)
     return (-1);
  return (r.h.cl);
}

int dpmi_dos_yield (void)
{
  IREGS regs;

  memset (&regs, 0, sizeof(regs));
  regs.r_ax = 0x1680;
  return GEN_RM_INTERRUPT (0x2F, &regs);
}
#endif /* USES_DPMI_API */


#if defined(__386__)
  static void unlock_cstart (void)
  {
  #if (DOSX & DOS4GW)
    dpmi_unlock_region (cstart_, 4096);
  #elif (DOSX & PHARLAP)
    _dx_ulock_pgsn ((void*)&cstart_, 4096);
  #endif
  }

  int dpmi_init (void)
  {
  #if (DOSX & (DOS4GW|WDOSX))
    /*
     * Test for non-Pharlap extender types.
     * Should we test for (and support) DOSX_WIN386?
     */
    switch (_Extender)
    {
      case DOSX_PHAR_V2:
      case DOSX_PHAR_V3:
      case DOSX_PHAR_V4:
      case DOSX_PHAR_V5:
      case DOSX_PHAR_V6:
      case DOSX_PHAR_V7:
      case DOSX_PHAR_V8:
           outsnl (_LANG("The Watt-32 library was not compiled for Pharlap."));
           return (-1);

      case DOSX_RATIONAL:
           if (_ExtenderSubtype & XS_RATIONAL_NONZEROBASE)
           {
             outsnl (_LANG("Only zero-based DOS4GW application supported."));
             return (-1);
           }
           break;  /* okay */

      default:
           outsnl (_LANG("Only DOS4GW style extenders supported"));
           return (-1);
    }

    dpmi_lock_region (cstart_, 4096);  /* Why is this needed? */

  #elif (DOSX & PHARLAP)
    _dx_lock_pgsn ((void*)&cstart_, 4096);
  #endif

    atexit (unlock_cstart);
    return (0);
  }

#if (DOSX == WDOSX)
  static int is_wdosx (void)
  {
    __asm {
        mov eax, 0EEFFh
        int 31h
        jc  not_wdosx
        cmp eax, 'WDSX'
        jne not_wdosx
    }
    return (1);
  not_wdosx:
    return (0);
  }
#endif /* DOSX == WDOSX */

#endif /* defined(__386__) */

#if (DOSX & PHARLAP) && 0    /* test (don't use) */
  unsigned cdecl mwargstack; /* linking Metaware libs with bcc32 app. */
  unsigned cdecl mwgoc;
#endif
