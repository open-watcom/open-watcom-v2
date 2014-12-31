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
* Description:  AUI status window.
*
****************************************************************************/


#include "auipvt.h"
#include <string.h>

void WndCreateStatusWindow( gui_colour_set *colour )
{
    GUICreateStatusWindow( WndMain->gui, 0, 0, colour );
    WndSetWndMax();
}


bool WndHaveStatusWindow( void )
{
    return( WndMain == NULL ? FALSE : GUIHasStatus( WndMain->gui ) );
}


void WndCloseStatusWindow( void )
{
    if( WndHaveStatusWindow() ) GUICloseStatusWindow( WndMain->gui );
    WndSetWndMax();
}

#define STAT_LEN        80
char WndStatusString[STAT_LEN+1];

bool WndStatusText( const char *text )
{
    bool        rc;

    if( strcmp( text, WndStatusString ) == 0 ) return( TRUE );
    if( WndStatusString != NULL ) {
        strncpy( WndStatusString, text, STAT_LEN );
        WndStatusString[STAT_LEN] = '\0';
    }
    rc = WndInternalStatusText( WndStatusString );
    return( rc );
}

bool GUIClearStatusText( gui_window *gui )
{
    if( gui == WndMain->gui ) {
        WndInternalStatusText( WndStatusString );
        return( TRUE );
    }
    return( FALSE );
}

void WndResetStatusText( void )
{
    WndInternalStatusText( WndStatusString );
}

bool WndInternalStatusText( char *text )
{
    if( text == NULL ) return( FALSE );
    if( !WndHaveStatusWindow() ) return( FALSE );
    GUIDrawStatusText( WndMain->gui, text );
    if( !GUIIsGUI() && !WndDoingRefresh ) {
        WndForceRefresh();
    }
    return( TRUE );
}
