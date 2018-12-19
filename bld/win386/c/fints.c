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
* Description:  DOS interrupt thunks for Win386 extender.
*
****************************************************************************/


#include <dos.h>
#include "fints.h"
#include "dosret.h"
#include "dointr.h"


extern int  BDDoDosCall( union REGS __far *, union REGS __far * );
#pragma aux BDDoDosCall =                         \
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

extern int  BDDoDosxCall( union REGS __far *, union REGS __far *, struct SREGS __far * );
#pragma aux BDDoDosxCall = \
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

void _fintr( int intno, union REGPACK __far *regs )
{
    _DoINTR( intno, regs, 0 );
}

void _fintrf( int intno, union REGPACK __far *regs )
{
    _DoINTR( intno, regs, regs->w.flags );
}

int _fintdos( union REGS __far *inregs, union REGS __far *outregs )
{
    int             status;

    status = BDDoDosCall( inregs, outregs );
    outregs->x.cflag = (status & 1);
    _dosretax( outregs->x.ax, status );
    return( outregs->x.ax );
}

int _fintdosx( union REGS __far *inregs, union REGS __far *outregs, struct SREGS __far *segregs )
{
    int             status;

    status = BDDoDosxCall( inregs, outregs, segregs );
    outregs->x.cflag = (status & 1);
    _dosretax( outregs->x.ax, status );
    return( outregs->x.ax );
}

int _fint86x( int intno, union REGS __far *inr, union REGS __far *outr, struct SREGS __far *sr )
{
    union REGPACK r;

    r.x.ax = inr->x.ax;
    r.x.bx = inr->x.bx;
    r.x.cx = inr->x.cx;
    r.x.dx = inr->x.dx;
    r.x.si = inr->x.si;
    r.x.di = inr->x.di;
    r.x.ds = sr->ds;
    r.x.es = sr->es;
//    r.x.bp = 0;             /* no bp in REGS union, set to 0 */
//    r.x.flags = ( inr->x.cflag ) ? INTR_CF : 0;

    _DoINTR( intno, &r, 0 );

    outr->x.ax = r.x.ax;
    outr->x.bx = r.x.bx;
    outr->x.cx = r.x.cx;
    outr->x.dx = r.x.dx;
    outr->x.si = r.x.si;
    outr->x.di = r.x.di;
    outr->x.cflag = ( (r.x.flags & INTR_CF) != 0 );
    sr->ds = r.x.ds;
    sr->es = r.x.es;
    return( r.x.ax );
}

int _fint86( int intno, union REGS __far *inr, union REGS __far *outr )
{
#ifdef DLL32
    static struct SREGS sr;
#else
    struct SREGS        sr;
#endif

    segread( &sr );
    return( _fint86x( intno, inr, outr, (struct SREGS __far *) &sr ) );
}
