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
  if (_osmajor >= 3)
  {
    IREGS regs;

#if (DOSX & (PHARLAP|DOS4GW|WDOSX))
    if (_watt_dosTbSize < sizeof(_country_info))
       return (0);
#endif
#if (DOSX == 0)        /* real-mode */
    _fmemset (&regs, 0, sizeof(regs));
    regs.r_dx = _FP_OFF( _country_info );
    regs.r_ds  = _FP_SEG( _country_info );
    regs.r_ax = 0x3800;
    GEN_RM_INTERRUPT (0x21, &regs);
#else
    memset (&regs, 0, sizeof(regs));
#if (DOSX & PHARLAP)
    regs.r_dx = RP_OFF (_watt_dosTbr);
    regs.r_ds  = RP_SEG (_watt_dosTbr);
#elif (DOSX & (DOS4GW|WDOSX))
    regs.r_ds   = _watt_dosTbSeg;
    regs.r_ax = 0x3800;
#endif
    if (!GEN_RM_INTERRUPT (0x21, &regs))
       return (0);
#endif
    if (regs.r_flags & CARRY_BIT)
       return (0);
#if (DOSX & PHARLAP)
    ReadRealMem (&_country_info, _watt_dosTbr, sizeof(_country_info));
#elif (DOSX & (DOS4GW|WDOSX))
    memcpy (&_country_info, SEG_OFS_TO_LIN(_watt_dosTbSeg,0), sizeof(_country_info));
#endif
    return (regs.r_bx);
  }
  return (0);
}

/*--------------------------------------------------------------*/

int GetCodePage (void)
{
  if ((_osmajor << 8) + _osminor >= 0x303)
  {
#if (DOSX & (PHARLAP|DOS4GW|WDOSX)) || (DOSX == 0)
    IREGS regs;

  #if (DOSX == 0)
    _fmemset (&regs, 0, sizeof(regs));
    regs.r_ax = 0x6601;
    GEN_RM_INTERRUPT (0x21, &regs);
  #else
    memset (&regs, 0, sizeof(regs));
    regs.r_ax = 0x6601;
    if (!GEN_RM_INTERRUPT (0x21, &regs))
       return (0);
  #endif
    if (regs.r_flags & CARRY_BIT)
       return (0);
    return (regs.r_bx);

#else
  #error Unsupported target
#endif
  }
  return (0);
}
