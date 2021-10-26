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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "wrglbl.h"
#include "ldstr.h"
#include "wrmsg.h"

/* routine to create a message box */
void WRDisplayMsg( const char *msg )
{
    char        *title;

    title = WRAllocRCString( WR_ERRMSG );

    if( !MessageBox( (HWND)NULL, msg, title, MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL ) ) {
        MessageBeep( (UINT)-1 );
    }

    if( title != NULL ) {
        WRFreeRCString( title );
    }
}

char *WRAllocRCString( msg_id id )
{
    return( AllocRCString( id ) );
}

void WRFreeRCString( char *str )
{
    FreeRCString( str );
}

int WRCopyRCString( msg_id id, char *buf, int bufsize )
{
    return( CopyRCString( id, buf, bufsize ) );
}

void WRInitDisplayError( HINSTANCE inst )
{
    SetInstance( inst );
}

void WRDisplayErrorMsg( msg_id msg )
{
    char        *title;

    title = WRAllocRCString( WR_ERRMSG );

    if( !RCMessageBox( (HWND)NULL, msg, title, MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL ) ) {
        MessageBeep( (UINT)-1 );
    }

    if( title != NULL ) {
        WRFreeRCString( title );
    }
}

void WRPrintErrorMsg( msg_id msg, ... )
{
    va_list     args;
    char        *str;
    char        buf[512];

    str = WRAllocRCString( msg );
    va_start( args, msg );
    vsprintf( buf, str, args );
    WRDisplayMsg( buf );
    va_end( args );
}
