/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "common.h"
#if defined( __DOS__ )
    #include <i86.h>
    #include "tinyio.h"
    #include "int10.h"
    #include "realmod.h"
#elif defined( __WINDOWS__ )
    #include <windows.h>
#elif defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #define INCL_DOS
    #include "os2.h"
#elif defined( __UNIX__ )
    #include <unistd.h>
    #if defined( __WATCOMC__ )
        #include <process.h>
    #endif
#else
    #error OS not supported
#endif
#include "aui.h"
#include "dip.h"
#include "wpaui.h"
#include "myassert.h"
#include "msg.h"
#include "memutil.h"
#include "iopath.h"
#include "pathlist.h"
#include "digcli.h"
#include "digld.h"
#include "utils.h"
#include "sampinfo.h"
#include "wpdata.h"
#include "pathgrp2.h"
#include "autoenv.h"

#include "clibext.h"


#if defined( __UNIX__ )
 #define PATH_NAME  "WD_PATH"
#else
 #define PATH_NAME  "PATH"
#endif
#define HELP_NAME  "WWINHELP"

char   *HelpPathList = NULL;
char   *FilePathList = NULL;
char   *DipExePathList = NULL;

void ReplaceExt( char * path, char * addext )
/*******************************************/
{
    pgroup2     pg;

    _splitpath2( path, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
#if defined(__UNIX__)
    if( stricmp( pg.ext, addext ) != 0 ) {
        strcat( path, addext );
    }
#else
    _makepath( path, pg.drive, pg.dir, pg.fname, addext );
#endif
}

static char *findFile( char *fullname, const char *name, char *path_list )
/************************************************************************/
{
    FILE            *fp;
    char            *p;
    char            c;

    fp = fopen( name, "rb" );
    if( fp != NULL ) {
        fclose( fp );
        strcpy( fullname, name );
        return( fullname );
    }
    if( path_list != NULL ) {
        while( (c = *path_list) != NULLCHAR ) {
            p = fullname;
            do {
                ++path_list;
                if( IS_PATH_LIST_SEP( c ) )
                    break;
                *p++ = c;
            } while( (c = *path_list) != NULLCHAR );
            c = p[-1];
            if( !IS_PATH_SEP( c ) ) {
                *p++ = DIR_SEP;
            }
            strcpy( p, name );
            fp = fopen( fullname, "rb" );
            if( fp != NULL ) {
                fclose( fp );
                return( fullname );
            }
        }
    }
    *fullname = '\0';
    return( NULL );
}


char *FindHelpFile( char *fullname, const char *help_name )
{
    if( findFile( fullname, help_name, HelpPathList ) == NULL ) {
        ErrorMsg( LIT( Unable_To_Open_Help ), help_name );
        return( NULL );
    }
    return( fullname );
}

#if defined( __UNIX__ ) || defined( __DOS__ )
FILE *DIGLoader( Open )( const char *name, size_t name_len, const char *ext, char *result, size_t max_result )
/************************************************************************************************************/
{
    char        realname[ _MAX_PATH2 ];
    char        *filename;
    FILE        *fp;

    /* unused parameters */ (void)max_result;

    memcpy( realname, name, name_len );
    realname[name_len] = '\0';
    if( ext != NULL && *ext != NULLCHAR ) {
        pgroup2     pg;

        _splitpath2( realname, pg.buffer, NULL, NULL, &pg.fname, NULL );
        _makepath( realname, NULL, NULL, pg.fname, ext );
    }
    filename = findFile( result, realname, FilePathList );
    if( filename == NULL ) {
        filename = findFile( result, realname, DipExePathList );
    }
    fp = NULL;
    if( filename != NULL )
        fp = fopen( filename, "rb" );
    return( fp );
}

int DIGLoader( Read )( FILE *fp, void *buff, size_t len )
{
    return( fread( buff, 1, len, fp ) != len );
}

int DIGLoader( Seek )( FILE *fp, unsigned long offs, dig_seek where )
{
    return( fseek( fp, offs, where ) );
}

int DIGLoader( Close )( FILE *fp )
{
    return( fclose( fp ) );
}
#endif

static char *AddPath( char *old_list, const char *path_list )
/***********************************************************/
{
    size_t          len;
    size_t          old_len;
    char            *new_list;
    char            *p;

    new_list = old_list;
    if( path_list != NULL && *path_list != NULLCHAR ) {
        len = strlen( path_list );
        if( old_list == NULL ) {
            p = new_list = ProfAlloc( len + 1 );
        } else {
            old_len = strlen( old_list );
            new_list = ProfAlloc( old_len + 1 + len + 1 );
            memcpy( new_list, old_list, old_len );
            ProfFree( old_list );
            p = new_list + old_len;
        }
        while( *path_list != NULLCHAR ) {
            if( p != new_list )
                *p++ = PATH_LIST_SEP;
            path_list = GetPathElement( path_list, NULL, &p );
        }
        *p = NULLCHAR;
    }
    return( new_list );
}

void InitPaths( void )
/********************/
{
    char        *env;
#if defined(__UNIX__)
    char        buff [ _MAX_PATH ];
    char        *p;
#endif

    watcom_setup_env();

    env = getenv( PATH_NAME );
    FilePathList = AddPath( FilePathList, env );
    HelpPathList = AddPath( HelpPathList, env );
    env = getenv( HELP_NAME );
    HelpPathList = AddPath( HelpPathList, env );
#if defined(__UNIX__)
    if( _cmdname( buff ) != NULL ) {
        p = strrchr( buff, '/' );
        if( p != NULL ) {
            *p = NULLCHAR;
            p = strrchr( buff, '/' );
            if( p != NULL ) {
                /* look in the sibling directories of where the executable is */
                strcpy( p + 1, "wd" );
                HelpPathList = AddPath( HelpPathList, buff );
                DipExePathList = AddPath( DipExePathList, buff );
                *p = NULLCHAR;
                HelpPathList = AddPath( HelpPathList, buff );
                DipExePathList = AddPath( DipExePathList, buff );
            }
        }
    }
    HelpPathList = AddPath( HelpPathList, "/usr/watcom/wd" );
    DipExePathList = AddPath( DipExePathList, "/usr/watcom/wd" );
    HelpPathList = AddPath( HelpPathList, "/usr/watcom" );
    DipExePathList = AddPath( DipExePathList, "/usr/watcom" );
#endif
}

void Ring( void )
/***************/
{
#if defined( __DOS__ )
    _BIOSVideoRingBell( BIOSData( BDATA_ACT_VPAGE, unsigned char ) );
#elif defined( __WINDOWS__ ) || defined( __NT__ )
    MessageBeep( 0 );
#elif defined( __QNX__ ) || defined( __LINUX__ )
    write( STDOUT_FILENO, "\a", 1 );
#elif defined( __OS2__ )
    DosBeep( 1000, 250 );
#endif
}

#ifndef NDEBUG
void AssertionFailed( char * file, unsigned line )
/************************************************/
{
    pgroup2     pg;
    char        buff[ 13 + _MAX_FNAME ];
    size_t      size;

    _splitpath2( file, pg.buffer, NULL, NULL, &pg.fname, NULL ); /* _MAX_FNAME */
    size = strlen( pg.fname );
    memcpy( buff, pg.fname, size );
    buff[size] = ' ';                                   /*   1 */
    utoa( line, &buff[size + 1], 10 );                  /*  10 */
                                                        /* '\0' + 1 */
                                                        /* --- */
                                                        /*  12+_MAX_FNAME */
    fatal( LIT( Assertion_Failed ), buff );
}
#endif
