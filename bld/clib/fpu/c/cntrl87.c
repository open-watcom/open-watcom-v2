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
#include <math.h>
#include "rtdata.h"

extern  void    __fstcw();
extern  void    __fldcw();

#if defined(__WINDOWS__) && !defined(__WINDOWS_386__)

extern __far _fpmath;

void __win87em_fldcw(unsigned int);
#pragma aux __win87em_fldcw = \
        "push   bx"                                     \
        "mov    bx, 4h"                                 \
        "call   far _fpmath"                            \
        "pop    bx"                                     \
        parm [ax]

unsigned int __win87em_fstcw(void);
#pragma aux __win87em_fstcw = \
        "push   bx"                                     \
        "mov    bx, 5h"                                 \
        "call   far _fpmath"                            \
        "pop    bx"                                     \
        value [ax]

#endif

#if defined(__386__)
#pragma aux     __fstcw = 0x36 0xd9 0x3f 0x9b parm caller [edi];
                        /* fstcw ss:[edi] */
                        /* fwait         */
#pragma aux     __fldcw = 0x36 0xd9 0x2f 0x9b parm caller [edi];
                        /* fldcw ss:[edi] */
                        /* fwait         */
#else
#pragma aux     __fstcw = 0x95                  /* xchg ax,bp */\
                          float 0x9b 0xd9 0x7e 0x00  /* fstcw 0[bp]*/\
                          float 0x9b 0xd9 0xd0  /* fnop       */\
                          0x95                  /* xchg ax,bp */\
                          parm caller [ax];
#pragma aux     __fldcw = 0x95                  /* xchg ax,bp */\
                          float 0x9b 0xd9 0x6e 0x00  /* fldcw 0[bp]*/\
                          float 0x9b 0xd9 0xd0  /* fnop       */\
                          0x95                  /* xchg ax,bp */\
                          parm caller [ax];
#endif

_WCRTLINK unsigned _control87( unsigned new, unsigned mask )
/**********************************************************/
{
    auto short unsigned int control_word;

    control_word = 0;
    if( _RWD_8087 ) {
        __fstcw( &control_word );
#if defined(__WINDOWS__) && !defined(__WINDOWS_386__)
        control_word = __win87em_fstcw();
#endif
        if( mask != 0 ) {
            control_word = (control_word & ~mask) | (new & mask);
            __fldcw( &control_word );
            __fstcw( &control_word );               /* 17-sep-91 */
#if defined(__WINDOWS__) && !defined(__WINDOWS_386__)
            __win87em_fldcw(control_word);
#endif
        }
    }
    return( control_word );
}
