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
* Description:  DOS EXE to binary conversion utility.
*
****************************************************************************/


// this small utility just strips off the header of a dos-type exe-file and
// performs any necessary relocations. if there are embedded relocations an
// additional command-line argument "-l=seg" is mandatory, indicating the
// address the image is supposed to be loaded at.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "watcom.h"             // unsigned_16, ..., endian-macros, ...
#include "exedos.h"             // dos_exe_header, ...
#include "banner.h"             // Watcom banner
#include "clibext.h"


#define BUF_SIZE  0x1000        // buffer size for file-ops (has to be >1)

#define ERR_NONE    0x00        // error-types used by parse_cmdline,
#define ERR_USAGE   0x01        // copy_bindata
#define ERR_RESTRIC 0x02
#define ERR_ALLOC   0x03
#define ERR_READ    0x04
#define ERR_WRITE   0x05


typedef struct reloc_table {
    unsigned_16     num;        // number of relocations in reloc-table
    unsigned_16     lseg;       // load-address of exe-program (segment)
    unsigned_32     reloc[1];   // array  of relocations in reloc-table
} reloc_table;


typedef struct reloc_offset {   // a single reloc-entry as found in an exe
    unsigned_16     offset;
    unsigned_16     segment;
} reloc_offset;


typedef struct arguments {
    FILE            *ifile;
    FILE            *ofile;
    struct {
        unsigned        be_ext   : 1;       // option 'x'
        unsigned        be_quiet : 1;       // option 'q'
        unsigned        disp_h   : 1;       // option 'h'
        unsigned        disp_r   : 1;       // option 'r'
        unsigned        have_l   : 1;       // option 'l'
        unsigned_16     lseg;               // arg to 'l'
    }               opt;
    char            iname[_MAX_PATH];
    char            oname[_MAX_PATH];
} arguments;


// skip over istream's header and copy the binary-data (i.e. everything but
// the header) to ostream. while doing so, apply any relocations, relying on
// the fact that the relocation items are sorted in ascending order.

int copy_bindata( FILE *istream, FILE *ostream, unsigned_32 bin_size,
                  unsigned_32 num_skip, reloc_table *reltab )
{

    unsigned_16     addr;               // orig. addr of bin_data to patch
    unsigned_8      *buffer;            // buffer of len BUF_SIZE for file-i/o
    unsigned_8      *bptr;              // ptr into buffer to (part of) reloc
    unsigned_16     carry;              // carry from lo-part; 0x0100 or 0x0000
    unsigned_16     cur_reloc;          // idx of current reloc to deal with
    unsigned_16     num_read;           // bytes to read; in (0, BUF_SIZE]
    unsigned_32     tot_read;           // total bytes read so far

    if( fseek( istream, num_skip, SEEK_SET ) ) {
        return( ERR_READ );
    }

    if( (buffer = malloc( BUF_SIZE )) == NULL ) {
        return( ERR_ALLOC );
    }

    tot_read  = 0;
    cur_reloc = 0;
    carry     = 0;

    for( ; bin_size > 0; bin_size -= num_read ) {
        num_read  = (bin_size > BUF_SIZE) ? BUF_SIZE : bin_size;

        if( !fread( buffer, num_read, 1, istream ) ) {
            free( buffer );
            return( ERR_READ );
        }

        // the application of relocations is a bit tricky, cause we read the
        // binary data in chunks of BUF_SIZE bytes; i.e. it is possible that
        // the reloc-addend (the load segment) has to be applied in two parts,
        // first the "lo"-part at the last byte of one buffer (actually a
        // window into istream), then the "hi"-part at the beginning of the
        // next window. In this case the x86 little-endian architecture comes
        // in very handy: We do the addition of the lower 8 bits, keep the
        // possible carry, write the buffer, fill the buffer with the next
        // data and add the upper 8 bits, taking care of the carry.

        if( cur_reloc < reltab->num ) {             // still relocs to apply?

            // is there a "hi"-part of a reloc to apply?
            if( reltab->reloc[cur_reloc] == tot_read - 1 ) {
                bptr  = buffer;
                addr  = *bptr << 8;
                addr += (reltab->lseg & 0xFF00) +  carry;
                *bptr = (unsigned_8)((addr & 0xFF00) >> 8);

                cur_reloc++;
            }

            // apply all relocs fitting in the buffer as a whole
            for( ; cur_reloc < reltab->num && reltab->reloc[cur_reloc] < tot_read + BUF_SIZE - 1; ++cur_reloc ) {
                bptr  = buffer + reltab->reloc[cur_reloc] - tot_read;
                addr  = GET_LE_16( *((unsigned_16 *)bptr) );
                addr += reltab->lseg;
                *((unsigned_16 *)bptr) = GET_LE_16( addr );
            }

            // is there a "lo"-part of a reloc to apply?
            if( cur_reloc < reltab->num
                && reltab->reloc[cur_reloc] == tot_read + BUF_SIZE - 1 ) {
                bptr  = buffer + BUF_SIZE - 1;
                addr  = *bptr;
                addr += (reltab->lseg & 0x00FF);
                *bptr = (unsigned_8)(addr & 0x00FF);

                carry = (addr & 0xFF00) ? 0x0100 : 0x0000;
            }
        }

        if( !fwrite( buffer, num_read, 1, ostream ) ) {
            free( buffer );
            return( ERR_WRITE );
        }

        tot_read += num_read;
    }

    free( buffer );

    return( ERR_NONE );
}


// display the passed exe-header to stdout

void disp_header( dos_exe_header *header )
{
    printf( "Signature                      %04X\n",   header->signature    );
    printf( "Size mod 512       (bytes)     %04X\n",   header->mod_size     );
    printf( "Number of pages                %04X\n",   header->file_size    );
    printf( "Number of relocations          %04X\n",   header->num_relocs   );
    printf( "Size of header     (paras)     %04X\n",   header->hdr_size     );
    printf( "Minimum allocation (paras)     %04X\n",   header->min_16       );
    printf( "Maximum allocation (paras)     %04X\n",   header->max_16       );
    printf( "Initial ss:sp             %04X:%04X\n",   header->SS_offset,
                                                       header->SP );
    printf( "Checksum                       %04X\n",   header->chk_sum      );
    printf( "Initial cs:ip             %04X:%04X\n",   header->CS_offset,
                                                       header->IP );
    printf( "Relocation-table at            %04X\n",   header->reloc_offset );
    printf( "Overlay number                 %04X\n\n", header->overlay_num  );
}


// display the passed relocation-table to stdout

void disp_reltab( reloc_table *reltab )
{
    int i;

    printf( "There are " );
    reltab->num ? printf( "%u", reltab->num ) : printf( "no" );
    printf( " relocations in the exe-file." );

    for( i = 0; i < reltab->num; i++ ) {
        if( i % 6 == 0 ) {
            printf( "\n" );
        }
        printf( "0x%08X  ", reltab->reloc[i] );
    }
    printf( "\n" );
}


// allocate an exe-header for stream and convert the header-entries to match
// the local endianess. the caller owns the returned memory.

dos_exe_header *get_header( FILE *stream )
{
    unsigned_16     *hdr_val;
    dos_exe_header  *header;
    int             i;

    header = malloc( sizeof( dos_exe_header ) );
    if( header ) {
        if( !fread( header, sizeof( dos_exe_header ), 1, stream ) ) {
            free( header );
            header = NULL;
        } else {
            // this is really crude, cause we assume the exe-header consists
            // solely of packed, unsigned_16's; otoh it does, and this will
            // not change in the future ...
            hdr_val = (unsigned_16 *)header;
            for( i = 0; i < ( sizeof( dos_exe_header ) >> 1 ); i++ ) {
                CONV_LE_16( *hdr_val );
                hdr_val++;
            }
        }
    }

    return( header );
}


// allocate a relocation-table for the exe-file stream making use of the
// passed header. populate the table using the local endianess. the caller
// owns the returned memory.

reloc_table *get_reltab( FILE *stream, dos_exe_header *header )
{
    unsigned_16     rel_num;
    reloc_offset    rel_off;
    reloc_table     *reltab;
    int             i;

    rel_num = header->num_relocs;
    reltab  = malloc( offsetof( reloc_table, reloc ) + rel_num * sizeof( unsigned_32 ) );
    if( reltab ) {
        reltab->num = rel_num;
        if( fseek( stream, header->reloc_offset, SEEK_SET ) ) {
            free( reltab );
            reltab = NULL;
        } else {
            for( i = 0; i < rel_num; i++ ) {
                if( !fread( &rel_off, sizeof( reloc_offset ), 1, stream ) ) {
                    free( reltab );
                    reltab = NULL;
                    break;
                } else {
                    reltab->reloc[i] = (GET_LE_16( rel_off.segment ) << 4)
                                      + GET_LE_16( rel_off.offset  );
                }
            }
        }
    }

    return( reltab );
}


// sort the relocation items for display as well as application.

int compare_u32( const void *arg1, const void *arg2 )
{
    unsigned_32 val1 = *((unsigned_32 *)arg1);
    unsigned_32 val2 = *((unsigned_32 *)arg2);

    return( val1 < val2 ? -1 : (val1 > val2) ? 1 : 0 );
}

void sort_reltab( reloc_table *reltab )
{
    qsort( reltab->reloc, reltab->num, sizeof( unsigned_32 ), compare_u32 );
    return;
}


// parse the command-line and initialize/populate the argument-structure arg.

int parse_cmdline( arguments *arg, int argc, char *argv[] )
{
    char    tmp_path[ _MAX_PATH2 ];
    char    *drive;
    char    *dir;
    char    *fname;
    char    *ext;
    int     i;

    arg->opt.be_ext   = 0;
    arg->opt.be_quiet = 0;
    arg->opt.disp_h   = 0;
    arg->opt.disp_r   = 0;
    arg->opt.have_l   = 0;

    // process the passed options
    for( i = 1; (i < argc) && ((*argv[i] == '-') || (*argv[i] == '/')); ++i ) {
        switch( argv[i][1] ) {
        case 'q':
            arg->opt.be_quiet = 1;
            break;
        case 'h':
            arg->opt.disp_h   = 1;
            break;
        case 'r':
            arg->opt.disp_r   = 1;
            break;
        case 'l':
            if( argv[i][2] != '=' ) {
                return( ERR_USAGE );
            }
            arg->opt.lseg     = (unsigned_16)strtol( argv[i] + 3, NULL, 0 );
            arg->opt.have_l   = 1;
            break;
        case 'x':
            arg->opt.be_ext   = 1;
            break;
        default :
            return( ERR_USAGE );
        }
    }

    // process file-name(s)
    if( i < argc ) {
        _splitpath2( argv[i], tmp_path, &drive, &dir, &fname, &ext );
        if( *ext == '\0' ) {
            strcpy( ext, "exe" );
        }
        _makepath( arg->iname, drive, dir, fname, ext );
        i++;
        if( i < argc ) {
            strncpy( arg->oname, argv[i], _MAX_PATH );
        }
        else {
            strcpy( ext, "bin" );
            _makepath( arg->oname, drive, dir, fname, ext );
        }
    }
    else {
        return( ERR_USAGE );
    }

    return( ERR_NONE );
}



int main( int argc, char *argv[] )
{
    arguments       arg;
    dos_exe_header  *header;
    reloc_table     *reltab;
    unsigned_32     bin_size;
    unsigned_32     tot_skip;
    int             result;

    result = parse_cmdline( &arg, argc, argv );
    if( !arg.opt.be_quiet ) {
        puts( banner1w( "EXE to Binary Converter", _EXE2BIN_VERSION_ ) );
        puts( banner2 );
        puts( banner2a( "2001" ) );
        puts( banner3 );
        puts( banner3a );
    }
    if( result ) {
        puts( "Usage:   exe2bin [options] exe_file[.exe] [bin_file]\n"
               "Options: -q        suppress informational messages\n"
               "         -h        display exe-header\n"
               "         -r        display relocations\n"
               "         -l=<seg>  relocate exe_file to segment <seg>\n"
               "         -x        extended behaviour, e.g. files > 64KB" );
        return( EXIT_FAILURE );
    }

    if( (arg.ifile = fopen( arg.iname, "rb" )) == NULL ) {
        printf( "Error opening %s for reading.\n", arg.iname );
        return( EXIT_FAILURE );
    }

    if( (header = get_header( arg.ifile )) == NULL || (header->signature != DOS_SIGNATURE) ) {
        printf( "Error. %s has no valid exe-header.\n", arg.iname );
        return( EXIT_FAILURE );
    }

    if( (reltab = get_reltab( arg.ifile, header )) == NULL ) {
        printf( "Error allocating/reading reloc-table.\n" );
        free( header );
        return( EXIT_FAILURE );
    }
    reltab->lseg = arg.opt.lseg;
    sort_reltab( reltab );

    if( arg.opt.disp_h ) {
        printf( "Header of %s (all numbers in hex)\n", arg.iname );
        disp_header( header );
    }

    if( arg.opt.disp_r ) {
        disp_reltab( reltab );
    }

    if( reltab->num && !arg.opt.have_l ) {
        printf( "Error. Option \"-l=<seg>\" mandatory (there are "
                "relocations).\n" );
        free( header );
        free( reltab );
        return( EXIT_FAILURE );
    }

    tot_skip  = header->hdr_size << 4;
    bin_size  = header->mod_size + ( (header->file_size - 1) << 9) - tot_skip;
    bin_size += header->mod_size ? 0: 512;

    // if we do not operate in extended mode check the various restrictions
    // of the original exe2bin. For com-files skip another 0x100 bytes.
    if( !arg.opt.be_ext ) {
        result = ERR_NONE;
        if( bin_size > 0x10000 ) {
            printf( "Error: Binary part exceeds 64 KBytes.\n" );
            result = ERR_RESTRIC;
        }
        if( header->SS_offset || header->SP ) {
            printf( "Error: Stack segment defined.\n" );
            result = ERR_RESTRIC;
        }
        if( header->CS_offset || (header->IP != 0x0000 && header->IP != 0x0100) ) {
            printf( "Error: CS:IP neither 0x0000:0x0000 nor 0x0000:0x0100.\n" );
            result = ERR_RESTRIC;
        }
        if( header->IP == 0x0100 && reltab->num != 0 ) {
            printf( "Error: com-file must not have relocations.\n" );
            result = ERR_RESTRIC;
        }
        if( result != ERR_NONE ) {
            free( header );
            free( reltab );
            return( EXIT_FAILURE );
        }
        if( header->IP == 0x0100 ) {
            tot_skip += 0x100;
            bin_size -= 0x100;
        }
    }

    if( (arg.ofile = fopen( arg.oname, "wb" )) == NULL ) {
        printf( "Error opening %s for writing.\n", arg.oname );
        free( header );
        free( reltab );
        return( EXIT_FAILURE );
    }

    if( (result = copy_bindata( arg.ifile, arg.ofile, bin_size, tot_skip, reltab )) != 0 ) {
        switch( result ) {
        case ERR_ALLOC:
            printf( "Error allocating file I/O buffer.\n" );
            break;
        case ERR_READ:
            printf( "Error reading while copying data.\n" );
            break;
        case ERR_WRITE:
            printf( "Error writing while copying data.\n" );
            break;
        }
        free( header );
        free( reltab );
        fclose( arg.ofile );
        remove( arg.oname );
        return( EXIT_FAILURE );
    }

    free( header );
    free( reltab );

    return( EXIT_SUCCESS );
}
