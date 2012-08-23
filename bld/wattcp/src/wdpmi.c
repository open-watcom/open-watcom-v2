/*
 *  DOS-extender/DPMI interface for Watcom-386 and Borland's bcc32.
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

#ifndef OLD_TURBOC  /* preprocessor in tcc <=2.01 have troubles here */

#ifdef __WATCOM386__
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

#elif defined(__BORLAND386__)
  #undef  FP_SEG
  #undef  FP_OFF
  #define FP_SEG(p) _DS
  #define FP_OFF(p) (DWORD)(p)
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
  if (r.w.cflag & 1)
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
  return ((r.w.cflag & 1) == 0);
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
  return ((r.w.cflag & 1) == 0);  /* Return 0 if failed */
}

int dpmi_real_interrupt (int intr, struct DPMI_regs *reg)
{
  union  REGS  r;
  struct SREGS s;

  memset (&r, 0, sizeof(r));
  segread (&s);
  r.w.ax  = 0x300;
  r.x.ebx = intr;
  r.w.cx  = 0;
  s.es    = FP_SEG (reg);
  r.x.edi = FP_OFF (reg);
  reg->r_flags = 0;
  reg->r_ss = reg->r_sp = 0;      /* DPMI host provides stack */

  int386x (0x31, &r, &r, &s);
  return ((r.w.cflag & 1) == 0);  /* Return 0 if failed */
}

int dpmi_alloc_callback (void (*callback)(), struct DPMI_callback *cb)
{
  union  REGS  r;
  struct SREGS s;

  memset (&r, 0, sizeof(r));
  segread (&s);
  r.w.ax  = 0x303;
  s.ds    = s.ds;
  r.x.esi = (DWORD) callback;
  s.es    = s.ds;
  r.x.edi = (DWORD) &cb->cb_reg;
  int386x (0x31, &r, &r, &s);
  if (r.w.cflag & 1)
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
  if (r.w.cflag & 1)
     return (-1);
  return (r.h.cl);
} 

int dpmi_dos_yield (void)
{
  struct DPMI_regs r;

  memset (&r, 0, sizeof(r));
  r.r_ax = 0x1680;
  return dpmi_real_interrupt (0x2F, &r);
}
#endif /* USES_DPMI_API */


#if defined(__WATCOM386__)
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

#elif defined(__BORLAND386__) && (DOSX == WDOSX)
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

  int dpmi_init (void)
  {
    if (!is_wdosx())
    {
      outsnl (_LANG("WDOSX extender not detected"));
      return (-1);
    }
    return (0);
  }

  int dpmi_real_interrupt2 (int intr, struct DPMI_regs *reg)
  {
    __asm {
        push ebx
        push edi
        mov  edi, reg
        mov  edx, esp        /* save esp */
        mov  ebx, intr
        mov  eax, 300h       /* simulate real-int */
        int  31h             /* es:edi -> regs */
        mov  esp, edx
        xor  eax, eax        /* assume ok */
        pop  edi
        pop  ebx
        jc   fail
        pop  ebp
        ret
    }
  fail:
    return (-1);
  }

  int int386 (int intno, union REGS *ireg, union REGS *oreg)
  {
    struct SREGS sreg;

    segread (&sreg);
    return int386x (intno, ireg, oreg, &sreg);
  }

  int int386x (int intno, union REGS *ireg, union REGS *oreg, struct SREGS *sreg)
  {
    static struct DPMI_regs rm_reg;

    rm_reg.r_ss = rm_reg.r_sp = rm_reg.r_flags = 0;
    rm_reg.r_ax = ireg->x.eax;
    rm_reg.r_bx = ireg->x.ebx;
    rm_reg.r_cx = ireg->x.ecx;
    rm_reg.r_dx = ireg->x.edx;
    rm_reg.r_si = ireg->x.esi;
    rm_reg.r_di = ireg->x.edi;
    rm_reg.r_ds = sreg->ds;
    rm_reg.r_es = sreg->es;

    if (dpmi_real_interrupt2(intno,&rm_reg) < 0)
    {
      oreg->x.cflag |= 1; /* Set carry bit */
      return (-1);
    }
    oreg->x.eax   = rm_reg.r_ax;
    oreg->x.ebx   = rm_reg.r_bx;
    oreg->x.ecx   = rm_reg.r_cx;
    oreg->x.edx   = rm_reg.r_dx;
    oreg->x.esi   = rm_reg.r_si;
    oreg->x.edi   = rm_reg.r_di;
    oreg->x.flags = rm_reg.r_flags;
    return (int)rm_reg.r_ax;
  }

  void segread (struct SREGS *sreg)
  {
    sreg->ds = _DS;
    sreg->es = _ES;
    sreg->ss = _SS;
    sreg->cs = _CS;
  }

#elif defined(__GNUC__) && (DOSX == WDOSX)
  static int is_wdosx (void)
  {
    #define SIGNATURE (('W'<<0) + ('D'<<8) + ('S'<<16) + ('X'<<24))
    __dpmi_regs reg;

    reg.d.eax = 0xEEFF;
    if (__dpmi_int (0x31, &reg) == 0 && reg.x.eax == SIGNATURE)
       return (1);
    return (0);
  }

  int dpmi_init (void)
  {
    if (!is_wdosx())
    {
      outsnl (_LANG("WDOSX extender not detected"));
      return (-1);
    }
    return (0);
  }
#endif /* __WATCOM386__ */

#if (DOSX & PHARLAP) && 0    /* test (don't use) */
  unsigned cdecl mwargstack; /* linking Metaware libs with bcc32 app. */
  unsigned cdecl mwgoc;
#endif

#endif /* OLD_TURBOC */
