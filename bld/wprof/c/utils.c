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
* Description:  File access utilities.
*
****************************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "common.h"

#if defined( __WINDOWS__ ) || defined( __NT__ )
#include <windows.h>
#endif
#if defined( __DOS__ ) || defined( __WINDOWS__ ) || defined( __NT__ )
#   include "tinyio.h"
#elif defined( __OS2__ )
#   define INCL_DOS
#   include "os2.h"
#elif defined( __UNIX__ )
#   include <unistd.h>
#   if defined( __WATCOMC__ )
#       include <process.h>
#   endif
#else
#   error OS not supported
#endif

#include "aui.h"
#include "dip.h"
#include "wpaui.h"
#include "myassert.h"
#include "msg.h"
#include "pathlist.h"

#if defined(__UNIX__)
 #define PATH_SEPARATOR '/'
 #define LIST_SEPARATOR ':'
 #define PATH_NAME  "WD_PATH"
#else
 #define PATH_SEPARATOR '\\'
 #define LIST_SEPARATOR ';'
 #define PATH_NAME  "PATH"
#endif
#define HELP_NAME  "WWINHELP"

extern void *ProfAlloc(size_t size);
extern void fatal(char *msg,... );
extern dig_fhandle DIGCliOpen(char *name,dig_open mode);
extern void AddPath( path_list **path_var, char * path_data );

path_list *     HelpPathList = NULL;
path_list *     FilePathList = NULL;
path_list *     DipExePathList = NULL;



extern void ReplaceExt( char * path, char * addext )
/**************************************************/
{
    char        buff [ _MAX_PATH2 ];
    char *      drive;
    char *      dir;
    char *      fname;
    char *      ext;

    _splitpath2( path, buff, &drive, &dir, &fname, &ext );
#if defined(__UNIX__)
    if( stricmp( ext, addext ) != 0 ) {
        strcat( path, addext );
    }
#else
    _makepath( path, drive, dir, fname, addext );
#endif
}



extern char * FindFile( char * path, char * name, path_list * path_tail )
/***********************************************************************/
{
    path_list *     path_item;
    file_handle     fh;

    fh = open( name, O_RDONLY | O_BINARY, S_IREAD );
    if( fh != -1 ) {
        close( fh );
        strcpy( path, name );
        return( path );
    }
    if( path_tail == NULL ) {
        return( NULL );
    }
    path_item = path_tail->next;
    for(;;) {
        strcpy( path, path_item->path_data );
        strcat( path, name );
        fh = open( path, O_RDONLY | O_BINARY, S_IREAD );
        if( fh != -1 ) {
            close( fh );
            return( path );
        }
        if( path_item == path_tail ) break;
        path_item = path_item->next;
    }
    return( NULL );
}



#if defined( __QNX__ ) || defined( __LINUX__ ) || defined( __DOS__ )
extern dig_fhandle FullPathOpen( char const *name, char *ext,
                                 char *result, unsigned max_res )
/***************************************************************/
{
    char        realname[ _MAX_PATH2 ];
    char *      filename;

    if( ext == NULL || *ext == '\0' ) {
        strcpy( realname, name );
    } else {
        _splitpath2( name, result, NULL, NULL, &filename, NULL );
        _makepath( realname, NULL, NULL, filename, ext );
    }
    filename = FindFile( result, realname, FilePathList );
    if( filename == NULL ) {
        filename = FindFile( result, realname, DipExePathList );
    }
    if( filename == NULL ) {
        return( -1 );
    }
    return( DIGCliOpen( filename, DIG_READ ) );
}

extern dig_fhandle PathOpen( char * name, unsigned len, char * ext )
/******************************************************************/
{
    char        path[ _MAX_PATH2 ];

    return( FullPathOpen( name, ext, path, sizeof( path ) ) );
}
#endif



extern void InitPaths()
/*********************/
{
    char *      env;
#if defined(__UNIX__)
    char        buff [ _MAX_PATH ];
    char        *p;
#endif

    env = getenv( PATH_NAME );
    if( env != NULL && *env != '\0' ) {
        AddPath( &FilePathList, env );
        AddPath( &HelpPathList, env );
    }
    env = getenv( HELP_NAME );
    if( env != NULL && *env != '\0' ) {
        AddPath( &HelpPathList, env );
    }
#if defined(__UNIX__)
    if( _cmdname( buff ) != NULL ) {
        p = strrchr( buff, '/' );
        if( p != NULL ) {
            *p = '\0';
            p = strrchr( buff, '/' );
            if( p != NULL ) {
                /* look in the sibling directories of where the executable is */
                strcpy( p + 1, "wd" );
                AddPath( &HelpPathList, buff );
                AddPath( &DipExePathList, buff );
                *p = '\0';
                AddPath( &HelpPathList, buff );
                AddPath( &DipExePathList, buff );
            }
        }
    }
    AddPath( &HelpPathList, "/usr/watcom/wd" );
    AddPath( &DipExePathList, "/usr/watcom/wd" );
    AddPath( &HelpPathList, "/usr/watcom" );
    AddPath( &DipExePathList, "/usr/watcom" );
#endif
}



extern void AddPath( path_list **path_var, char *path_data )
/**********************************************************/
{
    char            path[_MAX_PATH];
    path_list *     path_tail;
    path_list *     path_item;
    int             index;

    if( path_data == NULL ) return;
    path_tail = *path_var;
    for(;;) {
        if( *path_data == '\0' ) break;
        index = 0;
        while( *path_data != NULLCHAR ) {
            if( *path_data == LIST_SEPARATOR ) break;
            path[index++] = *path_data++;
        }
        if( index != 0 ) {
            if( path[index-1] != PATH_SEPARATOR ) {
                path[index++] = PATH_SEPARATOR;
            }
            path_item = ProfAlloc( sizeof(path_list)+index+1 );
            memcpy( path_item->path_data, path, index );
            path_item->path_data[index] = NULLCHAR;
            if( path_tail == NULL ) {
                path_item->next = path_item;
            } else {
                path_item->next = path_tail->next;
                path_tail->next = path_item;
            }
            path_tail = path_item;
        }
        if( *path_data == LIST_SEPARATOR ) {
            ++path_data;
        }
    }
    *path_var = path_tail;
}



#if defined( __QNX__ )
/*
    QNX only allows 32K-1 bytes to be read/written at any one time, so bust
    up any I/O larger than that.
*/
#define MAX_OS_TRANSFER (32U*1024 - 512)

STATIC unsigned doread( int file, void * buffer, unsigned len )
/*************************************************************/
{
    unsigned    total;
    int         h;
    int         amount;

    total = 0;
    for( ;; ) {
        if( len == 0 ) {
            return( total );
        }
        amount = len;
        if( amount > MAX_OS_TRANSFER ) {
            amount = MAX_OS_TRANSFER;
        }
        h = read( file, buffer, amount );
        if( h < 0 ) {
            return( h );
        }
        total += h;
        if( h != amount ) {
            return( total );
        }
        buffer = (char *)buffer + amount;
        len -= amount;
    }
}
#else
    #define doread( f, b, l )  read( f, b, l )
#endif

extern unsigned BigRead( int fh, void * buff, unsigned size )
/***********************************************************/
{
    return( doread( fh, buff, size ) );
}



#if defined( __DOS__ )
extern void DoRingBell( void );
#pragma aux DoRingBell =                                \
        " push   ebp            ",                      \
        " mov    ax, 0e07h      ",                      \
        " int    10h            ",                      \
        " pop    ebp            "                       \
        modify exact [ ax ];
#endif



extern void Ring( void )
/**********************/
{
#if defined( __DOS__ )
    DoRingBell();
#elif defined( __WINDOWS__ ) || defined( __NT__ )
    MessageBeep( 0 );
#elif defined( __QNX__ ) || defined( __LINUX__ )
    write( STDOUT_FILENO, "\a", 1 );
#elif defined( __OS2__ )
    DosBeep( 1000, 250 );
#endif
}



#ifndef NDEBUG
extern void AssertionFailed( char * file, unsigned line )
/*******************************************************/
{
    char        path[ _MAX_PATH2 ];
    char        buff[ 13+_MAX_FNAME ];
    char *      fname;
    unsigned    size;

    _splitpath2( file, path, NULL, NULL, &fname, NULL ); /* _MAX_FNAME */
    size = strlen( fname );
    memcpy( buff, fname, size );
    buff[size] = ' ';                                   /*   1 */
    utoa( line, &buff[size + 1], 10 );                  /*  10 */
                                                /* '\0'    + 1 */
                                                        /* --- */
                                                        /*  12+_MAX_FNAME */
    fatal( LIT( Assertion_Failed ), buff );
}
#endif
