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
* Description:  IBM HLL and pre-CV4 CodeView debug format dump routines.
*
****************************************************************************/


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "wio.h"
#include "wdglb.h"
#include "dumpwv.h"
#include "wdfunc.h"
#include "hll.h"


static  const_string_table hll_dir_info_msg[] = {
    "2cbDirHeader - length of directory header      = ",
    "2cbDirEntry  - lentgh of each entry            = ",
    "4cDir        - number of directory entries     = ",
    NULL
};

static  const_string_table hll_dir_entry_msg[] = {
    "2subsection  - subsection index                = ",
    "2iMod        - module index (1 based)          = ",
    "4lfo         - offset from base                = ",
    "4cb          - subsection size                 = ",
    NULL
};

static  const_string_table hll_sstModules_msg[] = {
    "2  Seg          - segment index                = ",
    "4  Off          - code start within segment    = ",
    "4  cbSeg        - size of code in segment      = ",
    "2  ovlNbr       - overlay number               = ",
    "2  iLib         - index into sstLibraries      = ",
    "2  cSeg         - number of segments in module = ",
    "2  Style        - debug info style             = ",
    "2  Version      - debug info version number    = ",
    NULL
};

static  const_string_table hll_seg_msg[] = {
    "2    Seg      - segment index                  = ",
    "4    Off      - code start within segment      = ",
    "4    cbSeg    - size of code in segment        = ",
    NULL
};

static  const_string_table cv_sstPublics_msg[] = {
    "2  offset       - symbol offset within segment = ",
    "2  seg          - segment index                = ",
    "2  type         - type index (0 if no data)    = ",
    NULL
};

static  const_string_table hll_sstPublics_msg[] = {
    "4  offset       - symbol offset within segment = ",
    "2  seg          - segment index                = ",
    "2  type         - type index (0 if no data)    = ",
    NULL
};

static  const_string_table cv_sstSrcLnSeg_msg[] = {
    "2  seg          - segment index                = ",
    "2  cPair        - number of line/offset pairs  = ",
    NULL
};

static  const_string_table cv_lnoff16_msg[] = {
    "2  line         - source line number           = ",
    "2  offset       - offset within segment        = ",
    NULL
};

static  const_string_table cv_dir_entry_msg[] = {
    "2SubSectionType  - subsection type             = ",
    "2ModuleIndex     - module index (1 based)      = ",
    "4lfoStart        - offset from base            = ",
    "2Size            - subsection size             = ",
    NULL
};

static  const_string_table cv_sstModules_msg[] = {
    "2  Seg          - segment index                = ",
    "2  Off          - code start within segment    = ",
    "2  cbSeg        - size of code in segment      = ",
    "2  ovlNbr       - overlay number               = ",
    "2  iLib         - index into sstLibraries      = ",
    "1  cSeg         - number of segments in module = ",
    "1  reserved     - padding                      = ",
    NULL
};

static  const_string_table cv_seg_msg[] = {
    "2    Seg      - segment index                  = ",
    "2    Off      - code start within segment      = ",
    "2    cbSeg    - size of code in segment        = ",
    NULL
};

#if 0 /* not currently used, code is broken. */
static  const_string_table hl4_linnum_first_msg[] = {
    "2  line        - line number (must be zero)     = ",
    "1  entryType   - format of entry                = ",
    "1  reserved    - padding                        = ",
    "2  num_entries - number of following entries    = ",
    "2  segNum      - segment number for table       = ",
    "4  offset      - offset/num entries (type 0x03) = ",
    NULL
};

static  const_string_table hl4_filetab_entry_msg[] = {
    "4  firstChar  - first displayable listing char = ",
    "4  numChars   - displayable chars per line     = ",
    "4  numFiles   - number of files in table       = ",
    NULL
};

static  const_string_table hl3_linnum_first_msg[] = {
    "2  line        - line number (must be zero)     = ",
    "1  entry_type  - format of entry                = ",
    "1  reserved    - padding                        = ",
    "2  num_entries - number of following entries    = ",
    "2  seg_num     - segment number for table       = ",
    "4  table_size  - file names table size          = ",
    NULL
};

static  const_string_table hl3_filetab_entry_msg[] = {
    "4  srcStart   - start of source                = ",
    "4  numRecords - number of source records       = ",
    "4  numFiles   - number of files in table       = ",
    NULL
};

static  const_string_table hll_linnum_entry_msg[] = {
    "2    line     - line number                    = ",
    "2    sfi      - source file index              = ",
    "4    offset   - offset within segment          = ",
    NULL
};
#endif

static  int     hll_level;

/*
 * dump_cv_sstPublics - dump CV sstPublic at 'offset'
 * from 'base 'containing 'size' bytes
 */
static void dump_cv_sstPublics( unsigned_32 base, unsigned_32 offset,
                                                  unsigned_32 size )
/*******************************************************************/
{
    cv3_public_16       pub16;
    unsigned_32         read;

    Wlseek( base + offset );
    Wdputs( "==== sstPublics at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    for( read = 0; read < size; read += sizeof( pub16 ) + pub16.name_len ) {
        Wread( &pub16, sizeof( pub16 ) );
        Dump_header( &pub16, cv_sstPublics_msg, 4 );
        Wdputs( "  symbol name: \"" );
        Dump_namel( pub16.name_len );
        Wdputslc( "\"\n" );
    }
    Wdputslc( "\n" );
}


/*
 * dump_hll_sstPublics - dump HLL sstPublics at 'offset'
 * from 'base 'containing 'size' bytes
 */
static void dump_hll_sstPublics( unsigned_32 base, unsigned_32 offset,
                                                   unsigned_32 size )
/********************************************************************/
{
    hll_public_32       pub32;
    unsigned_32         read;

    Wlseek( base + offset );
    Wdputs( "==== sstPublics at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    for( read = 0; read < size; read += sizeof( pub32 ) + pub32.name_len ) {
        Wread( &pub32, sizeof( pub32 ) );
        Dump_header( &pub32, hll_sstPublics_msg, 4 );
        Wdputs( "  symbol name: \"" );
        Dump_namel( pub32.name_len );
        Wdputslc( "\"\n" );
    }
    Wdputslc( "\n" );
}


/*
 * dump_cv_sstTypes - dump CV sstTypes at 'offset'
 * from 'base 'containing 'size' bytes
 */
static void dump_cv_sstTypes( unsigned_32 base, unsigned_32 offset,
                                                unsigned_32 size )
/*****************************************************************/
{
    cv3_lf_common       typ;
    cv3_lf_all          type;
    unsigned_32         read;
    unsigned            idx;

    Wlseek( base + offset );
    Wdputs( "==== sstTypes at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    Wdputslc( " index: len  id type\n" );
    idx = 0;
    for( read = 0; read < size; ) {
        /* seek to start of next type record */
        Wlseek( base + offset + read );
        Wread( &typ, sizeof( cv3_lf_common ) );
        Wdputs( "  " );
        Puthex( idx, 4 );
        Wdputs( ": " );
        Puthex( typ.length, 4 );
        Wdputs( " " );
        Puthex( typ.type, 2 );
        read += typ.length + sizeof( unsigned_8 ) + sizeof( unsigned_16 );
        switch( typ.type ) {
        case HLF_SKIP:
            Wdputslc( " SKIP\n" );
            break;
        case HLF_NULL:
            Wdputslc( " NULL\n" );
            break;
        case HLF_BITFIELD:
            Wread( &type, sizeof( cv3_lf_bitfield ) );
            Wdputs( " BITFIELD: size=" );
            Puthex( type.bitfield.length, 2 );
            Wdputs( " basetype=" );
            Puthex( type.bitfield.type, 2 );
            Wdputs( " offset=" );
            Puthex( type.bitfield.position, 2 );
            Wdputslc( "\n" );
            break;
        default:
            Wdputs( " unknown type code " );
            Puthex( typ.type, 2 );
            Wdputslc( "!\n" );
        }
        ++idx;
    }
    Wdputslc( "\n" );
}

/*
 * dump_cv_sstSymbols - dump CV sstSymbols at 'offset'
 * from 'base 'containing 'size' bytes
 */
static void dump_cv_sstSymbols( unsigned_32 base, unsigned_32 offset,
                                                  unsigned_32 size )
/*******************************************************************/
{
    cv3_ssr_all         ssr;
    unsigned_32         read;

    Wlseek( base + offset );
    Wdputs( "==== sstSymbols at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    Wdputslc( "len/code/desc\n" );
    for( read = 0; read < size; ) {
        Wread( &ssr, sizeof( cv3_ssr_common ) );
        Wdputs( "  " );
        Puthex( ssr.common.length, 2 );
        Wdputs( "/" );
        Puthex( ssr.common.code, 2 );
        Wdputs( "/" );
        /* back up so we can read the common part again */
        Wlseek( base + offset + read );
        read += ssr.common.length + 1;
        switch( ssr.common.code ) {
        case HLL_SSR_BEGIN:
            Wread( &ssr, sizeof( cv3_ssr_begin ) );
            Wdputs( "BEGIN:    offset=" );
            Puthex( ssr.begin.offset, 4 );
            Wdputs( " length=" );
            Puthex( ssr.begin.len, 4 );
            Wdputslc( "\n" );
            break;
        case HLL_SSR_PROC:
            Wread( &ssr, sizeof( cv3_ssr_proc ) );
            Wdputs( "PROC:     ofs=" );
            Puthex( ssr.proc.offset, 4 );
            Wdputs( " type=" );
            Puthex( ssr.proc.type, 4 );
            Wdputs( " len=" );
            Puthex( ssr.proc.len, 4 );
            Wdputs( " pro=" );
            Puthex( ssr.proc.prologue_len, 4 );
            Wdputs( " epi=" );
            Puthex( ssr.proc.prologue_body_len, 4 );
            Wdputs( " flg=" );
            Puthex( ssr.proc.flags, 2 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            Dump_namel( ssr.proc.name_len );
            Wdputslc( "\"\n" );
            break;
        case HLL_SSR_END:
            Wread( &ssr, sizeof( cv3_ssr_end ) );
            Wdputslc( "ENDBLK:\n" );
            break;
        case HLL_SSR_AUTO:
            Wread( &ssr, sizeof( cv3_ssr_auto ) );
            Wdputs( "AUTO:     offset=" );
            Puthex( ssr.auto_.offset, 4 );
            Wdputs( " type=" );
            Puthex( ssr.auto_.type, 4 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            Dump_namel( ssr.auto_.name_len );
            Wdputslc( "\"\n" );
            break;
        case HLL_SSR_STATIC:
            Wread( &ssr, sizeof( cv3_ssr_static ) );
            Wdputs( "STATIC:   offset=" );
            Puthex( ssr.static_.offset, 4 );
            Wdputs( " segment=" );
            Puthex( ssr.static_.seg, 4 );
            Wdputs( " type=" );
            Puthex( ssr.static_.type, 4 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            Dump_namel( ssr.static_.name_len );
            Wdputslc( "\"\n" );
            break;
        case HLL_SSR_REG:
            Wread( &ssr, sizeof( cv3_ssr_reg ) );
            Wdputs( "REGISTER: type=" );
            Puthex( ssr.reg.type, 4 );
            Wdputs( " no=" );
            Puthex( ssr.reg.reg, 2 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            Dump_namel( ssr.reg.name_len );
            Wdputslc( "\"\n" );
            break;
        case HLL_SSR_CHANGE_SEG:
            Wread( &ssr, sizeof( cv3_ssr_change_seg ) );
            Wdputs( "CHG_SEG:  segment=" );
            Puthex( ssr.change_seg.seg, 4 );
            Wdputs( " reserved=" );
            Puthex( ssr.change_seg.reserved, 4 );
            Wdputslc( "\n" );
            break;
        case HLL_SSR_TYPEDEF:
            Wread( &ssr, sizeof( cv3_ssr_typedef ) );
            Wdputs( "TYPEDEF:  type=" );
            Puthex( ssr.typedef_.type, 4 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            Dump_namel( ssr.typedef_.name_len );
            Wdputslc( "\"\n" );
            break;
        default:
            Wdputs( " unknown symbol code " );
            Puthex( ssr.common.code, 2 );
            Wdputslc( "!\n" );
            return;
        }
    }
    Wdputslc( "\n" );
}


/*
 * dump_cv_sstLibraries - dump CV sstLibraries at 'offset'
 * from 'base' containing 'size' bytes
 */
static void dump_cv_sstLibraries( unsigned_32 base, unsigned_32 offset,
                                                    unsigned_32 size )
/*********************************************************************/
{
    unsigned        index;
    unsigned_32     read;

    Wlseek( base + offset );
    Wdputs( "==== sstLibraries at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    index = 0;
    for( read = 0; read < size; ) {
        Wdputs( "  index: " );
        Puthex( index, 4 );
        Wdputs( "H  name: \"" );
        read += Dump_name() + 1;
        Wdputslc( "\"\n" );
        ++index;
    }
    Wdputslc( "\n" );
}


/*
 * dump_cv_sstModules - dump CV sstModules at 'offset' from 'base'
 */
static void dump_cv_sstModules( unsigned_32 base, unsigned_32 offset )
/********************************************************************/
{
    cv3_module_16       mod;
    cv3_seginfo_16      seg;
    bool                first = true;

    Wlseek( base + offset );
    Wdputs( "==== sstModules at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    Wread( &mod, sizeof( mod ) );
    Dump_header( &mod, cv_sstModules_msg, 4 );
    Wdputs( "  module name: \"" );
    Dump_namel( mod.name_len );
    Wdputslc( "\"\n" );
    if( mod.cSeg ) {
        while( --mod.cSeg ) {
            if( !first ) {
                Wdputslc( "    ====\n" );
            }
            Wread( &seg, sizeof( seg ) );
            Dump_header( &seg, cv_seg_msg, 4 );
            first = false;
        }
    }
    Wdputslc( "\n" );
}


/*
 * dump_hll_sstModules - dump HLL sstModules at 'offset' from 'base'
 */
static void dump_hll_sstModules( unsigned_32 base, unsigned_32 offset )
/*********************************************************************/
{
    hll_module          mod;
    hll_seginfo         seg;
    bool                first = true;

    Wlseek( base + offset );
    Wdputs( "==== sstModules at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    Wread( &mod, sizeof( mod ) );
    Dump_header( &mod, hll_sstModules_msg, 4 );
    hll_level = mod.Version >> 8;
    Wdputs( "  module name: \"" );
    Dump_namel( mod.name_len );
    Wdputslc( "\"\n" );
    if( mod.cSeg ) {
        while( --mod.cSeg ) {
            if( !first ) {
                Wdputslc( "    ====\n" );
            }
            Wread( &seg, sizeof( seg ) );
            Dump_header( &seg, hll_seg_msg, 4 );
            first = false;
        }
    }
    Wdputslc( "\n" );
}


/*
 * dump_cv_sstSrcLnSeg - dump sstSrcLnSeg at 'offset' from 'base'
 */
static void dump_cv_sstSrcLnSeg( unsigned_32 base, unsigned_32 offset )
/*********************************************************************/
{
    cv_linnum_seg       src_ln;
    cv3_linnum_entry_16 lo_16;

    Wlseek( base + offset );
    Wdputs( "==== sstSrcLnSeg at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    Wdputs( "  source file: \"" );
    Align_name( Dump_name() + 1, 2 );
    Wdputslc( "\"\n" );
    Wread( &src_ln, sizeof( src_ln ) );
    Dump_header( &src_ln, cv_sstSrcLnSeg_msg, 4 );
    while( src_ln.cPair-- ) {
        Wread( &lo_16, sizeof( lo_16 ) );
        Dump_header( &lo_16, cv_lnoff16_msg, 4 );
    }
    Wdputslc( "\n" );
}


/*
 * dump_hll_sstHLLSrc - dump HLL sstHLLSrc at 'offset' from 'base'
 * containing 'size' bytes
 */
static void dump_hll_sstHLLSrc( unsigned_32 base, unsigned_32 offset, unsigned_32 size )
/**************************************************************************************/
{
    /* unused parameters */ (void)size;

    Wlseek( base + offset );
    Wdputs( "==== sstHLLSrc at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
#if 0/* FIXME: structure changes broke this. */
    if( hll_level >= 0x04 ) {
        hl4_linnum_first_lines first_entry;
        unsigned_32         count = 0;

        while( count < size ) {
            Wread( &first_entry, sizeof( first_entry ) );
            Dump_header( &first_entry, hl4_linnum_first_msg, 4 );
            count += sizeof( first_entry );
            if( first_entry.core.entry_type == 0x03 ) {
                hl4_filetab_entry   ftab_entry;
                unsigned_32         index;

                Wread( &ftab_entry, sizeof( ftab_entry ) );
                Dump_header( &ftab_entry, hl4_filetab_entry_msg, 4 );
                count += sizeof( ftab_entry );

                for( index = 0; index < ftab_entry.numFiles; ++index ) {
                    Wdputs( "  file index: " );
                    Puthex( index, 4 );
                    Wdputs( "H name: \"" );
                    count += Dump_name() + 1;
                    Wdputslc( "\"\n" );
                }
                Wdputslc( "\n" );
            } else if( first_entry.core.entry_type == 0x00 ) {
                hl3_linnum_entry    lnum_entry;
                unsigned_32         index;

                for( index = 0; index < first_entry.num_line_entries; ++index ) {
                    Wread( &lnum_entry, sizeof( lnum_entry ) );
                    count += sizeof( lnum_entry );
                    Dump_header( &lnum_entry, hll_linnum_entry_msg, 4 );
                }
                Wdputslc( "\n" );
            } else {
                Wdputslc( "unsupported linnum table entry format\n" );
            }
        }
    } else {
        hl2_linnum_first    first_entry;
        unsigned_32         index;

        Wread( &first_entry, sizeof( first_entry ) );
        Dump_header( &first_entry, hl3_linnum_first_msg, 4 );
        if( first_entry.entry_type == 0x00 ) {
            hl3_filetab_entry   ftab_entry;
            hl3_linnum_entry    lnum_entry;

            for( index = 0; index < first_entry.num_entries; ++index ) {
                Wread( &lnum_entry, sizeof( lnum_entry ) );
                Dump_header( &lnum_entry, hll_linnum_entry_msg, 4 );
            }

            Wread( &ftab_entry, sizeof( ftab_entry ) );
            Dump_header( &ftab_entry, hl3_filetab_entry_msg, 4 );

            for( index = 0; index < ftab_entry.numFiles; ++index ) {
                Wdputs( "  file index: " );
                Puthex( index, 4 );
                Wdputs( "H name: \"" );
                Dump_name();
                Wdputslc( "\"\n" );
            }
            Wdputslc( "\n" );
        } else {
            Wdputslc( "unsupported linnum table entry format\n" );
        }
    }
#endif
}


/*
 * dump_cv_subsection - dump any CodeView subsection
 */
static void dump_cv_subsection( unsigned_32 base, cv3_dir_entry *dir )
/**************************************************************************/
{
    hll_sst         sst_index;

    sst_index = dir->subsection;
    switch( sst_index ) {
    case hll_sstModules:
        if( Debug_options & MODULE_INFO ) {
            dump_cv_sstModules( base, dir->lfo );
        }
        break;
    case hll_sstPublics:
        if( Debug_options & GLOBAL_INFO ) {
            dump_cv_sstPublics( base, dir->lfo, dir->cb );
        }
        break;
    case hll_sstTypes:
        if( Debug_options & MODULE_INFO ) {
            dump_cv_sstTypes( base, dir->lfo, dir->cb );
        }
        break;
    case hll_sstSymbols:
        if( Debug_options & MODULE_INFO ) {
            dump_cv_sstSymbols( base, dir->lfo, dir->cb );
        }
        break;
    case hll_sstLibraries:
        dump_cv_sstLibraries( base, dir->lfo, dir->cb );
        break;
    case hll_sstSrcLnSeg:
        if( Debug_options & LINE_NUMS ) {
            dump_cv_sstSrcLnSeg( base, dir->lfo );
        }
        break;
    }
}


/*
 * dump_hll_subsection - dump any HLL subsection
 */
static void dump_hll_subsection( unsigned_32 base, hll_dir_entry *dir )
/*********************************************************************/
{
    hll_sst         sst_index;

    sst_index = dir->subsection;
    switch( sst_index ) {
    case hll_sstModules:
        if( Debug_options & MODULE_INFO ) {
            dump_hll_sstModules( base, dir->lfo );
        }
        break;
    case hll_sstPublics:
        if( Debug_options & GLOBAL_INFO ) {
            dump_hll_sstPublics( base, dir->lfo, dir->cb );
        }
        break;
    case hll_sstLibraries:
        dump_cv_sstLibraries( base, dir->lfo, dir->cb );
        break;
    case hll_sstHLLSrc:
        if( Debug_options & LINE_NUMS ) {
            dump_hll_sstHLLSrc( base, dir->lfo, dir->cb );
        }
        break;
    }
}


/*
 * dump_cv - dump CV data at offset 'base' from start of file
 */
static void dump_cv( unsigned_32 base )
/*************************************/
{
    hll_trailer                 header;
    cv3_dir_entry               sst_dir_entry;
    unsigned_16                 num_entries;
    int                         i;

    Wlseek( base );
    Wread( &header, sizeof( header ) );
    if( memcmp( header.sig, HLL_NB02, HLL_SIG_SIZE ) != 0 ) {
        return;
    }
    Wlseek( base + header.offset );
    Wread( &num_entries, sizeof( num_entries ) );
    for( i = 0; i < num_entries; ++i ) {
        Wlseek( base + header.offset + 2 + i * sizeof( sst_dir_entry ) );
        Wread( &sst_dir_entry, sizeof( sst_dir_entry ) );
        Dump_header( &sst_dir_entry, cv_dir_entry_msg, 4 );
        Wdputslc( "\n" );
        dump_cv_subsection( base, &sst_dir_entry );
    }
}


/*
 * dump_hll - dump HLL data at offset 'base' from start of file
 */
static void dump_hll( unsigned_32 base )
/**************************************/
{
    hll_trailer                 header;
    hll_dirinfo                 dir_info;
    hll_dir_entry               dir_entry;
    unsigned_32                 i;

    Wlseek( base );
    Wread( &header, sizeof( header ) );
    if( memcmp( header.sig, HLL_NB04, HLL_SIG_SIZE ) != 0 ) {
        return;
    }
    Wlseek( base + header.offset );
    Wread( &dir_info, sizeof( dir_info ) );
    Dump_header( &dir_info , hll_dir_info_msg, 4 );
    Wdputslc( "\n" );
    for( i = 0; i < dir_info.cDir; ++i ) {
        Wlseek( base + header.offset + dir_info.cbDirHeader + i * dir_info.cbDirEntry );
        Wread( &dir_entry, sizeof( dir_entry ) );
        Dump_header( &dir_entry, hll_dir_entry_msg, 4 );
        Wdputslc( "\n" );
        dump_hll_subsection( base, &dir_entry );
    }
}


/*
 * Dmp_hll_head - dump IBM HLL or MS CodeView debugging information
 */
bool Dmp_hll_head( void )
/**********************/
{
    off_t           end_off;
    off_t           dbg_off;
    hll_trailer     trailer;

    end_off = lseek( Handle, 0, SEEK_END );
    Wlseek( end_off - sizeof( trailer ) );
    Wread( &trailer, sizeof( trailer ) );
    dbg_off = end_off - trailer.offset;
    if( memcmp( trailer.sig, HLL_NB04, HLL_SIG_SIZE ) == 0 ) {
        Banner( "HLL NB04 debugging information" );
        Wdputs( "debugging information base  = " );
        Puthex( dbg_off, 8 );
        Wdputslc( "H\n" );
        Wdputs( "subsection directory offset = " );
        Puthex( trailer.offset, 8 );
        Wdputslc( "H\n\n" );
        dump_hll( dbg_off );
        return( true );
    } else if( memcmp( trailer.sig, HLL_NB02, HLL_SIG_SIZE ) == 0 ) {
        Banner( "CodeView NB02 debugging information" );
        Wdputs( "debugging information base  = " );
        Puthex( dbg_off, 8 );
        Wdputslc( "H\n" );
        Wdputs( "subsection directory offset = " );
        Puthex( trailer.offset, 8 );
        Wdputslc( "H\n\n" );
        dump_cv( dbg_off );
        return( true );
    }
    return( false );
} /* Dmp_hll_head */
