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


#include "dll.h"        // needs to be first
#include "variety.h"
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "heap.h"
#include "heapacc.h"


#if defined(__386__)
#define STOSW   0x66 0xab
#define _DI     edi
#define _CX     ecx
#else
#define STOSW   0xab
#define _DI     di
#define _CX     cx
#endif

extern  void    far_memset(void _WCFAR *,int,unsigned);
#pragma aux     far_memset = \
        0xd1 0xe9       /* shr cx,1 */\
        0xf3 STOSW      /* rep stosw */\
        0x11 0xc9       /* adc cx,cx */\
        0xf3 0xaa       /* rep stosb */\
        parm caller [es _DI] [ax] [_CX] \
        modify exact [_DI _CX];



int __HeapSet( __segment seg, unsigned int fill )
{
    farfrlptr       curr;
    heapblk         _WCFAR *p;

    fill |= fill << 8;
    _AccessFHeap();
    for( ; seg != _NULLSEG; seg = p->nextseg ) {
        p = MK_FP( seg, 0 );
        curr = MK_FP( seg, p->freehead.next );
        while( FP_OFF( curr ) != offsetof( heapblk, freehead ) ) {
            far_memset( (void _WCFAR *)( curr + 1 ), fill, curr->len - sizeof( frl ) );
            curr = MK_FP( seg, curr->next );
        }
    }
    _ReleaseFHeap();
    return( _HEAPOK );
}
