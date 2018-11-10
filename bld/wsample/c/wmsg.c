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
* Description:  Message/resources support for the Execution Sampler.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/types.h>
#ifdef __WATCOMC__
    #include <process.h>
#endif
#if defined(__WINDOWS__)
    #include <windows.h>
#else
    #include "bool.h"
    #include "wressetr.h"
    #include "wresset2.h"
#endif
#include "wio.h"
#include "sample.h"
#include "smpstuff.h"
#include "wreslang.h"
#include "wmsg.h"

#include "clibext.h"


#define NO_RES_MESSAGE  "Error: could not open message resource file.\r\n"
#define NO_RES_SIZE     (sizeof( NO_RES_MESSAGE ) - 1)

char    FAR_PTR         *MsgArray[ERR_LAST_MESSAGE - ERR_FIRST_MESSAGE + 1];

#if !defined(__WINDOWS__)
static HANDLE_INFO      hInstance = { 0 };
#endif

#if defined(__WINDOWS__)
bool MsgInit( HINSTANCE inst )
#else
static bool MsgReadErrArray( void )
#endif
{
    int         i;
    char        buffer[128];
    unsigned    msg_shift;

    msg_shift = _WResLanguage() * MSG_LANG_SPACING;
    for( i = ERR_FIRST_MESSAGE; i <= ERR_LAST_MESSAGE; i++ ) {
#if !defined(__WINDOWS__)
        if( WResLoadString( &hInstance, i + msg_shift, (lpstr)buffer, sizeof( buffer ) ) <= 0 ) {
#else
        if( LoadString( inst, i + msg_shift, (LPSTR)buffer, sizeof( buffer ) ) <= 0 ) {
#endif
            if( i == ERR_FIRST_MESSAGE )
                return( false );
            buffer[0] = '\0';
        }
        MsgArray[i - ERR_FIRST_MESSAGE] = my_alloc( strlen( buffer ) + 1 );
        if( MsgArray[i - ERR_FIRST_MESSAGE] == NULL )
            return( false );
#ifdef FARDATA
        _fstrcpy( MsgArray[i - ERR_FIRST_MESSAGE], buffer );
#else
        strcpy( MsgArray[i - ERR_FIRST_MESSAGE], buffer );
#endif
    }
    return( true );
}

#if !defined(__WINDOWS__)
bool MsgInit( void )
{
    char        buffer[_MAX_PATH];
#if defined(_PLS)
    char        *fname;
    char        fullpath[_MAX_PATH];
#endif
    bool        rc;

    hInstance.status = 0;
    if( _cmdname( buffer ) != NULL ) {
        rc = OpenResFile( &hInstance, buffer );
#if defined(_PLS)
        if( !rc ) {
            _splitpath2( buffer, fullpath, NULL, NULL, &fname, NULL );
            _makepath( buffer, NULL, NULL, fname, ".exp" );
            _searchenv( buffer, "PATH", fullpath );
            if( fullpath[0] != '\0' ) {
                rc = OpenResFile( &hInstance, fullpath );
            }
        }
#endif
        if( rc ) {
            MsgReadErrArray();
            CloseResFile( &hInstance );
            return( true );
        }
    }
    CloseResFile( &hInstance );
    posix_write( STDOUT_FILENO, NO_RES_MESSAGE, NO_RES_SIZE );
    return( false );
}
#endif

void MsgFini( void )
{
    int          i;

    for( i = ERR_FIRST_MESSAGE; i <= ERR_LAST_MESSAGE; i++ ) {
        my_free( MsgArray[i - ERR_FIRST_MESSAGE] );
    }
}

void MsgPrintfUsage( int first_ln, int last_ln )
{
    for( ; first_ln <= last_ln; first_ln++ ) {
        Output( MsgArray[first_ln - ERR_FIRST_MESSAGE] );
        Output( "\r" );
    }
}
