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
* Description:  Init/Fini routines declarations.
*
****************************************************************************/


#ifndef _INITFINI_H_INCLUDED
#define _INITFINI_H_INCLUDED

#include "rtinit.h"

#if defined( _M_I86 )
    #define PARM1REG      __ax
    #define PARM2REG      __dx
#elif defined( _M_IX86 )
    #define PARM1REG      __eax
    #define PARM2REG      __edx
#else
    #define PARM1REG
    #define PARM2REG
#endif

extern void __InitRtns( unsigned );
#if defined( _M_IX86 )
  #pragma aux __InitRtns "*" __parm [PARM1REG]
#endif
// - takes priority limit parm in PARM1REG
//      code will run init routines whose
//      priority is <= PARM1REG (really [0-255])
//      PARM1REG==255 -> run all init routines
//      PARM1REG==15  -> run init routines whose priority is <= 15
#if defined( _M_I86 )
  extern void __far __FInitRtns( unsigned );
  #pragma aux __FInitRtns "*" __parm [PARM1REG]
#endif

extern void __FiniRtns( unsigned, unsigned );
#if defined( _M_IX86 )
  #pragma aux __FiniRtns "*" __parm [PARM1REG] [PARM2REG]
#endif
// - takes priority limit range in PARM1REG, PARM2REG
//      code will run fini routines whose
//      priority is >= PARM1REG (really [0-255]) and
//                  <= PARM2REG (really [0-255])
//      PARM1REG==0 ,PARM2REG==255 -> run all fini routines
//      PARM1REG==16,PARM2REG==255 -> run fini routines in range 16...255
//      PARM1REG==16,PARM2REG==40  -> run fini routines in range 16...40
#if defined( _M_I86 )
  extern void __far __FFiniRtns( unsigned, unsigned );
  #pragma aux __FFiniRtns "*" __parm [PARM1REG] [PARM2REG]
#endif

#if defined(__OS2__) && defined(__386__)
  #define EXIT_PRIORITY_CLIB              0x00009F00
#endif

#undef PARM1REG
#undef PARM2REG

#endif
