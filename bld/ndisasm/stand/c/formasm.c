/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Emit assembly language source.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dis.h"
#include "global.h"
#include "formasm.h"
#include "langenv.h"
#include "memfuncs.h"
#include "buffer.h"
#include "hashtabl.h"
#include "pass1.h"
#include "pass2.h"
#include "print.h"
#include "main.h"
#include "init.h"


#define STRING_LINE_LEN 41
#define UNPRINT_MAX_LEN 6

static const char   * const masmTypes[] = {
    NULL,       // 0
    "DB\t",     // 1
    "DW\t",     // 2
    NULL,       // 3
    "DD\t",     // 4
    NULL,       // 5
    "DF\t",     // 6
    NULL,       // 7
    "DQ\t"      // 8
};

static const char   * const unixTypes[] = {
    NULL,       // 0
    ".byte\t",  // 1
    ".word\t",  // 2
    NULL,       // 3
    ".long\t",  // 4
    NULL,       // 5
    NULL,       // 6
    NULL,       // 7
    ".quad\t"   // 8
};

// do a strncpy with max # characters = sec_size - current_position
// if # chars copied == sec_size - current_position and last character
// is not a NULL
//     <<.ascii>>
// otherwise
//     <<.asciz>>
// print it out
// update the current position
// repeat until end of section reached

typedef enum {
    ASCII,
    ASCIZ
} string_type;

bool IsMasmOutput( void )
{
    return( (DFormat & DFF_UNIX) == 0 );
}

static void printRawAndAddress( const char *raw_data, dis_sec_offset address )
{
    int     len;

    len = strlen( raw_data );
    switch( len ) {
    case 0:
        BufferConcat( "  " );
        /* fall through */
    case 1:
        BufferConcat( "  " );
        /* fall through */
    case 2:
        BufferConcat( "  " );
        /* fall through */
    case 3:
        BufferConcat( "00" );
        /* fall through */
    default:
        break;
    }
    switch( len ) {
    default:
        BufferHexU32( 2, raw_data[3] );
        /* fall through */
    case 3:
        BufferHexU32( 2, raw_data[2] );
        /* fall through */
    case 2:
        BufferHexU32( 2, raw_data[1] );
        /* fall through */
    case 1:
        BufferHexU32( 2, raw_data[0] );
        /* fall through */
    case 0:
        break;
    }
    BufferConcat( "     " );
    BufferHexU32( 4, address );
}

static bool printableChar( char c )
{
    // fixme: which characters should be printed as is and which not?
    if( isprint( c ) || c == '\n' || c == '\r' || c == '\t' || c == '\f' ) {
        return( true );
    } else {
        return( false );
    }
}

static bool printableString( const char *s )
{
    size_t  i;

    for( i = 0; i < strlen( s ); i++ ){
        if( !printableChar( s[i] ) ) {
            return( false );
        }
    }
    return( true );
}

static size_t printString( const char *string, string_type type )
{
    char        *buffer;
    size_t      i, j;

    buffer = MemAlloc( strlen( string ) * 2 + 1 );
    for( i = 0, j = 0; i < strlen( string ); i++ ) {
        switch( string[i] ){
        case '\n':
            buffer[j++] = '\\';
            buffer[j++] = 'n';
            break;
        case '\r':
            buffer[j++] = '\\';
            buffer[j++] = 'r';
            break;
        case '\t':
            buffer[j++] = '\\';
            buffer[j++] = 't';
            break;
        case '\f':
            buffer[j++] = '\\';
            buffer[j++] = 'f';
            break;
        case '\\':
            buffer[j++] = '\\';
            buffer[j++] = '\\';
            break;
        case '\"':
            buffer[j++] = QuoteChar;
            buffer[j++] = '\"';
            break;
        case '\'':
            buffer[j++] = '\\';
            buffer[j++] = '\'';
            break;
        default:
            buffer[j++] = string[i];
            break;
        }
    }
    buffer[j] = '\0';

    BufferQuoteText( buffer, '\"' );
    BufferPrint();
    MemFree( buffer );
    switch( type ) {
    case ASCII:
        return( strlen( string ) );
    case ASCIZ:
        return( strlen( string ) + 1 );
    }
    // shouldn't get here, but compiler complains.
    return( 0 );
}

static size_t tryDUP( unsigned_8 *bytes, size_t i, size_t size )
{
    size_t      d;
    size_t      dup;
    dis_value   value;


    if( i >= ( size - ( 8 * MIN_DUP_LINES ) ) )
        return( 0 );

    for( d = i + 8; d < ( size - 8 ); d += 8 ) {
        if( memcmp( &bytes[i], &bytes[d], 8 ) ) {
            return( 0 );
        }
    }

    d -= i;
    dup = d / 8;
    if( dup < MIN_DUP_LINES )
        return( 0 );

    BufferHexU32( 0, dup );
    BufferConcat( " DUP(" );
    value.u._32[I64HI32] = 0;
    for( dup = 0; dup < 7; dup++ ) {
        value.u._32[I64LO32] = bytes[i + dup];
        BufferHex( 2, value );
        BufferConcatChar( ',' );
    }
    value.u._32[I64LO32] = bytes[i + 7];
    BufferHex( 2, value );
    BufferConcatChar( ')' );
    return( d );
}

static void printRest( unsigned_8 *bytes, size_t size )
{
    size_t      i;
    size_t      d;
    const char  *btype;
    bool        is_masm;
    dis_value   value;

    is_masm = IsMasmOutput();
    if( is_masm ) {
        btype = "    DB\t";
    } else {
        btype = "    .byte\t";
    }
    BufferConcat( btype );
    value.u._32[I64HI32] = 0;
    for( i = 0; i < size; ) {
        // see if we can replace large chunks of homogenous
        // segment space by using the DUP macro
        if( is_masm && (i % 8) == 0 ) {
            d = tryDUP( bytes, i, size );
            if( d > 0 ) {
                i += d;
                if( i < size ) {
                    BufferConcatNL();
                    BufferConcat( btype );
                    BufferPrint();
                }
                continue;
            }
        }

        value.u._32[I64LO32] = bytes[i];
        BufferHex( 2, value );
        if( i < size - 1 ) {
            if( (i % 8) == 7 ) {
                BufferConcatNL();
                BufferConcat( btype );
                BufferPrint();
            } else {
                BufferConcat( ", " );
            }
        }
        i++;
    }
    BufferConcatNL();
    BufferPrint();
}

dis_sec_addend HandleAddend( ref_entry r_entry )
// sign-extend an addend value by the appropriate number of bits.
{
    dis_sec_addend  r_addend;
    int             bits;

    r_addend = r_entry->addend;
    switch( r_entry->type ) {
    case ORL_RELOC_TYPE_REL_14:
    case ORL_RELOC_TYPE_WORD_14:
    case ORL_RELOC_TYPE_TOCREL_14:
    case ORL_RELOC_TYPE_TOCVREL_14:
        bits = 14;
        break;
    case ORL_RELOC_TYPE_REL_24:
    case ORL_RELOC_TYPE_WORD_24:
    case ORL_RELOC_TYPE_PLTREL_24:
        bits = 24;
        break;
    case ORL_RELOC_TYPE_WORD_26:
        bits = 26;
        break;
    case ORL_RELOC_TYPE_REL_16:
    case ORL_RELOC_TYPE_WORD_16:
    case ORL_RELOC_TYPE_HALF_HI:
    case ORL_RELOC_TYPE_HALF_HA:
    case ORL_RELOC_TYPE_HALF_LO:
    case ORL_RELOC_TYPE_TOCREL_16:
    case ORL_RELOC_TYPE_TOCVREL_16:
    case ORL_RELOC_TYPE_GOT_16:
    case ORL_RELOC_TYPE_GOT_16_HI:
    case ORL_RELOC_TYPE_GOT_16_HA:
    case ORL_RELOC_TYPE_GOT_16_LO:
    case ORL_RELOC_TYPE_PLT_16_HI:
    case ORL_RELOC_TYPE_PLT_16_HA:
    case ORL_RELOC_TYPE_PLT_16_LO:
        bits = 16;
        break;
    case ORL_RELOC_TYPE_WORD_32:
    case ORL_RELOC_TYPE_WORD_32_NB:
    case ORL_RELOC_TYPE_JUMP:
    case ORL_RELOC_TYPE_SEC_REL:
    case ORL_RELOC_TYPE_REL_32:
    case ORL_RELOC_TYPE_REL_32_NOADJ:
    case ORL_RELOC_TYPE_REL_32_ADJ1:
    case ORL_RELOC_TYPE_REL_32_ADJ2:
    case ORL_RELOC_TYPE_REL_32_ADJ3:
    case ORL_RELOC_TYPE_REL_32_ADJ4:
    case ORL_RELOC_TYPE_REL_32_ADJ5:
    case ORL_RELOC_TYPE_PLTREL_32:
    case ORL_RELOC_TYPE_PLT_32:
    default:
        bits = 32;
        break;
    case ORL_RELOC_TYPE_REL_21_SH:
        // Will NEVER happen
        bits = 21;
        break;
    case ORL_RELOC_TYPE_WORD_64:
        // Will NEVER happen
        bits = 64;
        break;
    }
    if( (bits < 32) && (r_addend & (1 << (bits - 1) ) ) )
        r_addend |= ( 0xFFFFFFFF ^ ((1 << bits) - 1) );
    return( r_addend );
}

bool IsDataReloc( ref_entry r_entry )
{
    switch( r_entry->type ) {
    case ORL_RELOC_TYPE_ABSOLUTE:
    case ORL_RELOC_TYPE_WORD_32:
    case ORL_RELOC_TYPE_WORD_32_NB:
    case ORL_RELOC_TYPE_SECTION:
    case ORL_RELOC_TYPE_REL_32:
    case ORL_RELOC_TYPE_REL_32_NOADJ:
    case ORL_RELOC_TYPE_REL_32_ADJ1:
    case ORL_RELOC_TYPE_REL_32_ADJ2:
    case ORL_RELOC_TYPE_REL_32_ADJ3:
    case ORL_RELOC_TYPE_REL_32_ADJ4:
    case ORL_RELOC_TYPE_REL_32_ADJ5:
    case ORL_RELOC_TYPE_PLTREL_32:
    case ORL_RELOC_TYPE_PLT_32:
    case ORL_RELOC_TYPE_WORD_64:
        return( true );
    default:
        return( false );
    }
}

unsigned RelocSize( ref_entry r_entry )
{
    switch( r_entry->type ) {
    case ORL_RELOC_TYPE_ABSOLUTE:
    case ORL_RELOC_TYPE_WORD_32:
    case ORL_RELOC_TYPE_WORD_32_NB:
    case ORL_RELOC_TYPE_JUMP:
    case ORL_RELOC_TYPE_SECTION:
    case ORL_RELOC_TYPE_SEC_REL:
    case ORL_RELOC_TYPE_REL_21_SH:
    case ORL_RELOC_TYPE_WORD_24:
    case ORL_RELOC_TYPE_WORD_26:
    case ORL_RELOC_TYPE_REL_24:
    case ORL_RELOC_TYPE_REL_32:
    case ORL_RELOC_TYPE_REL_32_NOADJ:
    case ORL_RELOC_TYPE_REL_32_ADJ1:
    case ORL_RELOC_TYPE_REL_32_ADJ2:
    case ORL_RELOC_TYPE_REL_32_ADJ3:
    case ORL_RELOC_TYPE_REL_32_ADJ4:
    case ORL_RELOC_TYPE_REL_32_ADJ5:
    case ORL_RELOC_TYPE_PLTREL_24:
    case ORL_RELOC_TYPE_PLTREL_32:
    case ORL_RELOC_TYPE_PLT_32:
    case ORL_RELOC_TYPE_WORD_16_SEG:
    case ORL_RELOC_TYPE_REL_16_SEG:
        return( 4 );
    case ORL_RELOC_TYPE_WORD_16:
    case ORL_RELOC_TYPE_HALF_HI:
    case ORL_RELOC_TYPE_HALF_LO:
    case ORL_RELOC_TYPE_REL_16:
    case ORL_RELOC_TYPE_SEGMENT:
    case ORL_RELOC_TYPE_WORD_14:
    case ORL_RELOC_TYPE_REL_14:
    case ORL_RELOC_TYPE_TOCREL_16:
    case ORL_RELOC_TYPE_TOCREL_14:
    case ORL_RELOC_TYPE_TOCVREL_16:
    case ORL_RELOC_TYPE_TOCVREL_14:
    case ORL_RELOC_TYPE_GOT_16:
    case ORL_RELOC_TYPE_GOT_16_HI:
    case ORL_RELOC_TYPE_GOT_16_HA:
    case ORL_RELOC_TYPE_GOT_16_LO:
    case ORL_RELOC_TYPE_PLT_16_HI:
    case ORL_RELOC_TYPE_PLT_16_HA:
    case ORL_RELOC_TYPE_PLT_16_LO:
        return( 2 );
    case ORL_RELOC_TYPE_WORD_64:
        return( 8 );
    case ORL_RELOC_TYPE_WORD_32_SEG:
    case ORL_RELOC_TYPE_REL_32_SEG:
        return( 6 );
    case ORL_RELOC_TYPE_WORD_8:
    case ORL_RELOC_TYPE_REL_8:
    case ORL_RELOC_TYPE_WORD_HI_8:
    case ORL_RELOC_TYPE_REL_HI_8:
        return( 1 );
    default:
        // This should never happen, but 4 is the most likely size.
        return( 4 );
    }
}

unsigned HandleRefInData( ref_entry r_entry, void *data, bool asmLabels )
{
    unsigned            rv;
    const char          * const *types;
    char                buff[MAX_SYM_LEN];      // fixme: should be TS_MAX_OBJNAME or something
    dis_value           value;

    if( IsMasmOutput() ) {
        types = masmTypes;
    } else {
        types = unixTypes;
    }
    rv = RelocSize( r_entry );
    value.u._32[I64HI32] = 0;
    switch( rv ) {
    case 6:
        value.u._32[I64LO32] = *(unsigned_32 *)data;
        break;
    case 4:
        value.u._32[I64LO32] = *(unsigned_32 *)data;
        break;
    case 2:
        value.u._32[I64LO32] = *(unsigned_16 *)data;
        break;
    case 1:
        value.u._32[I64LO32] = *(unsigned_8 *)data;
        break;
    case 8:
        value.u._32[I64LO32] = 0;
        break;
    default:
        value.u._32[I64LO32] = 0;
        break;
    }
    if( asmLabels && types[rv] != NULL ) {
        BufferConcat( types[rv] );
    }
    HandleAReference( value, 0, RFLAG_DEFAULT | RFLAG_IS_IMMED, r_entry->offset, r_entry->offset + rv, &r_entry, buff );
    BufferConcat( buff );
    switch( rv ) {
    case 8:
        value.u._32[I64LO32] = *(unsigned_32 *)data;
        value.u._32[I64HI32] = *((unsigned_32 *)data + 1);
        if( value.u._32[I64LO32] != 0 || value.u._32[I64HI32] != 0 ) {
            BufferConcat( "+0x" );
            if( value.u._32[I64HI32] != 0 ) {
                BufferHexU32( 0, value.u._32[I64HI32] );
                BufferHexU32( 8, value.u._32[I64LO32] );
            } else {
                BufferHexU32( 0, value.u._32[I64LO32] );
            }
        }
        break;
    }
    return( rv );
}

static void printOut( const char *string, size_t offset, size_t size )
{
    const char          *string_left;
    size_t              item_size;
    bool                ascii;

    /* unused parameters */ (void)offset;

    ascii = !IsMasmOutput();
    item_size = 0;
    string_left = string;
    for( ; string_left < ( string + size ); ) {
        if( !ascii || !printableString( string_left ) || strlen( string_left ) == 0 ) {
            printRest( (unsigned_8 *)string_left, size - (size_t)( string_left - string ) );
            break;
        }
        if( strlen( string_left ) < ( size - item_size ) ) {
            BufferConcat( "    .asciiz\t" );
            item_size = printString( string_left, ASCIZ );
        } else {
            BufferConcat( "    .ascii\t" );
            item_size = printString( string_left, ASCII );
        }
        string_left += item_size;
        BufferConcatNL();
        BufferPrint();
    }
}

static label_entry dumpAsmLabel( label_entry l_entry, section_ptr section,
                                 dis_sec_offset curr_pos, dis_sec_offset end,
                                 unsigned_8 *contents, char *buffer )
{
    bool        raw;
    bool        is_masm;

    /* unused parameters */ (void)end;

    raw = ( buffer != NULL && contents != NULL );

    is_masm = IsMasmOutput();

    for( ; l_entry != NULL && ( l_entry->type == LTYP_ABSOLUTE || l_entry->offset <= curr_pos ); l_entry = l_entry->next ) {
        switch( l_entry->type ) {
        case LTYP_ABSOLUTE:
            // no print any absolute label here
            break;
        case LTYP_SECTION:
            if( is_masm )
                break;
            /* fall through */
        case LTYP_NAMED:
            if( strcmp( l_entry->label.name, section->name ) == 0 )
                break;
            /* fall through */
        case LTYP_UNNAMED:
            if( (DFormat & DFF_ASM) == 0 ) {
                BufferConcat( "\t     " );
                BufferHexU32( 4, curr_pos );
                BufferConcatChar( '\t' );
            }
            if( l_entry->type == LTYP_UNNAMED ) {
                BufferLabelNum( l_entry->label.number );
            } else {
                BufferQuoteName( l_entry->label.name );
            }
            if( (DFormat & DFF_ASM) && l_entry->offset != curr_pos ) {
                BufferConcat( " equ $-" );
                BufferDecimal( (int)( curr_pos - l_entry->offset ) );
            } else {
                BufferConcatChar( ':' );
            }
            if( (DFormat & DFF_ASM) == 0 && raw ) {
                strncpy( buffer, (char *)contents + curr_pos, sizeof( unsigned_32 ) );
                printRawAndAddress( buffer, curr_pos );
            }
            BufferConcatNL();
            BufferPrint();
        }
    }
    return( l_entry );
}

return_val DumpASMDataFromSection( unsigned_8 *contents, dis_sec_offset start,
                                   dis_sec_offset end, label_entry *lab_entry,
                                   ref_entry *reference_entry, section_ptr section )
{
    dis_sec_offset      curr_pos;
    size_t              curr_size;
    size_t              tmp_size;
    size_t              size;
    label_entry         l_entry;
    ref_entry           r_entry;
    char                *buffer;

    l_entry = *lab_entry;
    r_entry = *reference_entry;

    size = end - start;
    if( size < sizeof( unsigned_32 ) )
        size = sizeof( unsigned_32 );
    buffer = MemAlloc( size + 1 );
    if( buffer == NULL ) {
        PrintErrorMsg( RC_OUT_OF_MEMORY, WHERE_PRINT_SECTION );
        return( RC_OUT_OF_MEMORY );
    }

    for( curr_pos = start; curr_pos < end; curr_pos += curr_size ) {

        /* dump labels
         */
        l_entry = dumpAsmLabel( l_entry, section, curr_pos, end, contents, buffer );

        curr_size = end - curr_pos;
        if( l_entry != NULL ) {
            tmp_size = l_entry->offset - curr_pos;
            if( curr_size > tmp_size ) {
                curr_size = tmp_size;
            }
        }

        /* Skip over pair relocs */
        for( ; r_entry != NULL; r_entry = r_entry->next ) {
            if( r_entry->type != ORL_RELOC_TYPE_PAIR && r_entry->offset >= curr_pos ) {
                break;
            }
        }
        if( r_entry != NULL && r_entry->offset < (curr_pos + curr_size) ) {
            if( r_entry->offset == curr_pos ) {
                BufferConcat( "    " );
                curr_size = HandleRefInData( r_entry, contents + curr_pos, true );
                BufferConcatNL();
                BufferPrint();
                continue;
            } else {
                tmp_size = r_entry->offset - curr_pos;
                if( curr_size > tmp_size ) {
                    curr_size = tmp_size;
                }
            }
        }
        memcpy( buffer, contents + curr_pos, curr_size );
        buffer[curr_size] = 0;
        printOut( buffer, curr_pos, curr_size );
    }

    *lab_entry = l_entry;
    *reference_entry = r_entry;
    MemFree( buffer );

    return( RC_OKAY );
}

return_val DumpASMSection( section_ptr section, unsigned_8 *contents, dis_sec_size size, unsigned pass )
{
    hash_data           *h_data;
    label_list          sec_label_list;
    label_entry         l_entry;
    ref_list            sec_ref_list;
    ref_entry           r_entry;
    return_val          err;
    hash_key            h_key;

    h_key.u.sec_handle = section->shnd;
    h_data = HashTableQuery( HandleToLabelListTable, h_key );
    if( h_data != NULL ) {
        sec_label_list = h_data->u.sec_label_list;
        l_entry = sec_label_list->first;
    } else {
        sec_label_list = NULL;
        l_entry = NULL;
    }

    r_entry = NULL;
    h_data = HashTableQuery( HandleToRefListTable, h_key );
    if( h_data != NULL ) {
        sec_ref_list = h_data->u.sec_ref_list;
        if( sec_ref_list != NULL ) {
            r_entry = sec_ref_list->first;
        }
    }

    if( pass == 1 ) {
        DoPass1Relocs( contents, r_entry, 0, size );
        return( RC_OKAY );
    }

    if( size == 0 ) {
        if( IsMasmOutput() ) {
            PrintHeader( section );
            dumpAsmLabel( l_entry, section, 0, 0, NULL, NULL );
            PrintTail( section );
        }
        return( RC_OKAY );
    }

    PrintHeader( section );
    err = DumpASMDataFromSection( contents, 0, size, &l_entry, &r_entry, section );
    if( size > 0 ) {
        l_entry = dumpAsmLabel( l_entry, section, size, size, NULL, NULL );
    }
    BufferConcatNL();
    BufferPrint();
    if( err == RC_OKAY ) {
        PrintTail( section );
    }
    return( err );
}

static return_val bssUnixASMSection( section_ptr section, dis_sec_size size, label_entry l_entry )
{
    dis_sec_offset              dsiz = 0;
    char                        *prefix;
    label_entry                 prev_entry;

    if( ( size == 0 ) && ( l_entry == NULL ) )
        return( RC_OKAY );

    PrintHeader( section );
    prev_entry = NULL;
    for( ; l_entry != NULL; l_entry = l_entry->next ) {
        if( ( l_entry->type == LTYP_SECTION ) && ( strcmp( l_entry->label.name, section->name ) == 0 ) ) {
            continue;
        } else if( prev_entry == NULL ) {
            prev_entry = l_entry;
            continue;
        } else if( prev_entry->offset > l_entry->offset ) {
            continue;
        } else if( prev_entry->offset == l_entry->offset ) {
            dsiz = 0;
            prefix = "";
        } else {
            dsiz = l_entry->offset - prev_entry->offset;
            prefix = "    .lcomm\t";
        }
        switch( prev_entry->type ) {
        case LTYP_UNNAMED:
        case LTYP_SECTION:
        case LTYP_NAMED:
            BufferConcat( prefix );
            if( prev_entry->type == LTYP_UNNAMED ) {
                BufferLabelNum( prev_entry->label.number );
            } else {
                BufferQuoteName( prev_entry->label.name );
            }
            break;
        default:
            break;
        }
        if( dsiz ) {
            BufferConcat( ", 0x" );
            BufferHexU32( 8, dsiz );
        } else {
            BufferConcatChar( ':' );
        }
        BufferConcatNL();
        BufferPrint();
        prev_entry = l_entry;
    }
    if( prev_entry != NULL ) {
        if( prev_entry->offset < size ) {
            dsiz = size - prev_entry->offset;
            prefix = "    .lcomm\t";
        } else {
            dsiz = 0;
            prefix = "";
        }
        switch( prev_entry->type ) {
        case LTYP_UNNAMED:
        case LTYP_SECTION:
        case LTYP_NAMED:
            BufferConcat( prefix );
            if( prev_entry->type == LTYP_UNNAMED ) {
                BufferLabelNum( prev_entry->label.number );
            } else {
                BufferQuoteName( prev_entry->label.name );
            }
            break;
        default:
            break;
        }
        if( dsiz ) {
            BufferConcat( ", 0x" );
            BufferHexU32( 8, dsiz );
        } else {
            BufferConcatChar( ':' );
        }
        BufferConcatNL();
        BufferPrint();
    }
    BufferConcatNL();
    BufferPrint();

    return( RC_OKAY );
}

#define OFFSET_UNDEF    ((size_t)-1)

static return_val bssMasmASMSection( section_ptr section, dis_sec_size size, label_entry l_entry )
{
    size_t      offset = OFFSET_UNDEF;
    dis_value   value;

    PrintHeader( section );

    value.u._32[I64HI32] = 0;
    for( ; l_entry != NULL; l_entry = l_entry->next ) {
        if( l_entry->type != LTYP_SECTION ) {
            if( offset != l_entry->offset ) {
                BufferConcat( "    ORG " );
                value.u._32[I64LO32] = l_entry->offset;
                BufferHex( 8, value );
                offset = l_entry->offset;
                BufferConcatNL();
                BufferPrint();
            }

            switch( l_entry->type ) {
            case LTYP_UNNAMED:
                BufferLabelNum( l_entry->label.number );
                break;
            case LTYP_SECTION:
            case LTYP_NAMED:
                BufferQuoteName( l_entry->label.name );
                break;
            }

            BufferConcat( "    LABEL\tBYTE" );
            BufferConcatNL();
            BufferPrint();
        }
    }
    if( offset == OFFSET_UNDEF ) {
        if( size > 0 ) {
            BufferConcat( "    ORG 0" );
            BufferConcatNL();
            BufferPrint();
        }
        offset = 0;
    }
    if( size > offset ) {
        BufferConcat( "    ORG " );
        value.u._32[I64LO32] = size;
        BufferHex( 8, value );
        BufferConcatNL();
        BufferPrint();
    }

    PrintTail( section );

    BufferConcatNL();
    BufferPrint();

    return( RC_OKAY );
}

return_val BssASMSection( section_ptr section, dis_sec_size size, unsigned pass )
{
    hash_data           *h_data;
    label_list          sec_label_list;
    hash_key            h_key;

    if( pass == 1 ) {
        return RC_OKAY;
    }
    /* Obtain the Symbol Table */
    h_key.u.sec_handle = section->shnd;
    h_data = HashTableQuery( HandleToLabelListTable, h_key );
    if( h_data == NULL ) {
        return RC_OKAY;
    }
    sec_label_list = h_data->u.sec_label_list;

    if( IsMasmOutput() ) {
        return( bssMasmASMSection( section, size, sec_label_list->first ) );
    } else {
        return( bssUnixASMSection( section, size, sec_label_list->first ) );
    }
}

const char *SkipRef( ref_entry r_entry )
{
    hash_data   *h_data;
    hash_key    h_key;

    if( SkipRefTable != NULL && ( r_entry->label->type == LTYP_EXTERNAL_NAMED ) ) {
        h_key.u.string = r_entry->label->label.name;
        h_data = HashTableQuery( SkipRefTable, h_key );
        if( h_data != NULL ) {
            return( h_data->u.string );
        }
    }
    return( NULL );
}

ref_entry ProcessFpuEmulatorFixup( ref_entry r_entry, dis_sec_offset loop, const char **pfixup )
{
    const char  *fpu_fixup;

    if( r_entry != NULL && r_entry->offset == loop ) {
        fpu_fixup = SkipRef( r_entry );
        if( fpu_fixup != NULL ) {
            r_entry = r_entry->next;
            // there can be second fixup per instruction with 1 byte offset
            // it must be skipped too, displayed is first only
            // first one is significant, second one is segment override only
            if( r_entry != NULL && SkipRef( r_entry ) != NULL && ( r_entry->offset == loop + 1 ) ) {
                r_entry = r_entry->next;
            }
        }
    } else {
        fpu_fixup = NULL;
    }
    *pfixup = fpu_fixup;
    return( r_entry );
}
