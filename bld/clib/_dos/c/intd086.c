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

extern  short                   DoDosCall();
extern  short                   BDDoDosCall();
extern  int                     _dosretax();


#pragma aux                     DoDosCall = \
        0x06            /* push es      */ \
        0x55            /* push bp      */ \
        0x52            /* push dx      */ \
        0x8b 0x05       /* mov ax, [di] */ \
        0x8b 0x5d 0x02  /* mov bx,2[di] */ \
        0x8b 0x4d 0x04  /* mov cx,4[di] */ \
        0x8b 0x55 0x06  /* mov dx,6[di] */ \
        0x8b 0x75 0x08  /* mov si,8[di] */ \
        0x8b 0x7d 0x0a  /* mov di,10[di] */ \
        0xf8            /* clc          */ \
        0xcd 0x21       /* int 021h     */ \
        0x89 0xfd       /* mov bp,di    */ \
        0x5f            /* pop di       */ \
        0x89 0x05       /* mov  [di],ax */ \
        0x89 0x5d 0x02  /* mov 2[di],bx */ \
        0x89 0x4d 0x04  /* mov 4[di],cx */ \
        0x89 0x55 0x06  /* mov 6[di],dx */ \
        0x89 0x75 0x08  /* mov 8[di],si */ \
        0x89 0x6d 0x0a  /* mov 10[di],bp */ \
        0x1b 0xc0       /* sbb ax,ax (sets ax!=0 if carry set) */ \
        0x5d            /* pop bp */ \
        0x07            /* pop es */ \
        parm caller     [di] [dx] \
        value           [ax] \
        modify          [bx cx dx si di];

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

_WCRTLINK int intdos( union REGS *inregs, union REGS *outregs )
    {
        register short          status;

#if defined(__BIG_DATA__)
        status = BDDoDosCall( inregs, outregs );
#else
        status = DoDosCall( inregs, outregs );
#endif
        outregs->x.cflag = status;
        _dosretax( outregs->x.ax, status );
        return( outregs->x.ax );
    }
