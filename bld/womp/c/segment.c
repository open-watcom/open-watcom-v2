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


#include <string.h>
#include "womp.h"
#include "segment.h"
#include "memutil.h"
#include "myassert.h"
#include "fixup.h"

seghdr *SegNew( void ) {
/********************/
    seghdr  *new;

    new = MemAlloc( sizeof( *new ) );
    new->segdef = NULL;
    new->fix_first = NULL;
    new->data = NULL;
    return( new );
}

void SegKill( seghdr *seg ) {
/*************************/
    fixup   *cur;
    fixup   *next;

/**/myassert( seg != NULL );
    SegDetachData( seg );
    cur = seg->fix_first;
    while( cur ) {
        next = cur->next;
        FixKill( cur );
        cur = next;
    }
    MemFree( seg );
}

void SegAllocData( seghdr *seg, uint_16 seg_len ) {
/***********************************************/
/**/myassert( seg != NULL );
    seg->data = (uint_8 *)MemAlloc( seg_len );
    seg->alloc = seg_len;
}

void SegDetachData( seghdr *seg ) {
/*******************************/
/**/myassert( seg != NULL );
    if( seg->data != NULL ) {
        MemFree( seg->data );
        seg->data = NULL;
        seg->alloc = 0;
    }
}

void SegAddFix( seghdr *seg, fixup *fix ) {
/***************************************/
    fixup   **walk;
    uint_32 net_offset;
    uint_32 walk_offset;

/**/myassert( seg != NULL && fix != NULL );
    net_offset = fix->loc_offset;
    walk = &seg->fix_first;
    while( *walk ) {
        walk_offset = (*walk)->loc_offset;
        if( walk_offset == net_offset ) {
                /* duplicate offset, delete old */
            fix->next = (*walk)->next;
            FixKill( *walk );
            *walk = fix;
            return;
        } else if( walk_offset > net_offset ) {
            break;
        }
        walk = &(*walk)->next;
    }
    fix->next = *walk;
    *walk = fix;
}

fixup *SegFindFix( seghdr *seg, uint_32 offset ) {
/**********************************************/
    fixup   *walk;
    uint_32 walk_offset;

/**/myassert( seg != NULL );
    walk = seg->fix_first;
    while( walk ) {
        walk_offset = walk->loc_offset;
        if( walk_offset == offset ) {
            return( walk );
        } else if( walk_offset > offset ) {
            return( NULL );
        }
        walk = walk->next;
    }
    return( NULL );
}
