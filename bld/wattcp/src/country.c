/*
 *  Function for fetching DOS's country info
 *
 *  G. Vanem <giva@bgnett.no>, Aug-96
 *
 *  This module is currenly not in use. The reason for these funtions
 *  is automatic detection and selection of language forthe _LANG()
 *  function.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#include "wattcp.h"
#include "wdpmi.h"
#include "misc.h"
#include "country.h"

char _country_info[35];

/*--------------------------------------------------------------*/

int GetCountryCode (void)
{
#if (DOSX & DJGPP)    /* _osmajor/_osminor not set in crt0.o */
  _get_dos_version (0);
#endif

  if (_osmajor >= 3)
  {
#if (DOSX & DJGPP)
    __dpmi_regs reg;

    reg.d.edx = 0;
    reg.x.ds  = __tb / 16;
    reg.d.eax = 0x3800;
    __dpmi_int (0x21, &reg);
    if (reg.x.flags & 1)
       return (0);
    dosmemget (__tb, sizeof(_country_info), &_country_info);
    return (reg.x.bx);

#elif (DOSX & PHARLAP)
    SWI_REGS reg;

    if (_watt_dosTbSize < sizeof(_country_info))
       return (0);

    reg.edx = RP_OFF (_watt_dosTbr);
    reg.ds  = RP_SEG (_watt_dosTbr);
    reg.eax = 0x3800;
    _dx_real_int (0x21, &reg);
    if (reg.flags & 1)
       return (0);
    ReadRealMem (&_country_info, _watt_dosTbr, sizeof(_country_info));
    return (reg.ebx);

#elif (DOSX & (DOS4GW|WDOSX))
    union  REGS  reg;
    struct SREGS sreg;

    if (_watt_dosTbSize < sizeof(_country_info))
       return (0);
 
    reg.x.edx = 0;
    sreg.ds   = _watt_dosTbSeg;
    reg.x.eax = 0x3800;
    int386x (0x21, &reg, &reg, &sreg);
    if (reg.x.cflag)
       return (0);
    memcpy (&_country_info, SEG_OFS_TO_LIN(_watt_dosTbSeg,0),
            sizeof(_country_info));
    return (reg.w.bx);

#elif (DOSX & POWERPAK)
    UNFINISED();

#elif (DOSX == 0)        /* real-mode */
    union  REGS  reg;
    struct SREGS sreg;
 
    reg.x.dx = FP_OFF (_country_info);
    sreg.ds  = FP_SEG (_country_info);
    reg.x.ax = 0x3800;
    int86x (0x21, &reg, &reg, &sreg);
    if (reg.x.cflag)
       return (0);
    return (reg.x.bx);

#else
  #error Unsupported target
#endif
  }
  return (0);
}

/*--------------------------------------------------------------*/

int GetCodePage (void)
{
#if (DOSX & DJGPP)     /* _osmajor/_osminor not set in crt0.o */
  _get_dos_version (0);
#endif

  if ((_osmajor << 8) + _osminor >= 0x303)
  {
#if (DOSX & DJGPP)
    __dpmi_regs reg;

    reg.d.eax = 0x6601;
    __dpmi_int (0x21, &reg);
    if (reg.x.flags & 1)
       return (0);
    return (reg.x.bx);

#elif (DOSX & PHARLAP)
    SWI_REGS reg;

    reg.eax = 0x6601;
    _dx_real_int (0x21, &reg);
    if (reg.flags & 1)
       return (0);
    return (WORD)reg.ebx;

#elif (DOSX & (DOS4GW|WDOSX))
    struct DPMI_regs reg;

    reg.r_ax = 0x6601;
    if (!dpmi_real_interrupt (0x21, &reg))
       return (0);
    return (WORD)reg.r_bx;

#elif (DOSX & POWERPAK)
    UNFINISHED();

#elif (DOSX == 0)       /* real-mode */
    union REGS reg;

    reg.x.ax = 0x6601;
    int86 (0x21, &reg, &reg);
    if (reg.x.cflag)
       return (0);
    return (reg.x.bx);

#else
  #error Unsupported target
#endif
  }
  return (0);
}
