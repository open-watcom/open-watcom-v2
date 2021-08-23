#ifndef __IREGS_H
#define __IREGS_H

#define hiREG(r,b)  (r) = ((r) & 0xff) | ((unsigned short)(b) << 8)
#define loREG(r,b)  (r) = ((r) & 0xff00) | (unsigned byte)(b)

#define CARRY_BIT   1

/*
 * IREGS structures for pkt_api_entry()
 */
#if (DOSX & PHARLAP)
  #define IREGS      SWI_REGS
  #define r_flags    flags
  #define r_ax       eax
  #define r_bx       ebx
  #define r_dx       edx
  #define r_cx       ecx
  #define r_si       esi
  #define r_di       edi
  #define r_ds       ds
  #define r_es       es

  #define GEN_RM_INTERRUPT(i,r)  _dx_real_int ((UINT)i, r);
#elif (DOSX & DJGPP)
  #define IREGS      __dpmi_regs
  #define r_flags    x.flags
  #define r_ax       d.eax
  #define r_bx       d.ebx
  #define r_dx       d.edx
  #define r_cx       d.ecx
  #define r_si       d.esi
  #define r_di       d.edi
  #define r_ds       x.ds
  #define r_es       x.es

  #define GEN_RM_INTERRUPT(i,r)  __dpmi_int ((int)i, r);
#elif (DOSX & DOS4GW)
  #define IREGS      struct DPMI_regs  /* in wdpmi.h */

  #define GEN_RM_INTERRUPT(i,r)  dpmi_real_interrupt ((int)i, r);
#elif (DOSX & WDOSX)
  #define IREGS      struct DPMI_regs  /* in wdpmi.h */

  #define GEN_RM_INTERRUPT(i,r)  dpmi_real_interrupt2 ((int)i, r);
#elif (DOSX & POWERPAK)     /* to-do !! */
  typedef struct IREGS {    /* just for now */
          WORD  r_ax;
          WORD  r_bx;
          WORD  r_cx;
          WORD  r_dx;
          WORD  r_bp;
          WORD  r_si;
          WORD  r_di;
          WORD  r_ds;
          WORD  r_es;
          WORD  r_flags;
        } IREGS;
  #define GEN_RM_INTERRUPT(i,r)  UNFINISHED()

#else  /* r-mode targets */

  /* IREGS must have same layout and size as Borland's 'struct REGPACK'
   * and Watcom's 'union REGPACK'.
   */
  #ifdef __WATCOMC__
    #define IREGS      union REGPACK
    #define r_flags    w.flags
    #define r_ax       w.ax
    #define r_bx       w.bx
    #define r_dx       w.dx
    #define r_cx       w.cx
    #define r_si       w.si
    #define r_di       w.di
    #define r_ds       w.ds
    #define r_es       w.es

    #define GEN_RM_INTERRUPT(i,r)       intr ((int)i, r)
  #elif defined(_MSC_VER)
    typedef struct IREGS {
          union REGS    r;
          struct SREGS  s;
        } IREGS;
    #define r_flags    r.w.cflag
    #define r_ax       r.w.ax
    #define r_bx       r.w.bx
    #define r_dx       r.w.dx
    #define r_cx       r.w.cx
    #define r_si       r.w.si
    #define r_di       r.w.di
    #define r_ds       s.ds
    #define r_es       s.es

    #define GEN_RM_INTERRUPT(i,r)       int86x ((int)i, &r->r, &r->r, &r->s)
  #else
    #define IREGS      struct REGPACK

    #define GEN_RM_INTERRUPT(i,r)       intr ((int)i, r)
  #endif
#endif

#endif
