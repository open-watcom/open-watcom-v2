/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include <math.h>
#include <float.h>
#include "rtdata.h"

#if defined(__AXP__)

/*
 * FPCR Trap Disable Flags
 */
#define FPCR_INEXACT        0x40000000
#define FPCR_UNDERFLOW      0x20000000
#define FPCR_OVERFLOW       0x00080000
#define FPCR_ZERODIVIDE     0x00040000
#define FPCR_INVALID        0x00020000

extern unsigned long _GetFPCR(void);
extern void          _SetFPCR(unsigned long);

static unsigned int MapToCW(unsigned long fpcr)
{
    unsigned int cw;

    /*
     * The rounding bits are identical but in the highword of the fpcr.
     */
    cw = (fpcr >> 16) & ~(_MCW_RC);

    if (fpcr & FPCR_INEXACT)
        cw &= ~_EM_INEXACT;

    if (fpcr & FPCR_ZERODIVIDE)
        cw &= ~_EM_ZERODIVIDE;

    if (fpcr & FPCR_OVERFLOW)
        cw &= ~_EM_OVERFLOW;

    if (fpcr & FPCR_UNDERFLOW)
        cw &= ~_EM_UNDERFLOW;

    if (fpcr & FPCR_INVALID)
        cw &= ~_EM_INVALID;

    return cw;
} /* MapToCW() */


static unsigned long MapFromCW(unsigned int cw)
{
    unsigned long fpcr = 0L;

    /*
     * The rounding bits are identical but in the highword of the fpcr.
     */
    fpcr = (cw & ~_MCW_RC) << 16;

    if (!(cw & _EM_INEXACT))
        fpcr |= FPCR_INEXACT;

    if (!(cw & _EM_INVALID))
        fpcr |= FPCR_INVALID;

    if (!(cw & _EM_ZERODIVIDE))
        fpcr |= FPCR_ZERODIVIDE;

    if (!(cw & _EM_OVERFLOW))
        fpcr |= FPCR_OVERFLOW;

    if (!(cw & _EM_UNDERFLOW))
        fpcr |= FPCR_UNDERFLOW;

    return fpcr;
} /* MapFromCW() */
#endif


_WCRTLINK unsigned _controlfp(unsigned new, unsigned mask)
{
#if defined(_M_IX86)
    return _control87(new, mask);               /* JBS 99/09/16 */
#elif defined(__AXP__)
    unsigned int  cw;

    cw = MapToCW(_GetFPCR());

    if (mask)
    {
        cw = (cw & ~mask) | (new & mask);
        _SetFPCR(MapFromCW(cw));
    }

    return cw;
#elif defined(__PPC__)
    // No idea yet
#endif
} /* _controlfp() */
