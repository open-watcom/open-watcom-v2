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
* Description:  CodeView CV4 debug format dump routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "wio.h"
#include "wdglb.h"
#include "dumpwv.h"
#include "wdfunc.h"
#include "cv4.h"


static  const_string_table cv4_dir_hdr_msg[] = {
    "2cbDirHeader - length of directory header      = ",
    "2cbDirEntry  - lentgh of each entry            = ",
    "4cDir        - number of directory entries     = ",
    "4lfoNextDir  - offset of next directory        = ",
    "4flags       - directory and sst flags         = ",
    NULL
};

static  const_string_table cv4_dir_entry_msg[] = {
    "2  subsection  - subsection index              = ",
    "2  iMod        - module index (1 based)        = ",
    "4  lfo         - offset from base              = ",
    "4  cb          - subsection size               = ",
    NULL
};

static  const_string_table cv4_sstModule_msg[] = {
    "2    ovlNumber  - overlay number               = ",
    "2    iLib       - index into sstLibraries      = ",
    "2    cSeg       - number of code segments      = ",
    "2    Style      - debugging style              = ",
    NULL
};

static  const_string_table cv4_sstSrcModule_msg[] = {
    "2    cFile      - number of source files       = ",
    "2    cSeg       - number of code segments      = ",
    NULL
};

static  const_string_table cv4_src_file_msg[] = {
    "2      cSeg     - file's code segments         = ",
    "2      pad      - alignment padding            = ",
    NULL
};

static  const_string_table cv_sstPublic_msg[] = {
    "2  offset       - symbol offset within segment = ",
    "2  seg          - segment index                = ",
    "2  type         - type index (0 if no data)    = ",
    NULL
};

/*
static  const_string_table cv_sstPublic32_msg[] = {
    "4  offset       - symbol offset within segment = ",
    "2  seg          - segment index                = ",
    "2  type         - type index (0 if no data)    = ",
    NULL
};
*/

static  const_string_table cv4_sstSrcLnSeg_msg[] = {
    "2  seg          - segment index                = ",
    "2  cPair        - number of line/offset pairs  = ",
    NULL
};

static  const_string_table cv_lnoff16_msg[] = {
    "2  line         - source line number           = ",
    "2  offset       - offset within segment        = ",
    NULL
};


/*
 * read_name - read length-prefixed name into 'buffer'
 */
static int read_name( char *buffer )
/**********************************/
{
    unsigned_8      len;

    Wread( &len, 1 );
    Wread( buffer, len );
    buffer[len] = '\0';
    return( len );
}


/*
 * dump_name - dump length-prefixed name, align to n-byte boundary
 * return number of bytes read
 */
static int dump_name( bool align )
/********************************/
{
    char        name[256];
    unsigned    len, pad = 0;

    len = read_name( name ) + 1;
    Wdputs( name );
    if( align ) {
        pad = align - (len & (align - 1));
    }
    if( pad ) {
        lseek( Handle, pad, SEEK_CUR );
    }
    return( len + pad + 1 );
}


/*
 * dump_cv4_sstPublic - dump CV4 sstPublic at 'offset'
 * from 'base 'containing 'size' bytes
 */
static void dump_cv4_sstPublic( unsigned_32 base, unsigned_32 offset,
                                                 unsigned_32 size )
/*******************************************************************/
{
    cv_sst_public_16    pub16;
    unsigned_32         read = 0;
    unsigned_8          name_len;
    char                name[256];

    Wlseek( base + offset );
    Wdputs( "==== sstPublic at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    while( read < size ) {
        Wread( &pub16, sizeof( pub16 ) );
        name_len = (unsigned_8)pub16.name[0];
        Dump_header( &pub16, cv_sstPublic_msg );
        read += sizeof( pub16 );
        Wread( name, name_len );
        name[name_len] = '\0';
        Wdputs( "  symbol name: \"" );
        Wdputs( name );
        read += name_len;
        Wdputslc( "\"\n" );
    }
    Wdputslc( "\n" );
}


/*
 * dump_cv4_sstLibraries - dump CV4 sstLibraries at 'offset'
 * from 'base 'containing 'size' bytes
 */
static void dump_cv4_sstLibraries( unsigned_32 base, unsigned_32 offset,
                                                     unsigned_32 size )
/**********************************************************************/
{
    unsigned        index = 0;
    unsigned_32     read = 0;

    Wlseek( base + offset );
    Wdputs( "==== sstLibraries at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    while( read < size ) {
        Wdputs( "  index: " );
        Puthex( index, 4 );
        Wdputs( "H  name: \"" );
        read += dump_name( 0 );
        Wdputslc( "\"\n" );
        ++index;
    }
    Wdputslc( "\n" );
}


/*
 * dump_cv4_sstFileIndex - dump CV4 sstFileIndex at 'offset'
 * from 'base 'containing 'size' bytes
 */
static void dump_cv4_sstFileIndex( unsigned_32 base, unsigned_32 offset,
                                                     unsigned_32 size )
/**********************************************************************/
{
    unsigned_16     *file_index;
    unsigned_16     cMod;
    unsigned_16     cRef;
    unsigned_16     *ModStart;
    unsigned_16     *cRefCnt;
    unsigned_32     *NameRef;
    char            *Names;
    unsigned        mod, file;
    char            name[256];
    unsigned_8      *len;

    Wlseek( base + offset );
    Wdputs( "==== sstFileIndex at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n\n" );
    file_index = Wmalloc( size );
    Wread( file_index, size );
    cMod     = file_index[0];
    cRef     = file_index[1];
    ModStart = &file_index[2];
    cRefCnt  = &file_index[2 + cMod];
    NameRef  = (unsigned_32 *)(cRefCnt + cMod);
    Names    = (char *)(NameRef + cRef);
    Wdputslc( "  Module  File    Name\n" );
    for( mod = 0; mod < cMod; ++mod ) {
        for( file = 0; file < cRefCnt[mod]; ++file ) {
            /* CV spec says names are zero terminated, but actual NB11 format
             * data from MSVC 6 clearly uses length prefixed names!?!
             */
            Wdputs( "  " );
            Puthex( mod + 1, 4 );
            Wdputs( "H   " );
            Puthex( file, 4 );
            Wdputs( "H   \"" );
            len = (unsigned_8 *)(Names + NameRef[ModStart[mod] + file]);
            memcpy( name, (char *)len + 1, *len );
            name[*len] = '\0';
            Wdputs( name );
            Wdputslc( "\"\n" );
        }
    }
    Wdputslc( "\n" );
}


/*
 * dump_cv4_seg - dump CV4 cv_seginfo structure
 */
static void dump_cv4_seg( cv_seginfo *seg )
{
    Wdputs( "      " );
    Puthex( seg->Seg, 4 );
    Wdputs( "H         " );
    Puthex( seg->offset, 8 );
    Wdputs( "H   " );
    Puthex( seg->cbSeg, 8 );
    Wdputslc( "H\n" );
}


/*
 * dump_cv4_sstModule - dump CV4 sstModule at 'offset' from 'base'
 */
static void dump_cv4_sstModule( unsigned_32 base, unsigned_32 offset )
/********************************************************************/
{
    cv_sst_module       mod;
    cv_seginfo          seg;

    Wlseek( base + offset );
    Wdputs( "==== sstModule at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    Wread( &mod, sizeof( mod ) );
    Dump_header( &mod, cv4_sstModule_msg );
    if( mod.cSeg-- ) {
        Wdputslc( "      Segment idx   Offset      Code size\n" );
        dump_cv4_seg( mod.SegInfo );
        while( mod.cSeg-- ) {
            Wread( &seg, sizeof( seg ) );
            dump_cv4_seg( &seg );
        }
    } else {
        /* seek back in case there are no code segments in module */
        Wlseek( base + offset + sizeof( mod ) - sizeof( seg ) );
    }
    Wdputs( "    module name: " );
    dump_name( 0 );
    Wdputslc( "\n\n" );
}

/*
 * dump_cv4_src_file - dump CV4 sstSrcModule at 'offset' from 'base'
 */
static void dump_cv4_src_file( unsigned_32 base, unsigned_32 offset )
/*******************************************************************/
{
    cv_sst_src_module_file_table    file;
    unsigned_32                     seg_off;
    unsigned_16                     seg;
    unsigned_16                     cPair;
    unsigned_32                     loffset;
    unsigned_16                     linenumber;
    unsigned_32                     seg_idx;
    unsigned_32                     pair_idx;

    Wlseek( base + offset );
    Wread( &file, sizeof( file ) );

    Wdputs( "     File (at " );
    Puthex( offset, 8 );
    Wdputs( "): \"" );
    Wlseek( base + offset + file.cSeg * sizeof( unsigned_32 ) * 3 + sizeof( unsigned_32 ) );
    dump_name( 0 );
    Wdputslc( "\"\n" );

    Dump_header( &file, cv4_src_file_msg );

    /* loop over all segments in a file */
    for( seg_idx = 0; seg_idx < file.cSeg; ++seg_idx ) {
        Wlseek( base + offset + sizeof( unsigned_32 ) + sizeof( unsigned_32 ) * seg_idx );
        Wread( &seg_off, sizeof( seg_off ) );

        Wlseek( base + seg_off );
        Wread( &seg, sizeof( seg ) );
        Wread( &cPair, sizeof( cPair ) );

        Wdputs( "         offset   linenum hex/dec (segment " );
        Puthex( seg, 4 );
        Wdputs( ", " );
        Putdec( cPair );
        Wdputslc( " pairs)\n" );

        /* loop over all ofset/linnum pairs in a segment */
        for( pair_idx = 0; pair_idx < cPair; ++pair_idx ) {
            Wlseek( base + seg_off + sizeof( unsigned_32 ) + sizeof( unsigned_32 ) * pair_idx );
            Wread( &loffset, sizeof( loffset ) );
            Wlseek( base + seg_off + sizeof( unsigned_32 ) + sizeof( unsigned_32 ) * cPair + sizeof( unsigned_16 ) * pair_idx );
            Wread( &linenumber, sizeof( linenumber ) );
            Wdputs( "         " );
            Puthex( loffset, 8 );
            Wdputs( "        " );
            Puthex( linenumber, 4 );
            Wdputs( "/" );
            Putdec( linenumber );
            Wdputslc( "\n" );
        }
    }
}
/*
 * dump_cv4_sstModule - dump CV4 sstSrcModule at 'offset' from 'base'
 */
static void dump_cv4_sstSrcModule( unsigned_32 base, unsigned_32 offset )
/***********************************************************************/
{
    cv_sst_src_module_header    mod;
    unsigned_32                 file_off;
    unsigned_32                 file_idx;
    unsigned_32                 range_lo, range_hi;
    unsigned_32                 seg_idx;
    unsigned_16                 seg;

    Wlseek( base + offset );
    Wdputs( "==== sstSrcModule at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    Wread( &mod, sizeof( mod ) );
    Dump_header( &mod, cv4_sstSrcModule_msg );

    /* first dump the file information */
    file_off = mod.baseSrcFile[0];
    file_idx = 0;
    while( file_idx < mod.cFile ) {
        dump_cv4_src_file( base + offset, file_off );
        Wlseek( base + offset + sizeof( mod ) + sizeof( unsigned_32 ) * file_idx++ );
        Wread( &file_off, sizeof( file_off ) );
    }

    /* then dump the segment ranges and indices */
    offset += sizeof( mod ) + (mod.cFile - 1) * sizeof( unsigned_32 );
    seg_idx = 0;
    Wdputslc( "     Seg idx   Start     End\n" );
    do {
        Wlseek( base + offset + seg_idx * sizeof( unsigned_32 ) * 2);
        Wread( &range_lo, sizeof( unsigned_32 ) );
        Wread( &range_hi, sizeof( unsigned_32 ) );
        Wlseek( base + offset + mod.cSeg * sizeof( unsigned_32 ) * 2 + seg_idx * sizeof( unsigned_16 ) );
        Wread( &seg, sizeof( unsigned_16 ) );

        Wdputs( "      " );
        Puthex( seg, 4 );
        Wdputs( "     " );
        Puthex( range_lo, 8 );
        Wdputs( "  " );
        Puthex( range_hi, 8 );
        Wdputslc( "\n" );
    } while( ++seg_idx < mod.cSeg );

    Wdputslc( "\n\n" );
}

/*
 * dump_cv4_sstSegMap - dump CV4 sstSegInfo at 'offset' from
 *                      'base' for 'size' bytes.
 */
static void dump_cv4_sstSegMap( unsigned_32 base, unsigned_32 offset,
                                unsigned_32 size )
/**********************************************************************/
{
    cv_sst_seg_map *seg_map;
    unsigned_16     seg;

    Wlseek( base + offset );
    Wdputs( "==== sstSegMap at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );

    seg_map = Wmalloc( size );
    Wread( seg_map, size );

    Wdputs( "    cSeg    = " );
    Puthex( seg_map->cSeg, 4 );
    Wdputslc( "\n" );

    Wdputs( "    cSegLog = " );
    Puthex( seg_map->cSegLog, 4 );
    Wdputslc( "\n" );

    Wdputslc( "    Idx   Offset    Size      Flags OVL   Group Frame iClassName\n" );
    for( seg = 0; seg < seg_map->cSegLog; ++seg ) {
        Wdputs( "    " );
        Puthex( seg, 4 );
        Wdputs( "  " );
        Puthex( seg_map->segdesc[seg].offset, 8 );
        Wdputs( "  " );
        Puthex( seg_map->segdesc[seg].cbseg, 8 );
        Wdputs( "  " );
        Puthex( seg_map->segdesc[seg].u.flags, 4 );
        Wdputs( "  " );
        Puthex( seg_map->segdesc[seg].ovl, 4 );
        Wdputs( "  " );
        Puthex( seg_map->segdesc[seg].group, 4 );
        Wdputs( "  " );
        Puthex( seg_map->segdesc[seg].frame, 4 );
        Wdputs( "  " );
        Puthex( seg_map->segdesc[seg].iClassName, 4 );
        Wdputs( "\n" );
    }
    Wdputslc( "\n" );
}


typedef struct {
    unsigned_16     line;
    unsigned_16     offset;
} cv_srcln_off_16;


/*
 * dump_cv4_sstSrcLnSeg - dump sstSrcLnSeg at 'offset' from 'base'
 */
static void dump_cv4_sstSrcLnSeg( unsigned_32 base, unsigned_32 offset )
/**********************************************************************/
{
//    bool                first = true;
    cv_sst_src_lne_seg  src_ln;
    cv_srcln_off_16     lo_16;

    Wlseek( base + offset );
    Wdputs( "==== sstSrcLnSeg at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    Wdputs( "  source file: \"" );
    dump_name( 2 );
    Wdputslc( "\"\n" );
    Wread( &src_ln, sizeof( src_ln ) );
    Dump_header( &src_ln, cv4_sstSrcLnSeg_msg );
    while( src_ln.cPair-- ) {
        Wread( &lo_16, sizeof( lo_16 ) );
        Dump_header( &lo_16, cv_lnoff16_msg );
//        first = false;
    }
    Wdputslc( "\n" );
}


/*
 * dump_cv4_subsection - dump any CV4 subsection
 */
static void dump_cv4_subsection( unsigned_32 base, cv_directory_entry *dir )
/**************************************************************************/
{
    sst         sst_index;

    sst_index = dir->subsection;
    switch( sst_index ) {
    case sstModule:
        if( Debug_options & MODULE_INFO ) {
            dump_cv4_sstModule( base, dir->lfo );
        }
        break;
    case sstPublic:
        if( Debug_options & GLOBAL_INFO ) {
            dump_cv4_sstPublic( base, dir->lfo, dir->cb );
        }
        break;
    case sstSrcLnSeg:
        if( Debug_options & LINE_NUMS ) {
            dump_cv4_sstSrcLnSeg( base, dir->lfo );
        }
        break;
    case sstLibraries:
        if( Debug_options & MODULE_INFO ) {
            dump_cv4_sstLibraries( base, dir->lfo, dir->cb );
        }
        break;
    case sstFileIndex:
        if( Debug_options & MODULE_INFO ) {
            dump_cv4_sstFileIndex( base, dir->lfo, dir->cb );
        }
        break;
    case sstSegMap:
        if( Debug_options & MODULE_INFO ) {
            dump_cv4_sstSegMap( base, dir->lfo, dir->cb );
        }
        break;
    case sstSrcModule:
        if( Debug_options & LINE_NUMS ) {
            dump_cv4_sstSrcModule( base, dir->lfo );
        }
        break;
    }
}


/*
 * dump_cv4 - dump CV4 data at offset 'base' from start of file
 */
static void dump_cv4( unsigned_32 base )
/**************************************/
{
    cv_trailer                  header;
    cv_subsection_directory     sst_dir_hdr;
    cv_directory_entry          sst_dir_entry;
    int                         i;

    Wlseek( base );
    Wread( &header, sizeof( header ) );
    if( memcmp( header.sig, CV4_NB09, CV_SIG_SIZE ) == 0 ) {
        Wdputslc( "Signature NB09\n\n" );
    } else if( memcmp( header.sig, CV4_NB11, CV_SIG_SIZE ) == 0 ) {
        Wdputslc( "Signature NB11\n\n" );
    } else {
        return;
    }
    Wlseek( base + header.offset );
    Wread( &sst_dir_hdr, sizeof( sst_dir_hdr ) );
    Dump_header( &sst_dir_hdr , cv4_dir_hdr_msg );
    Wdputslc( "\n" );
    for( i = 0; i < sst_dir_hdr.cDir; ++i ) {
        Wlseek( base + header.offset + sst_dir_hdr.cbDirHeader + i * sst_dir_hdr.cbDirEntry );
        Wread( &sst_dir_entry, sizeof( sst_dir_entry ) );
        Dump_header( &sst_dir_entry, cv4_dir_entry_msg );
        Wdputslc( "\n" );
        dump_cv4_subsection( base, &sst_dir_entry );
    }
}


/*
 * Dmp_cv_head - dump CodeView debugging information
 */
bool Dmp_cv_head( void )
/**********************/
{
    off_t           end_off;
    off_t           dbg_off;
    cv_trailer      trailer;

    end_off = lseek( Handle, 0, SEEK_END );
    Wlseek( end_off - sizeof( trailer ) );
    Wread( &trailer, sizeof( trailer ) );
    dbg_off = end_off - trailer.offset;
    if( memcmp( trailer.sig, CV4_NB09, CV_SIG_SIZE ) == 0
     || memcmp( trailer.sig, CV4_NB11, CV_SIG_SIZE ) == 0 ) {
        Banner( "CodeView debugging information (CV4)" );
        Wdputs( "debugging information base  = " );
        Puthex( dbg_off, 8 );
        Wdputslc( "H\n" );
        Wdputs( "subsection directory offset = " );
        Puthex( trailer.offset, 8 );
        Wdputslc( "H\n" );
        dump_cv4( dbg_off );
        return( true );
    }
    return( false );
} /* Dmp_cv_head */
