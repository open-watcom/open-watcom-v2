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
#include "x86.h"
#include "x86types.h"
#include "madregs.h"

#define D       MAD_DEFAULT_HANDLING

static const mad_type_info_integer      U1 =
    { MTK_INTEGER, D,  8, 7, MNR_UNSIGNED,  ME_LITTLE };
static const mad_type_info_integer      U2 =
    { MTK_INTEGER, D, 16, 15, MNR_UNSIGNED,  ME_LITTLE };
static const mad_type_info_integer      U4 =
    { MTK_INTEGER, D, 32, 31, MNR_UNSIGNED,  ME_LITTLE };
static const mad_type_info_integer      U8 =
    { MTK_INTEGER, D, 64, 63, MNR_UNSIGNED,  ME_LITTLE };
static const mad_type_info_integer      U16 =
    { MTK_INTEGER, D, 64, 63, MNR_UNSIGNED,  ME_LITTLE }; // FIXME !! missing 128-bit support
static const mad_type_info_integer      I1 =
    { MTK_INTEGER, D,  8,  7, MNR_TWOS_COMP, ME_LITTLE };
static const mad_type_info_integer      I2 =
    { MTK_INTEGER, D, 16, 15, MNR_TWOS_COMP, ME_LITTLE };
static const mad_type_info_integer      I4 =
    { MTK_INTEGER, D, 32, 31, MNR_TWOS_COMP, ME_LITTLE };
static const mad_type_info_integer      I8 =
    { MTK_INTEGER, D, 64, 63, MNR_TWOS_COMP, ME_LITTLE };
static const mad_type_info_integer      BIT =
    { MTK_INTEGER, X86T_BIT,  8,  7, MNR_UNSIGNED,  ME_LITTLE };

static const mad_type_info_address N16 =
    { MTK_ADDRESS, D, 16, 0, MNR_UNSIGNED, ME_LITTLE, 0, 0 };
static const mad_type_info_address N32 =
    { MTK_ADDRESS, D, 32, 0, MNR_UNSIGNED, ME_LITTLE, 0, 0 };
static const mad_type_info_address F16 =
    { MTK_ADDRESS, D, 32, 0, MNR_UNSIGNED, ME_LITTLE, 16, 16 };
static const mad_type_info_address F32 =
    { MTK_ADDRESS, D, 48, 0, MNR_UNSIGNED, ME_LITTLE, 32, 16 };

static const mad_type_info_float F4 =
    { MTK_FLOAT, D, 32, 31, MNR_SIGN_MAG, ME_LITTLE,   127, 23, 2, 1, MTK_INTEGER, D,  8,  7, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float F8 =
    { MTK_FLOAT, D, 64, 63, MNR_SIGN_MAG, ME_LITTLE,  1023, 52, 2, 1, MTK_INTEGER, D, 11, 10, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float F10 =
    { MTK_FLOAT, D, 80, 79, MNR_SIGN_MAG, ME_LITTLE, 16383, 64, 2, 0, MTK_INTEGER, D, 15, 14, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float F10EMPTY =
    { MTK_FLOAT, X86T_F10EMPTY, 80, 79, MNR_SIGN_MAG, ME_LITTLE, 16383, 64, 2, 0, MTK_INTEGER, 15, 0, 14, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float F10SPECIAL =
    { MTK_FLOAT, X86T_F10SPECIAL, 80, 79, MNR_SIGN_MAG, ME_LITTLE, 16383, 64, 2, 0, MTK_INTEGER, 15, 0, 14, MNR_UNSIGNED, ME_LITTLE };

static const mad_type_info_basic IRET16 =
    { MTK_CUSTOM, X86T_IRET16, 3*16 };
static const mad_type_info_basic IRET32 =
    { MTK_CUSTOM, X86T_IRET32, 3*32 };

static const mad_type_info_basic POPA =
    { MTK_CUSTOM, X86T_POPA, 8*16 };
static const mad_type_info_basic POPAD =
    { MTK_CUSTOM, X86T_POPAD, 8*32 };
static const mad_type_info_basic TAG =
    { MTK_CUSTOM, X86T_TAG, 16 };
static const mad_type_info_basic FPPTR_REAL =
    { MTK_CUSTOM, X86T_FPPTR_REAL, 64 };
static const mad_type_info_basic FPPTR_16 =
    { MTK_CUSTOM, X86T_FPPTR_16, 64 };
static const mad_type_info_basic FPPTR_32 =
    { MTK_CUSTOM, X86T_FPPTR_32, 64 };
static const mad_type_info_basic MMX_TITLE =
    { MTK_CUSTOM, X86T_MMX_TITLE0, 8 };

static const mad_type_info_basic XMM_TITLE =
    { MTK_XMM, X86T_XMM_TITLE0, 8 };

#undef D

typedef struct mad_type_data {
    mad_string          name;
    unsigned            hex     : 1;
    unsigned            io      : 3;
    unsigned            mem     : 3;
    union {
        const mad_type_info_basic       *b;
        const mad_type_info             *info;
    }                   u;
} mad_type_data;

#define pick( e, n, h, iol, meml, info )        \
        { MAD_MSTR_##n, h, iol, meml, (mad_type_info_basic *)&info },

static const mad_type_data TypeArray[] = {
#include "x86types.h"
};

walk_result     DIGENTRY MITypeWalk( mad_type_kind tk, MI_TYPE_WALKER *wk, void *data )
{
    mad_type_handle     th;
    processor_level     iol;
    processor_level     meml;
    walk_result         wr;

    iol = LN;
    meml = LN;
    if( ( MCSystemConfig()->cpu & X86_CPU_MASK ) < X86_386 ) {
        if( tk & MAS_IO ) iol = L1;
        if( tk & MAS_MEMORY ) meml = L1;
    } else {
        if( tk & MAS_IO ) iol = L3;
        if( tk & MAS_MEMORY ) meml = L3;
    }

    for( th = 0; th < sizeof( TypeArray ) / sizeof( TypeArray[0] ); ++th ) {
        if( (int)TypeArray[th].io <= iol || (int)TypeArray[th].mem <= meml ) {
            if( tk & TypeArray[th].u.info->b.kind ) {
                wr = wk( th, data );
                if( wr != WR_CONTINUE ) return( wr );
            }
        }
    }
    return( WR_CONTINUE );
}

mad_string      DIGENTRY MITypeName( mad_type_handle th )
{
    return( TypeArray[th].name );
}

mad_radix       DIGENTRY MITypePreferredRadix( mad_type_handle th )
{
    return( TypeArray[th].hex ? 16 : 10 );
}

void            DIGENTRY MITypeInfo( mad_type_handle th, mad_type_info *ti )
{
    memcpy( ti, TypeArray[th].u.info, sizeof( *ti ) );
    if( TypeArray[th].u.b == &BIT.b || TypeArray[th].u.b == &MMX_TITLE || TypeArray[th].u.b == &XMM_TITLE ) {
        ti->b.handler_code = (unsigned_8)th;
    }
}

mad_type_handle DIGENTRY MITypeDefault( mad_type_kind tk, mad_address_format af, const mad_registers *mr, const address *ap )
{
    int         big;

    if( tk & MAS_IO ) {
        switch( tk & MTK_ALL ) {
        case MTK_BASIC:
        case MTK_INTEGER:
            return( X86T_BYTE );
        case MTK_ADDRESS:
            return( X86T_N16_PTR );
        }
        return( MAD_NIL_TYPE_HANDLE );
    }
    if( ap != NULL ) {
        big = BIG_SEG( *ap );
    } else if( mr != NULL ) {
        big = BIG_SEG( GetRegIP( mr ) );
    } else {
        big = ( ( MCSystemConfig()->cpu & X86_CPU_MASK ) >= X86_386);
    }
    switch( tk & MTK_ALL ) {
    case MTK_BASIC:
        return( X86T_BYTE );
    case MTK_INTEGER:
        return( big ? X86T_DWORD : X86T_WORD );
    case MTK_ADDRESS:
        if( big ) {
            return( (af == MAF_OFFSET) ? X86T_N32_PTR : X86T_F32_PTR );
        } else {
            return( (af == MAF_OFFSET) ? X86T_N16_PTR : X86T_F16_PTR );
        }
    case MTK_FLOAT:
        return( X86T_DOUBLE );
    }
    return( MAD_NIL_TYPE_HANDLE );
}

mad_status      DIGENTRY MITypeToString( mad_radix radix, const mad_type_info *mti, const void *data, char *buff, size_t *buff_size_p )
{
    size_t      buff_size;

    radix = radix;

    switch( mti->b.handler_code ) {
    case X86T_UNKNOWN:
    case X86T_F10EMPTY:
        buff_size = *buff_size_p;
        if( buff_size > 0 ) {
            --buff_size;
            *buff_size_p = buff_size;
            while( buff_size-- > 0 ) {
                *buff++ = '?';
            }
            *buff = '\0';
        }
        return( MS_OK );
    case X86T_PC:
    case X86T_RC:
    case X86T_IC:
    case X86T_BIT:
    case X86T_TAG:
    case X86T_FPPTR_REAL:
    case X86T_FPPTR_16:
    case X86T_FPPTR_32:
    case X86T_F10SPECIAL:
    case X86T_MMX_TITLE0:
    case X86T_MMX_TITLE1:
    case X86T_MMX_TITLE2:
    case X86T_MMX_TITLE3:
    case X86T_MMX_TITLE4:
    case X86T_MMX_TITLE5:
    case X86T_MMX_TITLE6:
    case X86T_MMX_TITLE7:
    case X86T_XMM_TITLE0:
    case X86T_XMM_TITLE1:
    case X86T_XMM_TITLE2:
    case X86T_XMM_TITLE3:
    case X86T_XMM_TITLE4:
    case X86T_XMM_TITLE5:
    case X86T_XMM_TITLE6:
    case X86T_XMM_TITLE7:
    case X86T_XMM_TITLE8:
    case X86T_XMM_TITLE9:
    case X86T_XMM_TITLE10:
    case X86T_XMM_TITLE11:
    case X86T_XMM_TITLE12:
    case X86T_XMM_TITLE13:
    case X86T_XMM_TITLE14:
    case X86T_XMM_TITLE15:
        *buff_size_p = RegDispType( mti->b.handler_code, data, buff, *buff_size_p );
        return( MS_OK );
    }
    return( MS_UNSUPPORTED );
}

mad_type_handle DIGENTRY MITypeForDIPType( const dip_type_info *ti )
{
    switch( ti->kind ) {
    case TK_DATA:
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        switch( ti->size ) {
        case 1:
            return( ti->modifier == TM_SIGNED ? X86T_CHAR : X86T_UCHAR );
        case 2:
            return( ti->modifier == TM_SIGNED ? X86T_SHORT : X86T_USHORT );
        case 4:
            return( ti->modifier == TM_SIGNED ? X86T_LONG : X86T_ULONG );
        case 8:
            return( ti->modifier == TM_SIGNED ? X86T_I64 : X86T_U64 );
        }
        break;
    case TK_REAL:
        switch( ti->size ) {
        case 4:         return( X86T_FLOAT );
        case 8:         return( X86T_DOUBLE );
        case 10:        return( X86T_EXTENDED );
        }
        break;
    case TK_CODE:
    case TK_ADDRESS:
    case TK_POINTER:
    case TK_FUNCTION:
        switch( ti->modifier ) {
        case TM_NONE:
        case TM_NEAR:
            return( ti->size == 16  ? X86T_N16_PTR : X86T_N32_PTR );
        default:
            return( ti->size == 32  ? X86T_F16_PTR : X86T_F32_PTR );
        }
        break;
    }
    return( MAD_NIL_TYPE_HANDLE );
}

mad_status      DIGENTRY MITypeConvert( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg seg )
{
    in_t = in_t; in_d = in_d; out_t = out_t; out_d = out_d; seg = seg;
    return( MS_UNSUPPORTED );
}
