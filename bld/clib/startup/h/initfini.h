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

#if defined(__386__)
    #define __PARMREG1      eax
    #define __PARMREG2      edx
#elif defined( _M_I86 )
    #define __PARMREG1      ax
    #define __PARMREG2      dx
#else
    #define __PARMREG1
    #define __PARMREG2
#endif

extern void __InitRtns( unsigned );
#if defined(_M_IX86)
  #pragma aux __InitRtns "*" parm [__PARMREG1]
#endif
// - takes priority limit parm in __PARMREG1
//      code will run init routines whose
//      priority is <= __PARMREG1 (really [0-255])
//      __PARMREG1==255 -> run all init routines
//      __PARMREG1==15  -> run init routines whose priority is <= 15
#if defined(_M_I86)
  extern void _WCI86FAR __FInitRtns(unsigned);
  #pragma aux __FInitRtns "*" parm [__PARMREG1]
#endif

extern void __FiniRtns(unsigned,unsigned);
#if defined(_M_IX86)
  #pragma aux __FiniRtns "*" parm [__PARMREG1] [__PARMREG2]
#endif
// - takes priority limit range in __PARMREG1, __PARMREG2
//      code will run fini routines whose
//      priority is >= __PARMREG1 (really [0-255]) and
//                  <= __PARMREG2 (really [0-255])
//      __PARMREG1==0 ,__PARMREG2==255 -> run all fini routines
//      __PARMREG1==16,__PARMREG2==255 -> run fini routines in range 16...255
//      __PARMREG1==16,__PARMREG2==40  -> run fini routines in range 16...40
#if defined(_M_I86)
  extern void _WCI86FAR __FFiniRtns( unsigned, unsigned );
  #pragma aux __FFiniRtns "*" parm [__PARMREG1] [__PARMREG2]
#endif

#if defined(__OS2__) && defined(__386__)
  #define EXIT_PRIORITY_CLIB              0x00009F00
#endif

#undef __PARMREG1
#undef __PARMREG2

#endif
