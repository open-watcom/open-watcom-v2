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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include "watcom.h"
#include "wpmsg.h"
#ifdef USE_WRESLIB
    #include "wressetr.h"
    #include "wresset2.h"
#else
    #include <windows.h>
#endif

#include "clibext.h"


#ifdef USE_WRESLIB
static HANDLE_INFO      hInstance = { 0 };
#else
static HINSTANCE        hInstance;
#endif

bool MsgInit( char *fname )
/*************************/
{
#ifdef USE_WRESLIB
    hInstance.status = 0;
    if( OpenResFile( &hInstance, fname ) ) {
        return( true );
    }
    CloseResFile( &hInstance );
    puts( NO_RES_MESSAGE );
    return( false );
#else
    hInstance = GetModuleHandle( NULL );
    return( true );
#endif
}

void MsgGet( int resourceid, char *buffer )
/*****************************************/
{
#ifdef USE_WRESLIB
    if( hInstance.status == 0 || WResLoadString( &hInstance, resourceid, (lpstr)buffer, MAX_RESOURCE_SIZE ) <= 0 ) {
        buffer[0] = '\0';
    }
#else
    if( LoadString( hInstance, resourceid, buffer, MAX_RESOURCE_SIZE ) <= 0 ) {
        buffer[0] = '\0';
    }
#endif
}

bool MsgFini( void )
/******************/
{
#ifdef USE_WRESLIB
    return( CloseResFile( &hInstance ) );
#else
    return( true );
#endif
}
