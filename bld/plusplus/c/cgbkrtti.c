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


#include <stddef.h>

#include "plusplus.h"
#include "cgback.h"
#include "cgbackut.h"
#include "ring.h"
#include "rtti.h"

static void dropTypeidRef( TYPE type )
{
    SYMBOL sym;

    DbgAssert( StructType( type ) != NULL );
    sym = TypeidAccess( type );
    TypeidRef( sym );
    DgPtrSymData( sym );
}

static void doRttiGen( RTTI_CLASS *r )
{
    SYMBOL sym;
    RTTI_VFPTR *vfptr;
    TYPE class_type;
    segment_id old_seg;
    target_offset_t delta_to_descriptor;
    unsigned leap_control;
    unsigned nleaps;
    GEN_LEAP *leaps;
    GEN_LEAP *leap;
    DbgStmt( target_offset_t offset );

    DbgAssert( r->done );
    if( r->cg_gen || ! r->gen ) {
        return;
    }
    sym = r->sym;
    if( r->too_big ) {
        old_seg = BESetSeg( sym->segid );
        DGLabel( FEBack( sym ) );
        DgByte( 0 );
        BESetSeg( old_seg );
        return;
    }
    class_type = r->class_type;
    nleaps = ScopeRttiLeaps( class_type, &leaps );
    DbgAssert( nleaps >= 1 );
    old_seg = BESetSeg( sym->segid );
    DGLabel( FEBack( sym ) );
    DbgStmt( offset = 0 );
    RingIterBeg( r->vfptrs, vfptr ) {
        DbgAssert( vfptr->offset == offset );
        DbgAssert( r->offset > offset );
        delta_to_descriptor = r->offset - vfptr->offset;
        DbgAssert(( delta_to_descriptor & RA_RUNTIME ) == 0 );
        DgOffset( delta_to_descriptor | ( vfptr->control & RA_RUNTIME ) );
        DgOffset( vfptr->delta );
        DbgStmt( offset += RttiAdjustSize() );
    } RingIterEnd( vfptr )
    /* class descriptor */
    dropTypeidRef( class_type );
    if( nleaps == 1 ) {
        DgOffset( -1 );
    } else {
        DbgAssert( class_type->u.c.info->vb_offset != (target_offset_t) -1 );
        DgOffset( class_type->u.c.info->vb_offset );
    }
    /* first leap indicates whether host class has virtual bases */
    leap_control = RL_NULL;
    if( TypeHasVirtualBases( class_type ) ) {
        leap_control |= RL_HAS_VBASE;
    }
    /* leaps */
    RingIterBeg( leaps, leap ) {
        --nleaps;
        if( leap->type != class_type ) {
            leap_control |= leap->control;
            if( leap_control & RL_AMBIGUOUS ) {
                leap_control |= RL_BAD_BASE;
            } else if(( leap_control & RL_PUBLIC ) == 0 ) {
                leap_control |= RL_BAD_BASE;
            }
            if( nleaps == 0 ) {
                leap_control |= RL_LAST;
            }
            leap_control &= RL_RUNTIME;
            DGInteger( leap->vb_index, T_UINT_2 );
            DGInteger( leap_control, T_UINT_2 );
            DgOffset( leap->offset );
            dropTypeidRef( leap->type );
            leap_control = RL_NULL;
        }
    } RingIterEnd( leap )
    BESetSeg( old_seg );
    ScopeRttiFreeLeaps( leaps );
    r->cg_gen = TRUE;
}

static char rotateChar( char c, unsigned rotate )
{
    unsigned new_char;
    if( 'a' <= c && c <= 'z' ) {
        // 0-25
        c -= 'a';
    } else if( 'A' <= c && c <= 'Z' ) {
        // 26-51
        c -= 'A';
        c += 26;
    } else if( '0' <= c && c <= '9' ) {
        // 52-61
        c -= '0';
        c += 52;
    } else if( c == '_' ) {
        c = 62;
    } else {
        // don't rotate the character
        return c;
    }
    new_char = ( c + rotate ) & 0x3f;
    if( new_char == 63 ) {
        // use the unused slot's transformation
        new_char = ( 63 + rotate ) & 0x3f;
        DbgAssert( new_char != 63 );
    }
    if( new_char <= 25 ) {
        c = (char)( new_char + 'a');
    } else if( new_char <= 51 ) {
        c = (char)(( new_char - 26 ) + 'A');
    } else if( new_char <= 61 ) {
        c = (char)(( new_char - 52 ) + '0');
    } else if( new_char == 62 ) {
        c = '_';
    } else {
        c = '@';
    }
    return c;
}


static void runThruSimpleCipher( char *name, unsigned len )
{
    unsigned rotate = 21;
    while( len != 0 ) {
        *name = rotateChar( *name, rotate );
        // ( rotate + 67 ) mod 101
        rotate += 67;
        if( rotate >= 101 ) {
            rotate -= 101;
        }
        ++name;
        --len;
    }
}

static void doTypeidGen( RTTI_TYPEID *r )
{
    char *raw_name;
    unsigned raw_len;
    segment_id old_seg;
    SYMBOL sym;

    sym = r->sym;
    if( ! SymIsReferenced( sym ) || SymIsInitialized( sym ) ) {
        return;
    }
    sym->flag |= SF_INITIALIZED;
    old_seg = BESetSeg( sym->segid );
    DGLabel( FEBack( sym ) );
    DgInitBytes( CgDataPtrSize(), 0 );
    raw_name = CppGetTypeidContents( r->type, &raw_len );
    if( CompFlags.obfuscate_typesig_names ) {
        runThruSimpleCipher( raw_name, raw_len );
    }
    DGString( raw_name, raw_len );
    DgByte( 0 );
    BESetSeg( old_seg );
}

void BeGenRttiInfo( void )
/************************/
{
    RttiWalk( doRttiGen );
    TypeidWalk( doTypeidGen );
}
