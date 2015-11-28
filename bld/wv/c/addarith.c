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


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgmem.h"
#include "mad.h"
#include "dbgovl.h"
#include "addarith.h"


typedef struct alias_info {
    unsigned            seg;
    unsigned            alias;
    struct alias_info   *next;
} alias_info;

extern unsigned         HugeShift( void );

static alias_info               *HeadAliasTbl = NULL;

void AddAliasInfo( unsigned seg, unsigned alias )
{
    alias_info          **owner;
    alias_info          *curr;

    if( alias == 0 ) {
        owner = &HeadAliasTbl;
        for( ;; ) {
            curr = *owner;
            if( curr == NULL ) break;
            if( curr->seg == seg ) {
                (*owner)->next = curr->next;
                _Free( curr );
                return;
            }
            owner = &curr->next;
        }
    } else {
        _Alloc( curr, sizeof( alias_info ) );
        if( curr != NULL ) {
            curr->seg = seg;
            curr->alias = alias;
            curr->next = HeadAliasTbl;
            HeadAliasTbl = curr;
        }
    }
}

void FreeAliasInfo( void )
{
    alias_info      *temp;

    while( HeadAliasTbl != NULL ) {
        temp = HeadAliasTbl->next;
        _Free( HeadAliasTbl );
        HeadAliasTbl = temp;
    }
}

void DeAlias( addr_ptr *a )
{
    alias_info      *cur_blk;

    for( cur_blk = HeadAliasTbl; cur_blk != NULL; cur_blk = cur_blk->next ) {
        if( a->segment == cur_blk->alias ) {
            a->segment = cur_blk->seg;
            return;
        }
    }
}

/*
 * AddrComp -- check if two addr_ptr's end up at the same physical address
 */


int AddrComp( address a, address b )
{
    if( a.sect_id == 0 ) a.sect_id = b.sect_id;
    if( b.sect_id == 0 ) b.sect_id = a.sect_id;
    if( a.sect_id > b.sect_id ) {
        return( 3 );
    } else if( a.sect_id < b.sect_id ) {
        return( -3 );
    } else {
        if( _IsOn( SW_IGNORE_SEGMENTS ) ) {
            return( MADAddrComp( &a, &b, MAF_OFFSET ) );
        } else {
            AddrFix( &a );
            AddrFix( &b );
            DeAlias( &a.mach );
            DeAlias( &b.mach );
            return( MADAddrComp( &a, &b, MAF_FULL ) );
        }
    }
}


/*
 * AddrAdd -- add a scalar to an address
 */

address AddrAdd( address a, long b )
{
    MADAddrAdd( &a, b, MAF_FULL );
    return( a );
}


address AddrAddWrap( address a, long b )
{
    MADAddrAdd( &a, b, MAF_OFFSET );
    return( a );
}


/*
 * AddrDiff -- return the difference between two addresses
 */

long AddrDiff( address a, address b )
{
    AddrFix( &a );
    AddrFix( &b );
    DeAlias( &a.mach );
    DeAlias( &b.mach );
    return( MADAddrDiff( &a, &b, MAF_FULL ) );
}


/*
 * SameAddrSpace - return true if two addresses are in the same address space
 */

bool SameAddrSpace( address a, address b )
{
    if( _IsOn( SW_IGNORE_SEGMENTS ) ) return( true );
    if( a.sect_id == 0 ) a.sect_id = b.sect_id;
    if( b.sect_id == 0 ) b.sect_id = a.sect_id;
    if( a.sect_id != b.sect_id ) return( false );
    AddrFix( &a );
    AddrFix( &b );
    DeAlias( &a.mach );
    DeAlias( &b.mach );
    return( a.mach.segment == b.mach.segment );
}
