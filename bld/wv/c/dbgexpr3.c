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
* Description:  Debugger expression handling, Part III (Type conversion).
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgstk.h"
#include "dbgerr.h"
#include "dbgitem.h"
#include "dbgmem.h"
#include "dbglit.h"
#include "ldsupp.h"
#include "mad.h"
#include "i64.h"
#include "dbgutil.h"
#include "dbgexpr3.h"
#include "dbgexpr2.h"
#include "dbgexpr.h"
#include "dbgloc.h"
#include "dbgprog.h"


extern stack_entry      *ExprSP;

typedef enum {
    I1,
    U1,
    I2,
    U2,
    I4,
    U4,
    I8,
    U8,
    F4,
    F8,
    F10,
    C8,
    C16,
    C20,
    STR,
    NP2,
    NP4,
    FP4,
    FP6,
    HP4,
    ERR,
    NUM_CLASSES
} conv_class;


static conv_class ConvIdx( dip_type_info *info )
{
    unsigned    size;

    if( info->kind == TK_STRING ) return( STR );
    if( info->size > sizeof( item_mach ) ) return( ERR );
    size = info->size;
    switch( info->kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        if( (info->modifier & TM_MOD_MASK) == TM_SIGNED ) {
            switch( size ) {
            case 1:
                return( I1 );
            case 2:
                return( I2 );
            case 4:
                return( I4 );
            case 8:
                return( I8 );
            }
        } else {
            switch( size ) {
            case 1:
                return( U1 );
            case 2:
                return( U2 );
            case 4:
                return( U4 );
            case 8:
                return( U8 );
            }
        }
        break;
    case TK_REAL:
        switch( size ) {
        case 4:
            return( F4 );
        case 8:
            return( F8 );
        case 10:
            return( F10 );
        }
        break;
    case TK_COMPLEX:
        switch( size ) {
        case 8:
            return( C8 );
        case 16:
            return( C16 );
        case 20:
            return( C20 );
        }
        break;
    case TK_POINTER:
    case TK_ADDRESS:
        switch( info->modifier & TM_MOD_MASK ) {
        case TM_NEAR:
            switch( size ) {
            case 2:
                return( NP2 );
            case 4:
                return( NP4 );
            }
            break;
        case TM_FAR:
            switch( size ) {
            case 4:
                return( FP4 );
            case 6:
                return( FP6 );
            }
            break;
        case TM_HUGE:
            switch( size ) {
            case 4:
                return( HP4 );
            }
        }
        break;
    }
    return( ERR );
}

void FromItem( item_mach *tmp, stack_entry *entry )
{
    unsigned            size;
    mad_type_info       src_type;
    mad_type_info       dst_type;
    dip_type_info       ti;

    if( entry->info.size > sizeof( *tmp ) ) {
        Error( ERR_NONE, LIT_ENG( ERR_TYPE_CONVERSION ) );
    }
    size = entry->info.size;
    switch( entry->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        MADTypeInfo( MADTypeForDIPType( &entry->info ), &src_type );
        if( (entry->info.modifier & TM_MOD_MASK) == TM_SIGNED ) {
            MADTypeInfoForHost( MTK_INTEGER, SIGNTYPE_SIZE( sizeof( entry->v.sint ) ), &dst_type );
        } else {
            MADTypeInfoForHost( MTK_INTEGER, sizeof( entry->v.sint ), &dst_type );
        }
        MADTypeConvert( &src_type, tmp, &dst_type, &entry->v.uint, 0 );
        return;
    case TK_REAL:
        MADTypeInfo( MADTypeForDIPType( &entry->info ), &src_type );
        MADTypeInfoForHost( MTK_FLOAT, sizeof( entry->v.real ), &dst_type );
        MADTypeConvert( &src_type, tmp, &dst_type, &entry->v.real, 0 );
        return;
    case TK_COMPLEX:
        ti.kind = TK_REAL;
        ti.size = entry->info.size / 2;
        ti.modifier = entry->info.modifier;
        MADTypeInfo( MADTypeForDIPType( &ti ), &src_type );
        MADTypeInfoForHost( MTK_FLOAT, sizeof( entry->v.cmplx.re ), &dst_type );
        MADTypeConvert( &src_type, tmp, &dst_type, &entry->v.cmplx.re, 0 );
        MADTypeConvert( &src_type, (unsigned_8 *)tmp + ti.size, &dst_type, &entry->v.cmplx.im, 0 );
        return;
    case TK_POINTER:
    case TK_ADDRESS:
        //NYI: use MAD conversion routines....
        switch( entry->info.modifier & TM_MOD_MASK ) {
        case TM_NEAR:
            switch( size ) {
            case 2:
                entry->v.addr.mach.offset = tmp->so;
                return;
            case 8:
                //NYI: 64 bit offsets
                entry->info.size = 4;
            case 4:
                entry->v.addr.mach.offset = tmp->lo;
                return;
            }
            break;
        case TM_FAR:
        case TM_HUGE:
            switch( size ) {
            case 4:
                entry->v.addr.sect_id = 0;
                entry->v.addr.indirect = false;
                ConvAddr32ToAddr48( tmp->sa, entry->v.addr.mach );
                return;
            case 6:
                entry->v.addr.sect_id = 0;
                entry->v.addr.indirect = false;
                entry->v.addr.mach = tmp->la;
                return;
            case sizeof( address ):
                /* it's an internal address symbol */
                entry->v.addr = tmp->xa;
                entry->info.size = 6;
                return;
            }
            break;
        }
        break;
    }
    Error( ERR_NONE, LIT_ENG( ERR_TYPE_CONVERSION ) );
}


void ToItem( stack_entry *entry, item_mach *tmp )
{
    unsigned            size;
    mad_type_info       src_type;
    mad_type_info       dst_type;

    if( entry->info.size > sizeof( *tmp ) ) {
        Error( ERR_NONE, LIT_ENG( ERR_TYPE_CONVERSION ) );
    }
    //NYI: use MAD routines for all conversions
    size = entry->info.size;
    switch( entry->info.kind ) {
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        MADTypeInfo( MADTypeForDIPType( &entry->info ), &dst_type );
        if( (entry->info.modifier & TM_MOD_MASK) == TM_SIGNED ) {
            MADTypeInfoForHost( MTK_INTEGER, SIGNTYPE_SIZE( sizeof( entry->v.sint ) ), &src_type );
        } else {
            MADTypeInfoForHost( MTK_INTEGER, sizeof( entry->v.sint ), &src_type );
        }
        MADTypeConvert( &src_type, &entry->v.uint, &dst_type, tmp, 0 );
        return;
    case TK_REAL:
        MADTypeInfo( MADTypeForDIPType( &entry->info ), &dst_type );
        MADTypeInfoForHost( MTK_FLOAT, sizeof( entry->v.real ), &src_type );
        MADTypeConvert( &src_type, &entry->v.real, &dst_type, tmp, 0 );
        return;
    case TK_COMPLEX:
        switch( size ) {
        case 8:
            tmp->sc.re = LDToD( &entry->v.cmplx.re );
            tmp->sc.im = LDToD( &entry->v.cmplx.im );
            return;
        case 16:
            tmp->lc.re = LDToD( &entry->v.cmplx.re );
            tmp->lc.im = LDToD( &entry->v.cmplx.im );
            return;
        case 20:
            tmp->xc = entry->v.cmplx;
            return;
        }
        break;
    case TK_POINTER:
    case TK_ADDRESS:
        switch( entry->info.modifier & TM_MOD_MASK ) {
        case TM_NEAR:
            switch( size ) {
            case 2:
                tmp->so = entry->v.addr.mach.offset;
                return;
            case 4:
                tmp->lo = entry->v.addr.mach.offset;
                return;
            case 8:
                tmp->qo.u._32[I64LO32] = entry->v.addr.mach.offset;
                tmp->qo.u._32[I64HI32] = 0;
            }
            break;
        case TM_FAR:
        case TM_HUGE:
            switch( size ) {
            case 4:
                ConvAddr48ToAddr32( entry->v.addr.mach, tmp->sa );
                return;
            case 6:
                tmp->la = entry->v.addr.mach;
                return;
            }
            break;
        }
        break;
    }
    Error( ERR_NONE, LIT_ENG( ERR_TYPE_CONVERSION ) );
}


OVL_EXTERN bool ConvU8( stack_entry *entry, conv_class from )
{
    unsigned_64  tmp;

    switch( from ) {
    case U1:
    case U2:
    case U4:
    case U8:
        tmp = entry->v.uint;
        break;
    case I1:
    case I2:
    case I4:
    case I8:
        tmp = entry->v.sint;
        break;
    case F4:
    case F8:
    case F10:
        //NYI: 64-bit support
        I32ToI64( LDToD( &entry->v.real ), &tmp );
        break;
    case C8:
    case C16:
    case C20:
        I32ToI64( LDToD( &entry->v.cmplx.re ), &tmp );
        break;
    case NP2:
    case NP4:
        //NYI: 64-bit offsets
        U32ToU64( entry->v.addr.mach.offset, &tmp );
        break;
    case FP4:
    case HP4:
        U32ToU64( entry->v.addr.mach.offset +
                ((long) entry->v.addr.mach.segment << 16), &tmp );
        break;
    case FP6:
        U32ToU64( entry->v.addr.mach.offset, &tmp );
        break;
    default:
        return( false );
    }
    entry->v.uint = tmp;
    return( true );
}

OVL_EXTERN bool ConvU1( stack_entry *entry, conv_class from )
{
    if( !ConvU8( entry, from ) ) return( false );
    U32ToU64( (unsigned_8)U32FetchTrunc( entry->v.uint ), &entry->v.uint );
    return( true );
}

OVL_EXTERN bool ConvU2( stack_entry *entry, conv_class from )
{
    if( !ConvU8( entry, from ) ) return( false );
    U32ToU64( (unsigned_16)U32FetchTrunc( entry->v.uint ), &entry->v.uint );
    return( true );
}

OVL_EXTERN bool ConvU4( stack_entry *entry, conv_class from )
{
    if( !ConvU8( entry, from ) ) return( false );
    U32ToU64( (unsigned_32)U32FetchTrunc( entry->v.uint ), &entry->v.uint );
    return( true );
}

OVL_EXTERN bool ConvI1( stack_entry *entry, conv_class from )
{
    if( !ConvU8( entry, from ) ) return( false );
    I32ToI64( (signed_8)U32FetchTrunc( entry->v.uint ), &entry->v.sint );
    return( true );
}

OVL_EXTERN bool ConvI2( stack_entry *entry, conv_class from )
{
    if( !ConvU8( entry, from ) ) return( false );
    I32ToI64( (signed_16)U32FetchTrunc( entry->v.uint ), &entry->v.sint );
    return( true );
}

OVL_EXTERN bool ConvI4( stack_entry *entry, conv_class from )
{
    if( !ConvU8( entry, from ) ) return( false );
    I32ToI64( (signed_32)U32FetchTrunc( entry->v.uint ), &entry->v.sint );
    return( true );
}

OVL_EXTERN bool ConvI8( stack_entry *entry, conv_class from )
{
    return( ConvU8( entry, from ) );
}

OVL_EXTERN bool ConvR10( stack_entry *entry, conv_class from )
{
    xreal       tmp;

    switch( from ) {
    case U1:
    case U2:
    case U4:
    case U8:
        //NYI: 64 bit support
        DToLD( U32FetchTrunc( entry->v.uint ), &tmp );
        break;
    case I1:
    case I2:
    case I4:
    case I8:
        //NYI: 64 bit support
        DToLD( I32FetchTrunc( entry->v.sint ), &tmp );
        break;
    case F4:
    case F8:
    case F10:
        tmp = entry->v.real;
        break;
    case C8:
    case C16:
    case C20:
        tmp = entry->v.cmplx.re;
        break;
    default:
        return( false );
    }
    entry->v.real = tmp;
    return( true );
}

OVL_EXTERN bool ConvR4( stack_entry *entry, conv_class from )
{
    if( !ConvR10( entry, from ) ) return( false );
    DToLD( (float)LDToD( &entry->v.real ), &entry->v.real );
    return( true );
}

OVL_EXTERN bool ConvR8( stack_entry *entry, conv_class from )
{
    if( !ConvR10( entry, from ) ) return( false );
    DToLD( (double)LDToD( &entry->v.real ), &entry->v.real );
    return( true );
}

OVL_EXTERN bool ConvC20( stack_entry *entry, conv_class from )
{
    xcomplex tmp;

    DToLD( 0.0, &tmp.im );

    switch( from ) {
    case U1:
    case U2:
    case U4:
    case U8:
        //NYI: 64 bit support
        DToLD( U32FetchTrunc( entry->v.uint ), &tmp.re );
        break;
    case I1:
    case I2:
    case I4:
    case I8:
        DToLD( I32FetchTrunc( entry->v.sint ), &tmp.re );
        break;
    case F4:
    case F8:
    case F10:
        tmp.re = entry->v.real;
        break;
    case C8:
    case C16:
    case C20:
        tmp = entry->v.cmplx;
        break;
    default:
        return( false );
    }
    entry->v.cmplx = tmp;
    return( true );
}

OVL_EXTERN bool ConvC8( stack_entry *entry, conv_class from )
{
    if( !ConvC20( entry, from ) ) return( false );
    DToLD( (float)LDToD( &entry->v.cmplx.re ), &entry->v.cmplx.re );
    DToLD( (float)LDToD( &entry->v.cmplx.im ), &entry->v.cmplx.im );
    return( true );
}

OVL_EXTERN bool ConvC16( stack_entry *entry, conv_class from )
{
    if( !ConvC20( entry, from ) ) return( false );
    DToLD( (double)LDToD( &entry->v.cmplx.re ), &entry->v.cmplx.re );
    DToLD( (double)LDToD( &entry->v.cmplx.im ), &entry->v.cmplx.im );
    return( true );
}

OVL_EXTERN bool ConvNP4( stack_entry *entry, conv_class from )
{
    addr48_off  tmp;

    switch( from ) {
    case U1:
    case U2:
    case U4:
    case U8:
        //NYI: 64 bit offsets
        tmp = U32FetchTrunc( entry->v.uint );
        break;
    case I1:
    case I2:
    case I4:
    case I8:
        //NYI: 64 bit offsets
        tmp = I32FetchTrunc( entry->v.sint );
        break;
    case NP2:
    case NP4:
    case FP4:
    case FP6:
        tmp = entry->v.addr.mach.offset;
        break;
    default:
        return( false );
    }
    if( (entry->flags & SF_CONST) && tmp == 0 ) {
        entry->v.addr = NilAddr;
    } else {
        entry->v.addr = DefAddrSpaceForAddr( Context.execution );
    }
    entry->v.addr.mach.offset = tmp;
    return( true );
}

OVL_EXTERN bool ConvNP2( stack_entry *entry, conv_class from )
{
    if( !ConvNP4( entry, from ) ) return( false );
    entry->v.addr.mach.offset = (addr32_off)entry->v.addr.mach.offset;
    return( true );
}

OVL_EXTERN bool ConvFP6( stack_entry *entry, conv_class from )
{
    address     tmp;

    tmp = DefAddrSpaceForAddr( Context.execution );
    switch( from ) {
    case U1:
    case U2:
    case U4:
    case U8:
        if( (entry->flags & SF_CONST) && U64Test( &entry->v.uint ) == 0 ) tmp = NilAddr;
        //NYI: 64 bit offsets
        tmp.mach.offset = U32FetchTrunc( entry->v.uint );
        break;
    case I1:
    case I2:
    case I4:
    case I8:
        if( (entry->flags & SF_CONST) && I64Test( &entry->v.sint ) == 0 ) tmp = NilAddr;
        //NYI: 64 bit offsets
        tmp.mach.offset = U32FetchTrunc( entry->v.sint );
        break;
    case NP2:
    case NP4:
        /* near pointers already have correct segment information filled in */
        tmp = entry->v.addr;
        break;
    case FP4:
    case FP6:
    case HP4:
        tmp = entry->v.addr;
        break;
    default:
        return( false );
    }
    entry->v.addr = tmp;
    return( true );
}

OVL_EXTERN bool ConvFP4( stack_entry *entry, conv_class from )
{
    if( !ConvFP6( entry, from ) ) return( false );
    entry->v.addr.mach.offset = (addr32_off)entry->v.addr.mach.offset;
    return( true );
}

OVL_EXTERN bool ConvSTR( stack_entry *entry, conv_class from )
{
    entry = entry;
    from = from;
    /* everything has already been taken care of */
    return( true );
}

OVL_EXTERN bool ConvERR( stack_entry *entry, conv_class from )
{
    entry = entry;
    from = from;
    return( false );
}

static bool (* const ConvFunc[])( stack_entry *, conv_class ) = {
    ConvI1,
    ConvU1,
    ConvI2,
    ConvU2,
    ConvI4,
    ConvU4,
    ConvI8,
    ConvU8,
    ConvR4,
    ConvR8,
    ConvR10,
    ConvC8,
    ConvC16,
    ConvC20,
    ConvSTR,
    ConvNP2,
    ConvNP4,
    ConvFP4,
    ConvFP6,
    ConvFP4, /* HP4 */
    ConvERR
};


/*
 * ConvertTo -- convert 'entry' to the given 'class'.
 *      'entry' should be an rvalue.
 */
void ConvertTo( stack_entry *entry, type_kind k, type_modifier m, unsigned s )
{
    conv_class  from;
    char        *dest;

    if( s == 0 && k == TK_INTEGER ) {
        s = DefaultSize( DK_INT );
    }
    if( entry->info.kind == k
     && entry->info.modifier == m
     && entry->info.size == s ) return;
    from = ConvIdx( &entry->info );
    switch( from ) {
    case U1:
        U32ToU64( U8FetchTrunc( entry->v.uint ), &entry->v.uint );
        break;
    case U2:
        U32ToU64( U16FetchTrunc( entry->v.uint ), &entry->v.uint );
        break;
    case U4:
        U32ToU64( U32FetchTrunc( entry->v.uint ), &entry->v.uint );
        break;
    case I1:
        I32ToI64( I8FetchTrunc( entry->v.uint ), &entry->v.uint );
        break;
    case I2:
        I32ToI64( I16FetchTrunc( entry->v.uint ), &entry->v.uint );
        break;
    case I4:
        I32ToI64( I32FetchTrunc( entry->v.uint ), &entry->v.uint );
        break;
    case F4:
        DToLD( (float)LDToD( &entry->v.real ), &entry->v.real );
        break;
    case F8:
        DToLD( (double)LDToD( &entry->v.real ), &entry->v.real );
        break;
    case C8:
        DToLD( (float)LDToD( &entry->v.cmplx.re ), &entry->v.cmplx.re );
        DToLD( (float)LDToD( &entry->v.cmplx.im ), &entry->v.cmplx.im );
        break;
    case C16:
        DToLD( (double)LDToD( &entry->v.cmplx.re ), &entry->v.cmplx.re );
        DToLD( (double)LDToD( &entry->v.cmplx.im ), &entry->v.cmplx.im );
        break;
    case NP2:
    case FP4:
        entry->v.addr.mach.offset &= 0xffff;
        break;
    case STR:
        if( k != TK_STRING ) {
            Error( ERR_NONE, LIT_ENG( ERR_TYPE_CONVERSION ) );
        }
        if( s > entry->info.size ) {
            /* have to expand string */
            _ChkAlloc( dest, s, LIT_ENG( ERR_NO_MEMORY_FOR_EXPR ) );
            memcpy( dest, entry->v.string.loc.e[0].u.p, entry->info.size );
            memset( &dest[entry->info.size], ' ', s - entry->info.size );
            if( AllocatedString( entry ) ) {
                _Free( entry->v.string.allocated );
            }
            entry->v.string.allocated = dest;
            LocationCreate( &entry->v.string.loc, LT_INTERNAL, dest );
        }
        break;
    default:
        break;
    }
    entry->info.kind = k;
    entry->info.modifier = m;
    entry->info.size = s;
    if( !ConvFunc[ ConvIdx( &entry->info ) ]( entry, from ) ) {
        Error( ERR_NONE, LIT_ENG( ERR_TYPE_CONVERSION ) );
    }
    entry->th = NULL;
}


static conv_class BinResult[NUM_CLASSES][NUM_CLASSES] = {
        /* I1  U1  I2  U2  I4  U4  I8  U8  F4  F8  F10 C8  C16 C20 STR NP2 NP4 FP4 FP6 HP4*/
/* I1 */ { I1, U1, I2, U2, I4, U4, I8, U8, F4, F8, F10,C8, C16,C20,ERR,NP2,NP4,FP4,FP6,HP4},
/* U1 */ { U1, U1, I2, U2, I4, U4, I8, U8, F4, F8, F10,C8, C16,C20,ERR,NP2,NP4,FP4,FP6,HP4},
/* I2 */ { I2, I2, I2, U2, I4, U4, I8, U8, F4, F8, F10,C8, C16,C20,ERR,NP2,NP4,FP4,FP6,HP4},
/* U2 */ { U2, U2, U2, U2, I4, U4, I8, U8, F4, F8, F10,C8, C16,C20,ERR,NP2,NP4,FP4,FP6,HP4},
/* I4 */ { I4, I4, I4, I4, I4, U4, I8, U8, F4, F8, F10,C8, C16,C20,ERR,NP2,NP4,FP4,FP6,HP4},
/* U4 */ { U4, U4, U4, U4, U4, U4, I8, U8, F4, F8, F10,C8, C16,C20,ERR,NP2,NP4,FP4,FP6,HP4},
/* I8 */ { I8, I8, I8, I8, I8, U8, I8, U8, F8, F8, F10,C8, C16,C20,ERR,NP2,NP4,FP4,FP6,HP4},
/* U8 */ { U8, U8, U8, U8, U8, U8, I8, U8, F8, F8, F10,C8, C16,C20,ERR,NP2,NP4,FP4,FP6,HP4},
/* F4 */ { F4, F4, F4, F4, F4, F4, F8, F8, F4, F8, F10,C8, C16,C20,ERR,ERR,ERR,ERR,ERR,ERR},
/* F8 */ { F8, F8, F8, F8, F8, F8, F8, F8, F8, F8, F10,C16,C16,C20,ERR,ERR,ERR,ERR,ERR,ERR},
/* F10*/ { F8, F8, F8, F8, F8, F8, F8, F8, F8, F8, F10,C20,C20,C20,ERR,ERR,ERR,ERR,ERR,ERR},
/* C8 */ { C8, C8, C8, C8, C8, C8, C16,C16,C8, C16,C20,C8, C16,C20,ERR,ERR,ERR,ERR,ERR,ERR},
/* C16*/ { C16,C16,C16,C16,C16,C16,C16,C16,C16,C16,C20,C16,C16,C20,ERR,ERR,ERR,ERR,ERR,ERR},
/* C20*/ { C20,C20,C20,C20,C20,C20,C20,C20,C20,C20,C20,C20,C20,C20,ERR,ERR,ERR,ERR,ERR,ERR},
/* STR*/ { ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,ERR,STR,ERR,ERR,ERR,ERR,ERR,ERR},
/* NP2*/ { NP2,NP2,NP2,NP2,NP2,NP2,NP2,NP2,ERR,ERR,ERR,ERR,ERR,ERR,ERR,NP2,NP4,FP4,FP6,HP4},
/* NP4*/ { NP4,NP4,NP4,NP4,NP4,NP4,NP4,NP4,ERR,ERR,ERR,ERR,ERR,ERR,ERR,NP4,NP4,FP4,FP6,HP4},
/* FP4*/ { FP4,FP4,FP4,FP4,FP4,FP4,FP4,FP4,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FP4,FP4,FP4,FP6,HP4},
/* FP6*/ { FP6,FP6,FP4,FP6,FP6,FP6,FP6,FP6,ERR,ERR,ERR,ERR,ERR,ERR,ERR,FP6,FP6,FP6,FP6,FP6},
/* HP4*/ { HP4,HP4,HP4,HP4,HP4,HP4,HP4,HP4,ERR,ERR,ERR,ERR,ERR,ERR,ERR,HP4,HP4,HP4,FP6,HP4}
};

static dip_type_info ResultInfo[] = {
        {  1, TK_INTEGER,       TM_SIGNED },
        {  1, TK_INTEGER,       TM_UNSIGNED },
        {  2, TK_INTEGER,       TM_SIGNED },
        {  2, TK_INTEGER,       TM_UNSIGNED },
        {  4, TK_INTEGER,       TM_SIGNED },
        {  4, TK_INTEGER,       TM_UNSIGNED },
        {  8, TK_INTEGER,       TM_SIGNED },
        {  8, TK_INTEGER,       TM_UNSIGNED },
        {  4, TK_REAL,          TM_NONE },
        {  8, TK_REAL,          TM_NONE },
        { 10, TK_REAL,          TM_NONE },
        {  8, TK_COMPLEX,       TM_NONE },
        { 16, TK_COMPLEX,       TM_NONE },
        { 20, TK_COMPLEX,       TM_NONE },
        {  0, TK_STRING,        TM_NONE },
        {  2, TK_POINTER,       TM_NEAR },
        {  4, TK_POINTER,       TM_NEAR },
        {  4, TK_POINTER,       TM_FAR },
        {  6, TK_POINTER,       TM_FAR },
        {  4, TK_POINTER,       TM_HUGE },
};

static void DoBinOp( stack_entry *left, stack_entry *right )
{
    conv_class      lclass;
    conv_class      rclass;
    conv_class      result_class;
    dip_type_info   *result_info;
    bool            promote_left;

    lclass = ConvIdx( &left->info );
    rclass = ConvIdx( &right->info );
    if( lclass == ERR || rclass == ERR ) {
        Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
    }
    result_class = BinResult[ lclass ][ rclass ];
    if( result_class == ERR ) {
        Error( ERR_NONE, LIT_ENG( ERR_TYPE_CONVERSION ) );
    }
    if( left->info.kind == TK_ENUM ) {
        result_info = &left->info;
    } else if( right->info.kind == TK_ENUM ) {
        result_info = &right->info;
    } else {
        result_info = &ResultInfo[ result_class ];
    }
    promote_left = false;
    if( lclass != result_class ) {
        promote_left = true;
        ConvertTo( left, result_info->kind,
                result_info->modifier, result_info->size );
    }
    if( rclass != result_class ) {
        ConvertTo( right, result_info->kind,
                result_info->modifier, result_info->size );
    }
    /* set up result type in left operand */
    if( left->th != NULL && right->th != NULL ) {
        /* if left side was promoted, use right's type. otherwise use left's */
        if( promote_left ) {
            MoveTH( right, left );
        }
    } else if( right->th != NULL ) {
        MoveTH( right, left );
    /* else use left->type */
    }
}

void BinOp( stack_entry *left, stack_entry *right )
{
    RValue( left );
    RValue( right );
    DoBinOp( left, right );
}

void AddOp( stack_entry *left, stack_entry *right )
{
    switch( left->info.kind ) {
    case TK_POINTER:
    case TK_ADDRESS:
        break;
    default:
        DoBinOp( left, right );
    }
}

void ToItemMAD( stack_entry *entry, item_mach *tmp, mad_type_info *mti )
{
    unsigned            bytes;
    mad_type_info       src;

    bytes = mti->b.bits / BITS_PER_BYTE;
    switch( mti->b.kind ) {
    case MTK_INTEGER:
        ConvertTo( entry, TK_INTEGER, TM_UNSIGNED, bytes );
        MADTypeInfoForHost( MTK_INTEGER, sizeof( entry->v.uint ), &src );
        break;
    case MTK_ADDRESS:
        if( mti->a.seg.bits == 0 ) {
            ConvertTo( entry, TK_ADDRESS, TM_NEAR, bytes );
            MADTypeInfoForHost( MTK_ADDRESS, sizeof( entry->v.addr.mach.offset ), &src );
        } else {
            ConvertTo( entry, TK_ADDRESS, TM_FAR, bytes );
            MADTypeInfoForHost( MTK_ADDRESS, sizeof( entry->v.addr.mach ), &src );
        }
        break;
    case MTK_FLOAT:
        ConvertTo( entry, TK_REAL, TM_NONE, bytes );
        MADTypeInfoForHost( MTK_FLOAT, sizeof( entry->v.real ), &src );
        break;
    case MTK_XMM:
        //MAD: nyi
        ToItem( entry, tmp );
        return;
    case MTK_CUSTOM:
        //MAD: nyi
        ToItem( entry, tmp );
        return;
    }
    if( MADTypeConvert( &src, &entry->v, mti, tmp, 0 ) != MS_OK ) {
        ToItem( entry, tmp );
    }
}
