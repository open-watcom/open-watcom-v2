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
* Description:  Implementation of _fstrset() - far strset().
*
****************************************************************************/


#include "variety.h"
#include <string.h>

#ifdef _M_I86

extern char _WCFAR *fast_strset( char _WCFAR *, char );

#pragma aux fast_strset = \
        0x1e            /* push ds */\
        0x56            /* push si */\
        0x50            /* push ax */\
        0x8e 0xda       /* mov ds,dx */\
        0x89 0xc6       /* mov si,ax */\
        0x88 0xdf       /* mov bh,bl */\
        0xad            /* L1:lodsw */\
        0x84 0xc0       /* test al,al */\
        0x74 0x0c       /* je L3 */\
        0x84 0xe4       /* test ah,ah */\
        0x74 0x05       /* je L2 */\
        0x89 0x5c 0xfe  /* mov -2[si],bx */\
        0xeb 0xf2       /* jmp L1 */\
        0x88 0x5c 0xfe  /* L2:mov -2[si],bl */\
        0x58            /* L3:pop ax */\
        0x5e            /* pop si */\
        0x1f            /* pop ds */\
        parm caller     [dx ax] [bl] \
        value           [dx ax] \
        modify exact    [bh];
#endif

_WCRTLINK char _WCFAR *_fstrset( char _WCFAR *s, int c )
{
//#ifdef _M_I86
    //return( fast_strset( s, c ) );
//#else
    char _WCFAR     *p;

    for( p = s; *p; ++p ) {
        *p = c;
    }
    return( s );
//#endif
}
