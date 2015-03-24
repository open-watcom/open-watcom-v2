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
* Description:  InfoBench help file index creation.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wio.h"
#include "watcom.h"
#include "bool.h"
#include "sorthelp.h"
#include "index.h"

#define STR_CNT 2
static int      dataPageCnt;
static int      indexPageCnt;
static int      topicCnt;


static unsigned markDataPages( void )
{
    unsigned            page;
    a_helpnode          *curnode;
    unsigned            size;
    unsigned            nodesize;

    size = sizeof( HelpPageHeader );
    curnode = HelpNodes;
    page = 0;
    topicCnt = 0;
    while( curnode != NULL ) {
        topicCnt++;
        nodesize = sizeof( PageIndexEntry ) + strlen( curnode->name ) + 1;
        if( nodesize + size > HPAGE_SIZE ) {
            page++;
            size = sizeof( HelpPageHeader );
        }
        size += nodesize;
        curnode->data_page = page;
        curnode = curnode->next;
    }
    return( page + 1 );
}


static unsigned pagesInNextLevel( unsigned pages_in_this_level )
{
    unsigned            ret;

    ret = pages_in_this_level / INDEX_ENTRIES_PER_PAGE;
    if( pages_in_this_level % INDEX_ENTRIES_PER_PAGE != 0 )
        ret++;
    return( ret );
}


static unsigned calcIndexPages( unsigned pagecnt )
{
    unsigned    numpages;
    unsigned    next_level;

    numpages = 0;
    for( ;; ) {
        next_level = pagesInNextLevel( pagecnt );
        pagecnt = next_level;
        numpages += next_level;
        if( pagecnt == 1 ) break;
    }
    return( numpages );
}


unsigned calcStringBlockSize( char **str )
{
    unsigned    blocksize;
    unsigned    i;

    blocksize = ( STR_CNT + 1 ) * sizeof( uint_16 );
    for( i = 0; i < STR_CNT; i++ ) {
        if( str[i] != NULL ) {
            blocksize += strlen( str[i] ) + 1;
        }
    }
    return( blocksize );
}


unsigned long CalcIndexSize( char **str, bool gen_str )
{
    unsigned long       ret;

    dataPageCnt = markDataPages();
    indexPageCnt = calcIndexPages( dataPageCnt );
    ret = ( dataPageCnt + indexPageCnt ) * HPAGE_SIZE
            + dataPageCnt * sizeof( uint_16 );
    if( gen_str ) {
        ret += sizeof( HelpHeader ) + calcStringBlockSize( str );
    } else {
        ret += sizeof( OldHeader );
    }
    return( ret );
}


static a_helpnode *findFirstPage( page )
{
    a_helpnode          *curnode;

    curnode = HelpNodes;
    while( curnode != NULL && curnode->data_page + indexPageCnt != page ) {
        curnode = curnode->next;
    }
    return( curnode );
}


static unsigned countPageEntries( unsigned page )
{
    unsigned            cnt;
    a_helpnode          *curnode;

    curnode = HelpNodes;
    cnt = 0;
    curnode = findFirstPage( page );
    while( curnode != NULL && curnode->data_page + indexPageCnt == page ) {
        cnt ++;
        curnode = curnode->next;
    }
    return( cnt );
}


static int writeOneDataPage( int fout, unsigned pagenum )
{
    char                *page;
    HelpPageHeader      *pagehdr;
    PageIndexEntry      *index;
    char                *strings;
    unsigned            stroff;
    unsigned            len;
    a_helpnode          *curnode;

    page = malloc( HPAGE_SIZE );
    memset( page, 0, HPAGE_SIZE );
    pagehdr = (HelpPageHeader *)page;
    index = (PageIndexEntry *)( page + sizeof( HelpPageHeader ) );
    pagehdr->type = PAGE_DATA;
    pagehdr->num_entries = countPageEntries( pagenum );
    pagehdr->page_num = pagenum;
    strings = (char *)index + pagehdr->num_entries * sizeof( PageIndexEntry );
    stroff = 0;
    curnode = findFirstPage( pagenum );
    while( curnode != NULL && curnode->data_page + indexPageCnt == pagenum ) {
        index->name_offset = stroff;
        index->entry_offset = curnode->fpos;
        strcpy( strings, curnode->name );
        len = strlen( curnode->name ) + 1;
        stroff += len;
        strings += len;
        curnode = curnode->next;
        index++;
    }
    write( fout, page, HPAGE_SIZE );
    free( page );
    return( 0 );
}


static int writeDataPages( int fout )
{
    unsigned            i;

    for( i=0; i < dataPageCnt; i++ ) {
        writeOneDataPage( fout, i + indexPageCnt );
    }
    return( 1 );
}


static char *getIndexString( void **pages, unsigned base, int datalevel )
{
    HelpIndexEntry      *entry;
    HelpPageHeader      *header;
    a_helpnode          *node;

    if( datalevel ) {
        node = findFirstPage( base );
        while( node->next != NULL &&
               node->next->data_page + indexPageCnt == base ) {
            node = node->next;
        }
        return( node->name );
    } else {
        header = pages[ base ];
        entry = (HelpIndexEntry *) ( (char *)header
                + sizeof( HelpPageHeader ) );
        return( entry[ header->num_entries - 1].start );
    }
}


static void fillIndexPage( void **pages, unsigned curpage, unsigned base,
                    int datalevel )
{
    HelpIndexEntry      *entry;
    HelpPageHeader      *header;
    unsigned            i;
    char                *src;

    header = pages[curpage];
    entry = (HelpIndexEntry *) ( (char *)header + sizeof( HelpPageHeader ) );
    for( i = 0; i < header-> num_entries; i++ ) {
        entry->nextpage = base;
        src = getIndexString( pages, base, datalevel );
        strncpy( entry->start, src, INDEX_LEN );
        entry->start[ INDEX_LEN -1 ] = '\0';
        base++;
        entry++;
    }
}


static void generateIndexLevel( void **pages, unsigned curbase,
                unsigned cur_level, unsigned prevbase, unsigned prev_level )
{
    unsigned            entries_per_page;
    unsigned            entries_last_page;
    unsigned            i;
    unsigned            base;
    HelpPageHeader      *header;

    entries_per_page = prev_level / cur_level;
    if( prev_level % cur_level != 0 ) {
        entries_per_page ++;
    }
    entries_last_page = prev_level - entries_per_page * ( cur_level - 1 );
    if( prevbase == -1 ) {
        base = indexPageCnt;
    } else {
        base = prevbase;
    }
    for( i = curbase; i < curbase + cur_level; i++ ) {
        pages[i] = malloc( HPAGE_SIZE );
        memset( pages[i], 0, HPAGE_SIZE );
    }
    /***********************************
     * for each B tree page in a level *
     ***********************************/
    for( i = curbase; i < curbase + cur_level; i++ ) {
        header = pages[i];
        header->type = PAGE_INDEX;
        if( i == curbase + cur_level - 1 ) {
            header->num_entries = entries_last_page;
        } else  {
            header->num_entries = entries_per_page;
        }
        header->page_num = i;
        fillIndexPage( pages, i, base, prevbase == -1 );
        base += entries_per_page;
    }
}


static int writeIndexPages( int fout )
{
    unsigned            prev_level;
    unsigned            cur_level;
    void                **pages;
    unsigned            curbase;
    unsigned            prevbase;
    unsigned            i;

    prev_level = dataPageCnt;
    prevbase = -1;
    pages = malloc( indexPageCnt * sizeof( void * ) );
    memset( pages, 0, indexPageCnt * sizeof( void * ) );
    curbase = indexPageCnt;

    /*************************
     * for each B tree level *
     *************************/
     for( ;; ) {
         cur_level = pagesInNextLevel( prev_level );
         curbase -= cur_level;
         generateIndexLevel( pages, curbase, cur_level, prevbase, prev_level );
         if( cur_level == 1 ) break;
         prev_level = cur_level;
         prevbase = curbase;
     }
     for( i = 0; i < indexPageCnt; i++ ) {
         write( fout, pages[i], HPAGE_SIZE );
         if( pages[i] != NULL ) free( pages[i] );
     }
     free( pages );
     return( 0 );
}


static int writePageItemNumIndex( int fout )
{
    uint_16     *index;
    uint_16     indexsize;
    unsigned    i;

    indexsize = dataPageCnt * sizeof( uint_16 );
    index = malloc( indexsize );
    index[0] = 0; //countPageEntries( indexPageCnt );
    for( i=1; i < dataPageCnt; i++ ) {
        index[i] = index[ i-1 ] + countPageEntries( indexPageCnt + i - 1 );
    }
    write( fout, index, indexsize );
    free( index );
    return( 0 );
}


static int writeHeader( int fout, int blocksize )
{
    HelpHeader          header;

    memset( &header, 0, sizeof( HelpHeader ) );
    header.sig[0] = HELP_SIG_1;
    header.sig[1] = HELP_SIG_2;
    header.ver_maj = HELP_MAJ_VER;
    header.ver_min = HELP_MIN_VER;
    header.indexpagecnt = indexPageCnt;
    header.datapagecnt = dataPageCnt;
    header.topiccnt = topicCnt;
    header.str_size = blocksize;
    write( fout, &header, sizeof( HelpHeader ) );
    return( 0 );
}


static int writeOldHeader( int fout )
{
    OldHeader           header;

    memset( &header, 0, sizeof( OldHeader ) );
    header.sig[0] = HELP_SIG_1;
    header.sig[1] = HELP_SIG_2;
    header.ver_maj = 1;
    header.ver_min = HELP_MIN_VER;
    header.indexpagecnt = indexPageCnt;
    header.datapagecnt = dataPageCnt;
    header.topiccnt = topicCnt;
    write( fout, &header, sizeof( OldHeader ) );
    return( 0 );
}

static void writeStrings( int fout, char **str )
{
    unsigned    i;
    uint_16     tmp[STR_CNT + 1];

    tmp[0] = STR_CNT;
    for( i = 0; i < STR_CNT; i++ ) {
        if( str[i] != NULL ) {
            tmp[i + 1] = strlen( str[i] ) + 1;
        } else {
            tmp[i + 1] = 0;
        }
    }
    write( fout, &tmp, sizeof( uint_16 ) * ( STR_CNT + 1 ) );
    for( i = 0; i < STR_CNT; i++ ) {
        if( str[i] != NULL ) {
            write( fout, str[i], tmp[i + 1] );
        }
    }
}

int WriteIndex( int fout, char **str, bool gen_str )
{
    int         blocksize;

    lseek( fout, 0, SEEK_SET );
    if( gen_str ) {
        blocksize = calcStringBlockSize( str );
        writeHeader( fout, blocksize );
        writeStrings( fout, str );
    } else {
        writeOldHeader( fout );
    }
    writePageItemNumIndex( fout );
    writeIndexPages( fout );
    writeDataPages( fout );
    return( 0 );
}
