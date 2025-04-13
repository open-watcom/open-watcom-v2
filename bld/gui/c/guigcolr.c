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


#include "guiwind.h"
#include "guistr.h"


static const char *ColourNames[GUI_NUM_COLOURS];

static bool ColourNamesInitialized = false;

static void InitColourNames( void )
{
    ColourNames[0] = LIT_GUI( Black );
    ColourNames[1] = LIT_GUI( Blue );
    ColourNames[2] = LIT_GUI( Green );
    ColourNames[3] = LIT_GUI( Cyan );
    ColourNames[4] = LIT_GUI( Red );
    ColourNames[5] = LIT_GUI( Magenta );
    ColourNames[6] = LIT_GUI( Brown );
    ColourNames[7] = LIT_GUI( White );
    ColourNames[8] = LIT_GUI( Grey );
    ColourNames[9] = LIT_GUI( BrightBlue );
    ColourNames[10] = LIT_GUI( BrightGreen );
    ColourNames[11] = LIT_GUI( BrightCyan );
    ColourNames[12] = LIT_GUI( BrightRed );
    ColourNames[13] = LIT_GUI( BrightMagenta );
    ColourNames[14] = LIT_GUI( BrightYellow );
    ColourNames[15] = LIT_GUI( BrightWhite );
}

static gui_colour InitColour;

static void GUICALLBACK PickColour( gui_window *wnd, gui_ctl_id list_id )
{
    int i;

    if( !ColourNamesInitialized ) {
        InitColourNames();
    }

    for( i = 0; i < GUI_NUM_COLOURS; i++ ) {
        GUIAddText( wnd, list_id, ColourNames[i] );
    }
    if(( InitColour >= 0 ) && ( InitColour < GUI_NUM_COLOURS )) {
        GUISetCurrSelect( wnd, list_id, (int)InitColour );
    }
}

bool GUIAPI GUIGetColourFromUser( const char *title, gui_colour *init, gui_colour *new_colour )
{
    int     result;

    if( new_colour == NULL ) {
        return( false );
    }
    if( init != NULL ) {
        InitColour = *init;
    } else {
        InitColour = GUI_BAD_COLOUR;
    }
    if( GUIDlgPick( title, &PickColour, &result ) ) {
        *new_colour = (gui_colour)result;
        return( true );
    }
    return( false );
}
