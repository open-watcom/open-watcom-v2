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
    mheapptr mhp1;
    frlptr   curr_frl;

    mhp1 = start;
    amount -= sizeof( miniheapblkp ) + TAG_SIZE;
    __nheapbeg = mhp1;
    mhp1->len  = amount + sizeof( miniheapblkp );
    mhp1->prev = NULL;
    mhp1->next = NULL;
    mhp1->rover = &mhp1->freehead;
    mhp1->freehead.prev = &mhp1->freehead;
    mhp1->freehead.next = &mhp1->freehead;
    mhp1->numalloc = 0;
    mhp1->numfree = 0;
    mhp1++;
    curr_frl = (frlptr)mhp1;
    /* fix up end of heap links */
    SET_BLK_END( (frlptr)( (PTR)curr_frl + amount ) );
    /* build a block for _nfree() */
    SET_BLK_SIZE_INUSE( curr_frl, amount );
    __nheapbeg->numalloc++;
    __nheapbeg->largest_blk = ~0;    /* set to largest value to be safe */
    _nfree( (void _WCNEAR *)FRL2CPTR( curr_frl ) );
}
