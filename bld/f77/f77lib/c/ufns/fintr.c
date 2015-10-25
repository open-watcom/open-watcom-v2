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


//
// INTR         : execute DOS interrupt
//

#include "ftnstd.h"

#include <stddef.h>
#include <dos.h>
#include "external.h"


#if defined( __386__ ) && !defined( __WINDOWS__ )

void    __fortran FINTR( intstar4 *int_no, intstar4 *regs ) {
//=========================================================

    union REGPACK       _regs;

    _regs.x.eax = regs[0];
    _regs.x.ebx = regs[1];
    _regs.x.ecx = regs[2];
    _regs.x.edx = regs[3];
    _regs.x.ebp = regs[4];
    _regs.x.edi = regs[5];
    _regs.x.esi = regs[6];
    _regs.x.ds = regs[7] & 0x0000ffff;
    _regs.x.fs = ( regs[7] & 0xffff0000 ) >> 16;
    _regs.x.es = regs[8] & 0x0000ffff;
    _regs.x.gs = ( regs[8] & 0xffff0000 ) >> 16;
    _regs.x.flags = regs[9];
    intr( *int_no, &_regs );
    regs[0] = _regs.x.eax;
    regs[1] = _regs.x.ebx;
    regs[2] = _regs.x.ecx;
    regs[3] = _regs.x.edx;
    regs[4] = _regs.x.ebp;
    regs[5] = _regs.x.edi;
    regs[6] = _regs.x.esi;
    regs[7] = ( _regs.x.fs << 16 ) | _regs.x.ds;
    regs[8] = ( _regs.x.gs << 16 ) | _regs.x.es;
    regs[9] = _regs.x.flags;
}


#else

void    __fortran FINTR( intstar4 *int_no, intstar4 *regs ) {
//=========================================================

    union REGPACK       _regs;

    _regs.w.ax = regs[0];
    _regs.w.bx = regs[1];
    _regs.w.cx = regs[2];
    _regs.w.dx = regs[3];
    _regs.w.bp = regs[4];
    _regs.w.di = regs[5];
    _regs.w.si = regs[6];
    _regs.w.ds = regs[7];
    _regs.w.es = regs[8];
    _regs.w.flags = regs[9];
    intr( *int_no, &_regs );
    regs[0] = _regs.w.ax;
    regs[1] = _regs.w.bx;
    regs[2] = _regs.w.cx;
    regs[3] = _regs.w.dx;
    regs[4] = _regs.w.bp;
    regs[5] = _regs.w.di;
    regs[6] = _regs.w.si;
    regs[7] = _regs.w.ds;
    regs[8] = _regs.w.es;
    regs[9] = _regs.w.flags;
}

#endif
