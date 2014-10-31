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
* Description:  PowerPC machine type handling.
*
****************************************************************************/


#include <string.h>
#include "ppc.h"
#include "ppctypes.h"
#include "madregs.h"

#define D       MAD_DEFAULT_HANDLING

// NYI: Hardcoded to big endian, should be dynamic
#define TARG_ENDIAN         ME_BIG

/* Register contents are kept in host format; hence we must not convert
 * them again and use special types for them.
 */
#ifdef __BIG_ENDIAN__
    #define HOST_ENDIAN     ME_BIG
#else
    #define HOST_ENDIAN     ME_LITTLE
#endif

static const mad_type_info_integer      U1 =
    { MTK_INTEGER, D,  8, 7, MNR_UNSIGNED,  TARG_ENDIAN };
static const mad_type_info_integer      U2 =
    { MTK_INTEGER, D, 16, 15, MNR_UNSIGNED,  TARG_ENDIAN };
static const mad_type_info_integer      U4 =
    { MTK_INTEGER, D, 32, 31, MNR_UNSIGNED,  TARG_ENDIAN };
static const mad_type_info_integer      U8 =
    { MTK_INTEGER, D, 64, 63, MNR_UNSIGNED,  TARG_ENDIAN };
static const mad_type_info_integer      I1 =
    { MTK_INTEGER, D,  8,  7, MNR_TWOS_COMP, TARG_ENDIAN };
static const mad_type_info_integer      I2 =
    { MTK_INTEGER, D, 16, 15, MNR_TWOS_COMP, TARG_ENDIAN };
static const mad_type_info_integer      I4 =
    { MTK_INTEGER, D, 32, 31, MNR_TWOS_COMP, TARG_ENDIAN };
static const mad_type_info_integer      I8 =
    { MTK_INTEGER, D, 64, 63, MNR_TWOS_COMP, TARG_ENDIAN };

static const mad_type_info_address N32 =
    { MTK_ADDRESS, D, 32, 0, MNR_UNSIGNED, TARG_ENDIAN, 0, 0 };
//static const mad_type_info_address N64 =
//    { MTK_ADDRESS, D, 64, 0, MNR_UNSIGNED, TARG_ENDIAN, 0, 0 };

static const mad_type_info_float F4 =
    { MTK_FLOAT, D, 32, 31, MNR_SIGN_MAG, TARG_ENDIAN,   127, 23, 2, 1, MTK_INTEGER, D,  8,  7, MNR_UNSIGNED, TARG_ENDIAN };
static const mad_type_info_float F8 =
    { MTK_FLOAT, D, 64, 63, MNR_SIGN_MAG, TARG_ENDIAN,  1023, 52, 2, 1, MTK_INTEGER, D, 11, 10, MNR_UNSIGNED, TARG_ENDIAN };

static const mad_type_info_integer      H_U1 =
    { MTK_INTEGER, D,  8, 7, MNR_UNSIGNED,  HOST_ENDIAN };
static const mad_type_info_integer      H_U2 =
    { MTK_INTEGER, D, 16, 15, MNR_UNSIGNED,  HOST_ENDIAN };
static const mad_type_info_integer      H_U4 =
    { MTK_INTEGER, D, 32, 31, MNR_UNSIGNED,  HOST_ENDIAN };
static const mad_type_info_integer      H_U8 =
    { MTK_INTEGER, D, 64, 63, MNR_UNSIGNED,  HOST_ENDIAN };
static const mad_type_info_integer      H_I1 =
    { MTK_INTEGER, D,  8,  7, MNR_TWOS_COMP, HOST_ENDIAN };
static const mad_type_info_integer      H_I2 =
    { MTK_INTEGER, D, 16, 15, MNR_TWOS_COMP, HOST_ENDIAN };
static const mad_type_info_integer      H_I4 =
    { MTK_INTEGER, D, 32, 31, MNR_TWOS_COMP, HOST_ENDIAN };
static const mad_type_info_integer      H_I8 =
    { MTK_INTEGER, D, 64, 63, MNR_TWOS_COMP, HOST_ENDIAN };

static const mad_type_info_address H_N32 =
    { MTK_ADDRESS, D, 32, 0, MNR_UNSIGNED, HOST_ENDIAN, 0, 0 };
//static const mad_type_info_address H_N64 =
//    { MTK_ADDRESS, D, 64, 0, MNR_UNSIGNED, HOST_ENDIAN, 0, 0 };

static const mad_type_info_float H_F4 =
    { MTK_FLOAT, D, 32, 31, MNR_SIGN_MAG, HOST_ENDIAN,   127, 23, 2, 1, MTK_INTEGER, D,  8,  7, MNR_UNSIGNED, HOST_ENDIAN };
static const mad_type_info_float H_F8 =
    { MTK_FLOAT, D, 64, 63, MNR_SIGN_MAG, HOST_ENDIAN,  1023, 52, 2, 1, MTK_INTEGER, D, 11, 10, MNR_UNSIGNED, HOST_ENDIAN };

#undef D

#define pick( e, n, h, info )   \
        { MAD_MSTR_##n, h, (mad_type_info_basic *)&info },

const mad_type_data TypeArray[] = {
#include "ppctypes.h"
};

walk_result     DIGENTRY MITypeWalk( mad_type_kind tk, MI_TYPE_WALKER *wk, void *data )
{
    mad_type_handle     th;
    walk_result         wr;

    if( !(tk & MAS_MEMORY) ) return( WR_CONTINUE );
    for( th = 0; th < sizeof( TypeArray ) / sizeof( TypeArray[0] ); ++th ) {
        if( (tk & TypeArray[th].u.info->b.kind)
         && TypeArray[th].name != MAD_MSTR_NIL ) {
            wr = wk( th, data );
            if( wr != WR_CONTINUE ) return( wr );
        }
    }
    return( WR_CONTINUE );
}

mad_string      DIGENTRY MITypeName( mad_type_handle th )
{
    return( TypeArray[th].name );
}

unsigned        DIGENTRY MITypePreferredRadix( mad_type_handle th )
{
    return( TypeArray[th].hex ? 16 : 10 );
}

void            DIGENTRY MITypeInfo( mad_type_handle th, mad_type_info *ti )
{
    memcpy( ti, TypeArray[th].u.info, sizeof( *ti ) );
}

mad_type_handle DIGENTRY MITypeDefault( mad_type_kind tk, mad_address_format af, mad_registers const *mr, address const *ap )
{
    af = af; mr = mr; ap = ap;

    if( tk & MAS_IO ) {
        return( MAD_NIL_TYPE_HANDLE );
    }
    switch( tk & MTK_ALL ) {
    case MTK_BASIC:
        return( PPCT_BYTE );
    case MTK_INTEGER:
        return( PPCT_WORD );
    case MTK_ADDRESS:
        return( PPCT_N32_PTR );
    case MTK_FLOAT:
        return( PPCT_DOUBLE );
    }
    return( MAD_NIL_TYPE_HANDLE );
}

mad_status      DIGENTRY MITypeToString( unsigned radix, const mad_type_info *mti, const void *data, char *buff, unsigned *max )
{
    radix = radix; mti = mti; data = data; max = max; buff = buff;

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
            return( ti->modifier == TM_SIGNED ? PPCT_CHAR : PPCT_UCHAR );
        case 2:
            return( ti->modifier == TM_SIGNED ? PPCT_SHORT : PPCT_USHORT );
        case 4:
            return( ti->modifier == TM_SIGNED ? PPCT_LONG : PPCT_ULONG );
        case 8:
            return( ti->modifier == TM_SIGNED ? PPCT_INT64 : PPCT_UINT64 );
        }
        break;
    case TK_REAL:
        switch( ti->size ) {
        case 4:         return( PPCT_FLOAT );
        case 8:         return( PPCT_DOUBLE );
        }
        break;
    case TK_CODE:
    case TK_ADDRESS:
    case TK_POINTER:
    case TK_FUNCTION:
        return( PPCT_N32_PTR );
    }
    return( MAD_NIL_TYPE_HANDLE );
}

mad_status      DIGENTRY MITypeConvert( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg seg )
{
    in_t = in_t; in_d = in_d; out_t = out_t; out_d = out_d; seg = seg;

    return( MS_UNSUPPORTED );
}
