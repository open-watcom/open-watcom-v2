/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include "dointr.h"


_WCRTLINK int int86x( int intno, union REGS *inregs, union REGS *outregs, struct SREGS *segregs )
{
    union REGPACK regs;

    regs.x.ax = inregs->x.ax;
    regs.x.bx = inregs->x.bx;
    regs.x.cx = inregs->x.cx;
    regs.x.dx = inregs->x.dx;
    regs.x.si = inregs->x.si;
    regs.x.di = inregs->x.di;
    regs.x.ds = segregs->ds;
    regs.x.es = segregs->es;
//    regs.x.bp = 0;             /* no bp in REGS union, set to 0 */
//    regs.x.flags = ( inregs->w.cflag ) ? INTR_CF : 0;

    _DoINTR( intno, &regs, 0 );

    outregs->x.ax = regs.x.ax;
    outregs->x.bx = regs.x.bx;
    outregs->x.cx = regs.x.cx;
    outregs->x.dx = regs.x.dx;
    outregs->x.si = regs.x.si;
    outregs->x.di = regs.x.di;
    outregs->x.cflag = ( (regs.x.flags & INTR_CF) != 0 );
    segregs->ds = regs.x.ds;
    segregs->es = regs.x.es;
    return( regs.x.ax );
}

_WCRTLINK int int86( int intno, union REGS *inregs, union REGS *outregs )
{
    struct SREGS segregs;

    segread( &segregs );
    return( int86x( intno, inregs, outregs, &segregs ) );
}
