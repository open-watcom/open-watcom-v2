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


// prefix indicatees how to format field data
// 1, 2 or 4 formats bytes in hex
// 0dd formats asciiz string as fixed width string of dd chars

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

#if 0 // SHL FIXME to work somewhen - we don't do hll NB03 yetP
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
#endif

static  const_string_table hll_linnum_entry_msg[] = {
    "2    line     - line number                    = ",
    "2    sfi      - source file index              = ",
    "4    offset   - offset within segment          = ",
    NULL
};

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
    Wdputs( "==== sstPublics (" );
    Puthex( hll_sstPublics, 3 );
    Wdputs( "H) at offset " );
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

#if 0 // FIXME to be gone
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
#endif // FIXME to be gone

/*
 * put_name - output string with single byte length prefix
 * @param p is pointer to name length prefix
 * @returns pointer to byte following name string
 * FIXME to be in wdio maybe
 */

static unsigned_8 *put_name( unsigned_8 *p)
{
    unsigned_8 len;
    len = *p;
    p++;
    fprintf( stdout, "%.*s", len, p );
    return p + len;
}

/*
 * put_name2 - output string with encoded length prefix
 * @param p is pointer to name length prefix
 * @returns pointer to byte following name string
 * FIXME to be in wdio maybe
 */

static unsigned_8 *put_name2( unsigned_8 *p)
{
    unsigned_16 len;
    len = *p++;
    if( len & 0x80 ) {
        len = ((len & 0x7f) << 8) | *p;
        p++;
    }
    else
      len &= 0x7f;
    fprintf( stdout, "%.*s", len, p );
    return p + len;
}

/*
 * dump_hll_sstSymbols - dump HLL sstSymbols at 'offset'
 * from 'base 'containing 'size' bytes
 */
static void dump_hll_sstSymbols( unsigned_32 base, unsigned_32 offset,
                                                  unsigned_32 size )
/*******************************************************************/
{
    // SHL FIXME to be finished
    union ssr_data {
        char            buf[300];
        hll_ssr_all     ssr;
    } u;
    unsigned_32         read = 0;
    unsigned_16         rec_len;
    unsigned_8 *        puch;

    // SHL FIXME hack cough - make work for all platforms - from bsedos.h
    typedef unsigned char UCHAR;
    typedef unsigned short USHORT;
    typedef short SHORT;
    typedef struct _DATETIME {
        UCHAR   hours;
        UCHAR   minutes;
        UCHAR   seconds;
        UCHAR   hundredths;
        UCHAR   day;
        UCHAR   month;
        USHORT  year;
        SHORT   timezone;
        UCHAR   weekday;
    } DATETIME;
    DATETIME *pdt;

    Wlseek( base + offset );
    Wdputs( "==== sstSymbols (" );
    Puthex( hll_sstSymbols, 3 );
    Wdputs( "H) at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );
    Wdputslc( "len/code/desc\n" );
    while( read < size ) {
        Wread( &rec_len, sizeof( unsigned_8 ) );

        // Read encoded length and check overflows
        if( rec_len & 0x80 ) {
          rec_len &= 0x7f;
          rec_len <<= 8;
          Wread( &rec_len, sizeof( unsigned_8 ) );
          read += 2;
        }
        else {
          rec_len &= 0x7f;
          read++;
        }

        Wdputs( "  " );
        Puthex( rec_len, 2 );
        Wdputs( "/" );

        if( rec_len > sizeof(u.buf) ) {
            Wdputs( " sstSymbols sub-record length exceeds 300 byte limit" );
            Wdputslc( "!\n" );
            break;
        }

        /* read sub-record data */
        Wread( &u.ssr, rec_len);
        read += rec_len;

        Puthex( u.ssr.common.code, 2 );
        Wdputs( "/" );

        switch( u.ssr.common.code ) {
        case HLL_SSR_BEGIN:
            Wdputs( "BEGIN:    offset=" );
            Puthex( u.ssr.begin.offset, 8 );
            Wdputs( " length=" );
            Puthex( u.ssr.begin.len, 4 );
            Wdputslc( "\n" );
            break;
        case HLL_SSR_PROC:
            u.ssr.proc.name[u.ssr.proc.name_len] = '\0';
            Wdputs( "PROC:     offset=" );
            Puthex( u.ssr.proc.offset, 8 );
            Wdputs( " type=" );
            Puthex( u.ssr.proc.type, 4 );
            Wdputs( " len=" );
            Puthex( u.ssr.proc.len, 8 );
            Wdputs( " pro=" );
            Puthex( u.ssr.proc.prologue_len, 4 );
            Wdputs( " epi=" );
            Puthex( u.ssr.proc.prologue_body_len, 8 );
            Wdputs( " flg=" );
            Puthex( u.ssr.proc.flags, 2 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.proc.name_len);
            Wdputslc( "\"\n" );
            break;
        case HLL_SSR_END:
            Wdputslc( "ENDBLK:\n" );
            break;
        case HLL_SSR_AUTO:
            Wdputs( "AUTO:     offset=" );
            Puthex( u.ssr.auto_.offset, 8 );
            Wdputs( " type=" );
            Puthex( u.ssr.auto_.type, 4 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.auto_.name_len );
            Wdputslc( "\"\n" );
            break;
        case HLL_SSR_STATIC:
            Wdputs( "STATIC:   offset=" );
            Puthex( u.ssr.static_.offset, 8 );
            Wdputs( " segment=" );
            Puthex( u.ssr.static_.seg, 4 );
            Wdputs( " type=" );
            Puthex( u.ssr.static_.type, 4 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.static_.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_TLS:
            Wdputs( "TLS:      offset=" );
            Puthex( u.ssr.tls.offset, 8 );
            Wdputs( " res=" );
            Puthex( u.ssr.tls.reserved, 2 );
            Wdputs( " type=" );
            Puthex( u.ssr.tls.type, 2 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.tls.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_CODE_LABEL:
            Wdputs( "CODELABEL: offset=" );
            Puthex( u.ssr.code_label.offset, 8 );
            Wdputs( " flg=" );
            Puthex( u.ssr.code_label.flags, 2 );
            Wdputslc( "\n" );
            Wdputs( " name: \"" );
            put_name( &u.ssr.code_label.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_REG:
            Wdputs( "REGISTER: type=" );
            Puthex( u.ssr.reg.type, 4 );
            Wdputs( " no=" );
            Puthex( u.ssr.reg.reg, 2 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.reg.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_REG_RELATIVE:
            Wdputs( "REGISTER RELATIVE: type=" );
            Puthex( u.ssr.reg_relative.type, 4 );
            Wdputs( " no=" );
            Puthex( u.ssr.reg_relative.reg, 2 );
            Wdputs( " offset=" );
            Puthex( u.ssr.reg_relative.offset, 8 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.reg_relative.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_CONSTANT:
            Wdputs( "CONSTANT:    : val=" );
            switch( u.ssr.constant.val_len) {
            case 1:
                Puthex( *u.ssr.constant.val, 1 );
                break;
            case 2:
                Puthex( *(unsigned_16*)u.ssr.constant.val, 2 );
                break;
            case 4:
                Puthex( *(unsigned_32*)u.ssr.constant.val, 4 );
                break;
            default:
                fprintf( stdout, "FIXME to decode val_len %u", u.ssr.constant.val_len );
            } // switch
            puch = u.ssr.constant.val + u.ssr.constant.val_len;      // Point at name length
            Wdputs( " name: \"" );
            put_name( puch );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_SKIP:
            Wdputs( "SKIP:    : type=FIXME - need struct" );
            Wdputslc( "\n" );
            break;

        case HLL_SSR_CHANGE_SEG:
            Wdputs( "CHG_SEG:  segment=" );
            Puthex( u.ssr.change_seg.seg, 4 );
            Wdputs( " reserved=" );
            Puthex( u.ssr.change_seg.reserved, 4 );
            Wdputslc( "\n" );
            break;
        case HLL_SSR_TYPEDEF:
            Wread( &u.ssr, sizeof( cv3_ssr_typedef ) );
            Wdputs( "TYPEDEF:  type=" );
            Puthex( u.ssr.typedef_.type, 4 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.reg.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_PUBLIC:
            Wdputs( "PUBLIC:    : offset=" );
            Puthex( u.ssr.public_.offset, 8 );
            Wdputs( " seg=" );
            Puthex( u.ssr.public_.seg, 4 );
            Wdputs( " type=" );
            Puthex( u.ssr.public_.type, 4 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.public_.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_MEMBER:
            Wdputs( "MEMBER:    : offset=" );
            Puthex( u.ssr.member.off_sub_rec, 8 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.member.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_BASED:
            Wdputs( "BASED:    : offset=" );
            Puthex( u.ssr.based.off_sub_rec, 8 );
            Wdputs( " type=" );
            Puthex( u.ssr.based.type, 4 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.based.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_TAG:
            Wdputs( "TAG:" );
            Wdputs( "      name: \"" );
            put_name( &u.ssr.tag.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_TABLE:
            Wdputs( "TABLE : type=FIXME" );
            Wdputslc( "\n" );
            break;

        case HLL_SSR_MAP:
            Wdputs( "MAP   : type=FIXME" );
            Wdputslc( "\n" );
            break;

        case HLL_SSR_TAG2:
            Wdputs( "TAG2:" );
            Wdputs( "     name: \"" );
            put_name2( &u.ssr.tag.name_len );
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_MEM_FUNC:
            Wdputs( "MEM_FUNC: offset=" );
            Puthex( u.ssr.mem_func.offset, 8 );
            Wdputs( " type=" );
            Puthex( u.ssr.mem_func.type, 4 );
            Wdputs( " len=" );
            Puthex( u.ssr.mem_func.len, 8 );
            Wdputs( " pro=" );
            Puthex( u.ssr.mem_func.prologue_len, 4 );
            Wdputs( " epi=" );
            Puthex( u.ssr.mem_func.prologue_body_len, 8 );
            Wdputs( " flg=" );
            Puthex( u.ssr.mem_func.flags, 2 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name2( &u.ssr.mem_func.name_len );      // name length is encoded
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_AUTO_SCOPED:
            Wdputs( "AUTO_SCOPED: type=FIXME" );
            Wdputslc( "\n" );
            break;

        case HLL_SSR_PROC2:
            // SHL FIXME to be done
            Wdputs( "PROC2:    offset=" );
            Puthex( u.ssr.proc2.offset, 8 );
            Wdputs( " type=" );
            Puthex( u.ssr.proc2.type, 4 );
            Wdputs( " len=" );
            Puthex( u.ssr.proc2.len, 8 );
            Wdputslc( "\n" );
            Wdputs( "      pro=" );
            Puthex( u.ssr.proc2.prologue_len, 8 );
            Wdputs( " epi=" );
            Puthex( u.ssr.proc2.prologue_body_len, 8 );
            Wdputs( " flg=" );
            Puthex( u.ssr.proc2.flags, 2 );
            Wdputslc( "\n" );
            Wdputs( "      name: \"" );
            put_name2( &u.ssr.proc2.name_len );       // name length is encoded
            Wdputslc( "\"\n" );
            break;

        case HLL_SSR_STATIC2:
            Wdputs( "STATIC2 : type=FIXME" );
            Wdputslc( "\n" );
            break;

        case HLL_SSR_BASED_MEMBER:
            Wdputs( "BASED_MEMBER: type=FIXME" );
            Wdputslc( "\n" );
            break;

        case HLL_SSR_CU_INFO:
            // SHL FIXME to be done
            Wdputs( "CUINFO:   language=" );
            Puthex( u.ssr.cu_info.language, 2 );
            Wdputs( " options =\"" );
            puch = put_name( &u.ssr.cu_info.options_len);
            Wdputslc( "\"\n" );
            // puch points at compiler date string length field
            Wdputs( "      build date=\"" );
            pdt = (DATETIME*)put_name( puch );
            // pdt points at timestamp
            Wdputs( "\" timestamp=" );
            // DATETIME        timestamp; /* DosGetDateTime() */
            fprintf(stdout, "%u/%u/%u %u:%02u:%02u",
                    pdt->month > 0 ? pdt->month - 1: pdt->month,
                    pdt->day,
                    pdt->year,
                    pdt->hours,
                    pdt->minutes,
                    pdt->seconds);
            Wdputslc( "\n" );
            break;

        case HLL_SSR_CU_FUNC_NUM:
            Wdputs( "FUNC_NUM: type=" );
            Wdputslc( "FIXME\n" );
            break;

        default:
            Wdputs( " unknown symbol code " );
            Puthex( u.ssr.common.code, 2 );
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
    Wdputs( "==== sstModules (" );
    Puthex( hll_sstModules, 3 );
    Wdputs( "H) at offset " );
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

    hl4_linnum_first_lines first_entry;
    unsigned_32         count = 0;

    Wlseek( base + offset );
    Wdputs( "==== sstHLLSrc (" );
    Puthex( hll_sstHLLSrc, 3 );
    Wdputs( "H) at offset " );
    Puthex( offset, 8 );
    Wdputslc( "\n" );

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
                count += Dump_name();
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

#if 0 /* FIXME: structure changes broke this - probably for not NB04. */
    if( hll_level >= 0x04 ) {
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
            dump_hll_sstSymbols( base, dir->lfo, dir->cb );
        }
        break;
    case hll_sstLibraries:
        // FIXME to be dump_hll_..
        dump_cv_sstLibraries( base, dir->lfo, dir->cb );
        break;
    case hll_sstSrcLnSeg:
        // FIXME to be dump_hll_..
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
    case hll_sstSymbols:
        if( Debug_options & MODULE_INFO ) {
            dump_hll_sstSymbols( base, dir->lfo, dir->cb );
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
        unsigned_32 offset = header.offset + dir_info.cbDirHeader + i * dir_info.cbDirEntry;
        Wdputs( "==== Directory entry #" );
        Putdec( i + 1 );
        Wdputs( " at offset " );
        Puthex( offset, 8 );
        Wdputslc( "\n" );

        Wlseek( base + offset );
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
