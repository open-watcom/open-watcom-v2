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
* Description:  Message output for librarian.
*
****************************************************************************/


#include "wlib.h"
#ifdef __WATCOMC__
    #include <process.h>
#endif
#include "wreslang.h"
#if defined( INCL_MSGTEXT )
#elif defined( USE_WRESLIB )
    #include "wressetr.h"
    #include "wresset2.h"
#else
    #include <windows.h>
#endif

#include "clibint.h"
#include "clibext.h"


#if defined( INCL_MSGTEXT )
static const char * const msg_text_array[] = {
    #define pick(c,e,j) e,
    #include "wlib.msg"
    #include "usagew.gh"
    #include "usagea.gh"
    #undef pick
};
#elif defined( USE_WRESLIB )
static HANDLE_INFO      hInstance = { 0 };
#else
static HINSTANCE        hInstance;
#endif
static unsigned         msgShift;

void InitMsg( void )
{
#if defined( INCL_MSGTEXT )
    msgShift = 0;
#elif defined( USE_WRESLIB )
  #if defined( IDE_PGM ) || !defined( __WATCOMC__ )
    char    imageName[_MAX_PATH];
  #else
    char    *imageName;
  #endif

  #if defined( IDE_PGM )
    _cmdname( imageName );
  #elif !defined( __WATCOMC__ )
    get_dllname( imageName, sizeof( imageName ) );
  #else
    imageName = _LpDllName;
  #endif
    hInstance.status = 0;
    if( OpenResFile( &hInstance, imageName ) ) {
        msgShift = _WResLanguage() * MSG_LANG_SPACING;
        return;
    }
    CloseResFile( &hInstance );
    FatalResError( NO_RES_MESSAGE "\n" );
#else
  #if defined( IDE_PGM )
    hInstance = GetModuleHandle( NULL );
  #elif !defined( __WATCOMC__ )
    char    imageName[_MAX_PATH];
    get_dllname( imageName, sizeof( imageName ) );
    hInstance = GetModuleHandle( imageName );
  #else
    hInstance = GetModuleHandle( _LpDllName );
  #endif
    msgShift = _WResLanguage() * MSG_LANG_SPACING;
#endif
}

void MsgGet( int msgid, char *buffer )
{
#if defined( INCL_MSGTEXT )
    strcpy( buffer, msg_text_array[msgid] );
#elif defined( USE_WRESLIB )
    if( hInstance.status == 0 || WResLoadString( &hInstance, msgid + msgShift, (lpstr)buffer, MAX_ERROR_SIZE ) <= 0 ) {
        buffer[0] = '\0';
    }
#else
    if( LoadString( hInstance, msgid + msgShift, buffer, MAX_ERROR_SIZE ) <= 0 ) {
        buffer[0] = '\0';
    }
#endif
}

void FiniMsg( void )
{
#if defined( INCL_MSGTEXT )
#elif defined( USE_WRESLIB )
    CloseResFile( &hInstance );
#else
#endif
}
