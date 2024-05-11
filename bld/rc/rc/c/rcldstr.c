/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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

#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <process.h>
#endif
#include "global.h"
#include "rcerrors.h"
#include "usage.rh"
#include "rcldstr.h"
#include "wreslang.h"
#if defined( INCL_MSGTEXT )
#elif defined( USE_WRESLIB )
    #include "wresset2.h"
    #include "rclayer0.h"
#else
    #include <windows.h>
#endif

#include "clibint.h"
#include "clibext.h"


#if defined( INCL_MSGTEXT )
static const char * const StringTable[] = {
    #define pick(c,e,j) e,
    #include "rc.msg"
    #include "usage.gh"
    #undef pick
};
#elif defined( USE_WRESLIB )
static HANDLE_INFO  hInstance = { 0 };
#else
static HINSTANCE    hInstance;
#endif
static unsigned     msgShift;

bool InitRcMsgs( void )
{
#if defined( INCL_MSGTEXT )
    msgShift = 0;
    return( true );
#elif defined( USE_WRESLIB )
    /*
     * minimum size of testbuf is 2 characters (one character + terminator)
     * otherwise WResLoadString return 0 (error)
     */
    char        testbuf[2];
  #if defined( IDE_PGM ) || !defined( __WATCOMC__ )
    char        imageName[_MAX_PATH];
  #else
    char        *imageName;
  #endif

  #if defined( IDE_PGM )
    _cmdname( imageName );
  #elif !defined( __WATCOMC__ )
    get_dllname( imageName, sizeof( imageName ) );
  #else
    imageName = _LpDllName;
  #endif
    /*
     * swap open functions so this file handle is not buffered.
     * This makes it easier for layer0 to fool WRES into thinking
     * that the resource information starts at offset 0
     */
    hInstance.status = 0;
    if( OpenRcMsgsFile( &hInstance, imageName ) ) {
        msgShift = _WResLanguage() * MSG_LANG_SPACING;
        if( GetRcMsg( MSG_USAGE_BASE, testbuf, sizeof( testbuf ) ) ) {
            return( true );
        }
    }
    CloseResFile( &hInstance );
    RcFatalError( ERR_RCSTR_NOT_FOUND );
    return( false );
#else
  #if defined( IDE_PGM )
    hInstance = GetModuleHandle( NULL );
  #elif !defined( __WATCOMC__ )
    char        imageName[_MAX_PATH];

    get_dllname( imageName, sizeof( imageName ) );
    hInstance = GetModuleHandle( imageName );
  #else
    hInstance = GetModuleHandle( _LpDllName );
  #endif
    msgShift = _WResLanguage() * MSG_LANG_SPACING;
    return( true );
#endif
}

bool GetRcMsg( unsigned resid, char *buff, int buff_len )
{
#if defined( INCL_MSGTEXT )
    /* unused parameters */ (void)buff_len;

    strcpy( buff, StringTable[resid] );
#elif defined( USE_WRESLIB )
    if( WResLoadString( &hInstance, resid + msgShift, buff, buff_len ) <= 0 ) {
        buff[0] = '\0';
        return( false );
    }
#else
    if( LoadString( hInstance, resid + msgShift, buff, buff_len ) <= 0 ) {
        buff[0] = '\0';
        return( false );
    }
#endif
    return( true );
}

void FiniRcMsgs( void )
{
#if defined( INCL_MSGTEXT )
#elif defined( USE_WRESLIB )
    CloseResFile( &hInstance );
#else
#endif
}

void ResetRcMsgs( void )
{
#if defined( INCL_MSGTEXT )
#elif defined( USE_WRESLIB )
    memset( &hInstance, 0, sizeof( HANDLE_INFO ) );
#else
    hInstance = NULL;
#endif
}

bool CheckRcMsgsFile( FILE *fp )
{
    /* unused parameters */ (void)fp;

#if defined( INCL_MSGTEXT )
#elif defined( USE_WRESLIB )
    if( hInstance.fp == fp ) {
        return( true );
    }
#else
#endif
    return( false );
}
