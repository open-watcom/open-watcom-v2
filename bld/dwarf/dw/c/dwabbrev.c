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
* Description:  Manage the .debug_abbrev section.
*
****************************************************************************/


#include "dwpriv.h"
#include "dwutils.h"
#include "dwabbrev.h"
#include "dwdecl.h"


#include "dwabinfo.gh"


static const struct {
    abbrev_code                 bit;
    char                        data[2];
} bitEncodings[] = {
    { AB_DECLARATION,           { DW_AT_declaration,    DW_FORM_flag } },
    { AB_ACCESSIBILITY,         { DW_AT_accessibility,  DW_FORM_data1 } },
    { AB_OPACCESS,              { DW_AT_accessibility,  DW_FORM_data1 } },
    { AB_TYPE,                  { DW_AT_type,           DW_FORM_ref4  } },
    { AB_NAME,                  { DW_AT_name,           DW_FORM_string } },
    { AB_START_SCOPE,           { DW_AT_start_scope,    DW_FORM_udata } },
    { AB_BYTE_SIZE,             { DW_AT_byte_size,      DW_FORM_udata } },
    { AB_DATA_LOCATION,         { DW_AT_data_member_location, DW_FORM_block1 } },
    { AB_RETURN_ADDR,           { DW_AT_return_addr,    DW_FORM_block1 } },
    { AB_ARTIFICIAL,            { DW_AT_artificial,     DW_FORM_flag } },
    { AB_MEMBER,                { DW_AT_containing_type,DW_FORM_ref4 } },
    { AB_SEGMENT,               { DW_AT_segment,        DW_FORM_block1 } },
    { AB_ADDRESS_CLASS,         { DW_AT_address_class,  DW_FORM_data1 } },
    { AB_VTABLE_LOC,            { DW_AT_vtable_elem_location, DW_FORM_block1 } }
};


static const uint_8 zeros[] = { 0, 0 };

/*
    Read the comments in dwmakeab.c for more information on the
    meaning of some of these variables.

    The actual map from W(u) to K(dim(W(u))) here is backwards from
    what was shown in the example in dwmakeab.c.  This is simply
    for performance reasons.
*/
static uint mapFromWToX( abbrev_code abbrev )
{
    uint                        result;
    abbrev_code                 mask;
    uint                        index;

    /* first we map W(u) onto K(dim(W(u))) */
    result = 0;
    index = ( abbrev & AB_ENUM_MASK ) - 1;
    for( mask = abbrevInfo[index].valid_mask & ~AB_ALWAYS; mask; mask >>= 1 ) {
        if( mask & 1 ) {
            result = result * 2 + ( abbrev & 1 );
        }
        abbrev >>= 1;
    }
    /* finally we map onto X */
    return( result + abbrevInfo[index].bit_index );
}

unsigned MarkAbbrevAsUsed( dw_client cli, abbrev_code *abbrev )
{
    static const uint_8         sibling_attr[] = {
        DW_AT_sibling,          DW_FORM_ref4
    };
    uint                        index;
    uint                        code;
    uint_8                      bit;
    uint_8                      buf[2 * MAX_LEB128];
    uint_8                      *end;
    const struct abbrev_info    *data;
    int                         i;

    data = &abbrevInfo[( *abbrev & AB_ENUM_MASK ) - 1];

    /* check for AT_decl_* */
    *abbrev |= CheckDecl( cli, data->valid_mask );

    /* check the emitted bit set to see if we've emitted this already */
    code = mapFromWToX( *abbrev );
    bit = 1 << ( code & 0x7 );
    index = code / 8;
    code++;             // make sure we don't get a zero code.
    if( cli->debug_abbrev.emitted[index] & bit )
        return( code );
    /* has not been emitted */
    cli->debug_abbrev.emitted[index] |= bit;
    /* if pre compiled abbrevs don't gen */
    if( cli->compiler_options & DW_CM_ABBREV_PRE )
        return( code );
    /* emit the abbrev number, and tag */
    end = ULEB128( buf, code );
    end = ULEB128( end, data->tag );
    CLIWrite( DW_DEBUG_ABBREV, buf, end - buf );

    /* add in the attributes that are always emitted */
    *abbrev |= data->valid_mask & AB_ALWAYS;

    /* emit the child byte */
    if( *abbrev & AB_SIBLING ) {
        buf[0] = DW_CHILDREN_yes;
        CLIWrite( DW_DEBUG_ABBREV, buf, 1 );
        CLIWrite( DW_DEBUG_ABBREV, sibling_attr, sizeof(sibling_attr) );
    } else {
        buf[0] = DW_CHILDREN_no;
        CLIWrite( DW_DEBUG_ABBREV, buf, 1 );
    }

    /* AT_decl_file and AT_decl_line must occur here */
    if( *abbrev & AB_DECL ) {
        static const uint_8     attr[] = {
            DW_AT_decl_file,    DW_FORM_udata,
            DW_AT_decl_line,    DW_FORM_udata,
            DW_AT_decl_column,  DW_FORM_udata
        };
        CLIWrite( DW_DEBUG_ABBREV, attr, sizeof( attr ) );
    }

    /* now emit the extra attributes */
    for( i = 0; i < sizeof(bitEncodings) / sizeof(bitEncodings[0]); ++i ) {
        if( *abbrev & bitEncodings[i].bit ) {
            CLIWrite( DW_DEBUG_ABBREV, bitEncodings[i].data, 2 );
        }
    }

    if( *abbrev & AB_LOWHIGH_PC ) {
        static const uint_8     lowhi_attrs[] = {
            DW_AT_low_pc,       DW_FORM_addr,
            DW_AT_high_pc,      DW_FORM_addr,
        };
        CLIWrite( DW_DEBUG_ABBREV, lowhi_attrs, sizeof( lowhi_attrs ) );
    }

    /* now do the AB_SUBR_DECLARATION kludge */
    if( data->valid_mask & AB_SUBR_DECLARATION ) {
        if( *abbrev & AB_SUBR_DECLARATION ) {
            buf[0] = DW_AT_declaration;
            buf[1] = DW_FORM_flag;
            CLIWrite( DW_DEBUG_ABBREV, buf, 2 );
        } else {
            static const uint_8 subr_attrs[] = {
                DW_AT_return_addr,      DW_FORM_block1,
                DW_AT_low_pc,           DW_FORM_addr,
                DW_AT_high_pc,          DW_FORM_addr,
            };
            CLIWrite( DW_DEBUG_ABBREV, subr_attrs, sizeof( subr_attrs ) );
        }
    }

    /* and finally the base information */
    if( data->data_len ) {
        CLIWrite( DW_DEBUG_ABBREV, &encodings[data->data_offset], data->data_len );
    }

    /* and the zero terminators */
    CLIWrite( DW_DEBUG_ABBREV, zeros, 2 );
    return( code );
}


void InitDebugAbbrev( dw_client cli )
{
    memset( cli->debug_abbrev.emitted, 0, sizeof( cli->debug_abbrev.emitted ) );
}


void FiniDebugAbbrev( dw_client cli )
{
    CLIWrite( DW_DEBUG_ABBREV, zeros, 1 );
}

void  GenAllAbbrev( dw_client  cli )
{
    abbrev_code                 abbrev;
    uint_32                     mask;
    uint                        index;
    uint                        count;
    uint                        mask_count;
    uint_32                     mask_bit;

    /* generate all abbrev codes */
    for( index = 0; index < (AB_MAX - 1); ++index ) {
        if( (index+1) == (AB_MAX - 1) ) {
            count = AB_LAST_CODE - abbrevInfo[index].bit_index;
        } else {
            count = abbrevInfo[index+1].bit_index-abbrevInfo[index].bit_index;
        }
        mask = abbrevInfo[index].valid_mask & ~AB_ALWAYS;
        for( mask_count = 0;  mask_count < count; ++mask_count ) {
            uint which_mask_bits;

            abbrev = 0;
            mask_bit = 0x80000000;
            for( which_mask_bits = mask_count; which_mask_bits; which_mask_bits >>=1 ) {
                while( (mask & mask_bit ) == 0 ) {  /* tab to next bit flag */
                    mask_bit >>= 1;
                }
                if( which_mask_bits & 1 ) {
                    abbrev |= mask_bit;
                }
                mask_bit >>= 1;
            }
            abbrev |= (index+1);
            MarkAbbrevAsUsed( cli, &abbrev );
        }
    }
}
