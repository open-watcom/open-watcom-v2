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
#include "guicolor.h"
#include "watcom.h"
#include "help.h"
#include "guistr.h"
#include "guievent.h"


gui_help_instance GUIAPI GUIHelpInit( gui_window *wnd, const char *file, const char *title )
{
    /* unused parameters */ (void)wnd; (void)file; (void)title;

    return( (gui_help_instance)true );
}

void GUIAPI GUIHelpFini( gui_help_instance inst, gui_window *wnd, const char *file )
{
    /* unused parameters */ (void)inst; (void)wnd; (void)file;
}

bool GUIAPI GUIShowHelp( gui_help_instance inst, gui_window *wnd, gui_help_actions act, const char *file, const char *topic )
{
    bool        ret;

    /* unused parameters */ (void)inst;

    ret = false;

    switch( act ) {
    case GUI_HELP_CONTENTS:
    case GUI_HELP_ON_HELP:
    case GUI_HELP_SEARCH:
        ret = GUIDisplayHelp( wnd, file, LIT( Table_of_Contents ) );
        break;
    case GUI_HELP_CONTEXT:
        break;
    case GUI_HELP_KEY:
        if( topic == NULL ) {
            topic = "";
        }
        ret = GUIDisplayHelp( wnd, file, topic );
        break;
    }

    return( ret );
}

bool GUIAPI GUIShowHtmlHelp( gui_help_instance inst, gui_window *wnd, gui_help_actions act, const char *file, const char *topic )
{
    /* unused parameters */ (void)inst; (void)wnd; (void)act; (void)file; (void)topic;

    return( false );
}

bool GUIAPI GUIDisplayHelp( gui_window *wnd, const char *file, const char *topic )
{
    bool colours_set;
    const char *helpfiles[2] = { NULL, NULL };

    /* unused parameters */ (void)wnd;

    helpfiles[0] = file;
    colours_set = GUISetDialColours();
    uipoplist( /* GUIAllEvents */ );
    helpinit( helpfiles, NULL );
    showhelp( topic, NULL, HELPLANG_ENGLISH );
    helpfini();
    uipushlist( GUIAllEvents );
    if( colours_set ) {
        GUIResetDialColours();
    }
    return( true );
}

bool GUIAPI GUIDisplayHelpId( gui_window *wnd, const char *file, gui_hlp_id id )
{
    /* unused parameters */ (void)wnd; (void)file; (void)id;

    return( false );
}
