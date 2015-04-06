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
* Description:  Help file list management.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#ifdef __UNIX__
    #include <dirent.h>
#else
    #include <direct.h>
#endif
#include <string.h>
#include "watcom.h"
#include "uidef.h"
#include "help.h"
#include "helpmem.h"
#include "helpio.h"
#include "index.h"
#include "search.h"
#include "clibext.h"

#define MAX_HELPFILES   100

typedef struct {
    char        *fname;
    char        *fpath;
} FileInfo;

typedef struct {
    unsigned    allocated;
    unsigned    used;
    FileInfo    *items[1];      /* dynamic array */
} FileList;

static void freeFileList( FileList *list )
{
    unsigned            i;

    for( i = 0; i < list->used; i++ ) {
        HelpMemFree( list->items[i]->fpath );
        HelpMemFree( list->items[i]->fname );
        HelpMemFree( list->items[i] );
    }
    HelpMemFree( list );
}

static void printDescrip( FileInfo *info)
{
    char        *buf;
    HelpFp      fp;
    char        tmp[_MAX_PATH];
    HelpHdl     hdl;

    strcpy( tmp, info->fpath );
#ifdef __UNIX__
    strcat( tmp, "/" );
#else
    strcat( tmp, "\\" );
#endif
    strcat( tmp, info->fname );
    SetHelpFileDefExt( tmp, tmp );
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

static void printFileList( FileList *list )
{
    unsigned            i;

    if( list->used > 0 ) {
        printf( "%-8s     ", list->items[0]->fname );
        printDescrip( list->items[0]);
        for( i = 1; i < list->used; i++ ) {
            /* eliminate duplicates */
            if( strcmp( list->items[i-1]->fname, list->items[i]->fname ) ) {
                printf( "%-8s     ", list->items[i]->fname );
                printDescrip( list->items[i]);
            }
        }
    }
}

static int compareStr( const void *arg1, const void *arg2 )
{
    FileInfo **str1 = (FileInfo**) arg1;
    FileInfo **str2 = (FileInfo**) arg2;
    return( strcmp( ( *str1 )->fname, ( *str2 )->fname ) );
}

static void sortFileList( FileList *list )
{
    qsort( list->items, list->used, sizeof( FileInfo * ), compareStr );
}

static void scanDirectory( char *buf, FileList *list )
{
    DIR                 *dirhdl;
    struct dirent       *dirent;
    char                fname[_MAX_FNAME];
    unsigned            len;

    dirhdl = opendir( buf );
    if ( dirhdl == NULL )
        return;
    dirent = readdir( dirhdl );
    while( dirent != NULL ) {
        len = strlen( dirent->d_name );
        if ( len < sizeof( DEF_EXT ) || stricmp ( &dirent->d_name[len - ( sizeof( DEF_EXT ) - 1 )], DEF_EXT ) != 0 ) {
            dirent = readdir( dirhdl );
            continue;
        }
        list->items[list->used] = HelpMemAlloc( sizeof( FileInfo ) );
        len = strlen( buf ) + 1;
        list->items[list->used]->fpath = HelpMemAlloc( len );
        strcpy( list->items[list->used]->fpath, buf );
        _splitpath( dirent->d_name, NULL, NULL, fname, NULL );
        len = strlen( fname ) + 1;
        list->items[list->used]->fname = HelpMemAlloc( len );
        strcpy( list->items[list->used]->fname, fname );
        list->used ++;
        if( list->used == list->allocated ) {
            list->allocated += MAX_HELPFILES;
            list = HelpMemRealloc( list, sizeof( FileList ) + list->allocated * sizeof( FileInfo * ) );
        }
        dirent = readdir( dirhdl );
    }
    closedir( dirhdl );
}

static void doFillFileList( const char *path, FileList *list )
{
    char                done;
    char                *path_start;
    char                *path_end;
    unsigned            len;
    char                buf[_MAX_PATH];

    done = 0;
    len = strlen( path ) + 1;
    path_start = HelpMemAlloc( len + 2 );
    strcpy( path_start, path );
    for( ;; ) {
        path_end = path_start;
#ifdef __UNIX__
        while( *path_end != ':' ) {
#else
        while( *path_end != ';' ) {
#endif
            if( *path_end == '\0' ) {
                done = 1;
                break;
            }
            path_end++;
        }
        *path_end = '\0';
        strcpy( buf, path_start );
        len = strlen( buf );
#ifdef __UNIX__
        if( buf[len] == '/' ) buf[len] = '\0';
#else
        if( buf[len] == '/' || buf[len] == '\\' ) buf[len] = '\0';
#endif
        scanDirectory( buf, list );
        if( done )
            break;
        path_start = path_end + 1;
    }
}

static void fillFileList( HelpSrchPathItem *srch, FileList *list )
{
    unsigned    i;
    const char  *cur;
    unsigned    done;

    done = 0;
    for( i = 0 ;; i++ ) {
        cur = NULL;
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
        if( cur != NULL ) {
            doFillFileList( cur, list );
        }
    }
}

static FileList *initFileList( void )
{
    FileList    *ret;

    ret = HelpMemAlloc( sizeof( FileList ) + MAX_HELPFILES * sizeof( FileInfo* ) );
    ret->allocated = MAX_HELPFILES;
    ret->used = 0;
    return( ret );
}

void PrintHelpFiles( HelpSrchPathItem *srch )
{
    FileList    *list;

    list = initFileList();
    fillFileList( srch, list );
    sortFileList( list );
    printFileList( list );
    freeFileList( list );
}
