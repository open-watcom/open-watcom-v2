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
* Description:  Utility to dump InfoBench help file contents.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "index.h"

char    Buffer[ PAGE_SIZE ];


void PrintHeader( HelpHeader *header )
{
    printf( "HELP HEADER\n" );
    printf( "    signature 1            %08lX\n", header->sig[0] );
    printf( "    signature 2            %08lX\n", header->sig[1] );
    printf( "    version maj            %04X\n", header->ver_maj );
    printf( "    version min            %04X\n", header->ver_min );
    printf( "    index page count       %d\n", header->indexpagecnt );
    printf( "    data page count        %d\n", header->datapagecnt );
    printf( "    topic count            %d\n", header->topiccnt );
    if( header->sig[0] == HELP_SIG_1 && header->sig[1] == HELP_SIG_2
        && header->ver_maj == HELP_MAJ_VER
        && header->ver_min == HELP_MIN_VER ) {
        printf( "    Header info OK\n" );
    }
}


void PrintStrings( char *buf )
{
    uint_16     str_cnt;
    uint_16     *len;
    uint_16     i;

    str_cnt = *(uint_16 *)buf;
    buf += sizeof( uint_16 );
    len = (uint_16 *)buf;
    buf += str_cnt * sizeof( uint_16 );
    printf( "DEFAULT HELP TOPIC AND DESCRIPTION:\n" );
    for( i = 0; i < str_cnt; i++ ) {
        if( *len != 0 ) {
            printf( "    %s\n", buf );
        }
        buf += *len;
        len++;
    }
}


void PrintItemIndex( HelpHeader *header  )
{
    unsigned            i;
    uint_16             *ptr;

    ptr = (uint_16 *) Buffer;
    printf( "HELP ITEM INDEX\n" );
    for( i = 0; i < header->datapagecnt; i++ ) {
        printf( "    data page %3d    item index %d\n",
                i + header->indexpagecnt, ptr[i] );
    }
}


void PrintDataPage( unsigned cnt )
{
    PageIndexEntry      *entry;
    char                *strings;
    unsigned            i;

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


void PrintIndexPage( unsigned cnt )
{
    HelpIndexEntry      *entry;
    unsigned            i;

    entry = (HelpIndexEntry *)(Buffer + sizeof( HelpPageHeader ));
    for( i = 0; i < cnt; i++ ) {
        printf( "page %4d  \"%s\"\n", entry->nextpage, entry->start );
        entry++;
    }
}


void PrintPage( void )
{
    HelpPageHeader      *header;

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
    int         fp;
    HelpHeader  header;
    unsigned    i;

    if( argc != 2 ) {
        printf( "Usage: helpdump <help file name>\n" );
        return;
    }
    fp = open( argv[1], O_RDONLY | O_BINARY );
    if( fp == -1 ) {
        printf( "Unable to open %s\n", argv[1] );
        return;
    }
    read( fp, &header, sizeof( HelpHeader ) );
    PrintHeader( &header );
    read( fp, Buffer, header.str_size );
    PrintStrings( Buffer );
    read( fp, Buffer, header.datapagecnt * sizeof( uint_16 ) );
    PrintItemIndex( &header );
    for( i = 0; i < header.indexpagecnt + header.datapagecnt; i++ ) {
        read( fp, Buffer, PAGE_SIZE );
        PrintPage();
    }
}
