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
* Description:  SYM file dump program - display MAPSYM generated symbolic
*               information in human-readable format.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "bool.h"
#include "mapsym.h"

static bool     DumpAlphaSorted = FALSE;    // dump alphabetically sorted sym tables

/* Heuristics to determine whether a file is a MAPSYM .sym file */
static bool isSymFile( FILE *f )
{
    sym_endmap          end_map;
    int                 ret;
    unsigned long       pos;

    /* seek to the end, read and check end map record */
    ret = fseek( f, -(long)sizeof( end_map ), SEEK_END );
    if( ret != 0 ) {
        return( FALSE );
    }
    pos = ftell( f );
    /* the endmap record must be 16-byte aligned */
    if( pos % 16 ) {
        return( FALSE );
    }
    if( fread( &end_map, 1, sizeof( end_map ), f ) != sizeof( end_map ) ) {
        return( FALSE );
    }
    if( end_map.zero != 0 ) {
        return( FALSE );
    }
    /* Check .sym file to make sure it's something reasonable */
    if( (end_map.major_ver < 3) || (end_map.major_ver > 5)
        || (end_map.minor_ver > 11) ) {
        return( FALSE );
    }

    /* looks like the right sort of .sym file */
    return( TRUE );
}

// Read a Pascal style string - limited to 255 chars max length
int readString( FILE *f, char *buf )
{
    unsigned_8  str_len;

    fread( &str_len, sizeof( str_len ), 1, f );
    fread( buf, str_len, 1, f );
    buf[str_len] = '\0';            // NUL terminate string
    return( str_len );
}

// Dump symbols, symbol offsets are stored in a table
static void dumpSymTable( FILE *f, int count, unsigned_32 base_ofs,
                          unsigned_32 table_ofs, bool big_syms )
{
    unsigned_16     *sym_tbl;
    size_t          tbl_size;
    sym_symdef      sym;
    sym_symdef_32   sym_32;
    char            name[256];
    int             i;

    tbl_size = count * sizeof( unsigned_16 );
    sym_tbl = malloc( tbl_size );
    if( sym_tbl == NULL ) {
        printf( "Error: memory allocation failed (%d bytes)\n", tbl_size );
        return;
    }
    fseek( f, base_ofs + table_ofs, SEEK_SET );
    fread( sym_tbl, 1, tbl_size, f );

    for( i = 0; i < count; ++i ) {
        fseek( f, base_ofs + sym_tbl[i], SEEK_SET );
        if( big_syms ) {
            fread( &sym_32, 1, SYM_SYMDEF_32_FIXSIZE, f );
            readString( f, name );
            printf( "    0x%08x : '%s'\n", sym_32.offset, name );
        } else {
            fread( &sym, 1, SYM_SYMDEF_FIXSIZE, f );
            readString( f, name );
            printf( "    0x%04x : '%s'\n", sym.offset, name );
        }
    }
    free( sym_tbl );
}

// Dump a number of segments in a chain
static void dumpLines( FILE *f, unsigned_32 base_ofs )
{
    sym_linedef     lines;
    sym_linerec     linerec;
    char            name[256];
    int             i;

    fseek( f, base_ofs, SEEK_SET );
    do {
        fread( &lines, 1, SYM_LINEDEF_FIXSIZE, f );
        readString( f, name );
        printf( "--Line number information for '%s'\n", name );
        printf( "  Line table at 0x%06x, %d record(s)\n",
                base_ofs + lines.lines_ofs, lines.lines_num );
        fseek( f, base_ofs + lines.lines_ofs, SEEK_SET );
        for( i = 1; i <= lines.lines_num; ++i ) {
            fread( &linerec, 1, sizeof( linerec ), f );
            if( linerec.line_offset == (unsigned_32)-1 ) {
                printf( "    Line %5d, offset 0x%04x\n",
                    i, linerec.code_offset );
            } else {
                printf( "    Line %5d, offset 0x%04x, file offset 0x%08x\n",
                    i, linerec.code_offset, linerec.line_offset );
            }
        }
    } while( lines.next_ptr != 0 );
}

// Dump segments in a chain
static void dumpSegments( FILE *f, int count )
{
    sym_segdef      seg;
    char            name[256];
    unsigned_32     seg_start;
    unsigned_32     sym_tab_offset;
    int             i;

    for( i = 0; i < count; ++i ) {
        seg_start = ftell( f );
        fread( &seg, 1, SYM_SEGDEF_FIXSIZE, f );
        readString( f, name );

        printf( "--Segment '%s'\n", name );
        printf( "  Next segment at 0x%06x, %d symbols, table at 0x%06x\n",
            SYM_PTR_TO_OFS( seg.next_ptr ), seg.num_syms,
            seg_start + seg.sym_tab_ofs );
        printf( "  Segment load address 0x%04x, phys addr 0x%04x, flags 0x%02x\n",
            seg.load_addr, seg.phys_0, seg.sym_type );
        printf( "  Line nums at 0x%06x, loaded flag 0x%02x, curr instance 0x%02x\n",
            SYM_PTR_TO_OFS( seg.linnum_ptr ), seg.is_loaded, seg.curr_inst );

        sym_tab_offset = seg.sym_tab_ofs;

        /* if alphabetically sorted symbol table is present, it'll be right after
         * the first table (which is sorted by address)
         */
        if( DumpAlphaSorted && (seg.sym_type & SYM_FLAG_ALPHA) ) {
            printf( "  Symbols in alphabetical order:\n" );
            sym_tab_offset += seg.num_syms * sizeof( unsigned_16 );
        }

        dumpSymTable( f, seg.num_syms, seg_start, sym_tab_offset, (seg.sym_type & SYM_FLAG_32BIT) != 0 );

        if( seg.linnum_ptr != 0 ) {
            dumpLines( f, SYM_PTR_TO_OFS( seg.linnum_ptr ) );
        }
        fseek( f, SYM_PTR_TO_OFS( seg.next_ptr ), SEEK_SET );
    };
}

int main( int argc, char *argv[] )
/********************************/
{
    FILE                *file;
    fpos_t              file_pos;
    sym_mapdef          map;
    sym_endmap          end_map;
    unsigned_32         map_start;
    char                name[256];

    if( argc < 2 ) {
        printf( "Usage:  symdump [-a] <file>\n" );
        printf( "Where <file> is MAPSYM style .sym file\n" );
        printf( "   -a  dump alphabetically sorted symbol tables (if provided)\n" );
        return( 1 );
    }

    if( !strcmp( argv[1], "-a" ) ) {
        DumpAlphaSorted = TRUE;
        ++argv;
    }
    file = fopen( argv[1], "rb" );
    if( file == NULL ) {
        printf( "Error opening file '%s'.\n", argv[1] );
        return( 2 );
    }

    if( !isSymFile( file ) ) {
        printf( "Not a supported .sym file.\n" );
        return( 2 );
    }

    map_start = 0;
    fseek( file, map_start, SEEK_SET );
    fgetpos( file, &file_pos );
    fread( &map, 1, SYM_MAPDEF_FIXSIZE, file );
    while( map.next_ptr != 0 ) {
        readString( file, name );
        printf( "Map '%s'\n", name );
        printf( "Next map at 0x%06x, entry point in segment 0x%04x, flags 0x%02x\n",
            SYM_PTR_TO_OFS( map.next_ptr ), map.entry_seg, map.abs_type );
        printf( "%d abs symbol(s), abs symbol table at 0x%06x, %d segment(s)\n",
            map.abs_sym_count, map_start + map.abs_tab_ofs, map.num_segs );
        printf( "First segment at 0x%06x, maximum symbol name length %d\n",
            SYM_PTR_TO_OFS( map.seg_ptr ), map.max_sym_len );

        if( (map.abs_sym_count > 0) && (map.abs_tab_ofs != 0)) {
            printf( "  Absolute symbols:\n" );
            dumpSymTable( file, map.abs_sym_count, map_start, map.abs_tab_ofs,
                          (map.abs_type & SYM_FLAG_32BIT) != 0 );
        }
        fseek( file, SYM_PTR_TO_OFS( map.seg_ptr ), SEEK_SET );
        dumpSegments( file, map.num_segs );

        map_start = SYM_PTR_TO_OFS( map.next_ptr );
        fseek( file, map_start, SEEK_SET );
        fgetpos( file, &file_pos );
        fread( &map, 1, SYM_MAPDEF_FIXSIZE, file );
    };

    fsetpos( file, &file_pos );
    fread( &end_map, 1, sizeof( end_map ), file );
    printf( "\nSymbol file version %d.%d\n", end_map.major_ver, end_map.minor_ver );

    if( fclose( file ) != 0 ) {
        printf( "Error closing file.\n" );
        return( 2 );
    }
    return( 0 );
}
