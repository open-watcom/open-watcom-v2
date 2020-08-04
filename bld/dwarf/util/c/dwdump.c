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
* Description:  DWARF dump program - display DWARF debugging information
*               in mostly human-readable format.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <setjmp.h>
#include "trmemcvr.h"
#include "orl.h"
#include <dr.h>
#include <dw.h>
#include "client.h"

#include "clibext.h"


#define MAX_SECS    255

typedef struct _dump_options {
    int         sections;
} dump_options;

typedef struct buff_entry {
    struct buff_entry   *next;
    char                buff[1];
} buff_entry, *buff_list;

static dw_out_offset   sectsizes[DR_DEBUG_NUM_SECTS];
unsigned_8      *sections[DR_DEBUG_NUM_SECTS];

bool            byte_swap = false;

static const char *secNames[] = {
    ".debug_info",
    ".debug_abbrev",
    ".debug_line",
    ".debug_macinfo",
    ".WATCOM_references",
    ".debug_str",
    ".debug_aranges"
};

struct section_data Sections[DR_DEBUG_NUM_SECTS];

static dump_options dump = { 0 };

static buff_list    buffList = NULL;

static int          sectionFound = 0;

static orl_return DoSection( orl_sec_handle o_shnd )
/**************************************************/
{
    /* unused parameters */ (void)o_shnd;

    return( ORL_OKAY );
}

#if 0
orl_return DoSymTable( orl_sec_handle orl_sec_hnd )
/*************************************************/
{
    return( ORL_OKAY );
}
#endif

static void *objRead( FILE *fp, size_t len )
/******************************************/
{
    buff_list   ptr;

    ptr = TRMemAlloc( sizeof( *buffList ) + len - 1 );
    ptr->next = buffList;
    buffList = ptr;
    if( fread( ptr->buff, 1, len, fp ) != len ) {
        TRMemFree( ptr );
        return( NULL );
    }
    return( ptr->buff );
}

static int objSeek( FILE *fp, long pos, int where )
/*************************************************/
{
    return( fseek( fp, pos, where ) );
}

static void freeBuffList( void )
/******************************/
{
    buff_list   next;

    while( buffList ) {
        next = buffList->next;
        TRMemFree( buffList );
        buffList = next;
    }
}

static orl_return ReadDbgInfoSec( orl_sec_handle o_shnd )
/*******************************************************/
{
    sectsizes[DW_DEBUG_INFO] = ORLSecGetSize( o_shnd );
    return( ORLSecGetContents( o_shnd, &sections[DW_DEBUG_INFO] ));
}

static orl_return ReadAbbrevSec( orl_sec_handle o_shnd )
/******************************************************/
{
    sectsizes[DW_DEBUG_ABBREV] = ORLSecGetSize( o_shnd );
    return( ORLSecGetContents( o_shnd, &sections[DW_DEBUG_ABBREV] ));
}

static orl_return ReadLineSec( orl_sec_handle o_shnd )
/****************************************************/
{
    sectsizes[DW_DEBUG_LINE] = ORLSecGetSize( o_shnd );
    return( ORLSecGetContents( o_shnd, &sections[DW_DEBUG_LINE] ));
}

static orl_return ReadMacinfoSec( orl_sec_handle o_shnd )
/*******************************************************/
{
    sectsizes[DW_DEBUG_MACINFO] = ORLSecGetSize( o_shnd );
    return( ORLSecGetContents( o_shnd, &sections[DW_DEBUG_MACINFO] ));
}

static orl_return ReadRefSec( orl_sec_handle o_shnd )
/***************************************************/
{
    sectsizes[DW_DEBUG_REF] = ORLSecGetSize( o_shnd );
    return( ORLSecGetContents( o_shnd, &sections[DW_DEBUG_REF] ));
}

static orl_return ReadStrSec( orl_sec_handle o_shnd )
/***************************************************/
{
    sectsizes[DW_DEBUG_STR] = ORLSecGetSize( o_shnd );
    if( sectsizes[DW_DEBUG_STR] == 0 )
        return( ORL_OKAY );
    else
        return( ORLSecGetContents( o_shnd, &sections[DW_DEBUG_STR] ));
}

static orl_return ReadARSec( orl_sec_handle o_shnd )
/**************************************************/
{
    sectsizes[DW_DEBUG_ARANGES] = ORLSecGetSize( o_shnd );
    return( ORLSecGetContents( o_shnd, &sections[DW_DEBUG_ARANGES] ));
}

static int setSects( orl_file_handle o_fhnd )
/*******************************************/
{
    int           i;

    memset( sections, 0, DR_DEBUG_NUM_SECTS * sizeof(unsigned_32) );
    memset( sectsizes, 0, DR_DEBUG_NUM_SECTS * sizeof(unsigned_32) );

    if( ORLFileScan( o_fhnd, secNames[0], &ReadDbgInfoSec ) != ORL_OKAY ) {
        printf( "Error reading %s section\n", secNames[0] );
        return( 1 );
    }
    if( ORLFileScan( o_fhnd, secNames[1], &ReadAbbrevSec ) != ORL_OKAY ) {
        printf( "Error reading %s section\n", secNames[1] );
        return( 2 );
    }
    if( ORLFileScan( o_fhnd, secNames[2], &ReadLineSec ) != ORL_OKAY ) {
        printf( "Error reading %s section\n", secNames[2] );
        return( 3 );
    }
    if( ORLFileScan( o_fhnd, secNames[3], &ReadMacinfoSec ) != ORL_OKAY ) {
        printf( "Error reading %s section\n", secNames[3] );
        return( 4 );
    }
    if( ORLFileScan( o_fhnd, secNames[4], &ReadRefSec ) != ORL_OKAY ) {
        printf( "Error reading %s section\n", secNames[4] );
        return( 5 );
    }
    if( ORLFileScan( o_fhnd, secNames[5], &ReadStrSec ) != ORL_OKAY ) {
        printf( "Error reading %s section\n", secNames[5] );
        return( 6 );
    }
    if( ORLFileScan( o_fhnd, secNames[6], &ReadARSec ) != ORL_OKAY ) {
        printf( "Error reading %s section\n", secNames[6] );
        return( 6 );
    }

    for( i = 0; i < DR_DEBUG_NUM_SECTS; i += 1 ) {
        Sections[i].cur_offset = 0;
        Sections[i].max_offset = sectsizes[i];

        if( sectsizes[i] != 0 ) {
            Sections[i].data = malloc( sectsizes[i] );
            if( Sections[i].data == NULL ) {
                fprintf( stderr, "Not enough memory\n" );
                exit(1);
            }
            memcpy(Sections[i].data, sections[i], sectsizes[i] );
        }
    }
    return( 0 );
}

int main( int argc, char *argv[] )
/********************************/
{
    orl_handle                  o_hnd;
    orl_file_handle             o_fhnd;
    orl_file_format             type;
    orl_file_flags              o_flags;
    FILE                        *fp;
    int                         c;
    char                        *secs[MAX_SECS];
    int                         num_secs = 0;
    ORLSetFuncs( orl_cli_funcs, objRead, objSeek, TRMemAlloc, TRMemFree );

    if( argc < 2 ) {
        printf( "Usage:  dwdump <file>\n" );
        printf( "Where <file> is a COFF, ELF or OMF object file\n" );
        printf( "dwdump reads and dumps DWARF debugging information\n" );
        return( EXIT_SUCCESS );
    }

    dump.sections++;

    fp = fopen( argv[1], "rb" );
    if( fp == NULL ) {
        printf( "Error opening file.\n" );
        return( EXIT_FAILURE );
    }
    TRMemOpen();
    o_hnd = ORLInit( &orl_cli_funcs );
    if( o_hnd == NULL ) {
        printf( "Got NULL orl_handle.\n" );
        return( EXIT_FAILURE );
    }
    type = ORLFileIdentify( o_hnd, fp );
    if( type == ORL_UNRECOGNIZED_FORMAT ) {
        printf( "The object file is not in either ELF, COFF or OMF format." );
        return( EXIT_FAILURE );
    }
    switch( type ) {
    case ORL_OMF:
        printf( "OMF" );
        break;
    case ORL_ELF:
        printf( "ELF" );
        break;
    case ORL_COFF:
        printf( "COFF" );
        break;
    default:
        printf( "Unknown" );
        break;
    }
    printf( " object file.\n" );
    o_fhnd = ORLFileInit( o_hnd, fp, type );
    if( o_fhnd == NULL ) {
        printf( "Got NULL orl_file_handle.\n" );
        return( EXIT_FAILURE );
    }

    o_flags = ORLFileGetFlags( o_fhnd );

#ifdef __BIG_ENDIAN__
    if( o_flags & ORL_FILE_FLAG_LITTLE_ENDIAN ) {
        byte_swap = true;
    }
#else
    if( o_flags & ORL_FILE_FLAG_BIG_ENDIAN ) {
        byte_swap = true;
    }
#endif

    if( num_secs ) {
        for( c = 0; c < num_secs; c++ ) {
            sectionFound = 0;
            if( ORLFileScan( o_fhnd, secs[c], &DoSection ) != ORL_OKAY ) {
                printf( "Error occured in scanning section '%s'.\n", secs[c] );
            }
            if( !sectionFound ) {
                printf( "Section '%s' not found in object.\n", secs[c] );
            }
        }
    } else {
        if( ORLFileScan( o_fhnd, NULL, &DoSection ) != ORL_OKAY ) {
            printf( "Error occured in scanning file.\n" );
            return( EXIT_FAILURE );
        }
    }

    setSects( o_fhnd );

    if( ORLFileFini( o_fhnd ) != ORL_OKAY ) {
        printf( "Error calling ORLFileFini.\n" );
        return( EXIT_FAILURE );
    }
    if( fclose( fp ) ) {
        printf( "Error closing file.\n" );
        return( EXIT_FAILURE );
    }
    if( ORLFini( o_hnd ) != ORL_OKAY ) {
        printf( "Error calling ORLFini.\n" );
    }

    DumpSections();

    freeBuffList();
#ifdef TRMEM
    TRMemPrtList();
#endif
    TRMemClose();
    return( EXIT_SUCCESS );
}
