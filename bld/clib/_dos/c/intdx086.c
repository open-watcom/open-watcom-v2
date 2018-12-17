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

extern short    DoDosxCall( void *in, void *out, void *sr );
#if !defined(__BIG_DATA__)
#pragma aux DoDosxCall = \
        "push bp"        /* ----------. */ \
        "push es"        /* ---------.| */ \
        "push bx"        /* --------.|| */ \
        "push ds"        /* -------.||| */ \
        "push dx"        /* ------.|||| */ \
        "mov  es,[bx]"   /*       ||||| */ \
        "mov  bp,6[bx]"  /*       ||||| */ \
        "mov  ax,0[di]"  /*       ||||| */ \
        "mov  bx,2[di]"  /*       ||||| */ \
        "mov  cx,4[di]"  /*       ||||| */ \
        "mov  dx,6[di]"  /*       ||||| */ \
        "mov  si,8[di]"  /*       ||||| */ \
        "mov  di,10[di]" /*       ||||| */ \
        "mov  ds,bp"     /*       ||||| */ \
        "clc"            /*       ||||| */ \
        "int 21h"        /*       ||||| */ \
        "push ds"        /* -----.||||| */ \
        "push di"        /* ----.|||||| */ \
        "mov  bp,sp"     /*     ||||||| */ \
        "mov  di,4[bp]"  /*     ||||||| */ \
        "mov  ds,6[bp]"  /*     ||||||| */ \
        "mov  0[di],ax"  /*     ||||||| */ \
        "mov  2[di],bx"  /*     ||||||| */ \
        "mov  4[di],cx"  /*     ||||||| */ \
        "mov  6[di],dx"  /*     ||||||| */ \
        "mov  8[di],si"  /*     ||||||| */ \
        "pop  10[di]"    /* ----'|||||| */ \
        "pop  ax"        /*(ds) -'||||| */ \
        "pop  bx"        /* ------'|||| */ \
        "pop  bx"        /* -------'||| */ \
        "pop  bx"        /* --------'|| */ \
        "mov  6[bx],ax"  /*          || */ \
        "mov  [bx],es"   /*          || */ \
        "sbb  ax,ax"     /*          || */ \
        "pop  es"        /* ---------'| */ \
        "pop  bp"        /* ----------' */ \
    __parm __caller [__di] [__dx] [__bx] \
    __value         [__ax] \
    __modify        [__bx __cx __dx __si __di]
#else
#pragma aux DoDosxCall = \
        "push ds"           \
        "mov  ds,di"        \
        "push bp"           \
        "mov  es,0[bx]"     \
        "mov  bp,6[bx]"     \
        "push dx"           \
        "push ax"           \
        "push ds"           \
        "push bx"           \
        "mov  ds,cx"        \
        "mov  ax,0[si]"     \
        "mov  bx,2[si]"     \
        "mov  cx,4[si]"     \
        "mov  dx,6[si]"     \
        "mov  di,10[si]"    \
        "mov  si,8[si]"     \
        "mov  ds,bp"        \
        "clc"               \
        "int 21h"           \
        "push ds"           \
        "push si"           \
        "mov  bp,sp"        \
        "mov  si,8[bp]"     \
        "mov  ds,10[bp]"    \
        "pop  bp"           \
        "mov  0[si],ax"     \
        "mov  2[si],bx"     \
        "mov  4[si],cx"     \
        "mov  6[si],dx"     \
        "mov  8[si],bp"     \
        "mov  10[si],di"    \
        "sbb  ax,ax"        \
        "pop  bx"           \
        "pop  si"           \
        "pop  ds"           \
        "mov  0[si],es"     \
        "mov  6[si],bx"     \
        "pop  bx"           \
        "pop  bx"           \
        "pop  bp"           \
        "pop  ds"           \
    __parm __caller [__si __cx] [__ax __dx] [__bx __di] \
    __value         [__ax] \
    __modify        [__di __es]
#endif

_WCRTLINK int intdosx( union REGS *inregs, union REGS *outregs, struct SREGS *segregs )
{
    register short          status;

    status = DoDosxCall( inregs, outregs, segregs );
    outregs->x.cflag = status;
    _dosretax( outregs->x.ax, status );
    return( outregs->x.ax );
}
