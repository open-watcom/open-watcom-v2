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


#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "dis.h"
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

extern hash_table       HandleToLabelListTable;
extern hash_table       HandleToRefListTable;
extern hash_table       SkipRefTable;
extern wd_options       Options;
extern dis_format_flags DFormat;
extern char             LabelChar;
extern char             QuoteChar;

static const char       * const masmTypes[] = {NULL,            // 0
                                "DB\t",         // 1
                                "DW\t",         // 2
                                NULL,           // 3
                                "DD\t",         // 4
                                NULL,           // 5
                                "DF\t",         // 6
                                NULL,           // 7
                                "DQ\t"          // 8
                                };

static const char       *const unixTypes[] = {NULL,             // 0
                                ".byte\t",      // 1
                                ".word\t",      // 2
                                NULL,           // 3
                                ".long\t",      // 4
                                NULL,           // 5
                                NULL,           // 6
                                NULL,           // 7
                                ".quad\t"       // 8
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

int IsMasmOutput()
{
    return( !( DFormat & DFF_X86_UNIX ) && ( GetFormat() == ORL_OMF ) );
}

static void printRawAndAddress( char * raw_data, orl_sec_offset address )
{
    switch( strlen( raw_data ) ) {
    case 0:
        BufferStore( "      00     %04X", address );
        break;
    case 1:
        BufferStore( "    00%02X     %04X", raw_data[0], address );
        break;
    case 2:
        BufferStore( "  00%02X%02X     %04X", raw_data[1], raw_data[0],
            address );
        break;
    case 3:
        BufferStore( "00%02X%02X%02X     %04X", raw_data[2], raw_data[1],
            raw_data[0], address );
        break;
    default:
        BufferStore( "%02X%02X%02X%02X     %04X", raw_data[3],
            raw_data[2], raw_data[1], raw_data[0], address );
        break;
    }
}

static bool printableChar( char c )
{
    // fixme: which characters should be printed as is and which not?
    if( isprint( c ) || c == '\n' || c == '\r' || c == '\t' || c == '\f' ) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static bool printableString( char *s )
{
    int i;
    for( i = 0; i < strlen( s ); i++ ){
        if( !printableChar( s[i] ) ) {
            return FALSE;
        }
    }
    return TRUE;
}

static orl_sec_offset printString( char * string, string_type type )
{
    char                *buffer;
    orl_sec_offset      i,j;

    buffer = MemAlloc( strlen( string ) * 2 + 1 );
    for( i=0, j=0; i < strlen( string ); i++ ){
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
    buffer[j] = 0;

    BufferStore( "\"%s\"", buffer );
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

static orl_sec_offset tryDUP(char *string, orl_sec_offset i, orl_sec_size size)
{
    orl_sec_offset      d;
    unsigned int        dup;


    if( i >= ( size - ( 8 * MIN_DUP_LINES ) ) ) return( 0 );

    for( d = i + 8; d < ( size - 8 ); d += 8 ) {
        if( memcmp( &string[i], &string[d], 8 ) ) return( 0 );
    }

    d -= i;
    dup = d / 8;
    if( dup < MIN_DUP_LINES ) return( 0 );

    BufferStore( "0%XH DUP(", dup, string[i] );

    for( dup = 0; dup < 7; dup++ ) {
        BufferHex( 2, string[i + dup] );
        BufferConcat( "," );
    }
    BufferHex( 2, string[i + 7] );
    BufferConcat( ")" );
    return( d );
}

static void printRest( char *string, orl_sec_size size )
{
    orl_sec_offset      i;
    orl_sec_offset      d;
    char                *btype;
    int                 is_masm;

    is_masm = IsMasmOutput();
    if( is_masm ) {
        btype = "    DB\t";
    } else {
        btype = "    .byte\t";
    }
    BufferConcat( btype );
    for( i = 0; i < size; ) {
        // see if we can replace large chunks of homogenous
        // segment space by using the DUP macro
        if( is_masm && !( i % 8 ) ) {
            d = tryDUP( string, i, size );
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

        BufferHex( 2, string[i] );
        if( i < size-1 ) {
            if( ( i % 8 ) == 7 ) {
                BufferConcatNL();
                BufferConcat( btype );
                BufferPrint();
            } else {
                BufferConcat(", ");
            }
        }
        i++;
    }
    BufferConcatNL();
    BufferPrint();
}

dis_value HandleAddend( ref_entry r_entry )
// sign-extend an addend value by the appropriate number of bits.
{
    dis_value   r_addend;
    int         bits;

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
    if( (bits != 32) && (r_addend & (1 << (bits-1) ) ) ) {
        return( r_addend | ( 0xFFFFFFFF ^ ( (1<<bits) - 1 ) ) );
    } else {
        return( r_addend );
    }
}

int IsDataReloc( ref_entry r_entry )
{
    switch( r_entry->type ) {
    case ORL_RELOC_TYPE_ABSOLUTE:
    case ORL_RELOC_TYPE_WORD_32:
    case ORL_RELOC_TYPE_WORD_32_NB:
    case ORL_RELOC_TYPE_SECTION:
    case ORL_RELOC_TYPE_REL_32:
    case ORL_RELOC_TYPE_PLTREL_32:
    case ORL_RELOC_TYPE_PLT_32:
    case ORL_RELOC_TYPE_WORD_64:
        return( 1 );
    default:
        return( 0 );
    }
}

orl_sec_offset RelocSize( ref_entry r_entry )
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
    case ORL_RELOC_TYPE_REL_24:
    case ORL_RELOC_TYPE_REL_32:
    case ORL_RELOC_TYPE_PLTREL_24:
    case ORL_RELOC_TYPE_PLTREL_32:
    case ORL_RELOC_TYPE_PLT_32:
    case ORL_RELOC_TYPE_WORD_16_SEG:
    case ORL_RELOC_TYPE_REL_16_SEG:
        return 4;
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
        return 2;
    case ORL_RELOC_TYPE_WORD_64:
        return 8;
    case ORL_RELOC_TYPE_WORD_32_SEG:
    case ORL_RELOC_TYPE_REL_32_SEG:
        return 6;
    case ORL_RELOC_TYPE_WORD_8:
    case ORL_RELOC_TYPE_REL_8:
    case ORL_RELOC_TYPE_WORD_HI_8:
    case ORL_RELOC_TYPE_REL_HI_8:
        return 1;
    default:
        // This should never happen, but 4 is the most likely size.
        return 4;
    }
}

orl_sec_offset HandleRefInData( ref_entry r_entry, void *data, bool asmLabels )
{
    orl_sec_offset      rv;
    const char          * const *types;
    char                buff[MAX_SYM_LEN];      // fixme: should be TS_MAX_OBJNAME or something

    if( IsMasmOutput() ) {
        types = masmTypes;
    } else {
        types = unixTypes;
    }

    rv = RelocSize( r_entry );
    switch( rv ) {
    case 6:
        if( asmLabels && types[rv] ) {
            BufferConcat( types[rv] );
        }
        HandleAReference(*((long *)data), 0, RFLAG_DEFAULT, r_entry->offset,
                         r_entry->offset + rv, &r_entry, buff );
        BufferConcat( buff );
        break;
    case 4:
        if( asmLabels ) {
            BufferConcat( types[rv] );
        }
        HandleAReference(*((long *)data), 0, RFLAG_DEFAULT, r_entry->offset,
                         r_entry->offset + rv, &r_entry, buff );
        BufferConcat( buff );
        break;
    case 2:
        if( asmLabels ) {
            BufferConcat( types[rv] );
        }
        HandleAReference(*((short*)data), 0, RFLAG_DEFAULT, r_entry->offset,
                         r_entry->offset + rv, &r_entry, buff );
        BufferConcat( buff );
        break;
    case 1:
        if( asmLabels ) {
            BufferConcat( types[rv] );
        }
        HandleAReference(*((char *)data), 0, RFLAG_DEFAULT, r_entry->offset,
                         r_entry->offset + rv, &r_entry, buff );
        BufferConcat( buff );
        break;
    case 8:
        if( asmLabels ) {
            BufferConcat( types[rv] );
        }
        HandleAReference( 0, 0, RFLAG_DEFAULT, r_entry->offset,
                          r_entry->offset + rv, &r_entry, buff );
        BufferConcat( buff );
        if( *((long *)data)!=0 || *((long *)data+4)!=0 ) {
            BufferConcat("+");
            if( *((long *)data+4)!=0 ) {
                BufferStore("0x%x", *((long *)data+4) );
                BufferStore("%08x", *((long *)data) );
            } else {
                BufferStore("0x%x", *((long *)data) );
            }
        }
        break;
    }
    return( rv );
}

static void printOut( char *string, orl_sec_offset offset, orl_sec_size size)
{
    char *              string_left = string;
    orl_sec_offset      curr_pos = 0;
    int                 ascii;

    ascii = !IsMasmOutput();

    while( 1 ) {
        if( string_left < ( string + size ) ) {
            if( ascii && printableString( string_left ) &&
                                                strlen( string_left ) > 0 ) {
                if( strlen( string_left ) < ( size - curr_pos ) ) {
                    BufferConcat( "    .asciiz\t" );
                    curr_pos = printString( string_left, ASCIZ );
                } else {
                    BufferConcat( "    .ascii\t" );
                    curr_pos = printString( string_left, ASCII );
                }
                string_left += curr_pos;
            } else {
                printRest( string_left, size - (int)( string_left - string ) );
                break;
            }
        } else {
            break;
        }
        BufferConcatNL();
        BufferPrint();
    }
}

static label_entry dumpAsmLabel( label_entry l_entry, section_ptr sec,
                                 orl_sec_offset curr_pos, orl_sec_offset end,
                                 char *contents, char *buffer )
{
    int         raw;
    int         is_masm;

    raw = buffer && contents;

    is_masm = IsMasmOutput();

    while( l_entry && l_entry->offset == curr_pos ) {
        switch( l_entry->type ) {
        case( LTYP_SECTION ):
            if( is_masm ) break;
            /* fall through */
        case( LTYP_NAMED ):
            if( sec && !strncmp( l_entry->label.name, sec->name, 8 ) ) break;
            /* fall through */
        case( LTYP_UNNAMED ):

            if( raw ) {
                strncpy( buffer, (contents + curr_pos), sizeof( unsigned_32 ) );
            }
            if( l_entry->type == LTYP_UNNAMED ) {
                if( !(DFormat & DFF_ASM) ) {
                    BufferStore( "\t     %04X\t%c$%d:", LabelChar, curr_pos,
                                 l_entry->label.number );
                    if( raw ) {
                        printRawAndAddress( buffer, curr_pos );
                    }
                } else {
                    BufferStore( "%c$%d:", LabelChar, l_entry->label.number );
                }
            } else {
                if( !(DFormat & DFF_ASM) ) {
                    BufferStore( "\t     %04X\t%s:", curr_pos,
                                 l_entry->label.name );
                    if( raw ) {
                        printRawAndAddress( buffer, curr_pos );
                    }
                } else {
                    BufferStore( "%s:", l_entry->label.name );
                }
            }
            BufferConcatNL();
            BufferPrint();
        }
        l_entry = l_entry->next;
    }
    return( l_entry );
}

return_val DumpASMDataFromSection( char *contents, orl_sec_offset start,
                                   orl_sec_offset end, label_entry *labent,
                                   ref_entry *refent, section_ptr sec )
{
    orl_sec_offset      curr_pos;
    orl_sec_size        curr_size;
    orl_sec_size        tmp_size;
    orl_sec_size        size;
    label_entry         l_entry;
    ref_entry           r_entry;
    char *              buffer;

    l_entry = *labent;
    r_entry = *refent;

    size = end - start;
    if( size < sizeof( unsigned_32 ) ) size = sizeof( unsigned_32 );
    buffer = MemAlloc( size + 1 );
    if( !buffer ) {
        PrintErrorMsg( OUT_OF_MEMORY, WHERE_PRINT_SECTION );
        return( OUT_OF_MEMORY );
    }

    for( curr_pos = start; curr_pos < end; curr_pos += curr_size ) {

        /* dump labels
         */
        l_entry = dumpAsmLabel( l_entry, sec, curr_pos, end, contents, buffer );

        curr_size = end - curr_pos;
        if( l_entry != NULL ) {
            tmp_size = l_entry->offset - curr_pos;
            if( tmp_size < curr_size ) {
                curr_size = tmp_size;
            }
        }

        /* Skip over pair relocs */
        while( r_entry != NULL
           && (r_entry->type == ORL_RELOC_TYPE_PAIR || r_entry->offset < curr_pos) ) {
            r_entry = r_entry->next;
        }
        if( r_entry != NULL && r_entry->offset < (curr_pos + curr_size) ) {
            if( r_entry->offset == curr_pos ) {
                BufferConcat("    ");
                curr_size = HandleRefInData(r_entry, contents + curr_pos, TRUE);
                BufferConcatNL();
                BufferPrint();
                continue;
            } else {
                tmp_size = r_entry->offset - curr_pos;
                if( tmp_size < curr_size ) {
                    curr_size = tmp_size;
                }
            }
        }
        memcpy( buffer, (contents + curr_pos), curr_size );
        buffer[ curr_size ] = 0;
        printOut( buffer, curr_pos, curr_size );
    }

    *labent = l_entry;
    *refent = r_entry;
    MemFree( buffer );

    return( OKAY );
}

return_val DumpASMSection( section_ptr sec, char * contents,
                        orl_sec_size size, unsigned pass )
{
    hash_data *         data_ptr;
    label_list          sec_label_list;
    label_entry         l_entry;
    ref_list            sec_ref_list;
    ref_entry           r_entry;
    return_val          err;

    data_ptr = HashTableQuery( HandleToLabelListTable, (hash_value) sec->shnd );
    if( data_ptr ) {
        sec_label_list = (label_list) *data_ptr;
        l_entry = sec_label_list->first;
    } else {
        sec_label_list = NULL;
        l_entry = NULL;
    }

    data_ptr = HashTableQuery( HandleToRefListTable, (hash_value) sec->shnd );
    if( data_ptr ) {
        sec_ref_list = (ref_list) *data_ptr;
        if( sec_ref_list != NULL ) {
            r_entry = sec_ref_list->first;
        }
    } else {
        r_entry = NULL;
    }

    if( pass == 1 ) {
        DoPass1Relocs( contents, r_entry, 0, size );
        return( OKAY );
    }

    if( size == 0 ) {
        if( IsMasmOutput() ) {
            PrintHeader( sec );
            dumpAsmLabel( l_entry, sec, 0, 0, NULL, NULL );
            PrintTail( sec );
        }
        return( OKAY );
    }

    PrintHeader( sec );
    err = DumpASMDataFromSection( contents, 0, size, &l_entry, &r_entry, sec );
    if( size > 0 ) {
        l_entry = dumpAsmLabel( l_entry, sec, size, size, NULL, NULL );
    }
    BufferConcatNL();
    BufferPrint();
    if( err == OKAY ) {
        PrintTail( sec );
    }
    return( err );
}

static return_val bssUnixASMSection( section_ptr sec, orl_sec_size size,
                                     label_entry l_entry )
{
    orl_sec_offset              offset = 0;
    orl_sec_offset              dsiz = 0;
    char                        *prefix;

    if( !size ) return( OKAY );

    PrintHeader( sec );
    while( l_entry ) {
        if( ( l_entry->type == LTYP_SECTION ) &&
          !strcmp( l_entry->label.name, sec->name ) ) {
            l_entry = l_entry->next;
        } else if( offset >= l_entry->offset ) {
            dsiz = 0;
            prefix = "";
            if( !l_entry->next ) {
                if( offset < size ) {
                    dsiz = size - offset;
                    prefix = "    .lcomm\t";
                }
            } else if( offset < l_entry->next->offset ) {
                dsiz = l_entry->next->offset - offset;
                prefix = "    .lcomm\t";
            }

            switch( l_entry->type ) {
            case LTYP_UNNAMED:
                BufferStore( "%s%c$%d", prefix, LabelChar,
                             l_entry->label.number );
                break;
            case LTYP_SECTION:
            case LTYP_NAMED:
                BufferStore( "%s%s", prefix, l_entry->label.name );
                break;
            default:
                break;
            }

            if( dsiz ) {
                BufferStore( ", 0x%08x", dsiz );
            } else {
                BufferConcat( ":" );
            }

            l_entry = l_entry->next;
            BufferConcatNL();
            BufferPrint();
        } else {
            offset = l_entry->offset;
        }
    }

    BufferConcatNL();
    BufferPrint();

    return( OKAY );
}

static return_val bssMasmASMSection( section_ptr sec, orl_sec_size size,
                                     label_entry l_entry )
{
    int                         offset = -1;

    PrintHeader( sec );

    if( size > 0 ) {
        while( l_entry ) {
            if( offset != l_entry->offset ) {
                BufferStore( "    ORG " );
                BufferHex( 8, l_entry->offset );
                offset = l_entry->offset;
                BufferConcatNL();
                BufferPrint();
            }

            if( l_entry->type != LTYP_SECTION ) {
                switch( l_entry->type ) {
                    case LTYP_UNNAMED:
                        BufferStore("%c$%d", LabelChar, l_entry->label.number );
                        break;
                    case LTYP_SECTION:
                    case LTYP_NAMED:
                        BufferStore("%s", l_entry->label.name );
                        break;
                }

                BufferConcat( "    LABEL\tBYTE" );
                BufferConcatNL();
                BufferPrint();
            }
            l_entry = l_entry->next;
        }

        BufferStore( "    ORG " );
        BufferHex( 8, size );
        BufferConcatNL();
        BufferPrint();
    }

    PrintTail( sec );

    BufferConcatNL();
    BufferPrint();

    return( OKAY );
}

return_val BssASMSection( section_ptr sec, orl_sec_size size, unsigned pass )
{
    hash_data *                 data_ptr;
    label_list                  sec_label_list;

    if( pass == 1 ) {
        return OKAY;
    }
    /* Obtain the Symbol Table */
    data_ptr = HashTableQuery( HandleToLabelListTable, (hash_value) sec->shnd );
    if( !data_ptr ) {
        return OKAY;
    }
    sec_label_list = (label_list) *data_ptr;

    if( IsMasmOutput() ) {
        return( bssMasmASMSection( sec, size, sec_label_list->first ) );
    } else {
        return( bssUnixASMSection( sec, size, sec_label_list->first ) );
    }
}

int SkipRef( ref_entry r_entry )
{
    if( SkipRefTable && ( r_entry->label->type == LTYP_EXTERNAL_NAMED ) ) {
        if( HashTableQuery( SkipRefTable,
                            (hash_value)(r_entry->label->label.name) ) ) {
            return( 1 );
        }
    }
    return( 0 );
}
