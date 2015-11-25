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
* Description:  Help file searching routines.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include "wio.h"
#include "watcom.h"
#include "bool.h"
#include "index.h"
#include "helpmem.h"
#include "helpio.h"
#include "search.h"

#include "clibext.h"


#define DEFAULTTOPIC "TABLE OF CONTENTS"

int                     curFile = -1;
char                    curPage[HLP_PAGE_SIZE];
HelpPageHeader          *pageHeader;
char                    *stringBlock;
void                    *pageIndex;

static void loadPage( HelpHdl hdl, unsigned long pagenum )
{
    unsigned long       offset;
    unsigned            tmp;

    if( curFile == hdl->fp && pageHeader->page_num == pagenum ) return;
    if( hdl->header.ver_maj == 1 ) {
        tmp = sizeof( HelpHeader ) - sizeof( uint_16 );   // no str_size
    } else {
        tmp = sizeof( HelpHeader );
    }
    offset = tmp + hdl->header.str_size + pagenum * HLP_PAGE_SIZE
           + hdl->header.datapagecnt * sizeof( uint_16 );
    HelpSeek( hdl->fp, offset, HELP_SEEK_SET );
    HelpRead( hdl->fp, curPage, HLP_PAGE_SIZE );
    curFile = hdl->fp;
    pageHeader = (HelpPageHeader *)curPage;
    pageIndex = curPage + sizeof( HelpPageHeader );
    if( pageHeader->type == PAGE_DATA ) {
        stringBlock = curPage + sizeof( HelpPageHeader )
                    + pageHeader->num_entries * sizeof( PageIndexEntry );
    }
}

static void loadNextPage( HelpHdl hdl, const char *name )
{
    unsigned            i;
    HelpIndexEntry      *entry;

    entry = pageIndex;
    for( i = 0; i < pageHeader->num_entries; i++ ) {
        if( strnicmp( entry->start, name, INDEX_LEN - 1 ) >= 0 ) {
            entry ++;
            break;
        }
        entry++;
    }
    entry --;   /* if we've read through the entire list load the last page */
    loadPage( hdl, entry->nextpage );
}

/*
 * doFindEntry - find the closest match that is alphabetically before
 *                      name in the current page. If no entry on the current
 *                      page comes before name return the first entry on the
 *                      page.
 */
static char *doFindEntry( const char *name, unsigned *entry_num )
{
    unsigned            i;
    unsigned            len;
    PageIndexEntry      *entry;
    int                 cmpret;

    entry = pageIndex;
    len = strlen( name );
    for( i = 0; i < pageHeader->num_entries; i++ ) {
        cmpret = strnicmp( stringBlock + entry[i].name_offset, name, len );
        if( cmpret == 0 ) {
            break;
        }
        if( cmpret > 0 ) {
            if( i > 0 ) {
                i --;
            }
            break;
        }
    }
    if( i == pageHeader->num_entries ) {
        i--;
    }
    if( entry_num != NULL ) {
        *entry_num = i;
    }
    return( stringBlock + entry[i].name_offset );
}

static char *findEntry( HelpHdl hdl, const char *name, unsigned *entry_num )
{
    char        *ret;
    unsigned    pagecnt;
    unsigned    basepage;
    int         cmpret;

    pagecnt = hdl->header.datapagecnt + hdl->header.indexpagecnt;
    basepage = pageHeader->page_num;
    ret = doFindEntry( name, entry_num );
    cmpret = stricmp( ret, name );
    while( cmpret < 0 && pageHeader->page_num < pagecnt - 1 ) {
        loadPage( hdl, pageHeader->page_num + 1 );
        ret = doFindEntry( name, entry_num );
        cmpret = stricmp( ret, name );
    }
    if( cmpret > 0 && pageHeader->page_num > basepage ) {
        loadPage( hdl, pageHeader->page_num - 1 );
        ret = doFindEntry( name, entry_num );
    }
    return( ret );
}

#if 0
char *HelpFindPrev( HelpSrchInfo *info )
{
    PageIndexEntry      *entry;
    char                *ret;

    if( info->page == 0 && info->entry == 0 ) return( NULL );
    if( info->entry == 0 ) {
        info->page--;
        loadPage( info->hdl, info->page );
        info->entry = pageHeader->num_entries - 1;
    } else {
        info->entry--;
        loadPage( info->hdl, info->page );
    }
    entry = pageIndex;
    info->offset = entry[ info->entry ].entry_offset;
    ret = stringBlock + entry[ info->entry ].name_offset;
    return( ret );
}

unsigned long HelpGetOffset( HelpSrchInfo cursor )
{
    return( cursor.offset );
}


char *HelpFindNext( HelpSrchInfo *info )
{
    char                *ret;
    PageIndexEntry      *entry;
    unsigned            pagecnt;

    pagecnt = info->hdl->header.datapagecnt + info->hdl->header.indexpagecnt;
    if( info->page == pagecnt ) return( NULL );
    loadPage( info->hdl, info->page );
    info->entry++;
    if( info->entry == pageHeader->num_entries ) {
        info->page++;
        if( info->page == pagecnt ) return( NULL );
        loadPage( info->hdl, info->page );
        info->entry = 0;
    }
    entry = pageIndex;
    info->offset = entry[ info->entry ].entry_offset;
    ret = stringBlock + entry[ info->entry ].name_offset;
    return( ret );
}
#endif

unsigned HelpFindFirst( HelpHdl hdl, char *name, HelpSrchInfo *info )
{
    unsigned            ret;
    PageIndexEntry      *entry;

    loadPage( hdl, 0 );
    while( pageHeader->type != PAGE_DATA ) {
        loadNextPage( hdl, name );
    }
    findEntry( hdl, name, &ret );
    if( info != NULL ) {
        info->hdl = hdl;
        info->entry = ret;
        info->page = pageHeader->page_num;
        entry = pageIndex;
        info->offset = entry[ info->entry ].entry_offset;
    }
    ret += hdl->itemindex[ pageHeader->page_num - hdl->header.indexpagecnt ];
    return( ret );
}

char *HelpGetIndexedTopic( HelpHdl hdl, unsigned index )
{
    int                 i;
    PageIndexEntry      *entry;

    if( hdl == NULL || index >= hdl->header.topiccnt )
        return( NULL );
    for( i = 0; i < hdl->header.datapagecnt - 1; i++ ) {
        if( hdl->itemindex[i + 1] > index ) {
            break;
        }
    }
    loadPage( hdl, i + hdl->header.indexpagecnt );
    index -= hdl->itemindex[i];
    entry = pageIndex;
    entry += index;
    return( stringBlock + entry->name_offset );
}

unsigned long HelpFindTopicOffset( HelpHdl hdl, char *topic )
{
    unsigned            entry_num;
    PageIndexEntry      *entry;
    char                *foundtopic;

    if( hdl == NULL )
        return( (unsigned long)-1 );
    loadPage( hdl, 0 );
    while( pageHeader->type != PAGE_DATA ) {
        loadNextPage( hdl, topic );
    }
    foundtopic = findEntry( hdl, topic, &entry_num );
    if( !stricmp( foundtopic, topic ) ) {
        entry = pageIndex;
        return( entry[ entry_num ].entry_offset );
    } else {
        return( (unsigned long)-1 );
    }
}

HelpHdl InitHelpSearch( HelpFp fp )
{
    HelpHdl     hdl;
    unsigned    len;
    char        *topic;
    char        *description;
    uint_16     str_cnt;
    uint_16     *str_len;
    char        *ptr;
    char        *buffer;

    HelpSeek( fp, 0, HELP_SEEK_SET );
    hdl = HelpMemAlloc( sizeof( struct HelpHdl ) );
    hdl->fp = fp;
    HelpRead( fp, &( hdl->header ), sizeof( HelpHeader ) );
    if( hdl->header.sig[0] != HELP_SIG_1
        || hdl->header.sig[1] != HELP_SIG_2
        || hdl->header.ver_min != HELP_MIN_VER ) {
        HelpMemFree( hdl );
        hdl = NULL;
    } else if( hdl->header.ver_maj != HELP_MAJ_VER ) {
        if( hdl->header.ver_maj != 1 ) {
            HelpMemFree( hdl );
            hdl = NULL;
        } else {
            HelpSeek( fp, -sizeof( uint_16 ), SEEK_CUR ); // no str_size in header
            topic = HelpMemAlloc( strlen( DEFAULTTOPIC ) + 1 );
            strcpy( topic, DEFAULTTOPIC );
            hdl->def_topic = topic;
            hdl->desc_str = NULL;
            hdl->header.str_size = 0;   // no str_size in old header format
            len = hdl->header.datapagecnt * sizeof( uint_16 );
            hdl->itemindex = HelpMemAlloc( len );
            HelpRead( fp, hdl->itemindex, len );
        }
    } else {
        buffer = HelpMemAlloc( hdl->header.str_size );
        HelpRead( fp, buffer, hdl->header.str_size );
        ptr = buffer;
        str_len = (uint_16 *)ptr;
        str_cnt = *str_len;
        str_len++;
        if( *str_len != 0 ) {
            topic = HelpMemAlloc( *str_len );
            ptr += (str_cnt + 1) * sizeof( uint_16 );
            strcpy( topic, ptr);        // assume topic is first string
        } else {
            topic = HelpMemAlloc( strlen( DEFAULTTOPIC ) + 1 );
            strcpy( topic, DEFAULTTOPIC );
        }
        ptr = buffer;
        ptr += ( str_cnt + 1 ) * ( sizeof( uint_16 ) );
        ptr += ( *str_len ) * ( sizeof( char ) );
        str_len++;
        if( *str_len != 0 ) {
            description = HelpMemAlloc( *str_len );
            strcpy( description, ptr );
        } else {
            description = NULL;
        }
        HelpMemFree( buffer );
        hdl->def_topic = topic;
        hdl->desc_str = description;
        len = ( hdl->header.datapagecnt ) * ( sizeof( uint_16 ) );
        hdl->itemindex = HelpMemAlloc( len );
        HelpRead( fp, hdl->itemindex, len );
    }

    return( hdl );
}

char *GetDefTopic( HelpHdl hdl )
{
    char        *topic;

    if( hdl == NULL ) {
        topic = DEFAULTTOPIC;
    } else {
        topic = hdl->def_topic;
    }
    return( topic );
}

char *GetDescrip( HelpHdl hdl )
{
    char        *description;

    if( hdl == NULL ) {
        description = NULL;
    } else {
        description = hdl->desc_str;
    }
    return( description );
}

void FiniHelpSearch( HelpHdl hdl )
{
    if( hdl != NULL ) {
        if( hdl->itemindex != NULL ) {
            HelpMemFree( hdl->itemindex );
        }
        if( hdl->def_topic != NULL ) {
            HelpMemFree( hdl->def_topic );
        }
        if( hdl->desc_str != NULL ) {
            HelpMemFree( hdl->desc_str );
        }
        HelpMemFree( hdl );
    }
}

#ifdef TEST_SEARCH
#include "trmemcvr.h"

void main( int argc, char *argv[] )
{
    HelpFp              fp;
    HelpHdl             hdl;
    char                name[_MAX_PATH];
    char                *cur;
    HelpSrchInfo        cursor;
    unsigned            i;

    if( argc != 2 ) {
        printf( "USAGE:\n" );
        printf( "exename <help file>\n" );
        return;
    }
    fp = HelpOpen( argv[1], HELP_OPEN_RDONLY | HELP_OPEN_BINARY );
    if( fp == -1 ) {
        printf( "Unable to open %s\n", argv[1] );
        return;
    }
    TRMemOpen();
    hdl = InitHelpSearch( fp );
    for( ;; ) {
        gets( name );
        if( !strcmp( name, "bob" ) ) break;
        cur = HelpFindFirst( hdl, name, &cursor );
        for( i=0; i < 5; i++ ) {
            if( cur == NULL ) break;
            printf( "     %s\n", cur );
            HelpMemFree( cur );
            cur = HelpFindNext( &cursor );
        }
        if( cur != NULL ) {
            HelpMemFree( cur );
        }
    }
    FiniHelpSearch( hdl );
    HelpClose( fp );
    TRMemClose();
}
#endif
