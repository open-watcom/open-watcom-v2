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

_WCRTLINK int int86x( int intno, union REGS *inr, union REGS *outr, struct SREGS *sr )
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
        intr( intno, &r );
        outr->x.ax = r.x.ax;
        outr->x.bx = r.x.bx;
        outr->x.cx = r.x.cx;
        outr->x.dx = r.x.dx;
        outr->x.si = r.x.si;
        outr->x.di = r.x.di;
        outr->x.cflag = r.x.flags & INTR_CF;
        sr->ds = r.x.ds;
        sr->es = r.x.es;
        return( r.x.ax );
    }

_WCRTLINK int int86( int intno, union REGS *inr, union REGS *outr )
    {
        struct SREGS sr;

        segread( &sr );
        return( int86x( intno, inr, outr, &sr ) );
    }
