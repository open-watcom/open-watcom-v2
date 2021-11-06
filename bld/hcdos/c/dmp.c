/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Utility to dump InfoBench help file contents.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "helpidx.h"


static char     Buffer[HLP_PAGE_SIZE];


static void ReadHeader( &header, fp )
{
    uint_32 u32;
    uint_16 u16;

    fread( &u32, sizeof( u32 ), 1, fp );
    header->sig1 = u32;
    fread( &u32, sizeof( u32 ), 1, fp );
    header->sig2 = u32;
    fread( &u16, sizeof( u16 ), 1, fp );
    header->ver_maj = u16;
    fread( &u16, sizeof( u16 ), 1, fp );
    header->ver_min = u16;
    fread( &u16, sizeof( u16 ), 1, fp );
    header->indexpagecnt = u16;
    fread( &u16, sizeof( u16 ), 1, fp );
    header->datapagecnt = u16;
    fread( &u32, sizeof( u32 ), 1, fp );
    header->topiccnt = u32;
    if( header->ver_maj == HELP_MAJ_V1 ) {
        u16 = 0;
    } else {
        fread( &u16, sizeof( u16 ), 1, fp );
    }
    header->str_size = u16;
    fseek( fp, 6 * sizeof( uint_16 ), SEEK_CUR );
}


static void PrintHeader( const HelpHeader *header )
{
    printf( "HELP HEADER\n" );
    printf( "    signature 1            %08lX\n", header->sig1 );
    printf( "    signature 2            %08lX\n", header->sig2 );
    printf( "    version maj            %04X\n", header->ver_maj );
    printf( "    version min            %04X\n", header->ver_min );
    printf( "    index page count       %d\n", header->indexpagecnt );
    printf( "    data page count        %d\n", header->datapagecnt );
    printf( "    topic count            %d\n", header->topiccnt );
    if( header->sig1 == HELP_SIG_1 && header->sig2 == HELP_SIG_2
        && ( header->ver_maj == HELP_MAJ_V1
          || header->ver_maj == HELP_MAJ_VER )
        && header->ver_min == HELP_MIN_VER ) {
        printf( "    Header info OK\n" );
    }
}


static void PrintStrings( const char *buf )
{
    uint_16         str_cnt;
    const uint_16   *str_len;
    uint_16         i;

    str_cnt = *(uint_16 *)buf;
    buf += sizeof( uint_16 );
    str_len = (uint_16 *)buf;
    buf += str_cnt * sizeof( uint_16 );
    printf( "DEFAULT HELP TOPIC AND DESCRIPTION:\n" );
    for( i = 0; i < str_cnt; i++ ) {
        if( *str_len != 0 ) {
            printf( "    %s\n", buf );
        }
        buf += *str_len++;
    }
}


static void PrintItemIndex( const HelpHeader *header  )
{
    unsigned            i;
    const uint_16       *ptr;

    ptr = (uint_16 *)Buffer;
    printf( "HELP ITEM INDEX\n" );
    for( i = 0; i < header->datapagecnt; i++ ) {
        printf( "    data page %3d    item index %d\n",
                i + header->indexpagecnt, ptr[i] );
    }
}


static void PrintDataPage( unsigned cnt )
{
    const PageIndexEntry    *entry;
    const char              *strings;
    unsigned                i;

    entry = (PageIndexEntry *)(Buffer + sizeof( HelpPageHeader ) );
    strings = Buffer + sizeof( HelpPageHeader ) +
              cnt * sizeof( PageIndexEntry );
    for( i = 0; i < cnt; i++ ) {
        printf( "data offset %08lX  string offset %04X \"%s\"\n",
                entry->entry_offset, entry->name_offset,
                strings + entry->name_offset );
        entry++;
    }
}


static void PrintIndexPage( unsigned cnt )
{
    const HelpIndexEntry    *entry;
    unsigned                i;

    entry = (HelpIndexEntry *)(Buffer + sizeof( HelpPageHeader ));
    for( i = 0; i < cnt; i++ ) {
        printf( "page %4d  \"%s\"\n", entry->nextpage, entry->start );
        entry++;
    }
}


static void PrintPage( void )
{
    const HelpPageHeader    *header;

    header = (HelpPageHeader *)Buffer;
    printf( "***************************************************************\n" );
    printf( "PAGE %d\n", header->page_num );
    printf( "    type                   %s\n", header->type?"INDEX":"DATA" );
    printf( "    num_entries            %d\n", header->num_entries );
    printf( "    page_num               %d\n", header->page_num );
    printf( "ENTRIES\n" );
    if( header->type == PAGE_DATA ) {
        PrintDataPage( header->num_entries );
    } else {
        PrintIndexPage( header->num_entries );
    }
}


void main( int argc, char *argv[] )
{
    FILE        *fp;
    HelpHeader  header;
    unsigned    i;

    if( argc != 2 ) {
        printf( "Usage: helpdump <help file name>\n" );
        return;
    }
    fp = fopen( argv[1], "rb" );
    if( fp == NULL ) {
        printf( "Unable to open %s\n", argv[1] );
        return;
    }
    ReadHeader( &header, fp );
    PrintHeader( &header );
    fread( Buffer, header.str_size, 1, fp );
    PrintStrings( Buffer );
    fread( Buffer, header.datapagecnt * sizeof( uint_16 ), 1, fp );
    PrintItemIndex( &header );
    for( i = 0; i < header.indexpagecnt + header.datapagecnt; i++ ) {
        fread( Buffer, HLP_PAGE_SIZE, 1, fp );
        PrintPage();
    }
}
