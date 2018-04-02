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


#include "axp.h"
#include "axptypes.h"
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
static const mad_type_info_integer      I1 =
    { MTK_INTEGER, D,  8,  7, MNR_TWOS_COMP, ME_LITTLE };
static const mad_type_info_integer      I2 =
    { MTK_INTEGER, D, 16, 15, MNR_TWOS_COMP, ME_LITTLE };
static const mad_type_info_integer      I4 =
    { MTK_INTEGER, D, 32, 31, MNR_TWOS_COMP, ME_LITTLE };
static const mad_type_info_integer      I8 =
    { MTK_INTEGER, D, 64, 63, MNR_TWOS_COMP, ME_LITTLE };

static const mad_type_info_address N32 =
    { MTK_ADDRESS, D, 32, 0, MNR_UNSIGNED, ME_LITTLE, 0, 0 };
//static const mad_type_info_address N64 =
//    { MTK_ADDRESS, D, 64, 0, MNR_UNSIGNED, ME_LITTLE, 0, 0 };

static const mad_type_info_float F4 =
    { MTK_FLOAT, D, 32, 31, MNR_SIGN_MAG, ME_LITTLE,   127, 23, 2, 1, MTK_INTEGER, D,  8,  7, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float F8 =
    { MTK_FLOAT, D, 64, 63, MNR_SIGN_MAG, ME_LITTLE,  1023, 52, 2, 1, MTK_INTEGER, D, 11, 10, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float V4 =
    { MTK_FLOAT, AXPT_F_FLOAT, 32, 31, MNR_SIGN_MAG, ME_LITTLE,   128, 23, 2, 1, MTK_INTEGER, D,  8,  7, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float V8G =
    { MTK_FLOAT, AXPT_G_FLOAT, 64, 63, MNR_SIGN_MAG, ME_LITTLE,  1024, 52, 2, 1, MTK_INTEGER, D, 11, 10, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float V8D =
    { MTK_FLOAT, AXPT_D_FLOAT, 64, 63, MNR_SIGN_MAG, ME_LITTLE,  128, 55, 2, 1, MTK_INTEGER, D, 8, 7, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float RV4 =
    { MTK_FLOAT, D, 32, 31, MNR_SIGN_MAG, ME_LITTLE,   128, 23, 2, 1, MTK_INTEGER, D,  8,  7, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float RV8G =
    { MTK_FLOAT, D, 64, 63, MNR_SIGN_MAG, ME_LITTLE,  1024, 52, 2, 1, MTK_INTEGER, D, 11, 10, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float RV8D =
    { MTK_FLOAT, D, 64, 63, MNR_SIGN_MAG, ME_LITTLE,  128, 55, 2, 1, MTK_INTEGER, D, 8, 7, MNR_UNSIGNED, ME_LITTLE };

#undef D

#define pick( e, n, h, info )   \
        { MAD_MSTR_##n, h, (mad_type_info_basic *)&info },

const mad_type_data TypeArray[] = {
#include "axptypes.h"
};

walk_result MADIMPENTRY( TypeWalk )( mad_type_kind tk, MI_TYPE_WALKER *wk, void *data )
{
    mad_type_handle     mth;
    walk_result         wr;

    if( !(tk & MAS_MEMORY) ) return( WR_CONTINUE );
    for( mth = 0; mth < sizeof( TypeArray ) / sizeof( TypeArray[0] ); ++mth ) {
        if( (tk & TypeArray[mth].u.mti->b.kind)
         && TypeArray[mth].name != MAD_MSTR_NIL ) {
            wr = wk( mth, data );
            if( wr != WR_CONTINUE ) {
                return( wr );
            }
        }
    }
    return( WR_CONTINUE );
}

mad_string MADIMPENTRY( TypeName )( mad_type_handle mth )
{
    return( TypeArray[mth].name );
}

mad_radix MADIMPENTRY( TypePreferredRadix )( mad_type_handle mth )
{
    return( TypeArray[mth].hex ? 16 : 10 );
}

void MADIMPENTRY( TypeInfo )( mad_type_handle mth, mad_type_info *mti )
{
    memcpy( mti, TypeArray[mth].u.mti, sizeof( *mti ) );
}

mad_type_handle MADIMPENTRY( TypeDefault )( mad_type_kind tk, mad_address_format af, const mad_registers *mr, const address *ap )
{
    /* unused parameters */ (void)af; (void)mr; (void)ap;

    if( tk & MAS_IO ) {
        return( MAD_NIL_TYPE_HANDLE );
    }
    switch( tk & MTK_ALL ) {
    case MTK_BASIC:
        return( AXPT_BYTE );
    case MTK_INTEGER:
        return( AXPT_LWORD );
    case MTK_ADDRESS:
        return( AXPT_N32_PTR );
    case MTK_FLOAT:
        return( AXPT_DOUBLE );
    }
    return( MAD_NIL_TYPE_HANDLE );
}

static mad_status DoConvert( const mad_type_info *in_mti, const void *in_d, const mad_type_info *out_mti, void *out_d )
{
    const unsigned_16   *p1;
    unsigned_16 *p2;
    unsigned_16 temp[4];
    mad_status  ms;

    p1 = in_d;
    switch( in_mti->b.handler_code ) {
    case AXPT_F_FLOAT:
        temp[0] = p1[1];
        temp[1] = p1[0];
        return( MCTypeConvert( TypeArray[AXPT_RF_FLOAT].u.mti, temp, out_mti, out_d, 0 ) );
    case AXPT_G_FLOAT:
        temp[0] = p1[3];
        temp[1] = p1[2];
        temp[2] = p1[1];
        temp[3] = p1[0];
        return( MCTypeConvert( TypeArray[AXPT_RG_FLOAT].u.mti, temp, out_mti, out_d, 0 ) );
    case AXPT_D_FLOAT:
        temp[0] = p1[3];
        temp[1] = p1[2];
        temp[2] = p1[1];
        temp[3] = p1[0];
        return( MCTypeConvert( TypeArray[AXPT_RD_FLOAT].u.mti, temp, out_mti, out_d, 0 ) );
    }
    p2 = out_d;
    switch( out_mti->b.handler_code ) {
    case AXPT_F_FLOAT:
        ms =  MCTypeConvert( in_mti, in_d, TypeArray[AXPT_RF_FLOAT].u.mti, temp, 0 );
        if( ms != MS_OK ) return( ms );
        p2[1] = temp[0];
        p2[0] = temp[1];
        break;
    case AXPT_G_FLOAT:
        ms =  MCTypeConvert( in_mti, in_d, TypeArray[AXPT_RG_FLOAT].u.mti, temp, 0 );
        if( ms != MS_OK ) return( ms );
        p2[3] = temp[0];
        p2[2] = temp[1];
        p2[1] = temp[2];
        p2[0] = temp[3];
        break;
    case AXPT_D_FLOAT:
        ms =  MCTypeConvert( in_mti, in_d, TypeArray[AXPT_RD_FLOAT].u.mti, temp, 0 );
        if( ms != MS_OK ) return( ms );
        p2[3] = temp[0];
        p2[2] = temp[1];
        p2[1] = temp[2];
        p2[0] = temp[3];
        break;
    default:
        return( MS_UNSUPPORTED );
    }
    return( MS_OK );
}

mad_status MADIMPENTRY( TypeToString )( mad_radix radix, const mad_type_info *mti, const void *data, char *buff, size_t *buff_size_p )
{
    mad_status          ms;
    const mad_type_info *new_mti;
    unsigned_16         temp[4];

    switch( mti->b.handler_code ) {
    case AXPT_F_FLOAT:
        new_mti = TypeArray[AXPT_FLOAT].u.mti;
        break;
    case AXPT_G_FLOAT:
    case AXPT_D_FLOAT:
        new_mti = TypeArray[AXPT_DOUBLE].u.mti;
        break;
    default:
        return( MS_UNSUPPORTED );
    }
    ms = DoConvert( mti, data, new_mti, &temp );
    if( ms != MS_OK )
        return( ms );
    return( MCTypeToString( radix, new_mti, temp, buff, buff_size_p ) );
}

mad_type_handle MADIMPENTRY( TypeForDIPType )( const dig_type_info *ti )
{
    switch( ti->kind ) {
    case TK_DATA:
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        switch( ti->size ) {
        case 1:
            return( ti->modifier == TM_SIGNED ? AXPT_CHAR : AXPT_UCHAR );
        case 2:
            return( ti->modifier == TM_SIGNED ? AXPT_SHORT : AXPT_USHORT );
        case 4:
            return( ti->modifier == TM_SIGNED ? AXPT_LONG : AXPT_ULONG );
        case 8:
            return( ti->modifier == TM_SIGNED ? AXPT_INT64 : AXPT_UINT64 );
        }
        break;
    case TK_REAL:
        switch( ti->modifier ) {
        case TM_VAX1:
            switch( ti->size ) {
            case 4:             return( AXPT_F_FLOAT );
            case 8:             return( AXPT_G_FLOAT );
            }
            break;
        case TM_VAX2:
            switch( ti->size ) {
            case 8:             return( AXPT_D_FLOAT );
            }
            break;
        default:
            switch( ti->size ) {
            case 4:             return( AXPT_FLOAT );
            case 8:             return( AXPT_DOUBLE );
            }
        }
        break;
    case TK_CODE:
    case TK_ADDRESS:
    case TK_POINTER:
    case TK_FUNCTION:
        return( AXPT_N32_PTR );
    }
    return( MAD_NIL_TYPE_HANDLE );
}

mad_status MADIMPENTRY( TypeConvert )( const mad_type_info *in_mti, const void *in_d, const mad_type_info *out_mti, void *out_d, addr_seg seg )
{
    /* unused parameters */ (void)seg;

    return( DoConvert( in_mti, in_d, out_mti, out_d ) );
}
