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
#include "tinyio.h"

#if !defined(__WINDOWS_386__)
extern  int                     _dosretax( int eax, int carry );

extern  int                     DoDosCall( void *in, void *out );
#pragma aux                     DoDosCall = \
        "push ebp"      \
        "push edx"      \
        "mov eax,[edi]" \
        "mov ebx,4[edi]"\
        "mov ecx,8[edi]"\
        "mov edx,12[edi]"\
        "mov esi,16[edi]"\
        "mov edi,20[edi]"\
        "clc"           \
        _INT_21         \
        "mov ebp,edi"   \
        "pop edi"       \
        "mov [edi],eax" \
        "mov 4[edi],ebx"\
        "mov 8[edi],ecx"\
        "mov 12[edi],edx"\
        "mov 16[edi],esi"\
        "mov 20[edi],ebp"\
        "sbb eax,eax"   \
        "pop ebp"       \
        parm caller     [edi] [edx] \
        value           [eax] \
        modify          [ebx ecx edx esi edi];

#else
#include <stddef.h>
extern int __pascal _clib_intdos( union REGS *, union REGS * );
#endif

_WCRTLINK int intdos( union REGS *inregs, union REGS *outregs )
{
#if !defined(__WINDOWS_386__)
    int status;

    status = DoDosCall( inregs, outregs );
    outregs->x.cflag = status;
    _dosretax( outregs->x.eax, status );
    return( outregs->x.eax );
#else
    return( _clib_intdos( inregs, outregs ) );
#endif
}
