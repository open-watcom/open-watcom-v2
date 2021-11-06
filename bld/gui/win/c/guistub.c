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


#include "guiwind.h"

void GUIAPI GUISetBetweenTitles( int between_titles )
{
    (void)between_titles;
}

bool GUIAPI GUIGetCursorPos( gui_window *wnd, gui_point *point )
{
    (void)wnd;
    (void)point;
    return( false );
}

bool GUIAPI GUISetCursorPos( gui_window *wnd, const gui_point *point )
{
    (void)wnd;
    (void)point;
    return( false );
}

bool GUIAPI GUIGetCursorType( gui_window *wnd, gui_cursor_type *cursor )
{
    (void)wnd;
    (void)cursor;
    return( false );
}

bool GUIAPI GUISetCursorType( gui_window *wnd, gui_cursor_type cursor )
{
    (void)wnd;
    (void)cursor;
    return( false );
}

bool GUIAPI GUISetBackgroundChar( gui_window *wnd, char background )
{
    (void)wnd;
    (void)background;
    return( false );
}

void GUIAPI GUIGMouseOn( void )
{
}

void GUIAPI GUIGMouseOff( void )
{
}

void GUIAPI GUISetBackgroundColour( gui_colour_set *colour )
{
    (void)colour;
}

void GUIAPI GUISetDialogColours( gui_colour_set *colours )
{
    (void)colours;
}

void GUIAPI GUIGetDialogColours( gui_colour_set *colours )
{
    (void)colours;
}

void GUIAPI GUISetCharacter( gui_draw_char draw_char, int new )
{
    (void)draw_char;
    (void)new;
}

int GUIAPI GUIGetCharacter( gui_draw_char draw_char )
{
    (void)draw_char;
    return( ' ');
}
