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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "guiwind.h"
#include "guix.h"

bool GUIChangeFont( gui_window *wnd )
{
    wnd = wnd;
    return( false );
}

char *GUIGetFontInfo( gui_window *wnd )
{
    wnd = wnd;
    return( NULL );
}

bool GUISetFontInfo( gui_window *wnd, char *info )
{
    wnd = wnd;
    info = info;
    return( false );
}

bool GUISetSystemFont( gui_window *wnd, bool fixed )
{
    wnd = wnd;
    fixed = fixed;
    return( false );
}

bool GUIFontsSupported( void )
{
    return( false );
}

char *GUIGetFontFromUser( char *fontinfo )
{
    fontinfo = fontinfo;
    return( NULL );
}

void *GUISetMouseCursor( gui_mouse_cursor type )
{
    type = type;
    return( NULL );
}

void GUIResetMouseCursor( void *old )
{
    old=old;
}

bool GUI3DDialogInit( void )
{
    return( false );
}

bool GUISetHorizontalExtent( gui_window *wnd, unsigned id, int extent )
{
    wnd = wnd;
    id = id;
    extent = extent;
    return( false );
}

bool GUIEnableControl( gui_window *wnd, unsigned id, bool enable )
{
    wnd = wnd;
    id = id;
    enable = enable;
    return( false );
}

bool GUIIsControlEnabled( gui_window *wnd, unsigned id )
{
    wnd = wnd;
    id = id;
    return( false );
}

void GUIHideControl( gui_window *wnd, unsigned id )
{
    wnd = wnd;
    id = id;
}

void GUIShowControl( gui_window *wnd, unsigned id )
{
    wnd = wnd;
    id = id;
}

bool GUISetEditSelect( gui_window *wnd, unsigned id, int start, int end )
{
    wnd = wnd;
    id = id;
    start = start;
    end = end;
    return( false );
}

bool GUIGetEditSelect( gui_window *wnd, unsigned id, int *start, int *end )
{
    wnd = wnd;
    id = id;
    start = start;
    end = end;
    return( false );
}

void GUIScrollCaret( gui_window *wnd, unsigned id )
{
    wnd = wnd;
    id = id;
}
