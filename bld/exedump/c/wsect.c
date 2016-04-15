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
* Description:  DWARF section processing.
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "wdglb.h"
#include "wdfunc.h"

#include "clibext.h"


static readable_name readableTAGs[] = {
    #define DWTAGI( __n, __v  )   table( DW_TAG_##__n ),
    #include "dwtagi.h"
    #undef  DWTAGI
};

#define NUM_TAGS        ( sizeof( readableTAGs ) / sizeof( readableTAGs[0] ) )

static readable_name readableFORMs[] = {
    #define DWFORMI( __n, __v  )   table( DW_FORM_##__n ),
    #include "dwformi.h"
    #undef  DWFORMI
};
#define NUM_FORMS       ( sizeof( readableFORMs ) / sizeof( readableFORMs[0] ) )

static readable_name readableATs[] = {
    #define DWATI( __n, __v  )   table( DW_AT_##__n ),
    #include "dwati.h"
    #undef  DWATI
};
#define NUM_ATS         ( sizeof( readableATs ) / sizeof( readableATs[0] ) )



static readable_name readableReferenceOps[] = {
    table( REF_BEGIN_SCOPE ),
    table( REF_END_SCOPE ),
    table( REF_SET_FILE ),
    table( REF_SET_LINE ),
    table( REF_SET_COLUMN ),
    table( REF_ADD_LINE ),
    table( REF_ADD_COLUMN ),
    table( REF_COPY )
};
#define NUM_REFERENCE_OPS \
    ( sizeof( readableReferenceOps ) / sizeof( readableReferenceOps[0] ) )

static const_string_table sectionNames[] = {
    ".debug_info",
    ".debug_pubnames",
    ".debug_aranges",
    ".debug_line",
    ".debug_loc",
    ".debug_abbrev",
    ".debug_macinfo",
    ".debug_str",
    ".WATCOM_references"
};


static int compare_table( const void *pa, const void *pb )
/********************************************************/
{
    const readable_name *a = pa;
    const readable_name *b = pb;

    if( a->value > b->value ) {
        return( 1 );
    } else if( a->value == b->value ) {
        return( 0 );
    }
    return( -1 );
}


static void sort_tables( void )
/*****************************/
{
    static int  done = 0;

    if( !done ) {
        done = 1;
        qsort( readableTAGs, NUM_TAGS, sizeof( readable_name ), compare_table );
        qsort( readableFORMs, NUM_FORMS, sizeof( readable_name ), compare_table );
        qsort( readableATs, NUM_ATS, sizeof( readable_name ), compare_table );
    }
}


const char *Getname( uint_32 value, readable_name *table, size_t size )
/***************************************************************/
{
    readable_name       dummy;
    readable_name       *result;

    dummy.value = value;
    result = bsearch( &dummy, table, size, sizeof( readable_name ), compare_table );
    if( result == NULL ) return( NULL );
    return( (const char *)result->name );
}


static void getTAG( uint_32 value )
/*********************************/
{
    const char  *result;
    int         i;

    result = Getname( value, readableTAGs, NUM_TAGS );
    if( result == NULL ) {
        Wdputs( "TAG_" );
        Puthex( value, 8 );
        Wdputs( "                  " );
    } else {
        Wdputs( result );
        for( i = strlen( result ); i < 30; i++ ) {
            Wdputc( ' ' );
        }
    }
}


static void getFORM( uint_32 value )
/**********************************/
{
    const char  *result;
    int         i;

    result = Getname( value, readableFORMs, NUM_FORMS );
    if( result == NULL ) {
        Wdputs( "FORM_" );
        Puthex( value, 8 );
        Wdputs( "     " );
    } else {
        Wdputs( result );
        for( i = strlen( result ); i < 18; i++ ) {
            Wdputc( ' ' );
        }
    }
}


static void getAT( uint_32 value )
/********************************/
{
    const char  *result;
    int         i;

    result = Getname( value, readableATs, NUM_ATS );
    if( result == NULL ) {
        Wdputs( "AT_" );
        Puthex( value, 8 );
        Wdputs( "                   " );
    } else {
        Wdputs( result );
        for( i = strlen( result ); i < 30; i++ ) {
            Wdputc( ' ' );
        }
    }
}


static void dump_hex( const uint_8 *input, uint length )
/******************************************************/
{
    char        *p;
    int         i;
    uint        offset;
    uint        old_offset;
    char        hex[ 80 ];
    char        printable[ 17 ];
    int         ch;

    offset = 0;
    for( ;; ) {
        i = 0;
        p = hex;
        old_offset = offset;
        for( ;; ) {
            if( offset == length ) break;
            if( i > 0xf ) break;
            if( i == 0x8 ) {
                *p++ = ' ';
            }
            ch = input[ offset ];
            p += sprintf( p, " %02x", ch );
            printable[ i ] = isprint( ch ) ? ch : '.';
            ++i;
            ++offset;
        }
        *p = 0;
        printable[ i ] = 0;
        Puthex( old_offset, 8 );
        Wdputc( ':' );
        Wdputs( hex );
        Wdputs( " <" );
        Wdputs( printable );
        Wdputslc( ">\n" );
//      Wdputs( "%08lx:%-49s <%s>\n", old_offset, hex, printable );
        p = printable;
        i = 0;
        if( offset == length ) break;
    }
}

uint_8 *DecodeULEB128( const uint_8 *input, uint_32 *value )
/**********************************************************/
{
    uint_32     result;
    uint        shift;
    uint_8      byte;

    result = 0;
    shift = 0;
    do {
        byte = *input++;
        result |= ( byte & 0x7f ) << shift;
        shift += 7;
    } while( byte & 0x80 );
    *value = result;
    return( (uint_8 *)input );
}

uint_8 *DecodeSLEB128( const uint_8 *input, int_32 *value )
/*********************************************************/
{
    int_32      result;
    uint        shift;
    uint_8      byte;

    result = 0;
    shift = 0;
    do {
        byte = *input++;
        result |= ( byte & 0x7f ) << shift;
        shift += 7;
    } while( byte & 0x80 );
    if( ( shift < 32 ) && ( byte & 0x40 ) ) {
        result |= - ( 1 << shift );
    }
    *value = result;
    return( (uint_8 *)input );
}

uint_8 *find_abbrev( uint_32 start, uint_32 code )
/************************************************/
{
    uint_8      *p;
    uint_8      *stop;
    uint_32     tmp;
    uint_32     attr;

    p = Sections[ DW_DEBUG_ABBREV ].data;
    p += start;
    stop = p + Sections[ DW_DEBUG_ABBREV ].max_offset;
    for( ;; ) {
        if( p >= stop )
            return( NULL );
        p = DecodeULEB128( p, &tmp );
        if( tmp == code )
            return( p );
        if( p >= stop )
            return( NULL );
        p = DecodeULEB128( p, &tmp );
        if( p >= stop )
            return( NULL );
        p++;
        for( ;; ) {
            p = DecodeULEB128( p, &attr );
            if( p >= stop )
                return( NULL );
            p = DecodeULEB128( p, &tmp );
            if( p >= stop )
                return( NULL );
            if( attr == 0 ) {
                break;
            }
        }
    }
}

static dw_locop_op const LocOpr[] = {
#define DW_LOC_OP( __n, __v )    __v,
#include "dwlocinf.h"
#undef DW_LOC_OP
};

static const_string_table OpName[] = {
#define DW_LOC_OP( __n, __v )    #__n,
#include "dwlocinf.h"
#undef DW_LOC_OP
};
static const_string_table RegName[] = {
#define DW_REG( __n )    #__n,
#include "dwreginf.h"
#undef DW_REG
};

static uint_8 const *GetInt( uint_8 const *p, uint_32 *ret, int size )
/********************************************************************/
{
    switch( size ) {
    case 0:
        *ret = 0;
        break;
    case 1:
        *ret = *p;
        break;
    case 2:
        *ret = get_u16( (uint_16 *)p );
        break;
    case 4:
        *ret = get_u32( (uint_32 *)p );
        break;
    default:
        *ret = 0xffffffff;
        break;
    }
    p += size;
    return( p );
}

static void DmpLoc( uint_8 const *p, uint length, uint addr_size )
/****************************************************************/
{
    uint_8 const    *end;
    uint_8          op;
    dw_locop_op     opr;
    int_32          op1s;
    uint_32         op1u;
    int_32          op2s;
    uint_32         addr;

    end = &p[length];

    Wdputslc( "\n            Loc expr: " );
    if( p == end ) {
      Wdputslc( "<NULL>\n" );
    }
    while( p  < end ) {
        op = *p;
        ++p;

        Wdputs( OpName[ op ] );
        opr = LocOpr[ op ];
        if( opr == DW_LOP_REG1 || opr == DW_LOP_BRG1 ) {
            Wdputs( "/" );
        } else {
            Wdputs( " " );
        }
        switch( opr ) {
        case DW_LOP_NOOP:
            break;
        case DW_LOP_ADDR:
            if( addr_size == 4 ) {
                addr = *(uint_32 *)p;
            } else if( addr_size == 2 ) {
                addr = *(uint_16 *)p;
            } else if( addr_size == 1 ) {
                addr = *(uint_8 *)p;
            } else {
                addr = 0;
            }
            Puthex( addr, addr_size * 2 );
            p += addr_size;
            break;
        case DW_LOP_OPU1:
            op1u = *(uint_8 *)p;
            p += sizeof( uint_8 );
            Putdec( op1u );
            break;
        case DW_LOP_OPS1:
            op1s = *(int_8 *)p;
            p += sizeof(int_8 );
            Putdec( op1s );
            break;
        case DW_LOP_OPU2:
            op1u = get_u16( (uint_16 *)p );
            p += sizeof( uint_16 );
            Putdec( op1u );
            break;
        case DW_LOP_OPS2:
            op1s = get_s16( (int_16 *)p );
            p += sizeof( int_16 );
            Putdec( op1s );
            break;
        case DW_LOP_OPU4:
            op1u = get_u32( (uint_32 *)p );
            p += sizeof( uint_32 );
            Putdec( op1u );
            break;
        case DW_LOP_OPS4:
            op1s = get_s32( (int_32 *)p );
            p += sizeof( int_32 );
            Putdec( op1s );
            break;
        case DW_LOP_U128:
            p = DecodeULEB128( p, &op1u );
            Putdec( op1u );
            break;
        case DW_LOP_S128:
            p = DecodeSLEB128( p, &op1s );
            Putdecs( op1s );
            break;
        case DW_LOP_U128_S128:
            p = DecodeULEB128( p, &op1u );
            p = DecodeSLEB128( p, &op2s );
            Putdec( op1u );
            Wdputs( "," );
            Putdecs( op2s );
            break;
        case DW_LOP_LIT1:
            op1u = op-DW_OP_lit0;
            op = DW_OP_lit0;
            break;
        case DW_LOP_REG1:
            op1u = op-DW_OP_reg0;
            op = DW_OP_reg0;
            Wdputs( RegName[ op1u] );
            break;
        case DW_LOP_BRG1:
            op1u = op-DW_OP_breg0;
            p = DecodeSLEB128( p, &op2s );
            Wdputs( RegName[ op1u] );
            if( op2s < 0 ) {
                Wdputs( " -" );
                op2s = -op2s;
            } else {
                Wdputs( " +" );
            }
            Putdec( op2s );
            op = DW_OP_breg0;
            break;
        }
        Wdputslc( " " );
    }
        Wdputslc( "\n" );
}

static void DmpLocList( uint_32 start, uint addr_size )
/*****************************************************/
{
    uint_32         low;
    uint_32         high;
    int             len;
    uint_8 const    *p;
    uint_8 const    *stop;

    p = Sections[ DW_DEBUG_LOC ].data;
    stop = p + Sections[ DW_DEBUG_LOC ].max_offset;
    if( p == NULL ) {
        Wdputslc( "Error: No location list section\n" );
        return;
    }
    p += start;
    while( p < stop ) {
        p = GetInt( p, &low, addr_size );
        p = GetInt( p, &high, addr_size );
        if( low == high && low == 0 ) {
            Wdputslc( "        <end>\n" );
            return;
        }
        len = get_u16( (uint_16 *)p );
        p+= sizeof( uint_16 );
        Wdputslc( "\n         Range: " );
        Puthex( low, addr_size * 2 );
        Wdputs( ":" );
        Puthex( high, addr_size * 2 );
        Wdputslc( "\n" );
        DmpLoc( p, len, addr_size );
        p += len;
    }
}

typedef struct {
    uint_8 const    *p;
    uint_8          *abbrev;
    int             addr_size;
    uint_32         cu_header;
} info_state;

static bool dump_tag( info_state *info )
/**************************************/
{
    uint_8          *abbrev;
    uint_32         attr;
    uint_32         offset;
    uint_32         form;
    uint_32         len;
    uint_32         tmp;
    int_32          itmp;
    bool            is_loc;
    uint_8 const    *p;

    p = info->p;
    abbrev = info->abbrev;
    for( ;; ) {
        abbrev = DecodeULEB128( abbrev, &attr );
        abbrev = DecodeULEB128( abbrev, &form );
        if( attr == 0 ) break;
        Wdputs( "        " );
        getAT( attr );
        if( attr == DW_AT_location
         || attr == DW_AT_segment
         || attr == DW_AT_return_addr
         || attr == DW_AT_frame_base
         || attr == DW_AT_static_link
         || attr == DW_AT_data_member_location
         || attr == DW_AT_string_length
         || attr == DW_AT_vtable_elem_location
         || attr == DW_AT_WATCOM_parm_entry
         || attr == DW_AT_use_location ) {
            is_loc = true;
       } else {
            is_loc = false;
       }
decode_form:
        switch( form ) {
        case DW_FORM_addr:
            if( info->addr_size == 4 ) {
                tmp = get_u32( (uint_32 *)p );
            } else if( info->addr_size == 2 ) {
                tmp = get_u16( (uint_16 *)p );
            } else if( info->addr_size == 1 ) {
                tmp = *(uint_8 *)p;
            } else {
                tmp = info->addr_size;
                Wdputs( "?addr:" );
            }
            p += info->addr_size;
            Puthex( tmp, info->addr_size*2 );
            Wdputslc( "\n" );
            break;
        case DW_FORM_block:
            p = DecodeULEB128( p, &len );
            if( is_loc ) {
                DmpLoc( p, len, info->addr_size );
            } else {
                Wdputslc( "\n" );
                dump_hex( p, len );
            }
            p += len;
            break;
        case DW_FORM_block1:
            len = *p++;
            if( is_loc ) {
                DmpLoc( p, len, info->addr_size );
            } else {
                Wdputslc( "\n" );
                dump_hex( p, len );
            }
            p += len;
            break;
        case DW_FORM_block2:
            len = get_u16( (uint_16 *)p );
            p += sizeof( uint_16 );
            if( is_loc ) {
                DmpLoc( p, len, info->addr_size );
            } else {
                Wdputslc( "\n" );
                dump_hex( p, len );
            }
            p += len;
            break;
        case DW_FORM_block4:
            len = get_u32( (uint_32 *)p );
            p += sizeof( uint_32 );
            if( is_loc ) {
                DmpLoc( p, len, info->addr_size );
            } else {
                Wdputslc( "\n" );
                dump_hex( p, len );
            }
            p += len;
            break;
        case DW_FORM_data1:
            Puthex( *p++, 2 );
            Wdputslc( "\n" );
            break;
        case DW_FORM_ref1:
            Puthex( info->cu_header + *p++ , 2 );
            Wdputslc( "\n" );
            break;
        case DW_FORM_data2:
            Puthex( get_u16( (uint_16 *)p ), 4 );
            Wdputslc( "\n" );
            p += sizeof( uint_16 );
            break;
        case DW_FORM_ref2:
            Puthex( info->cu_header + get_u16( (uint_16 *)p ), 4 );
            Wdputslc( "\n" );
            p += sizeof( uint_16 );
            break;
        case DW_FORM_data4:
            if( is_loc ) {
                DmpLocList( get_u32( (uint_32 *)p ), info->addr_size );
            } else {
                Puthex( get_u32( (uint_32 *)p ), 8 );
                Wdputslc( "\n" );
            }
            p += sizeof( uint_32 );
            break;
        case DW_FORM_ref4:
            Puthex( info->cu_header + get_u32( (uint_32 *)p ), 8 );
            Wdputslc( "\n" );
            p += sizeof( uint_32 );
            break;
        case DW_FORM_flag:
            Wdputs( *p++ ? "True" : "False" );
            Wdputslc( "\n" );
            break;
        case DW_FORM_indirect:
            p = DecodeULEB128( p, &form );
            Wdputc( '(' );
            getFORM( form );
            Wdputc( ')' );
            goto decode_form;
        case DW_FORM_sdata:
            p = DecodeSLEB128( p, &itmp );
            Puthex( itmp, 8 );
            Wdputslc( "\n" );
            break;
        case DW_FORM_string:
            Wdputc( '"' );
            Wdputs( (char *)p );
            Wdputslc( "\"\n" );
            p += strlen( (const char *)p ) + 1;
            break;
        case DW_FORM_strp:
            offset = get_u32( (uint_32 *)p );
            if( offset > Sections[ DW_DEBUG_STR ].max_offset ) {
                Wdputslc( "Error: strp - invalid offset\n" );
            } else {
                Wdputs( (const char *)Sections[ DW_DEBUG_STR ].data + offset );
                Wdputslc( "\n" );
            }
            p += sizeof( uint_32 );
            break;
        case DW_FORM_udata:
            p = DecodeULEB128( p, &tmp );
            Puthex( tmp, 8 );
            Wdputslc( "\n" );
            break;
        case DW_FORM_ref_udata:
            p = DecodeULEB128( p, &tmp );
            Puthex( info->cu_header + tmp, 8 );
            Wdputslc( "\n" );
            break;
        case DW_FORM_ref_addr:
            if( is_loc ) { // history
                DmpLocList( get_u32( (uint_32 *)p ), info->addr_size );
            } else {
                Puthex( get_u32( (uint_32 *)p ), 8 );
                Wdputslc( "\n" );
            }
            p += sizeof( uint_32 );
            break;
        default:
            Wdputslc( "unknown form\n" );
            info->p = p;
            return( false );
        }
    }
    info->p = p;
    return( true );
}


static void dump_info( const uint_8 *input, uint length )
/*******************************************************/
{
    const uint_8    *p;
    uint_32         abbrev_code;
    uint_32         abbrev_offset;
    uint_8          *abbrev;
    uint_32         tag;
    uint_32         unit_length;
    const uint_8    *unit_base;
    info_state      state;

    p = input;
    state.addr_size = 0;
    while( p - input < length ) {
        state.cu_header = p-input;
        unit_length = get_u32( (uint_32 *)p );
        unit_base = p + sizeof( uint_32 );

        Wdputs( "Offset: " );
        Puthex( p - input, 8 );
        Wdputs( "  Length: " );
        Puthex( unit_length, 8 );
        Wdputslc( "  Version: " );
        Puthex( get_u16( (uint_16 *)(p + 4) ), 4 );
        Wdputs( " Abbrev: " );
        abbrev_offset =  get_u32( (uint_32 *)(p + 6) );
        Puthex( abbrev_offset, 8 );
        state.addr_size = *(p + 10);
        Wdputs( " Address Size " );
        Puthex( *(p + 10), 2 );
        Wdputslc( "\n" );
        p += 11;
        while( p - unit_base < unit_length ) {
            Wdputs( "Offset: " );
            Puthex( p - input, 8 );
            p = DecodeULEB128( p, &abbrev_code );
            Wdputs( "  Code: " );
            Puthex( abbrev_code, 8 );
            Wdputslc( "\n" );
            if( abbrev_code == 0 ) continue;
            abbrev = find_abbrev( abbrev_offset, abbrev_code );
            if( abbrev == NULL ) {
                Wdputs( "can't find abbreviation " );
                Puthex( abbrev_code, 8 );
                Wdputslc( "\n" );
                break;
            }
            if( p >= input + length ) break;
            abbrev = DecodeULEB128( abbrev, &tag );
            Wdputs( "        " );
            getTAG( tag );
            Wdputslc( "\n" );
            abbrev++;
            state.abbrev = abbrev;
            state.p = p;
            if( !dump_tag( &state ) )break;
            p = state.p;
        }
    }
}

#if 0    // used for debugging
static bool skip_tag( info_state *info )
/*****************************************************/
{
    uint_8      *abbrev;
    uint_32     attr;
    uint_32     form;
    uint_32     len;
    uint_32     tmp;
    int_32      itmp;
    bool        is_loc;
    char const  *p;

    p = info->p;
    abbrev = info->abbrev;
    for( ;; ) {
        abbrev = DecodeULEB128( abbrev, &attr );
        abbrev = DecodeULEB128( abbrev, &form );
        if( attr == 0 ) break;
        if( attr == DW_AT_location
         || attr == DW_AT_segment
         || attr == DW_AT_return_addr
         || attr == DW_AT_frame_base
         || attr == DW_AT_static_link
         || attr == DW_AT_data_member_location
         || attr == DW_AT_string_length
         || attr == DW_AT_use_location ) {
            is_loc = true;
       } else {
            is_loc = false;
       }
decode_form:
        switch( form ) {
        case DW_FORM_addr:
            if( info->addr_size == 4 ) {
                tmp = get_u32( (uint_32 *)p );
            } else if( info->addr_size == 2 ) {
                tmp = get_u16( (uint_16 *)p );
            } else if( info->addr_size == 1 ) {
                tmp = *(uint_8 *)p;
            } else {
                tmp = info->addr_size;
                Wdputs( "?addr:" );
            }
            p += info->addr_size;
            break;
        case DW_FORM_block:
            p = DecodeULEB128( p, &len );
            p += len;
            break;
        case DW_FORM_block1:
            len = *p++;
            p += len;
            break;
        case DW_FORM_block2:
            len = get_u16( (uint_16 *)p );
            p += sizeof( uint_16 );
            p += len;
            break;
        case DW_FORM_block4:
            len = get_u32( (uint_32 *)p );
            p += sizeof( uint_32 );
            p += len;
            break;
        case DW_FORM_data1:
            ++p;
            break;
        case DW_FORM_ref1:
            ++p;
            break;
        case DW_FORM_data2:
            p += sizeof( uint_16 );
            break;
        case DW_FORM_ref2:
            p += sizeof( uint_16 );
            break;
        case DW_FORM_data4:
            p += sizeof( uint_32 );
            break;
        case DW_FORM_ref4:
            p += sizeof( uint_32 );
            break;
        case DW_FORM_flag:
            ++p;
            break;
        case DW_FORM_indirect:
            p = DecodeULEB128( p, &form );
            goto decode_form;
        case DW_FORM_sdata:
            p = DecodeSLEB128( p, &itmp );
            break;
        case DW_FORM_string:
            p += strlen( p ) + 1;
            break;
        case DW_FORM_strp:
            abort();
            break;
        case DW_FORM_udata:
            p = DecodeULEB128( p, &tmp );
            break;
        case DW_FORM_ref_udata:
            p = DecodeULEB128( p, &tmp );
            break;
        case DW_FORM_ref_addr:
            p += sizeof( uint_32 );
            break;
        default:
            Wdputslc( "unknown form\n" );
            info->p = p;
            return( false );
        }
    }
    info->p = p;
    return( true );
}

static void dump_info_headers( const char *input, uint length )
/*************************************************************/
{
    const uint_8 *p;
    uint_32     abbrev_code;
    uint_32     abbrev_offset;
    uint_8 *    abbrev;
    uint_32     tag;
    uint_32     unit_length;
    uint_32     tag_offset;
    const uint_8 *unit_base;
    info_state  state;
    bool        found;

    p = input;
    state.addr_size = 0;
    found = false;
    while( p - input < length ) {
        state.cu_header = p - input;
        unit_length = get_u32( (uint_32 *)p );
        unit_base = p + sizeof( uint_32 );
        Wdputs( "Length: " );
        Puthex( unit_length, 8 );
        Wdputslc( "\nVersion: " );
        Puthex( get_u16( (uint_16 *)(p + 4) ), 4 );
        Wdputslc( "\nAbbrev: " );
        abbrev_offset =  get_u32( (uint_32 *)(p + 6) );
        Puthex( abbrev_offset, 8 );
        state.addr_size = *(p + 10);
        Wdputslc( "\nAddress Size " );
        Puthex( *(p + 10), 2 );
        Wdputslc( "\n" );
        p += 11;
        while( p - unit_base < unit_length ) {
            tag_offset = p - input;
            p = DecodeULEB128( p, &abbrev_code );
            if( abbrev_code == 0 ) continue;
            abbrev = find_abbrev( abbrev_offset, abbrev_code );
            if( abbrev == NULL ) {
                Wdputs( "can't find abbreviation " );
                Puthex( abbrev_code, 8 );
                Wdputslc( "\n" );
                break;
            }
            if( p >= input + length ) break;
            abbrev = DecodeULEB128( abbrev, &tag );
            abbrev++;
            state.abbrev = abbrev;
            state.p = p;
            if( tag_offset == 0x59a125 ) {
                found = true;
            }
            if( found ) {
                Wdputs( "Offset: " );
                Puthex(  tag_offset, 8 );
                Wdputs( "  Code: " );
                Puthex( abbrev_code, 8 );
                Wdputslc( "\n" );
                Wdputs( "        " );
                getTAG( tag );
                Wdputslc( "\n" );
               if( !dump_tag( &state ) )break;
            } else {
                skip_tag( &state );
            }
            p = state.p;
        }
        if( found )break;
    }
}
#endif

extern void dump_abbrevs( const uint_8 *input, uint length )
/**********************************************************/
{
    const uint_8    *p;
    uint_32         tmp;
    uint_32         attr;
    uint_32         abbr_off;
//    bool            start;

    if( Sections[ DW_DEBUG_ABBREV ].data == 0 ) {
        Sections[ DW_DEBUG_ABBREV ].data = Wmalloc( length );
        Sections[ DW_DEBUG_ABBREV ].max_offset = length;
        memcpy( Sections[ DW_DEBUG_ABBREV ].data, input, length );
    }
    p = input;
    for( ;; ) {
        if( p > input + length )
            break;
        abbr_off = tmp = p - input;
        p = DecodeULEB128( p, &tmp );
        if( tmp == 0 ) {
            Wdputslc( "End_CU\n" );
//            start = true;
            continue; /* compile unit separator */
        }
        Wdputs( "Offset: " );
        Puthex( abbr_off, 8 );
        Wdputs( "  Code: " );
        Puthex( tmp, 8 );
        Wdputslc( "\n" );
        if( p >= input + length )
            break;
        if( tmp == 0 )
            continue; /* compile unit separator */
        p = DecodeULEB128( p, &tmp );
        Wdputs( "        " );
        getTAG( tmp );
        Wdputslc( "\n" );
        if( *p == DW_CHILDREN_yes ) {
            Wdputslc( "has children\n" );
        } else {
            Wdputslc( "childless\n" );
        }
        p++;
        for( ;; ) {
            if( p > input + length )
                break;
            p = DecodeULEB128( p, &attr );
            if( p > input + length )
                break;
            p = DecodeULEB128( p, &tmp );
            if( attr == 0 && tmp == 0 ) {
                Wdputslc( "        End_form\n" );
                break;
            }
            Wdputs( "        " );
            getAT( attr );
            Wdputs( "        " );
            getFORM( tmp );
            Wdputslc( "\n" );
            if( attr == 0 ) {
                break;
            }
        }
    }
}


static void get_reference_op( uint_8 value )
/******************************************/
{
    const char  *result;
    int         i;

    result = Getname( value, readableReferenceOps, NUM_REFERENCE_OPS );
    if( result == NULL ) {
        Wdputs( "REF_" );
        Puthex( value, 2 );
        Wdputs( "          " );
    } else {
        Wdputs( result );
        for( i = strlen( result ); i < 16; i++ ) {
            Wdputc( ' ' );
        }
    }
}

typedef struct {
    int         size;   /* available room */
    int         free;   /* next free entry */
    uint_32 *   stack;  /* values */
} scope_stack;

typedef struct {
    scope_stack     scope;
    uint_32         dependent;
    uint_32         file;
    uint_32         line;
    uint_8          column;
} ref_info;

#define SCOPE_GUESS 0x50

static void ScopePush( scope_stack * stack, uint_32 entry )
/************************************************************/
{
    if( stack->stack == NULL ) {
        stack->stack = Wmalloc( SCOPE_GUESS * sizeof( uint_32 ) );
    }
    if( stack->free >= stack->size ) {
        stack->size += SCOPE_GUESS;
        stack->stack = realloc( stack->stack, stack->size * sizeof( uint_32 ) );
    }

    stack->stack[ stack->free ] = entry;
    stack->free += 1;
}

static uint_32 ScopePop( scope_stack *stack )
/*******************************************/
{
    if( stack->free <= 0 ) {
        // error
    }

    stack->free -= 1;
    return stack->stack[ stack->free ];
}

static void PutRefRegisters( ref_info *registers )
/************************************************/
{
    Wdputs( "[ " );
    if( registers->scope.free > 0 ) {
        Puthex( registers->scope.stack[ registers->scope.free - 1 ], 8 );
    } else {
        Puthex( 0, 8 );
    }
    Wdputs( " " );
    Putdec( registers->file );
    Wdputs( " " );
    Putdec( registers->line );
    Wdputs( " " );
    Putdec( registers->column );
    Wdputs( " " );
    Puthex( registers->dependent, 8 );
    Wdputs( " ]" );
}

static void dump_ref( const uint_8 *input, uint length )
/******************************************************/
{
    const uint_8    *p;
    uint_8          op_code;
    uint_32         tmp;
    int_32          itmp;
    uint_32         unit_length;
    const uint_8    *unit_base;
    ref_info        registers =  { { 0, 0, NULL }, 0L, 1L, 1L, 1 };

    p = input;

    while( p - input < length ) {
        unit_length = get_u32( (uint_32 *)p );
        p += sizeof( uint_32 );
        unit_base = p;
        Wdputs( "total_length: " );
        Puthex( get_u32( (uint_32 *)p ), 8 );
        Wdputslc( "\n" );

        while( p - unit_base < unit_length ) {
            op_code = *p++;
            if( op_code < REF_CODE_BASE ) {
                get_reference_op( op_code );
                switch( op_code ) {
                case REF_BEGIN_SCOPE:
                    Puthex( get_u32( (uint_32 *)p ), 8 );
                    ScopePush( &registers.scope, get_u32( (uint_32 *)p ) );
                    p += sizeof( uint_32 );
                    break;
                case REF_END_SCOPE:
                    ScopePop( &registers.scope );
                    break;
                case REF_COPY:
                    Wdputs( "    " );
                    PutRefRegisters( &registers );
                    break;
                case REF_SET_FILE:
                    p = DecodeULEB128( p, &tmp );
                    Putdec( tmp );
                    registers.file = tmp;
                    break;
                case REF_SET_LINE:
                    p = DecodeULEB128( p, &tmp );
                    Putdec( tmp );
                    registers.line = tmp;
                    registers.column = 0;
                    break;
                case REF_SET_COLUMN:
                    p = DecodeULEB128( p, &tmp );
                    Putdec( tmp );
                    registers.column = tmp;
                    break;
                case REF_ADD_LINE:
                    p = DecodeSLEB128( p, &itmp );
                    Putdecs( itmp );
                    registers.line += itmp;
                    break;
                case REF_ADD_COLUMN:
                    p = DecodeULEB128( p, &tmp );
                    Putdec( tmp );
                    registers.column += tmp;
                    break;
                }
            } else {
                op_code -= REF_CODE_BASE;
                Wdputs( "REF line += " );
                Putdec( op_code / REF_COLUMN_RANGE );
                registers.line += op_code / REF_COLUMN_RANGE;

                Wdputs( ", column += " );
                Putdec( op_code % REF_COLUMN_RANGE );
                registers.column += op_code % REF_COLUMN_RANGE;

                Wdputs( ", " );
                Puthex( get_u32( (uint_32 *)p ), 8 );
                registers.dependent = get_u32( (uint_32 *)p );

                Wdputs( "    " );
                PutRefRegisters( &registers );

                p += sizeof( uint_32 );
            }
            Wdputslc( "\n" );
        }
    }
}


static void dump_aranges( const uint_8 *p, uint length )
/******************************************************/
{
    const uint_8    *end;
    const uint_8    *unit_end;
    uint_32         unit_length;
    uint_32         dbg_offset;
    uint_32         addr;
    uint_32         seg;
    uint_32         len;
    int             addr_size;
    int             seg_size;

    end = &p[length];
    addr_size = 0;
    while( p  < end ) {
        unit_length = get_u32( (uint_32 *)p );
        unit_end = &p[unit_length + sizeof( uint_32 )];
        Wdputs( "Length: " );
        Puthex( unit_length, 8 );
        Wdputslc( "\nVersion: " );
        Puthex( get_u16( (uint_16 *)(p + 4) ), 4 );
        Wdputslc( "\nDbg Info: " );
        dbg_offset = get_u32( (uint_32 *)(p + 6) );
        Puthex( dbg_offset, 8 );
        addr_size = *(p + 10);
        Wdputslc( "\nAddress Size " );
        Puthex( addr_size, 2 );
        seg_size = *(p + 11);
        Wdputslc( "\nSegment Size " );
        Puthex( seg_size, 2 );
        Wdputslc( "\n" );
        p += 12;
        Wdputslc( "    Address       Length\n" );
        while( p < unit_end ) {
            p = GetInt( p, &addr, addr_size );
            p = GetInt( p, &seg, seg_size );
            p = GetInt( p, &len, addr_size );
            if( addr == 0 && seg == 0 && len == 0 )
                break;
            Wdputs( "    " );
            if( seg_size != 0 ) {
                Puthex( seg, seg_size * 2 );
                Wdputs( ":" );
                Puthex( addr, addr_size * 2 );
            } else {
                Puthex( addr, addr_size * 2 );
                Wdputs( "     " );
            }
            Wdputs( " " );
            Puthex( len, addr_size * 2 );
            Wdputslc( "\n" );
        }
        p = unit_end;
    }
}


static void dump_pubnames( const uint_8 *p, uint length )
/*******************************************************/
{
    const uint_8    *end;
    const uint_8    *unit_end;
    uint_32         unit_length;
    uint_32         dbg_offset;
    uint_32         dbg_length;
    uint_32         offset;

    end = &p[length];
    while( p  < end ) {
        unit_length = get_u32( (uint_32 *)p );
        unit_end = &p[unit_length + sizeof( uint_32 )];
        Wdputs( "Length: " );
        Puthex( unit_length, 8 );
        Wdputslc( "\nVersion: " );
        Puthex( get_u16( (uint_16*)(p + 4) ), 2 );
        Wdputslc( "\nDbg Info: " );
        dbg_offset =  get_u32( (uint_32 *)(p + 6) );
        Puthex( dbg_offset, 8 );
        Wdputslc( "\nDbg Length: " );
        dbg_length =  get_u32( (uint_32 *)(p + 10) );
        Puthex( dbg_length, 8 );
        Wdputslc( "\n" );
        p += 14;
        Wdputslc( "    Offset       Name\n" );
        while( p < unit_end ) {
            offset = get_u32( (uint_32 *)p );
            if( offset == 0 ) {
                break;
            }
            p += sizeof( uint_32 );
            Wdputs( "    " );
            Puthex( offset, 8 );
            Wdputs( "     " );
            Wdputs( (char *)p );
            Wdputslc( "\n" );
            p += strlen( (char *)p ) + 1;
        }
        p = unit_end;
    }
}

void Dump_specific_section( uint sect, const uint_8 *data, uint len )
/*******************************************************************/
{
    sort_tables();
    switch( sect ) {
    case DW_DEBUG_ABBREV:
          dump_abbrevs( data, len );
        break;
    case DW_DEBUG_INFO:
        dump_info( data, len );
        break;
    case DW_DEBUG_ARANGES:
          dump_aranges( data, len );
        break;
    case DW_DEBUG_LINE:
          Dump_lines( data, len );
        break;
    case DW_DEBUG_REF:
          dump_ref( data, len );
        break;
    case DW_DEBUG_PUBNAMES:
          dump_pubnames( data, len );
        break;
    default:
        dump_hex( data, len );
        break;
    }
}

void Dump_all_sections( void )
/****************************/
{
    uint        sect;

    sect = 0;
    for( ;; ) {
        Wdputs( sectionNames[ sect ] );
        Wdputslc( ":\n" );
        Dump_specific_section( sect, Sections[ sect ].data, Sections[ sect ].max_offset );
        ++sect;
        if( sect == DW_DEBUG_MAX ) break;
        Wdputslc( "\n" );
    }
}

uint Lookup_section_name( const char *name )
/******************************************/
{
    uint        sect;

    for( sect = 0 ; sect < DW_DEBUG_MAX ; sect++ ) {
        if( stricmp( sectionNames[sect], name ) == 0 ) {
            return sect;
        }
    }
    return( sect );
}
