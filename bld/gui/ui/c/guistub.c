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


bool GUIAPI GUIChangeFont( gui_window *wnd )
{
    /* unused parameters */ (void)wnd;

    return( false );
}

char * GUIAPI GUIGetFontInfo( gui_window *wnd )
{
    /* unused parameters */ (void)wnd;

    return( NULL );
}

bool GUIAPI GUISetFontInfo( gui_window *wnd, char *info )
{
    /* unused parameters */ (void)wnd; (void)info;

    return( false );
}

bool GUIAPI GUISetSystemFont( gui_window *wnd, bool fixed )
{
    /* unused parameters */ (void)wnd; (void)fixed;

    return( false );
}

bool GUIAPI GUIFontsSupported( void )
{
    return( false );
}

char * GUIAPI GUIGetFontFromUser( char *fontinfo )
{
    /* unused parameters */ (void)fontinfo;

    return( NULL );
}

gui_mcursor_handle GUIAPI GUISetMouseCursor( gui_mcursor_type type )
{
    switch( type ) {
    case GUI_ARROW_CURSOR:
    case GUI_HOURGLASS_CURSOR:
    case GUI_CROSS_CURSOR:
        return( (void *)"" );
    default:
        return( NULL );
    }
}

void GUIAPI GUIResetMouseCursor( gui_mcursor_handle old )
{
    /* unused parameters */ (void)old;
}

bool GUIAPI GUI3DDialogInit( void )
{
    return( false );
}

void GUIAPI GUI3DDialogFini( void )
{
}

bool GUIAPI GUISetHorizontalExtent( gui_window *wnd, gui_ctl_id id, gui_ord extentx )
{
    /* unused parameters */ (void)wnd; (void)id; (void)extentx;

    return( false );
}

bool GUIAPI GUIEnableControl( gui_window *wnd, gui_ctl_id id, bool enable )
{
    /* unused parameters */ (void)wnd; (void)id; (void)enable;

    return( false );
}

bool GUIAPI GUIIsControlEnabled( gui_window *wnd, gui_ctl_id id )
{
    /* unused parameters */ (void)wnd; (void)id;

    return( false );
}

void GUIAPI GUIHideControl( gui_window *wnd, gui_ctl_id id )
{
    /* unused parameters */ (void)wnd; (void)id;
}

void GUIAPI GUIShowControl( gui_window *wnd, gui_ctl_id id )
{
    /* unused parameters */ (void)wnd; (void)id;
}

bool GUIAPI GUISetEditSelect( gui_window *wnd, gui_ctl_id id, int start, int end )
{
    /* unused parameters */ (void)wnd; (void)id; (void)start; (void)end;

    return( false );
}

bool GUIAPI GUIGetEditSelect( gui_window *wnd, gui_ctl_id id, int *start, int *end )
{
    /* unused parameters */ (void)wnd; (void)id; (void)start; (void)end;

    return( false );
}

void GUIAPI GUIScrollCaret( gui_window *wnd, gui_ctl_id id )
{
    /* unused parameters */ (void)wnd; (void)id;
}
