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
#include "guixdlg.h"

/*
 * GUIIsChecked -- find out if the given control is checked
 */

unsigned GUIIsChecked( gui_window *wnd, unsigned id )
{
    VFIELD      *field;
    a_radio     *radio;
    a_check     *check;
    unsigned    ret;

    ret = GUI_NOT_CHECKED;
    field = GUIGetField( wnd, id );
    if( field != NULL ) {
        switch( field->typ ) {
        case FLD_RADIO :
            radio = ( a_radio *)field->ptr;
            if( radio->group->value == ( GUI_FIRST_USER_EVENT + id ) ) {
                ret = GUI_CHECKED;
            }
            break;
        case FLD_CHECK :
            check = ( a_check *)field->ptr;
            if( _checked( check ) ) {
                ret = GUI_CHECKED;
            }
            break;
        }
    }
    return( ret );
}

/*
 * GUISetChecked -- set the given control as checked or not checked
 */

bool GUISetChecked( gui_window * wnd, unsigned id, unsigned checked )
{
    VFIELD      *field;
    a_radio     *radio;
    a_check     *check;
    unsigned    prev_radio;

    prev_radio = 0;
    field = GUIGetField( wnd, id );
    if( field == NULL ) {
        return( false );
    }
    // UI does not support 3 state checkboxes
    if( checked == GUI_INDETERMINANT ) {
        checked = GUI_NOT_CHECKED;
    }
    switch( field->typ ) {
    case FLD_RADIO :
        radio = ( a_radio *)field->ptr;
        prev_radio = radio->group->value - GUI_FIRST_USER_EVENT;
        if( checked ) {
            radio->group->value = GUI_FIRST_USER_EVENT + id;
        } else {
            if( radio->group->value == ( GUI_FIRST_USER_EVENT + id ) ) {
                radio->group->value = -1;
            }
        }
        GUIRefreshControl( wnd, prev_radio );
        break;
    case FLD_CHECK :
        check = ( a_check *)field->ptr;
        check->val = checked;
        break;
    default :
        return( false );
    }
    GUIRefreshControl( wnd, id );
    return( true );
}
