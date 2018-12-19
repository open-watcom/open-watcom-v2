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

extern short    DoDosCall( void *in, void *out );
#if !defined(__BIG_DATA__)
#pragma aux DoDosCall = \
        "push es"           \
        "push bp"           \
        "push dx"           \
        "mov  ax,0[di]"     \
        "mov  bx,2[di]"     \
        "mov  cx,4[di]"     \
        "mov  dx,6[di]"     \
        "mov  si,8[di]"     \
        "mov  di,10[di]"    \
        "clc"               \
        "int 21h"           \
        "mov  bp,di"        \
        "pop  di"           \
        "mov  0[di],ax"     \
        "mov  2[di],bx"     \
        "mov  4[di],cx"     \
        "mov  6[di],dx"     \
        "mov  8[di],si"     \
        "mov  10[di],bp"    \
        "sbb  ax,ax"        \
        "pop  bp"           \
        "pop  es"           \
    __parm __caller [__di] [__dx] \
    __value         [__ax] \
    __modify        [__bx __cx __dx __di __si]
#else
#pragma aux DoDosCall = \
        "push ds"           \
        "push es"           \
        "push bp"           \
        "push cx"           \
        "mov  ds,dx"        \
        "mov  bp,bx"        \
        "mov  ax,0[si]"     \
        "mov  bx,2[si]"     \
        "mov  cx,4[si]"     \
        "mov  dx,6[si]"     \
        "mov  di,10[si]"    \
        "mov  si,8[si]"     \
        "clc"               \
        "int 21h"           \
        "xchg si,bp"        \
        "pop  ds"           \
        "mov  0[si],ax"     \
        "mov  2[si],bx"     \
        "mov  4[si],cx"     \
        "mov  6[si],dx"     \
        "mov  8[si],bp"     \
        "mov  10[si],di"    \
        "pop  bp"           \
        "pop  es"           \
        "pop  ds"           \
        "sbb  ax,ax"        \
    __parm __caller [__dx __si] [__cx __bx] \
    __value         [__ax] \
    __modify        [__ax __dx __di]
#endif

_WCRTLINK int intdos( union REGS *inregs, union REGS *outregs )
{
    register short          status;

    status = DoDosCall( inregs, outregs );
    outregs->x.cflag = (status & 1);
    _dosretax( outregs->x.ax, status );
    return( outregs->x.ax );
}
