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

void GUISetBetweenTitles( int between_titles )
{
    between_titles = between_titles;
}

bool GUIGetCursorPos( gui_window *wnd, gui_point *point )
{
    wnd = wnd;
    point = point;
    return( false );
}

bool GUISetCursorPos( gui_window *wnd, gui_point *point )
{
    wnd = wnd;
    point = point;
    return( false );
}

bool GUIGetCursorType( gui_window *wnd, gui_char_cursor *cursor )
{
    wnd = wnd;
    cursor = cursor;
    return( false );
}

bool GUISetCursorType( gui_window *wnd, gui_char_cursor cursor )
{
    wnd = wnd;
    cursor = cursor;
    return( false );
}

bool GUISetBackgroundChar( gui_window *wnd, char background )
{
    wnd = wnd;
    background = background;
    return( false );
}

void GUIGMouseOn( void )
{
}

void GUIGMouseOff( void )
{
}

void GUISetBackgroundColour( gui_colour_set *colour )
{
    colour = colour;
}

void GUISetDialogColours( gui_colour_set *colours )
{
    colours = colours;
}

void GUIGetDialogColours( gui_colour_set *colours )
{
    colours = colours;
}

void GUISetCharacter( gui_draw_char draw_char, int new )
{
    draw_char = draw_char;
    new = new;
}

int GUIGetCharacter( gui_draw_char draw_char )
{
    draw_char = draw_char;
    return( ' ');
}
