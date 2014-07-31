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
* Description:  DWARF dump routines.
*
****************************************************************************/


#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wio.h"
#include "wdglb.h"
#include "dumpwv.h"
#include "wdfunc.h"


static  const_string_table mdh_msg[] = {
    "1EXE major                 = ",
    "1EXE minor                 = ",
    "1OBJ major                 = ",
    "1OBJ minor                 = ",
    "2Language data size        = ",
    "2Segment table size        = ",
    "4Total size of debug info  = ",
    NULL
};

typedef struct {
    char        signature[4];
    unsigned_32 vendor_id;
    unsigned_32 info_type;
    unsigned_32 info_size;
} debug_header;

static void set_sects( void )
/***************************/
{
    unsigned_32         sectsizes[DR_DEBUG_NUM_SECTS];
    unsigned_32         sections[DR_DEBUG_NUM_SECTS];
    int                 i;

    memset( sections, 0, DR_DEBUG_NUM_SECTS * sizeof(unsigned_32) );
    memset( sectsizes, 0, DR_DEBUG_NUM_SECTS * sizeof(unsigned_32) );

    Wread( &sections[DW_DEBUG_INFO], sizeof(unsigned_32) );
    Wread( &sectsizes[DW_DEBUG_INFO], sizeof(unsigned_32) );
    Wread( &sections[DW_DEBUG_REF], sizeof(unsigned_32) );
    Wread( &sectsizes[DW_DEBUG_REF], sizeof(unsigned_32) );
    Wread( &sections[DW_DEBUG_ABBREV], sizeof(unsigned_32) );
    Wread( &sectsizes[DW_DEBUG_ABBREV], sizeof(unsigned_32) );
    Wread( &sections[DW_DEBUG_LINE], sizeof(unsigned_32) );
    Wread( &sectsizes[DW_DEBUG_LINE], sizeof(unsigned_32) );
    Wread( &sections[DW_DEBUG_MACINFO], sizeof(unsigned_32) );
    Wread( &sectsizes[DW_DEBUG_MACINFO], sizeof(unsigned_32) );

    for( i = 0; i < DR_DEBUG_NUM_SECTS; i += 1 ) {
        Sections[i].cur_offset = 0;
        Sections[i].max_offset = sectsizes[i];

        if( sectsizes[i] != 0 ) {
            Wlseek( sections[i] );
            Sections[i].data = Wmalloc( sectsizes[i] );
            if( Sections[i].data == NULL ) {
                Wdputslc( "Not enough memory\n" );
                exit(1);
            }
            Wread( Sections[i].data, sectsizes[i] );
        }
    }
}


static bool os2_debug( void )
/***************************/
{
    Wlseek( 0 );
    Wread( &Dos_head, sizeof( Dos_head ) );
    if( Dos_head.signature == DOS_SIGNATURE ) {
        if( Dos_head.reloc_offset == OS2_EXE_HEADER_FOLLOWS ) {
            Wlseek( OS2_NE_OFFSET );
            Wread( &New_exe_off, sizeof( New_exe_off ) );
            Wlseek( New_exe_off );
        }
    }
    // MZ stub is optional
    Wread( &Os2_386_head, sizeof( Os2_386_head ) );
    if( Os2_386_head.signature == OSF_FLAT_SIGNATURE ||
        Os2_386_head.signature == OSF_FLAT_LX_SIGNATURE ) {
        if( Os2_386_head.debug_len ) {
            Wlseek( Os2_386_head.debug_off );
            return( Dmp_elf_header( Os2_386_head.debug_off ) );
        } else {
            Wdputslc( "No OS/2 LE or LX debugging info\n" );
        }
    }
    return( FALSE );
}

/*
 * Dump the Master Debug Header, if any.
 */
static void dmp_master( master_dbg_header mdh )
/*********************************************/
{
    int                     i;

    Banner( "Master Debug Info" );
    Dump_header( (char *)&mdh.exe_major_ver, mdh_msg );
    Wdputslc( "\n" );

    Curr_sectoff -= (long)mdh.debug_size;
    Wlseek( Curr_sectoff );
    Lang_lst = Wmalloc( mdh.lang_size );
    Wread( Lang_lst, mdh.lang_size );
    Curr_sectoff += (long)mdh.lang_size;
    i = 0;
    Wdputslc( "Languages\n" );
    Wdputslc( "=========\n" );
    while( i < mdh.lang_size ) {
        Wdputs( &Lang_lst[i] );
        Wdputslc( "\n" );
        i += strlen( &Lang_lst[i] ) + 1;
    }
    Wdputslc( "\n" );

    Wbuff = Wmalloc( MAX_BUFF );
    Wread( Wbuff, mdh.segment_size );
    Curr_sectoff += (long)mdh.segment_size;
    Wdputslc( "Segments\n" );
    Wdputslc( "========\n" );
    i = 0;
    while( i < mdh.segment_size ) {
        Puthex( *(unsigned_16 *)&Wbuff[i], 4 );
        Wdputslc( "\n" );
        i += sizeof( unsigned_16);
    }
    Wdputslc( "\n" );
}

/*
 * Dump the Debug Header, if any.
 */
bool Dmp_mdbg_head( void )
/************************/
{
    debug_header        dbg;
    char                *signature = "TIS";
    unsigned_16         cnt;
    master_dbg_header   mdh;

    cnt = 0;
    Curr_sectoff = lseek( Handle, 0, SEEK_END );
    Wlseek( Curr_sectoff -(int)sizeof( master_dbg_header ) );
    Wread( &mdh, sizeof( master_dbg_header ) );
    if( mdh.signature == VALID_SIGNATURE &&
        mdh.exe_major_ver == EXE_MAJOR_VERSION &&
        (signed)mdh.exe_minor_ver <= EXE_MINOR_VERSION &&
        mdh.obj_major_ver == OBJ_MAJOR_VERSION &&
        mdh.obj_minor_ver <= OBJ_MINOR_VERSION ) {
        dmp_master( mdh );
        Dump_section();
        return( 1 );
    } else {
        Wlseek( 0 );
        // Handle ELF and NE/LX executables without TIS signature
        if( Dmp_elf_header( 0 ) || os2_debug() ) {
            return( 1 );    // Don't dump debug data twice
        }
        for( ;; ) {
            Wlseek( Curr_sectoff -(int)sizeof( debug_header ) );
            Wread( &dbg, sizeof( debug_header ) );
            if( memcmp( dbg.signature, signature, 4 ) != 0 ) {
                if( cnt ) {
                    return( 1 );
                } else {
                    return( 0 );
                }
            }
            cnt++;
            Wdputs( "size of information = " );
            Puthex( dbg.info_size, 4 );
            Wdputslc( "\n" );
            Curr_sectoff -= dbg.info_size;
            Wlseek( Curr_sectoff );
            if( !Dmp_elf_header( Curr_sectoff ) ) {
                Banner( "Data" );
                Dmp_seg_data( Curr_sectoff, dbg.info_size - sizeof( debug_header ) );
            }
        }
    }
}

/*
 * verify browser identification bytes
 */
bool Dmp_dwarf( void )
/********************/
{
    char        *mbrHeaderString = "WBROWSE";
    int         mbrHeaderStringLen = 7;
    unsigned    mbrHeaderSignature = 0xcbcb;
    char        mbrHeaderVersion = '1';
    unsigned_16 signature;
    char        buf[7];

    Wlseek( 0 );
    Wread( &signature, sizeof(unsigned_16) );
    if( signature != mbrHeaderSignature ){
        return( 0 );
    }
    Wread( buf, mbrHeaderStringLen );
    if( memcmp( buf, mbrHeaderString, mbrHeaderStringLen ) != 0 ) {
        return( 0 );
    }
    Wread( buf, sizeof(char) );
    if( buf[0] != mbrHeaderVersion ) {
        return( 0 );
    }
    set_sects();
    Dump_all_sections();
    return( 1 );
}
