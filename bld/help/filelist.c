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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include<stdio.h>
#include<stdlib.h>
#include<direct.h>
#include<string.h>
#include "uidef.h"
#include "help.h"
#include "helpmem.h"
#include "helpio.h"
#include "index.h"
#include "search.h"

#define MAX_HELPFILES   100
#define FILE_CNT        sizeof( FileDescs ) / sizeof( FileDesc )
#define DEF_EXT         ".IHP"

typedef struct {
    char        *fname;
    char        *fpath;
} FileInfo;

typedef struct {
    unsigned    allocated;
    unsigned    used;
    FileInfo    *items[1];      /* dynamic array */
} FileList;

typedef struct {
    char        *helpfname;
    char        *descrip;
} FileDesc;

static FileDesc         FileDescs[] = {
    "CGUIDE",   "",
    "CLIB",     "",
    "CPPLIB",   "",
    "LGUIDE",   "",
    "PGUIDE",   "",
    "RESCOMP",  "",
    "TOOLS",    "",
    "WCCERRS",  "",
    "WD",       "",
    "WPPERRS",  "",
    "WPROF",    ""
};

#if(0)
static int fn_comp( char *p1, FileDesc *p2 )
{
    return( strcmp( p1, p2->helpfname ) );
}
#endif

static void freeFileList( FileList *list ) {
    unsigned            i;

    for( i=0; i < list->used; i++ ) {
        HelpMemFree( list->items[i]->fpath );
        HelpMemFree( list->items[i]->fname );
        HelpMemFree( list->items[i] );
    }
    HelpMemFree( list );
}

static void printDescrip( FileInfo *info, unsigned cnt ) {
    char        *buf;
    HelpFp      fp;
    char        tmp[ _MAX_PATH ];
    FileDesc    *key;
    HelpHdl     hdl;

    cnt = cnt;
    strupr( info->fname );
//    key  = bsearch( info->fname, FileDescs, cnt, sizeof( FileDesc ), fn_comp);
//    disabled default text
    key = NULL;
    if( key != NULL ) {
        printf( "%s\n", key->descrip );
    } else {
        strcpy( tmp, info->fpath );
        strcat( tmp, "\\" );
        strcat( tmp, info->fname );
        strcat( tmp, DEF_EXT );
        fp = HelpOpen( tmp, HELP_OPEN_RDONLY | HELP_OPEN_BINARY );
        if( fp != -1 ) {
            hdl = InitHelpSearch( fp );
            buf = GetDescrip( hdl );
            if( buf != NULL ) {
                printf( "%s", buf );
            }
            FiniHelpSearch( hdl );
        }
        printf( "\n" );
        HelpClose( fp );
    }
}

static void printFileList( FileList *list ) {

    unsigned            i;

    if( list->used > 0 ) {
        printf( "%-8s     ", list->items[0]->fname );
        printDescrip( list->items[0], FILE_CNT );
        for( i=1; i < list->used; i++ ) {
            /* eliminate duplicates */
            if( strcmp( list->items[i-1]->fname, list->items[i]->fname ) ) {
                printf( "%-8s     ", list->items[i]->fname );
                printDescrip( list->items[i], FILE_CNT );
            }
        }
    }
}

static int compareStr( FileInfo **str1, FileInfo **str2 ) {
    return( strcmp( ( *str1 )->fname, ( *str2 )->fname ) );
}

static void sortFileList( FileList *list ) {
    qsort( list->items, list->used, sizeof( FileInfo * ), compareStr );
}

static void scanDirectory( char *buf, FileList *list ) {

    DIR                 *dirhdl;
    struct dirent       *dirent;
    char                fname[_MAX_FNAME];
    unsigned            len;
    char                newpath[ _MAX_PATH ];

    strcpy( newpath, buf );
    strcat( buf, "\\*.IHP" );
    dirhdl = opendir( buf );
    dirent = readdir( dirhdl );
    while( dirent != NULL ) {
        list->items[ list->used ] = HelpMemAlloc( sizeof( FileInfo ) );
        len = strlen( newpath ) + 1;
        list->items[ list->used ]->fpath = HelpMemAlloc( len );
        strcpy( list->items[ list->used ]->fpath, newpath );
        _splitpath( dirent->d_name, NULL, NULL, fname, NULL );
        len = strlen( fname ) + 1;
        list->items[ list->used ]->fname = HelpMemAlloc( len );
        strcpy( list->items[ list->used ]->fname, fname );
        list->used ++;
        if( list->used == list->allocated ) {
            list->allocated += MAX_HELPFILES;
            list = HelpMemRealloc( list, sizeof( FileList )
                                   + list->allocated * sizeof( FileInfo* ) );
        }
        dirent = readdir( dirhdl );
    }
    closedir( dirhdl );
}

static doFillFileList( char *cur, const FileList *list ) {

    char                done;
    char                *path;
    unsigned            len;
    char                buf[ _MAX_PATH ];

    done = 0;
    len = strlen( cur ) + 1;
    path = HelpMemAlloc( len + 2 );
    strcpy( path, cur );
    cur = path;
    for( ;; ) {
        while( *cur != ';' ) {
            if( *cur == '\0' ) {
                done = 1;
                break;
            }
            cur++;
        }
        *cur = '\0';
        strcpy( buf, path );
        len = strlen( buf );
        if( buf[len] == '\\' ) buf[len] = '\0';
        scanDirectory( buf, list );
        if( done ) break;
        path = cur + 1;
        cur = path;
    }
}

static fillFileList( HelpSrchPathItem *srch, FileList *list ) {

    unsigned    i;
    char        *cur;
    unsigned    done;

    done = 0;
    for( i=0 ;; i++ ) {
        switch( srch[i].type ) {
        #ifndef __NETWARE_386__
        case SRCHTYPE_ENV:
            cur = getenv( srch[i].info );
            break;
        #endif
        case SRCHTYPE_PATH:
            cur = srch[i].info;
            break;
        case SRCHTYPE_EOL:
            done = 1;
            break;
        }
        if( done ) break;
        if( cur != NULL ) doFillFileList( cur, list );
    }
}

static FileList *initFileList( void ) {
    FileList    *ret;

    ret = HelpMemAlloc( sizeof( FileList )
                        + MAX_HELPFILES * sizeof( FileInfo* ) );
    ret->allocated = MAX_HELPFILES;
    ret->used = 0;
    return( ret );
}

void PrintHelpFiles( HelpSrchPathItem *srch ) {

    FileList    *list;

    list = initFileList();
    fillFileList( srch, list );
    sortFileList( list );
    printFileList( list );
    freeFileList( list );
}
