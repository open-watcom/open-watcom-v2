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
#include "iregs.h"


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
    IREGS regs;

    memset (&regs, 0, sizeof(regs));
    regs.r_dx = 0;
    regs.r_ds = __tb / 16;
    regs.r_ax = 0x3800;
    GEN_RM_INTERRUPT (0x21, &regs);
    if (regs.r_flags & CARRY_BIT)
       return (0);
    dosmemget (__tb, sizeof(_country_info), &_country_info);
    return (regs.r_bx);

#elif (DOSX & PHARLAP)
    IREGS regs;

    if (_watt_dosTbSize < sizeof(_country_info))
       return (0);

    memset (&regs, 0, sizeof(regs));
    regs.r_dx = RP_OFF (_watt_dosTbr);
    regs.r_ds = RP_SEG (_watt_dosTbr);
    regs.r_ax = 0x3800;
    GEN_RM_INTERRUPT (0x21, &regs);
    if (regs.r_flags & CARRY_BIT)
       return (0);
    ReadRealMem (&_country_info, _watt_dosTbr, sizeof(_country_info));
    return (regs.r_bx);

#elif (DOSX & (DOS4GW|WDOSX))
    IREGS  regs;

    if (_watt_dosTbSize < sizeof(_country_info))
       return (0);

    memset (&regs, 0, sizeof(regs));
    regs.r_dx = 0;
    regs.r_ds = _watt_dosTbSeg;
    regs.r_ax = 0x3800;
    GEN_RM_INTERRUPT (0x21, &regs);
    if (regs.r_flags & CARRY_BIT)
       return (0);
    memcpy (&_country_info, SEG_OFS_TO_LIN(_watt_dosTbSeg,0), sizeof(_country_info));
    return (regs.r_bx);

#elif (DOSX & POWERPAK)
    UNFINISHED();

#elif (DOSX == 0)        /* real-mode */
    IREGS  regs;

    memset (&regs, 0, sizeof(regs));
    regs.r_dx = FP_OFF (_country_info);
    regs.r_ds = FP_SEG (_country_info);
    regs.r_ax = 0x3800;
    GEN_RM_INTERRUPT (0x21, &regs);
    if (regs.r_flags & CARRY_BIT)
       return (0);
    return (regs.r_bx);

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
    IREGS regs;

    memset (&regs, 0, sizeof(regs));
    regs.r_ax = 0x6601;
    GEN_RM_INTERRUPT (0x21, &regs);
    if (regs.r_flags & CARRY_BIT)
       return (0);
    return (regs.r_bx);

#elif (DOSX & PHARLAP)
    IREGS regs;

    memset (&regs, 0, sizeof(regs));
    regs.r_ax = 0x6601;
    GEN_RM_INTERRUPT (0x21, &regs);
    if (regs.r_flags & CARRY_BIT)
       return (0);
    return (WORD)regs.r_bx;

#elif (DOSX & (DOS4GW|WDOSX))
    IREGS regs;

    memset (&regs, 0, sizeof(regs));
    regs.r_ax = 0x6601;
    if (!dpmi_real_interrupt (0x21, &regs))
       return (0);
    if (regs.r_flags & CARRY_BIT)
       return (0);
    return (WORD)regs.r_bx;

#elif (DOSX & POWERPAK)
    UNFINISHED();

#elif (DOSX == 0)       /* real-mode */
    IREGS regs;

    memset (&regs, 0, sizeof(regs));
    regs.r_ax = 0x6601;
    GEN_RM_INTERRUPT (0x21, &regs);
    if (regs.r_flags & CARRY_BIT)
       return (0);
    return (regs.r_bx);

#else
  #error Unsupported target
#endif
  }
  return (0);
}
