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
#include "banner.h"
#include "sample.h"
#include "smpstuff.h"
#include "wreslang.h"
#include "wmsg.h"
#include "pathgrp2.h"

#include "clibext.h"


enum {
    MSG_RC_COUNT = 0
    #define pick(c,e,j) + 1
    #include "wsample.gh"
    #undef pick
};

enum {
    MSG_USAGE_COUNT = 0
    #define pick(c,e,j) + 1
    #include "usage.gh"
    #undef pick
};

#if defined( __DOS4G__ ) || defined( __PHARLAP__ )
enum {
    MSG_EXC_COUNT = 0
    #define pick(c,e,j) + 1
    #include "except.gh"
    #undef pick
};
#endif

char FAR_PTR            *MsgArray[MSG_RC_COUNT];
#if defined( __DOS4G__ ) || defined( __PHARLAP__ )
char FAR_PTR            *MsgExcArray[MSG_EXC_COUNT];
#endif

static char FAR_PTR     *MsgUsageArray[MSG_USAGE_COUNT];

#if !defined(__WINDOWS__)
static HANDLE_INFO      hInstance = { 0 };
#endif

#if defined(__WINDOWS__)
static bool MsgReadErrArray( HINSTANCE inst, char FAR_PTR **array, int min, int count )
#else
static bool MsgReadErrArray( char FAR_PTR **array, int min, int count )
#endif
{
    int         i;
    char        buffer[128];
    unsigned    msg_shift;

    msg_shift = _WResLanguage() * MSG_LANG_SPACING;
    for( i = 0; i < count; i++ ) {
#if defined(__WINDOWS__)
        if( LoadString( inst, min + i + msg_shift, (LPSTR)buffer, sizeof( buffer ) ) <= 0 ) {
#else
        if( WResLoadString( &hInstance, min + i + msg_shift, (lpstr)buffer, sizeof( buffer ) ) <= 0 ) {
#endif
            if( i == 0 )
                return( false );
            buffer[0] = '\0';
        }
        array[i] = my_alloc( strlen( buffer ) + 1 );
        if( array[i] == NULL )
            return( false );
#ifdef FARDATA
        _fstrcpy( array[i], buffer );
#else
        strcpy( array[i], buffer );
#endif
    }
    return( true );
}

#if defined(__WINDOWS__)
bool MsgInit( HINSTANCE inst )
#else
bool MsgInit( void )
#endif
{
#if defined(__WINDOWS__)
    MsgReadErrArray( inst, MsgArray, MSG_RC_BASE, MSG_RC_COUNT );
    MsgReadErrArray( inst, MsgUsageArray, MSG_USAGE_BASE, MSG_USAGE_COUNT );
    return( true );
#else
    char        buffer[_MAX_PATH];
    bool        rc;

    hInstance.status = 0;
    if( _cmdname( buffer ) != NULL ) {
        rc = OpenResFile( &hInstance, buffer );
  #if defined( __PHARLAP__ )
        if( !rc ) {
            pgroup2     pg;

            _splitpath2( buffer, pg.buffer, NULL, NULL, &pg.fname, NULL );
            _makepath( buffer, NULL, NULL, pg.fname, "exp" );
            _searchenv( buffer, "PATH", pg.buffer );
            if( pg.buffer[0] != '\0' ) {
                rc = OpenResFile( &hInstance, pg.buffer );
            }
        }
  #endif
        if( rc ) {
            MsgReadErrArray( MsgArray, MSG_RC_BASE, MSG_RC_COUNT );
            MsgReadErrArray( MsgUsageArray, MSG_USAGE_BASE, MSG_USAGE_COUNT );
#if defined( __DOS4G__ ) || defined( __PHARLAP__ )
            MsgReadErrArray( MsgExcArray, MSG_EXC_BASE, MSG_EXC_COUNT );
#endif
            CloseResFile( &hInstance );
            return( true );
        }
    }
    CloseResFile( &hInstance );
    puts( NO_RES_MESSAGE );
    return( false );
#endif
}

void MsgFini( void )
{
    int          i;

    for( i = 0; i < MSG_RC_COUNT; i++ ) {
        my_free( MsgArray[i] );
    }
    for( i = 0; i < MSG_USAGE_COUNT; i++ ) {
        my_free( MsgUsageArray[i] );
    }
#if defined( __DOS4G__ ) || defined( __PHARLAP__ )
    for( i = 0; i < MSG_EXC_COUNT; i++ ) {
        my_free( MsgExcArray[i] );
    }
#endif
}

void Usage( void )
{
    int     i;
    char    FAR_PTR *str;

    Output( banner1w( "Execution Sampler", _WSAMP_VERSION_ ) ); OutputNL();
    Output( banner2 ); OutputNL();
    Output( banner2a( 1989 ) ); OutputNL();
    Output( banner3 ); OutputNL();
    Output( banner3a ); OutputNL();
    OutputNL();
    for( i = 0; i < MSG_USAGE_COUNT; i++ ) {
        str = MsgUsageArray[i];
        if( *str != '\0' ) {
            Output( str );
            OutputNL();
        }
    }
}
