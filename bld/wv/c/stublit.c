/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdlib.h>
#include <string.h>
#include <process.h>
#ifdef __OS2__
    #include <os2.h>
#else
    #include <windows.h>
#endif
#include "dbgdefn.h"
#include "dbgmem.h"
#include "wreslang.h"
#include "wv.rh"
#include "dbglit.h"
#if defined( USE_WRESLIB )
    #include "wressetr.h"
    #include "wresset2.h"
#endif


#if defined( USE_WRESLIB )
static HANDLE_INFO      hInstance = { 0 };
#endif
static unsigned         msgShift;

static void InitMsg( void )
{
#if defined( USE_WRESLIB )
    char        name[_MAX_PATH];

    hInstance.status = 0;
    if( _cmdname( name ) != NULL && OpenResFile( &hInstance, name ) ) {
        msgShift = _WResLanguage() * MSG_LANG_SPACING;
        return;
    }
    CloseResFile( &hInstance );
#endif
    msgShift = _WResLanguage() * MSG_LANG_SPACING;
}

static void FiniMsg( void )
{
#if defined( USE_WRESLIB )
    CloseResFile( &hInstance );
#endif
}

char *DUILoadString( dui_res_id id )
{
    char        buffer[256];
    char        *ret;
    int         len;

#if defined( USE_WRESLIB )
    if( hInstance.status == 0 || WResLoadString( &hInstance, id + msgShift, buffer, sizeof( buffer ) ) <= 0 ) {
        buffer[0] = '\0';
        return( NULL );
    }
    len = strlen( buffer ) + 1;
#else
  #ifdef __OS2__
    len = WinLoadString( 0, NULLHANDLE, id + msgShift, sizeof( buffer ), buffer );
  #else
    len = LoadString( GetModuleHandle( NULL ), id + msgShift, buffer, sizeof( buffer ) );
  #endif
    if( len <= 0 ) {
        buffer[0] = '\0';
        return( NULL );
    }
    buffer[len++] = NULLCHAR;
#endif
    ret = MemAlloc( len );
    strcpy( ret, buffer );
    return( ret );
}

void DUIFreeString( void *ptr )
{
    MemFree( ptr );
}

void DUIInitLiterals( void )
{
    InitMsg();
}

void DUIFiniLiterals( void )
{
    FiniMsg();
}
