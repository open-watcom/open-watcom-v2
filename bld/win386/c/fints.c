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


extern  int                     BDDoDosCall( union REGS __far *, union REGS __far * );
extern  int                     BDDoDosxCall( union REGS __far *, union REGS __far *, struct SREGS __far * );
extern  int                     _dosretax( int, int );

#pragma aux               BDDoDosCall =                         \
        0x1e           /* push ds */                            \
        0x06           /* push es         */                    \
        0x55           /* push bp         */                    \
        0x51           /* push cx         */                    \
        0x8e 0xda      /* mov ds,dx */                          \
        0x89 0xdd      /* mov bp, bx      */                    \
        0x8b 0x44 0x00 /* mov ax, 0[ si ] */                    \
        0x8b 0x5c 0x02 /* mov bx, 2[ si ] */                    \
        0x8b 0x4c 0x04 /* mov cx, 4[ si ] */                    \
        0x8b 0x54 0x06 /* mov dx, 6[ si ] */                    \
        0x8b 0x7c 0x0a /* mov di, a[ si ] */                    \
        0x8b 0x74 0x08 /* mov si, 8[ si ] */                    \
        0xf8           /* clc             */                    \
        0xcd 0x21      /* int 021h        */                    \
        0x87 0xf5      /* xchg si, bp     */                    \
        0x1f           /* pop ds          */                    \
        0x89 0x44 0x00 /* mov 0[ si ], ax */                    \
        0x89 0x5c 0x02 /* mov 2[ si ], bx */                    \
        0x89 0x4c 0x04 /* mov 4[ si ], cx */                    \
        0x89 0x54 0x06 /* mov 6[ si ], dx */                    \
        0x89 0x6c 0x08 /* mov 8[ si ], bp */                    \
        0x89 0x7c 0x0a /* mov a[ si ], di */                    \
        0x5d           /* pop bp          */                    \
        0x07           /* pop es          */                    \
        0x1f           /* pop ds */                             \
        0x19 0xc0      /* sbb ax, ax      */                    \
        parm caller [ si dx ] [ bx cx ]                         \
        value[ ax ]                                             \
        modify [ ax dx di ];

#pragma aux                BDDoDosxCall =            \
        0x1e            /* push ds */                \
        0x8e 0xdf       /* mov ds,di */              \
        0x55            /* push bp         */        \
        0x8e 0x07       /* mov es, [ bx ]  */        \
        0x8b 0x6f 0x06  /* mov bp, 6[ bx ] */        \
        0x52            /* push dx         */        \
        0x50            /* push ax         */        \
        0x1e            /* push ds         */        \
        0x53            /* push bx         */        \
        0x8e 0xd9       /* mov ds, cx      */        \
        0x8b 0x04       /* mov ax, [ si ]  */        \
        0x8b 0x5c 0x02  /* mov bx, 2[ si ] */        \
        0x8b 0x4c 0x04  /* mov cx, 4[ si ] */        \
        0x8b 0x54 0x06  /* mov dx, 6[ si ] */        \
        0x8b 0x7c 0x0a  /* mov di, a[ si ] */        \
        0x8b 0x74 0x08  /* mov si, 8[ si ] */        \
        0x8e 0xdd       /* mov ds, bp      */        \
        0xf8            /* clc             */        \
        0xcd 0x21       /* int 21          */        \
        0x1e            /* push ds         */        \
        0x56            /* push si         */        \
        0x89 0xe5       /* mov bp, sp      */        \
        0x8b 0x76 0x08  /* mov si, 8[ bp ] */        \
        0x8e 0x5e 0x0a  /* mov ds, a[ bp ] */        \
        0x5d            /* pop bp          */        \
        0x89 0x04       /* mov [ si ], ax  */        \
        0x89 0x5c 0x02  /* mov 2[ si ], bx */        \
        0x89 0x4c 0x04  /* mov 4[ si ], cx */        \
        0x89 0x54 0x06  /* mov 6[ si ], dx */        \
        0x89 0x6c 0x08  /* mov 8[ si ], bp */        \
        0x89 0x7c 0x0a  /* mov a[ si ], di */        \
        0x19 0xc0       /* sbb ax, ax      */        \
        0x5b            /* pop bx          */        \
        0x5e            /* pop si          */        \
        0x1f            /* pop ds          */        \
        0x8c 0x04       /* mov [ si ], es  */        \
        0x89 0x5c 0x06  /* mov 6[ si ], bx */        \
        0x5b            /* pop bx          */        \
        0x5b            /* pop bx          */        \
        0x5d            /* pop bp          */        \
        0x1f            /* pop ds */                 \
        parm caller [ si cx ] [ ax dx ] [ bx di ]    \
        value [ ax ]                                 \
        modify [ di es ];

extern  void            _DoINTR( int, union REGPACK __far * );
#pragma aux             _DoINTR parm [bx] [ax dx] modify [cx si di es];

void _fintr( int intno, union REGPACK __far *regs )
{
    _DoINTR( intno, regs );
}

int _fintdos( union REGS __far *inregs, union REGS __far *outregs )
{
    int             status;

    status = BDDoDosCall( inregs, outregs );
    outregs->x.cflag = status;
    _dosretax( outregs->x.ax, status );
    return( outregs->x.ax );
}

int _fintdosx( union REGS __far *inregs, union REGS __far *outregs, struct SREGS __far *segregs )
{
    int             status;

    status = BDDoDosxCall( inregs, outregs, segregs );
    outregs->x.cflag = status;
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
    _fintr( intno, (union REGPACK __far *) &r );
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
