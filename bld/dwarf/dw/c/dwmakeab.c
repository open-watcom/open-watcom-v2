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
* Description:  This file contains the definitions of DWARF abbreviations
*               in a user editable form. Compile and run this file to
*               create abbreviation tables used inside dwabbrev.c.
*
****************************************************************************/


#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define NO_ENUM
#define AB_BITVECT_SIZE 1
#include "dwpriv.h"
#include "dwutils.h"
#include "dwabbrev.h"

#include "dwutils.c"

#define MAX_CODES       29

typedef struct {
    char *              name;           // enumerated name
    uint_32             tag;            // tag for this abbreviation
    abbrev_code         valid_mask;     // valid bits for this abbreviation
    uint_32             data[MAX_CODES]; // attr/form pairs
} abbrev_data;

/*
    Some useful combinations of bits
*/
#define DECL                    AB_DECL
#define COMMON_NBITS            AB_ACCESSIBILITY


abbrev_data const abbrevInfo[] = {

    {
        "AB_COMPILE_UNIT",
        DW_TAG_compile_unit,
        AB_SIBLING | AB_LOWHIGH_PC,
        {
            DW_AT_name,                 DW_FORM_string,
            DW_AT_stmt_list,            DW_FORM_data4,
            DW_AT_language,             DW_FORM_udata,
            DW_AT_comp_dir,             DW_FORM_string,
            DW_AT_producer,             DW_FORM_string,
            DW_AT_identifier_case,      DW_FORM_udata,
            DW_AT_macro_info,           DW_FORM_data4,
            DW_AT_base_types,           DW_FORM_ref_addr,
            DW_AT_WATCOM_memory_model,  DW_FORM_data1,
            DW_AT_WATCOM_references_start, DW_FORM_data4,
            0
        }
    },

    {
        "AB_BASE_TYPE",
        DW_TAG_base_type,
        AB_NAME,
        {
            DW_AT_encoding,             DW_FORM_data1,
            DW_AT_byte_size,            DW_FORM_data1,
            0
        }
    },

    {
        "AB_TYPEDEF_TYPE",
        DW_TAG_typedef,
        DECL | AB_NAME | COMMON_NBITS,
        {
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_INDIRECT_TYPE",
        DW_TAG_typedef,
        0,
        {
            DW_AT_type,     DW_FORM_ref_addr,
            0
        }
    },
    {
        "AB_POINTER_TYPE",
        DW_TAG_pointer_type,
        AB_ADDRESS_CLASS | AB_SEGMENT,
        {
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_REFERENCE_TYPE",
        DW_TAG_reference_type,
        AB_ADDRESS_CLASS,
        {
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_CONST_TYPE",
        DW_TAG_const_type,
        0,
        {
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_VOLATILE_TYPE",
        DW_TAG_volatile_type,
        0,
        {
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_ADDR_CLASS_TYPE",
        DW_TAG_WATCOM_address_class_type,
        0,
        {
            DW_AT_type,                 DW_FORM_ref4,
            DW_AT_address_class,        DW_FORM_data1,
        }
    },

    {
        "AB_SIMPLE_ARRAY_TYPE",
        DW_TAG_array_type,
         0,
        {
            DW_AT_count,                DW_FORM_udata,
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_ARRAY_TYPE",
        DW_TAG_array_type,
        DECL | AB_NAME | COMMON_NBITS | AB_SIBLING,
        {
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_ARRAY_TYPE_WITH_STRIDE",
        DW_TAG_array_type,
        DECL | AB_NAME | COMMON_NBITS | AB_SIBLING,
        {
            DW_AT_stride_size,          DW_FORM_udata,
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_ARRAY_BOUND",
        DW_TAG_subrange_type,
        AB_TYPE,
        {
            DW_AT_lower_bound,          DW_FORM_udata,
            DW_AT_upper_bound,          DW_FORM_udata,
            0
        }
    },

    {
        "AB_ARRAY_VARDIM",
        DW_TAG_subrange_type,
        AB_TYPE,
        {
            DW_AT_lower_bound,          DW_FORM_ref4,
            DW_AT_count,                DW_FORM_ref4,
            0
        }
    },

    {
        "AB_FRIEND",
        DW_TAG_friend,
        0,
        {
            DW_AT_friend,               DW_FORM_ref4,
            0
        }
    },

    {
        "AB_CLASS_TYPE",
        DW_TAG_class_type,
        DECL | AB_NAME | COMMON_NBITS | AB_SIBLING,
        {
            DW_AT_byte_size,            DW_FORM_udata,
            0
        }
    },
    {
        "AB_STRUCT_TYPE",
        DW_TAG_structure_type,
        DECL | AB_NAME | COMMON_NBITS | AB_SIBLING,
        {
            DW_AT_byte_size,            DW_FORM_udata,
            0
        }
    },
    {
        "AB_UNION_TYPE",
        DW_TAG_union_type,
        DECL | AB_NAME | COMMON_NBITS | AB_SIBLING,
        {
            DW_AT_byte_size,            DW_FORM_udata,
            0
        }
    },
    {
        "AB_INHERITANCE",
        DW_TAG_inheritance,
        DECL | AB_ACCESSIBILITY,
        {
            DW_AT_data_member_location, DW_FORM_block1,
            DW_AT_virtuality,           DW_FORM_data1,
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_FIELD",
        DW_TAG_member,
        DECL | AB_OPACCESS | AB_ARTIFICIAL,
        {
            DW_AT_name,                 DW_FORM_string,
            DW_AT_data_member_location, DW_FORM_block1,
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_BITFIELD",
        DW_TAG_member,
        DECL | AB_ACCESSIBILITY | AB_BYTE_SIZE,
        {
            DW_AT_bit_offset,           DW_FORM_udata,
            DW_AT_bit_size,             DW_FORM_udata,
            DW_AT_artificial,           DW_FORM_flag,
            DW_AT_name,                 DW_FORM_string,
            DW_AT_data_member_location, DW_FORM_block1,
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_ENUMERATION",
        DW_TAG_enumeration_type,
        DECL | AB_NAME | COMMON_NBITS | AB_SIBLING,
        {
            DW_AT_byte_size,            DW_FORM_udata,
            0
        }
    },

    {
        "AB_ENUMERATOR",
        DW_TAG_enumerator,
        DECL,
        {
            DW_AT_const_value,          DW_FORM_udata,
            DW_AT_name,                 DW_FORM_string,
            0
        }
    },

    {
        "AB_SUBROUTINE_TYPE",
        DW_TAG_subroutine_type,
        DECL | AB_NAME | COMMON_NBITS | AB_SIBLING |
            AB_ADDRESS_CLASS,
        {
            DW_AT_prototyped,           DW_FORM_flag,
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_STRING",
        DW_TAG_string_type,
        DECL | AB_NAME | COMMON_NBITS | AB_BYTE_SIZE,
        {
            0
        }
    },

    {
        "AB_STRING_WITH_LOC",
        DW_TAG_string_type,
        DECL | AB_NAME | COMMON_NBITS | AB_BYTE_SIZE,
        {
            DW_AT_string_length,        DW_FORM_block1,
            0
        }
    },

    {
        "AB_MEMBER_POINTER",
        DW_TAG_ptr_to_member_type,
        DECL | AB_NAME | AB_ADDRESS_CLASS,
        {
            DW_AT_type,                 DW_FORM_ref4,
            DW_AT_containing_type,      DW_FORM_ref4,
            DW_AT_use_location,         DW_FORM_block2,
            0
        }
    },

    {
        "AB_LEXICAL_BLOCK",
        DW_TAG_lexical_block,
        AB_NAME | AB_SEGMENT | AB_SIBLING,
        {
            DW_AT_low_pc,               DW_FORM_addr,
            DW_AT_high_pc,              DW_FORM_addr,
            0
        }
    },

    {
        "AB_COMMON_BLOCK",
        DW_TAG_common_block,
        DECL | AB_NAME | AB_SEGMENT | AB_DECLARATION | AB_SIBLING,
        {
            DW_AT_location,             DW_FORM_block1,
            0
        }
    },

    {
        "AB_INLINED_SUBROUTINE",
        DW_TAG_inlined_subroutine,
        AB_SEGMENT | AB_SIBLING | AB_RETURN_ADDR,
        {
            DW_AT_abstract_origin,      DW_FORM_ref4,
            DW_AT_low_pc,               DW_FORM_addr,
            DW_AT_high_pc,              DW_FORM_addr,
            0
        }
    },

    {
        "AB_SUBROUTINE",
        DW_TAG_subprogram,
        DECL | COMMON_NBITS | AB_START_SCOPE | AB_SIBLING | AB_TYPE
             | AB_MEMBER | AB_VTABLE_LOC | AB_SEGMENT,
        {
            DW_AT_name,                 DW_FORM_string,
            DW_AT_external,             DW_FORM_flag,
            DW_AT_inline,               DW_FORM_data1,
            DW_AT_calling_convention,   DW_FORM_data1,
            DW_AT_prototyped,           DW_FORM_flag,
            DW_AT_virtuality,           DW_FORM_data1,
            DW_AT_artificial,           DW_FORM_flag,
            DW_AT_return_addr,          DW_FORM_block1,
            DW_AT_low_pc,               DW_FORM_addr,
            DW_AT_high_pc,              DW_FORM_addr,
            DW_AT_address_class,        DW_FORM_data1,
            DW_AT_frame_base,           DW_FORM_block1,
            0
        }
    },
    {
        "AB_SUBROUTINE_DECL",
        DW_TAG_subprogram,
        DECL | COMMON_NBITS | AB_SIBLING | AB_TYPE
        | AB_MEMBER | AB_VTABLE_LOC,
        {
            DW_AT_name,                 DW_FORM_string,
            DW_AT_external,             DW_FORM_flag,
            DW_AT_inline,               DW_FORM_data1,
            DW_AT_calling_convention,   DW_FORM_data1,
            DW_AT_prototyped,           DW_FORM_flag,
            DW_AT_virtuality,           DW_FORM_data1,
            DW_AT_artificial,           DW_FORM_flag,
            DW_AT_declaration,          DW_FORM_flag,
            DW_AT_address_class,        DW_FORM_data1,
            DW_AT_frame_base,           DW_FORM_block1,
            0
        }
    },

    {
        "AB_ENTRY_POINT",
        DW_TAG_entry_point,
        DECL | COMMON_NBITS | AB_START_SCOPE | AB_RETURN_ADDR | AB_SIBLING | AB_SEGMENT
        | AB_TYPE,
        {
            DW_AT_low_pc,               DW_FORM_addr,
            DW_AT_address_class,        DW_FORM_data1,
            DW_AT_name,                 DW_FORM_string,
            0
        }
    },
#if 0
    {
        "AB_MEMFUNCDECLSEG",
        DW_TAG_subprogram,
        DECL | AB_ARTIFICIAL,
        {
            DW_AT_accessibility,        DW_FORM_data1,
            DW_AT_type,                 DW_FORM_ref4,
            DW_AT_name,                 DW_FORM_string,
            DW_AT_declaration,          DW_FORM_flag,
            DW_AT_inline,               DW_FORM_data1,
            DW_AT_segment,              DW_FORM_block1,
            DW_AT_location,             DW_FORM_block1,
            0
        }
    },
#endif
    {
        "AB_MEMFUNCDECL",
        DW_TAG_subprogram,
        DECL | AB_ARTIFICIAL,
        {
            DW_AT_accessibility,        DW_FORM_data1,
            DW_AT_type,                 DW_FORM_ref4,
            DW_AT_name,                 DW_FORM_string,
            DW_AT_declaration,          DW_FORM_flag,
            DW_AT_inline,               DW_FORM_data1,
            0
        }
    },
    {
        "AB_VIRTMEMFUNCDECL",
        DW_TAG_subprogram,
        DECL,
        {
            DW_AT_accessibility,        DW_FORM_data1,
            DW_AT_type,                 DW_FORM_ref4,
            DW_AT_name,                 DW_FORM_string,
            DW_AT_declaration,          DW_FORM_flag,
            DW_AT_inline,               DW_FORM_data1,
            DW_AT_virtuality,           DW_FORM_data1,
            DW_AT_vtable_elem_location, DW_FORM_block1,
            0
        }
    },
    {
        "AB_FORMAL_PARM_TYPE",
        DW_TAG_formal_parameter,
        DECL | AB_NAME,
        {
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },
    {
        "AB_FORMAL_PARAMETER",
        DW_TAG_formal_parameter,
        DECL,
        {
            DW_AT_name,                 DW_FORM_string,
            DW_AT_location,             DW_FORM_block1,
            DW_AT_WATCOM_parm_entry,    DW_FORM_block1,
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_FORMAL_PARAMETER_WITH_DEFAULT",
        DW_TAG_formal_parameter,
        DECL,
        {
            DW_AT_default_value,        DW_FORM_block1,
            DW_AT_name,                 DW_FORM_string,
            DW_AT_location,             DW_FORM_block1,
            DW_AT_WATCOM_parm_entry,    DW_FORM_block1,
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_ELLIPSIS",
        DW_TAG_unspecified_parameters,
        DECL,
        {
            0
        }
    },

    {
        "AB_LABEL",
        DW_TAG_label,
        AB_NAME | AB_START_SCOPE | AB_SEGMENT,
        {
            DW_AT_low_pc,               DW_FORM_addr,
            0
        }
    },

    {
        "AB_VARIABLE",
        DW_TAG_variable,
        DECL | COMMON_NBITS | AB_MEMBER | AB_SEGMENT
            | AB_DECLARATION,
        {
            DW_AT_external,             DW_FORM_flag,
            DW_AT_artificial,           DW_FORM_flag,
            DW_AT_name,                 DW_FORM_string,
            DW_AT_location,             DW_FORM_block1,
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },

    {
        "AB_CONSTANT",
        DW_TAG_variable,
        DECL | COMMON_NBITS | AB_MEMBER,
        {
            DW_AT_external,             DW_FORM_flag,
            DW_AT_const_value,          DW_FORM_block1,
            DW_AT_name,                 DW_FORM_string,
            DW_AT_type,                 DW_FORM_ref4,
            0
        }
    },
    {
        "AB_NAMELIST",
        DW_TAG_namelist,
        DECL | AB_SIBLING,
        {
            DW_AT_name,                 DW_FORM_string,
            0
        }
    },
    {
        "AB_NAMELIST_ITEM",
        DW_TAG_namelist_item,
        DECL,
        {
            DW_AT_namelist_item,        DW_FORM_ref4,
            0
        }
    },

    {
        "AB_COMMON_INCLUSION",
        DW_TAG_common_inclusion,
        DECL,
        {
            DW_AT_common_reference,     DW_FORM_ref4,
            0
        }
    },
    {
        "AB_NAMESPACE",
        DW_TAG_WATCOM_namespace,
        DECL | AB_NAME | AB_SIBLING,
        {
            0
        }
    },
};
#define AB_MAX  ( sizeof( abbrevInfo ) / sizeof( abbrevInfo[0] ) )


typedef struct {
    size_t              data_offset;
    size_t              data_len;
} extra_info;

extra_info abbrevExtra[AB_MAX];


void emitEnum(
    FILE *                      fp )
{
    uint                        u;

    fprintf( fp, "enum {\n    AB_PADDING,\n" );
    for( u = 0; u < AB_MAX; ++u ) {
        fprintf( fp, "    %s,\n", abbrevInfo[u].name );
    }
    fprintf( fp, "    AB_MAX\n};" );
}

size_t topOfEncoding;
uint_8 encodingBuf[AB_MAX * MAX_LEB128 * MAX_CODES];
uint_8 tempEncoding[MAX_LEB128 * MAX_CODES];

#define ANCHOR_NONE ((size_t)-1)

size_t addToEncoding(
    size_t                      this_size )
{
    size_t                      anchor;
    size_t                      dest;
    size_t                      src;

    if( this_size == 0 ) {
        return( 0 );
    }
    /*
        Scan the sequences of bytes we have so far, and see if
        we can overlay this one on top of another one.
    */
    dest = 0;
    src = 0;
    anchor = ANCHOR_NONE;
    while( dest < topOfEncoding ) {
        if( encodingBuf[dest] != tempEncoding[src] ) {
            src = 0;
            if( anchor == ANCHOR_NONE ) {
                ++dest;
            } else {
                dest = anchor + 1;
                anchor = ANCHOR_NONE;
            }
        } else {
            if( anchor == ANCHOR_NONE ) {
                anchor = dest;
            }
            ++dest;
            ++src;
            if( src == this_size ) {
                /* we've found a substring that matches exactly */
                return( anchor );
            }
        }
    }
    /*
        At this point we know that src bytes of the source string match
        the last src bytes of the destination string.
    */
    memcpy( &encodingBuf[dest], &tempEncoding[src], this_size - src );
    topOfEncoding += this_size - src;
    if( anchor == ANCHOR_NONE ) {
        return( dest );
    }
    return( anchor );
}


void emitEncodings(
    FILE *                      fp )
{
/*
    The plan is to take the above table and compress it into a smaller
    form.  The first obvious thing is to do the ULEB128 encodings now
    since they are compile-time constant, and much smaller.
*/
    uint                u;
    uint_8              *end;
    uint_8              *p;
    uint_32 const       *data;

    topOfEncoding = 0;
    for( u = 0; u < AB_MAX; ++u ) {
        /*
            Determine what the sequence of bytes is for this abbreviation
        */
        end = tempEncoding;
        for( data = abbrevInfo[u].data; *data; ++data ) {
            end = ULEB128( end, *data );
        }
        abbrevExtra[u].data_len = end - tempEncoding;
        abbrevExtra[u].data_offset = addToEncoding( end - tempEncoding );
    }

    fprintf( fp, "\nstatic const uint_8 encodings[] = {\n    /* 0x00 */ " );
    end = encodingBuf + topOfEncoding;
    p = encodingBuf;
    for(;;) {
        fprintf( fp, "0x%02x", *p );
        ++p;
        if( p == end ) break;
        fprintf( fp, "," );
        if( ( p - encodingBuf ) % 8 == 0 ) {
            fprintf( fp, "\n    /* 0x%02x */ ", (unsigned)( p - encodingBuf ) );
        }
    }
    fprintf( fp, "\n};\n\n" );
}


uint CountBits(
    uint_32                     value )
{
    uint                        number;

    number = 0;
    while( value ) {
        ++number;
        value &= value - 1;
    }
    return( number );
}


/*
    We must have a way to determine if a particular abbreviation has
    been emitted.

    An abbrev_code has a base portion that corresponds to the entries
    in abbrevInfo.  Then any bit from valid_mask can be set on
    to add attribute/forms to the abbreviation.

    Let 0 <= u < AB_MAX.

    Let V be the vector space GF(2)**32.  i.e. a vector space with
    32 dimensions, each of which is either 0 or 1.

    Then abbrevInfo[u].valid_mask can be considered to be a
    projection of V onto a subspace, call that subspace W(u).
    There are 2**dim(W(u)) possible vectors in W(u).  This is the
    number of bits we will require to represent each of the
    vectors in W(u).

    For example, suppose abbrevInfo[u].valid_mask == 0xa. Then

        W(u) = { 0b0000, 0b0010, 0b1000, 0b1010 }.

    Note that { 0b0010, 0b1000 } forms a basis for W(u), hence
    dim(W(u)) = 2.

    Let K(i) be the binary representation of the set of
    integers { 0, 1, ..., 2**i - 1 }.  Then K(i) can be considered
    a subspace of V. There is a natural mapping from W(u) to K(dim(W(u))).
    Using the W(u) example from above, we would map:

        W(u)    -->     K(2)
        0b0000          0b00
        0b0010          0b01
        0b1000          0b10
        0b1010          0b11

    The mapping basically "delete"s the co-ordinates of V that are
    not used in W(u).  We'll use this method to represent when an
    abbreviation has been output.  Basically we'll just concatenate
    all the spaces K(dim(W(u))) together for all u into a bitvector X.

    Like this:

                X =      x          K( dim( W( u ) ) )
                    0 <= u < AB_MAX

    (The 'x' indicates cross-product.)

    Isn't linear algebra wonderful? :)

    It would be slow for us to have to calculate the offset of a
    particular u in X every time, so we'll precalculate it for each u
    and store it in a structure.

    DJG
*/

uint_32 emitInfo( FILE *fp )
{
    uint_32                     total;
    uint                        u;

    fprintf( fp,"\nstatic const struct abbrev_info {\n"
                "    abbrev_code        valid_mask;\n"
                "    uint_16            bit_index;\n"
                "    uint_8             data_offset;\n"
                "    uint_8             data_len;\n"
                "    uint_16            tag;\n"
                "} abbrevInfo[] = {\n" );
    total = 1;
    u = 0;
    for(;;) {
        fprintf( fp, "/*%-32s*/{ 0x%08lx, 0x%04x, 0x%02x, 0x%02x, 0x%02x }",
            abbrevInfo[u].name,
            (unsigned long)abbrevInfo[u].valid_mask,
            (unsigned)total,
            (unsigned)abbrevExtra[u].data_offset,
            (unsigned)abbrevExtra[u].data_len,
            abbrevInfo[u].tag
        );
        if( abbrevExtra[u].data_offset > 0xff ) {
            /* just change the uint_8 data_offset in the above structure */
            fprintf( stderr, "data_offset too large, must increase size of data_offset field\n" );
            exit( 1 );
        }
        if( abbrevExtra[u].data_len > 0xff ) {
            /* just change the uint_8 data_len in the above structure */
            fprintf( stderr, "data_len too large, must increase size of data_len field\n" );
            exit( 1 );
        }
        if( abbrevInfo[u].tag > 0xffff ) {
            /* just change the uint_16 tag in the above structure */
            fprintf( stderr, "Tag too large, must increase size of tag field\n" );
            exit( 1 );
        }
        total += 1 << CountBits( abbrevInfo[u].valid_mask & ~AB_ALWAYS );
        ++u;
        if( u == AB_MAX ) break;
        if( total > 0xffff ) {
            /* just change the uint_16 bit_index in the above structure */
            fprintf( stderr, "Index too large, must increase size of index field\n" );
            exit( 1 );
        }
        fprintf( fp, ",\n" );
    }
    fprintf( fp, "\n};\n" );
    return( total );
}


int main( void )
{

    FILE *                      fp;
    uint_32                     total;

    fp = fopen( "dwabinfo.gh", "w" );
    if( fp == NULL ) {
        fprintf( stderr, "unable to open dwabinfo.gh for writing: %s",
            strerror( errno ) );
        return( 1 );
    }
    fprintf( fp, "/* this file created by dwmakeab.c */\n" );
    emitEncodings( fp );
    total = emitInfo( fp );
    fclose( fp );

    fp = fopen( "dwabenum.gh", "w" );
    if( fp == NULL ) {
        fprintf( stderr, "unable to open dwabenum.gh for writing: %s",
            strerror( errno ) );
        return( 1 );
    }
    fprintf( fp, "/* this file created by dwmakeab.c */\n" );
    /* we need this value for determining the size of the dw_client struct */
    fprintf( fp, "\n#define AB_LAST_CODE    0x%08lx\n\n", (unsigned long)total );
    fprintf( fp, "\n#define AB_BITVECT_SIZE 0x%08lx\n\n", (unsigned long)( ( total + 7 ) / 8 ) );
    emitEnum( fp );
    fprintf( fp, "\n" );
    fclose( fp );

    return( 0 );
}
