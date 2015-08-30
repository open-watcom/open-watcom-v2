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
* Description:  Functions to set dialog box controls.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "wio.h"
#include "gui.h"
#include "guistr.h"
#include "guiutil.h"
#include "guidlg.h"
#include "dlgbutn.h"
#include "dlggen.h"
#include "setup.h"
#include "setupinf.h"
#include "genvbl.h"
#include "dynarray.h"
#include "genctrl.h"

#include "clibext.h"


void set_dlg_textwindow( gui_control_info *gui_controls,
                        int num_controls, const char *s,
                        gui_ctl_id id,
                        gui_ord x1,                 // top-left x
                        gui_ord y1,                 // top-left y
                        gui_ord x2,                 // bottom-right x
                        int lines,                  // number of lines
                        gui_scroll_styles scroll )  // scroll style
/*****************************************************************/
// see gui.h for gui_scroll_styles
{
    gui_control_info    *control;

    control = &gui_controls[num_controls];

    control->control_class = GUI_EDIT_MLE;
    control->text = GUIStrDup( s, NULL );
    DLG_SET_RECT( *control, x1, y1, x2, y1 + lines );
    control->parent = NULL;
    control->scroll = scroll;
    control->style  = GUI_TAB_GROUP | GUI_CONTROL_READONLY | GUI_CONTROL_BORDER;
    control->id     = id;
}


void set_dlg_dynamstring( gui_control_info *gui_controls,
                        int num_controls, const char *s,
                        gui_ctl_id id, gui_ord x1, gui_ord y1, gui_ord x2 )
/*************************************************************************/
{
    gui_control_info    *control;

    control = &gui_controls[num_controls];
    control->control_class = GUI_STATIC;
    control->text = GUIStrDup( s, NULL );
    DLG_SET_RECT( *control, x1, y1, x2, y1 );
    control->parent     = NULL;
    control->scroll     = GUI_NOSCROLL;
    control->style      = GUI_CONTROL_NOPREFIX | GUI_AUTOMATIC;
    control->id         = id;
}


void set_dlg_radio( gui_control_info *gui_controls,
                    int num_controls, int num_radio_buttons, const char *s,
                    gui_ctl_id id, gui_ord x1, gui_ord y1, gui_ord x2 )
/*************************************************************************/
{
    gui_control_info    *control;

    control = &gui_controls[num_controls];

    control->control_class = GUI_RADIO_BUTTON;
    control->text = GUIStrDup( s, NULL );
    DLG_SET_RECT( *control, x1, y1, x2, y1 );
    control->parent = NULL;
    control->scroll = GUI_NOSCROLL;
    control->style  = GUI_GROUP | GUI_TAB_GROUP | GUI_AUTOMATIC;
    if( num_radio_buttons > 2 ) {
        if( (gui_controls)[num_controls - 1].control_class == GUI_RADIO_BUTTON ) {
            (gui_controls)[num_controls - 1].style = GUI_NONE | GUI_TAB_GROUP |
                                                     GUI_AUTOMATIC;
        }
    }
    control->id     = id;
}

void set_dlg_check( gui_control_info *gui_controls,
                    int num_controls, const char *s,
                    gui_ctl_id id, gui_ord x1, gui_ord y1, gui_ord x2 )
/*********************************************************************/
{
    gui_control_info    *control;

    control = &gui_controls[num_controls];

    control->control_class = GUI_CHECK_BOX;
    control->text = GUIStrDup( s, NULL );
    DLG_SET_RECT( *control, x1, y1, x2, y1 );
    control->parent = NULL;
    control->scroll = GUI_NOSCROLL;
    control->style  = GUI_TAB_GROUP | GUI_AUTOMATIC;
    control->id     = id;
}

void set_dlg_edit( gui_control_info *gui_controls,
                    int num_controls, const char *s,
                    gui_ctl_id id, gui_ord x1, gui_ord y1, gui_ord x2 )
/*********************************************************************/
{
    int                 text_size;
    gui_control_info    *control;

    control = &gui_controls[num_controls];

    control->control_class = GUI_EDIT;
    control->text = GUIStrDup( s, NULL );
    if( s != NULL && s[0] != '\0' ) {
        text_size = strlen( s );
        DLG_SET_RECT( *control, x1 + text_size + 2, y1, text_size + 3, y1 );
    } else {
        DLG_SET_RECT( *control, x1, y1, x2 - 3, y1 );
    }
    control->parent = NULL;
    control->scroll = GUI_NOSCROLL;
    control->style  = GUI_TAB_GROUP | GUI_CONTROL_BORDER;
    control->id     = id;
}

char *TrimQuote( char *text )
/***************************/
{
    char                *p;

    if( *text == '"' ) {
        ++text;
        for( p = text; *p != '"'; ++p ) ;
        *p = '\0';
    }
    return( text );
}


gui_ctl_id set_dlg_push_button( vhandle var_handle, const char *text,
                        gui_control_info *gui_controls,
                        int num_controls, int row, int num_buttons,
                        int of, int cols, int bwidth )
/*****************************************************************/
{
    int                 but_pos = 0;
    gui_control_info    *control;
    gui_ctl_id          id;
    char                *str;

    control = &gui_controls[num_controls];
    control->control_class = GUI_PUSH_BUTTON;

    str = GUIStrDup( text, NULL );
    switch( *str ) {
    case '0':
        id = CTL_FIRST;
        ++str;
        break;
    case '-':
        id = CTL_PREVIOUS;
        ++str;
        break;
    case '+':
        id = CTL_OK;
        ++str;
        break;
    case '^':
        id = CTL_CANCEL;
        ++str;
        break;
    case '*':
        id = CTL_DONE;
        ++str;
        break;
    case '?':
        id = CTL_HELP;
        ++str;
        break;
    case '>':
        id = CTL_SKIP;
        ++str;
        break;
    default:
        str = TrimQuote( str );
        if( stricmp( str, "Cancel" ) == 0 ) {
            id = CTL_CANCEL;
        } else if( stricmp( str, "Done" ) == 0 ) {
            id = CTL_DONE;
        } else if( stricmp( str, "OK" ) == 0 ) {
            id = CTL_OK;
        } else if( stricmp( str, "Options" ) == 0 ) {
            id = CTL_OPTIONS;
        } else if( stricmp( str, "Help" ) == 0 ) {
            id = CTL_HELP;
        } else {
            id = VarGetId( var_handle );
            if( id == CTL_NULL ) {
                // dummy_var allows control to have an id - used by dynamic visibility feature
                id = VarGetId( MakeDummyVar() );
            }
        }
        break;
    }
    str = TrimQuote( str );
    control->text = str;
    but_pos = BUTTON_POS( num_buttons, of, cols, bwidth );

    DLG_SET_RECT( *control, but_pos, row, but_pos + BW, row );
    control->parent = NULL;
    control->scroll = GUI_NOSCROLL;
    control->style  = GUI_TAB_GROUP | GUI_AUTOMATIC;
    control->id = id;
    if( id == CTL_OK ) {
        control->control_class = GUI_DEFPUSH_BUTTON;
    }
    return( id );
}
