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
#include "guixedit.h"
#include "guixdlg.h"
#include "guilistb.h"
#include "guicontr.h"
#include "guixutil.h"
#include "guiutil.h"

static char *GetText( gui_window *wnd, unsigned id, int choice, bool get_curr )
{
    VFIELD              *field;
    an_edit_control     *edit_control;
    a_list              *list;
    a_combo_box         *combo_box;
    char                *text;

    field = GUIGetField( wnd, id );
    if( field != NULL ) {
        switch( field->typ ) {
        case FLD_CHECK:
            if( GUIStrDup( field->u.check->str, &text ) ) {
                return( text );
            }
            break;

        case FLD_RADIO:
            if( GUIStrDup( field->u.radio->str, &text ) ) {
                return( text );
            }
            break;

        case FLD_HOT:
            if( GUIStrDup( field->u.hs->str, &text ) ) {
                return( text );
            }
            break;

        case FLD_TEXT :
            if( GUIStrDup( field->u.str, &text ) ) {
                return( text );
            }
            break;
        case FLD_EDIT :
        case FLD_INVISIBLE_EDIT :
            edit_control = field->u.edit;
            return( GUIMakeEditCopy( edit_control->buffer, edit_control->length ) );
        case FLD_COMBOBOX :
            if( get_curr ) {
                combo_box = field->u.combo;
                edit_control = &combo_box->edit;
                return( GUIMakeEditCopy( edit_control->buffer, edit_control->length ) );
            }
            /* fall through */
        case FLD_PULLDOWN :
        case FLD_LISTBOX :
        case FLD_EDIT_MLE :
            list = GUIGetList( field );
            if( list != NULL  ){
                return( GUIGetListBoxText( list, choice, get_curr ) );
            }
            break;
        }
    }
    return( NULL );
}

char *GUIGetText( gui_window *wnd, unsigned id )
{
    return( GetText( wnd, id, 0, true ) );
}

char *GUIGetListItem( gui_window *wnd, unsigned id, int choice )
{
    return( GetText( wnd, id, choice, false ) );
}
