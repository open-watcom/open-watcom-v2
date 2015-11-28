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
* Description:  Access to program memory.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgitem.h"
#include "dbgerr.h"
#include "mad.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgmemor.h"
#include "dbgmain.h"
#include "dbgovl.h"
#include "remcore.h"
#include "dbgreg.h"
#include "addarith.h"
#include "dbgevent.h"
#include "dbgupdt.h"


//MAD: convert this stuff to work off of mad_type_handle's ?

static unsigned Sizes[IT_MAX] = {
    0,
    #define pick(e,s,t,n) s,
    #include "_dbgitem.h"
    #undef pick
};

void ChangeMemUndoable( address addr, const void *item, unsigned size )
{
    char                *p;
    const unsigned char *it;
    char                *end;

    if( AdvMachState( ACTION_MODIFY_MEMORY ) ) {
        ChangeMem( addr, item, size );
        it = item;
        end = TxtBuff + TXT_LEN;
        p = Format( TxtBuff, "%s %A", GetCmdName( CMD_MODIFY ), addr );
        for( ; size > 0; --size ) {
            p = StrCopy( ", ", p );
            p = CnvULong( *it++, p, end - p );
        }
        RecordEvent( TxtBuff );
        DbgUpdate( UP_MEM_CHANGE );
        CollapseMachState();
    }
}

unsigned        ProgPeekWrap(address addr,char * buff,unsigned length )
{
    unsigned    peek1,peek2;

    peek1 = ProgPeek( addr, buff, length );
    if( peek1 == 0 || peek1 == length )
        return( peek1 );
    peek2 = ProgPeek( AddrAddWrap( addr, peek1 ), buff+peek1, length-peek1 );
    return( peek1+peek2 );
}

static item_type ItemType( unsigned size )
{
    item_type   i;

    for( i = IT_NIL + 1; i < IT_MAX; ++i ) {
        if( Sizes[i] == size ) {
            return( i );
        }
    }
    return( IT_NIL );
}


unsigned ItemSize( item_type typ )
{
    return( Sizes[typ & IT_TYPE_MASK] );
}


static bool ItemGet( address *addr, item_mach *item, item_type typ )
{
    unsigned    size = Sizes[typ & IT_TYPE_MASK];

    if( typ & IT_DEC )
        addr->mach.offset -= size;
    if( typ & IT_IO ) {
        if( PortPeek( addr->mach.offset, item, size ) != size ) {
            if( typ & IT_ERR ) {
                Error( ERR_NONE, LIT_ENG( ERR_NO_READ_PORT ), *addr );
            }
            return( false );
        }
    } else {
        if( ProgPeek( *addr , item, size ) != size ) {
            if( typ & IT_ERR ) {
                AddrFix( addr );
                Error( ERR_NONE, LIT_ENG( ERR_NO_READ_MEM ), *addr );
            }
            return( false );
        }
    }
    if( typ & IT_INC )
        addr->mach.offset += size;
    return( true );

}


static bool ItemPut( address *addr, const item_mach *item, item_type typ )
{
    unsigned    size = Sizes[typ & IT_TYPE_MASK];

    if( typ & IT_DEC )
        addr->mach.offset -= size;
    if( typ & IT_IO ) {
        if( PortPoke( addr->mach.offset, item, size ) != size ) {
            if( typ & IT_ERR ) {
                Error( ERR_NONE, LIT_ENG( ERR_NO_WRITE_PORT ), *addr );
            }
            return( false );
        }
    } else {
        if( ChangeMem( *addr, item, size ) != size ) {
            if( typ & IT_ERR ) {
                Error( ERR_NONE, LIT_ENG( ERR_NO_WRITE_MEM ), *addr );
            }
            return( false );
        }
    }
    if( typ & IT_INC )
        addr->mach.offset += size;
    return( true );

}

static item_type ItemTypeFromMADType( mad_type_handle th )
{
    mad_type_info       mti;

    MADTypeInfo( th, &mti );
    switch( mti.b.kind ) {
    case MTK_INTEGER:
    case MTK_XMM:
    case MTK_CUSTOM:
        return( ItemType( mti.b.bits / BITS_PER_BYTE ) );
    case MTK_ADDRESS:
        switch( mti.b.bits ) {
        case 16:
            return( IT_SO );
        case 32:
            if( mti.a.seg.bits == 0 ) {
                return( IT_LO );
            } else {
                return( IT_SA );
            }
        case 48:
            return( IT_LA );
        }
        break;
    case MTK_FLOAT:
        switch( mti.b.bits ) {
        case 32:
            return( IT_SF );
        case 64:
            return( IT_LF );
        case 80:
            return( IT_XF );
        }
        break;
    }
    return( IT_NIL );
}

item_type ItemGetMAD( address *addr, item_mach *item, item_type ops, mad_type_handle th )
{
    item_type   it;

    it = ItemTypeFromMADType( th );
    if( !ItemGet( addr, item, it | ops ) )
        return( IT_NIL );
    return( it );
}

item_type ItemPutMAD( address *addr, const item_mach *item, item_type ops, mad_type_handle th )
{
    item_type   it;

    it = ItemTypeFromMADType( th );
    if( !ItemPut( addr, item, it | ops ) )
        return( IT_NIL );
    return( it );
}
