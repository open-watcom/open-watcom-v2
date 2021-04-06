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
#include "bool.h"
#include "help.h"
#include "helpmem.h"
#include "filelist.h"
#include "pathgrp2.h"

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
    FILE        *fp;
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
    fp = HelpOpen( tmp );
    if( fp != NULL ) {
        hdl = InitHelpSearch( fp );
        buf = GetDescrip( hdl );
        if( buf != NULL ) {
            printf( "%s", buf );
        }
        FiniHelpSearch( hdl );
        HelpClose( fp );
    }
    printf( "\n" );
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

static FileList *scanDirectory( char *buf, FileList *list )
{
    DIR                 *dirp;
    struct dirent       *dire;
    pgroup2             pg;
    unsigned            len;

    dirp = opendir( buf );
    if( dirp != NULL ) {
        while( (dire = readdir( dirp )) != NULL ) {
            len = strlen( dire->d_name );
            if ( len < ( sizeof( "." DEF_EXT ) - 1 ) || stricmp( &dire->d_name[len - ( sizeof( "." DEF_EXT ) - 1 )], "." DEF_EXT ) != 0 ) {
                continue;
            }
            list->items[list->used] = HelpMemAlloc( sizeof( FileInfo ) );
            list->items[list->used]->fpath = HelpDupStr( buf );
            _splitpath2( dire->d_name, pg.buffer, NULL, NULL, &pg.fname, NULL );
            list->items[list->used]->fname = HelpDupStr( pg.fname );
            list->used++;
            if( list->used == list->allocated ) {
                list->allocated += MAX_HELPFILES;
                list = HelpMemRealloc( list, sizeof( FileList ) + list->allocated * sizeof( FileInfo * ) );
            }
        }
        closedir( dirp );
    }
    return( list );
}

static FileList *doFillFileList( const char *path, FileList *list )
{
    char                done;
    char                *path_start;
    char                *path_end;
    char                *p;
    unsigned            len;
    char                buf[_MAX_PATH];

    done = 0;
    len = strlen( path ) + 1;
    path_start = HelpMemAlloc( len + 2 );
    strcpy( path_start, path );
    p = path_start;
    for( ;; ) {
        path_end = p;
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
        strcpy( buf, p );
        len = strlen( buf ) - 1;
#ifdef __UNIX__
        if( buf[len] == '/' )
            buf[len] = '\0';
#else
        if( buf[len] == '/' || buf[len] == '\\' )
            buf[len] = '\0';
#endif
        list = scanDirectory( buf, list );
        if( done )
            break;
        p = path_end + 1;
    }
    HelpMemFree( path_start );
    return( list );
}

static FileList *fillFileList( HelpSrchPathItem *srch, FileList *list )
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
        if( done )
            break;
        if( cur != NULL ) {
            list = doFillFileList( cur, list );
        }
    }
    return( list );
}

static FileList *initFileList( void )
{
    FileList    *list;

    list = HelpMemAlloc( sizeof( FileList ) + MAX_HELPFILES * sizeof( FileInfo* ) );
    list->allocated = MAX_HELPFILES;
    list->used = 0;
    return( list );
}

void PrintHelpFiles( HelpSrchPathItem *srch )
{
    FileList    *list;

    list = initFileList();
    list = fillFileList( srch, list );
    sortFileList( list );
    printFileList( list );
    freeFileList( list );
}
