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
#include "xmp.h"
#include "xmptypes.h"
#include "madregs.h"

#define D       MAD_DEFAULT_HANDLING

#if 0
static const mad_type_info_integer      U1 =
    { MTK_INTEGER, D,  8, 7, MNR_UNSIGNED,  ME_LITTLE };
#endif
static const mad_type_info_integer      U2 =
    { MTK_INTEGER, D, 16, 15, MNR_UNSIGNED,  ME_LITTLE };
#if 0
static const mad_type_info_integer      U4 =
    { MTK_INTEGER, D, 32, 31, MNR_UNSIGNED,  ME_LITTLE };
static const mad_type_info_integer      U8 =
    { MTK_INTEGER, D, 64, 63, MNR_UNSIGNED,  ME_LITTLE };
#endif
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

static const mad_type_info_float F4 =
    { MTK_FLOAT, D, 32, 31, MNR_SIGN_MAG, ME_LITTLE,   127, 23, 2, 1, MTK_INTEGER, D,  8,  7, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float F8 =
    { MTK_FLOAT, D, 64, 63, MNR_SIGN_MAG, ME_LITTLE,  1023, 52, 2, 1, MTK_INTEGER, D, 11, 10, MNR_UNSIGNED, ME_LITTLE };

#undef D

#define pick( e, n, h, info )   \
        { MSTR_##n, h, (mad_type_info_basic *)&info },

const mad_type_data TypeArray[] = {
#include "xmptypes.h"
};

walk_result     DIGENTRY MITypeWalk( mad_type_kind tk, MI_TYPE_WALKER *wk, void *data )
{
    mad_type_handle     th;
    walk_result         wr;

    if( !(tk & MAS_MEMORY) ) return( WR_CONTINUE );
    for( th = 0; th < sizeof( TypeArray ) / sizeof( TypeArray[0] ); ++th ) {
        if( (tk & TypeArray[th].u.info->b.kind)
         && TypeArray[th].name != MSTR_NIL ) {
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

mad_type_handle DIGENTRY MITypeDefault( mad_type_kind tk, mad_address_format af, const mad_registers *mr, const address *ap )
{
    if( tk & MAS_IO ) {
        return( MAD_NIL_TYPE_HANDLE );
    }
    switch( tk & MTK_ALL ) {
    case MTK_BASIC:
        return( XMPT_BYTE );
    case MTK_INTEGER:
        return( XMPT_INT );
    case MTK_ADDRESS:
        return( XMPT_N32_PTR );
    case MTK_FLOAT:
        return( XMPT_DOUBLE );
    }
    return( MAD_NIL_TYPE_HANDLE );
}

mad_status      DIGENTRY MITypeToString( unsigned radix, const mad_type_info *mti, const void *data, unsigned *max, char *buff )
{
    return( MS_UNSUPPORTED );
}

mad_type_handle DIGENTRY MITypeForDIPType( const type_info *ti )
{
    switch( ti->kind ) {
    case TK_DATA:
    case TK_BOOL:
    case TK_ENUM:
    case TK_CHAR:
    case TK_INTEGER:
        switch( ti->size ) {
        case 1:
            return( XMPT_BYTE );
        case 2:
            return( XMPT_SHORT );
        case 4:
            return( XMPT_INT );
        }
        break;
    case TK_REAL:
        switch( ti->size ) {
        case 4:         return( XMPT_FLOAT );
        case 8:         return( XMPT_DOUBLE );
        }
        break;
    case TK_CODE:
    case TK_ADDRESS:
    case TK_POINTER:
    case TK_FUNCTION:
        return( XMPT_N32_PTR );
    }
    return( MAD_NIL_TYPE_HANDLE );
}

mad_status      DIGENTRY MITypeConvert( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg seg )
{
    return( MS_UNSUPPORTED );
}
