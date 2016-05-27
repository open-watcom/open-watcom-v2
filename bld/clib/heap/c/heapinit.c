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
#include <stdlib.h>
#include <malloc.h>
#include "heap.h"

void _WCFAR __HeapInit( void _WCNEAR *start, unsigned int amount )
{
    mheapptr p1;
    tag *last_tag;

    p1 = start;
    amount -= sizeof( miniheapblkp ) + TAG_SIZE;
    __nheapbeg = p1;
    p1->len  = amount + sizeof( miniheapblkp );
    p1->prev = NULL;
    p1->next = NULL;
    p1->rover = &p1->freehead;
    p1->freehead.prev = &p1->freehead;
    p1->freehead.next = &p1->freehead;
    p1->numalloc = 0;
    p1->numfree = 0;
    ++p1;
    /* fix up end of heap links */
    last_tag = (tag *) ( (PTR)p1 + amount );
    last_tag[0] = END_TAG;
    /* build a block for _nfree() */
    ((frlptr)p1)->len = amount | 1;
    ++__nheapbeg->numalloc;
    __nheapbeg->largest_blk = ~0;    /* set to largest value to be safe */
    _nfree( (PTR)p1 + TAG_SIZE );
}
