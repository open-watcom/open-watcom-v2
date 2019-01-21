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
#include <dos.h>
#include "dosret.h"

#if !defined(__WINDOWS_386__)

extern int  DoDosxCall( void *in, void *out, void *sr );
#pragma aux DoDosxCall = \
        "push ebp"         /* -----------. */ \
        "push es"          /* ----------.| */ \
        "push ebx"         /* ---------.|| */ \
        "push ds"          /* --------.||| */ \
        "push edx"         /* -------.|||| */ \
        "mov  es,[ebx]"    /*        ||||| */ \
        "mov  bp,6[ebx]"   /*        ||||| */ \
        "mov  eax,0[edi]"  /*        ||||| */ \
        "mov  ebx,4[edi]"  /*        ||||| */ \
        "mov  ecx,8[edi]"  /*        ||||| */ \
        "mov  edx,12[edi]" /*        ||||| */ \
        "mov  esi,16[edi]" /*        ||||| */ \
        "mov  edi,20[edi]" /*        ||||| */ \
        "mov  ds,ebp"      /*        ||||| */ \
        "clc"              /*        ||||| */ \
        "int 21h"          /*        ||||| */ \
        "push ds"          /* ------.||||| */ \
        "push edi"         /* -----.|||||| */ \
        "mov  ebp,esp"     /*      ||||||| */ \
        "mov  edi,8[ebp]"  /*      ||||||| */ \
        "mov  ds,12[ebp]"  /*      ||||||| */ \
        "mov  0[edi],eax"  /*      ||||||| */ \
        "mov  4[edi],ebx"  /*      ||||||| */ \
        "mov  8[edi],ecx"  /*      ||||||| */ \
        "mov  12[edi],edx" /*      ||||||| */ \
        "mov  16[edi],esi" /*      ||||||| */ \
        "pop  20[edi]"     /* -----'|||||| */ \
        "pop  eax"         /*(ds) --'||||| */ \
        "pop  ebx"         /* -------'|||| */ \
        "pop  ebx"         /* --------'||| */ \
        "pop  ebx"         /* ---------'|| */ \
        "mov  6[ebx],ax"   /*           || */ \
        "mov  0[ebx],es"   /*           || */ \
        "sbb  eax,eax"     /*           || */ \
        "pop  es"          /* ----------'| */ \
        "pop  ebp"         /* -----------' */ \
    __parm __caller [__edi] [__edx] [__ebx] \
    __value         [__eax] \
    __modify        [__ebx __ecx __edx __edi __esi]

#else

#include <stddef.h>
#include "clibxw32.h"

#endif

_WCRTLINK int intdosx( union REGS *inregs, union REGS *outregs, struct SREGS *segregs )
{
#if !defined(__WINDOWS_386__)
    register int            status;

    status = DoDosxCall( inregs, outregs, segregs );
    outregs->x.cflag = (status & 1);
    _dosretax( outregs->x.eax, status );
    return( outregs->x.eax );
#else
    return( _clib_intdosx( inregs, outregs, segregs ) );
#endif
}
