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

#if !defined(__WINDOWS_386__)

extern  int                     _dosretax( int eax, int carry );

extern  int                     DoDosxCall( void *in, void *out, void *sr );
#pragma aux                     DoDosxCall = \
        0x55            /* push ebp        -----. */\
        0x06            /* push es         ----.| */\
        0x53            /* push ebx        ---.|| */\
        0x1e            /* push ds         --.||| */\
        0x52            /* push edx        -.|||| */\
        0x8e 0x03       /* mov es,[ebx]     ||||| */\
        0x66            /* opndsize:        ||||| */\
        0x8b 0x6b 0x06  /* mov bp,6[ebx]    ||||| 07-sep-89*/\
        0x8b 0x07       /* mov eax, [edi]   ||||| */\
        0x8b 0x5f 0x04  /* mov ebx,4[edi]   ||||| */\
        0x8b 0x4f 0x08  /* mov ecx,8[edi]   ||||| */\
        0x8b 0x57 0x0c  /* mov edx,12[edi]  ||||| */\
        0x8b 0x77 0x10  /* mov esi,16[edi]  ||||| */\
        0x8b 0x7f 0x14  /* mov edi,20[edi]  ||||| */\
        0x8e 0xdd       /* mov ds,bp        ||||| */\
        0xf8            /* clc              ||||| */\
        0xcd 0x21       /* int 021h         ||||| */\
        0x1e            /* push ds       --.||||| */\
        0x57            /* push edi      -.|||||| */\
        0x89 0xe5       /* mov ebp,esp    ||||||| */\
        0x8b 0x7d 0x08  /* mov edi,8[ebp] ||||||| 07-sep-89*/\
        0x8e 0x5d 0x0c  /* mov ds,12[ebp] ||||||| 07-sep-89*/\
        0x89 0x07       /* mov  [edi],eax ||||||| */\
        0x89 0x5f 0x04  /* mov 4[edi],ebx ||||||| */\
        0x89 0x4f 0x08  /* mov 8[edi],ecx ||||||| */\
        0x89 0x57 0x0c  /* mov 12[edi],edx||||||| */\
        0x89 0x77 0x10  /* mov 16[edi],esi||||||| */\
        0x8f 0x47 0x14  /* pop 20[edi]   -'|||||| */\
        0x58            /* pop eax (ds)  --'||||| */\
        0x5b            /* pop ebx       ---'|||| */\
        0x5b            /* pop ebx       ----'||| */\
        0x5b            /* pop ebx       -----'|| */\
        0x66            /* opndsize:           || */\
        0x89 0x43 0x06  /* mov 6[ebx],ax       || */\
        0x8c 0x03       /* mov [ebx],es        || */\
        0x1b 0xc0       /* sbb eax,eax         || */\
        0x07            /* pop es  ------------'| */\
        0x5d            /* pop ebp -------------' */\
        parm caller     [edi] [edx] [ebx] \
        value           [eax] \
        modify          [ebx ecx edx esi edi];
#else
#include <stddef.h>
extern int pascal _clib_intdosx( union REGS *, union REGS *, struct SREGS * );
#endif


_WCRTLINK int intdosx( union REGS *inregs, union REGS *outregs, struct SREGS *segregs )
{
#if !defined(__WINDOWS_386__)
    register int            status;

    status = DoDosxCall( inregs, outregs, segregs );
    outregs->x.cflag = status;
    _dosretax( outregs->x.eax, status );
    return( outregs->x.eax );
#else
    return( _clib_intdosx( inregs, outregs, segregs ) );
#endif
}
