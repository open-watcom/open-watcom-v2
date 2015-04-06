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
#include "guixutil.h"
#include "guixedit.h"
#include "guisetup.h"
#include "guixdlg.h"
#include "guicontr.h"
#include "guikey.h"
#include "guistr.h"
#include "guiutil.h"
#include "guihotsp.h"
#include <string.h>


bool GUISetEditText( an_edit_control *edit_control, char const *text, bool is_GUI_data )
/*
 * "free" edit_control->buffer and "strdup" text to it.
 * isGUIdata chooses between local and ui functions to allow ui to realloc
 */
{
    void        *(*allocate)( size_t size );
    void        (*dealloc)( void *ptr );
    char const  *filler;
    size_t      fillerLength;
    char        *new;

    if( is_GUI_data ) {
        allocate = GUIMemAlloc;
        dealloc = GUIMemFree;
    }
    else {
        allocate = uimalloc;
        dealloc = uifree;
    }
    filler = ( text == NULL ) ? LIT( Empty ) : text;
    fillerLength = strlen( filler );
    new = allocate( fillerLength + 1 );
    if( new == NULL ) {
        return( false );
    }
    strcpy( new, filler );
    dealloc( edit_control->buffer );
    edit_control->buffer = new;
    edit_control->length = fillerLength;
    return( true );
}

/*
 * GUISetText - set the text in the given control
 */

bool GUISetText( gui_window *wnd, gui_ctl_id id, const char *text )
{
    VFIELD              *field;
    a_dialog            *ui_dlg_info;
    a_combo_box         *combo_box;
    bool                ret;
    an_edit_control     *edit;

    field = GUIGetField( wnd, id );
    if( field == NULL ) {
        return( false );
    }
    ui_dlg_info = GUIGetDialog( wnd );
    edit = NULL;
    switch( field->typ ) {
    case FLD_HOT:
        ret = GUISetHotSpotText( field->u.hs, text );
        break;

    case FLD_CHECK :
    case FLD_RADIO :
    case FLD_TEXT :
        {
            char    **fldtext;
            char    *new_str;

            new_str = GUIStrDup( text, &ret );
            if( ret ) {
                fldtext = NULL;
                switch( field->typ ) {
                case FLD_CHECK: fldtext = &field->u.check->str; break;
                case FLD_RADIO: fldtext = &field->u.radio->str; break;
                case FLD_TEXT: fldtext = &field->u.str; break;
                }
                GUIMemFree( *fldtext );
                *fldtext = new_str;
            }
        }
        break;

    case FLD_EDIT :
    case FLD_INVISIBLE_EDIT :
        edit = (an_edit_control *)field->u.edit;
        break;
    case FLD_COMBOBOX :
        combo_box = field->u.combo;
        edit = &combo_box->edit;
        break;
    default :
        return( false );   /* without redrawing field */
        break;
    }
    if( ( edit != NULL ) && ( ui_dlg_info != NULL ) ) {
        ret = GUISetEditText( edit, text, field->typ != FLD_EDIT );
        if( ret ) {
            uiupdateedit( ui_dlg_info, field );
        }
    }
    if( ret && ( ui_dlg_info != NULL ) ) {
        GUIRefreshControl( wnd, id );
    }
    return( ret );
}

/*
 * GUIClearText - clear the text in the given control
 */

bool GUIClearText( gui_window *wnd, gui_ctl_id id )
{
    return( GUISetText( wnd, id, NULL ) );
}

/* NYI */

bool GUISelectAll( gui_window *wnd, gui_ctl_id id, bool select )
{
    wnd = wnd;
    id = id;
    select = select;
    return( false );
}

