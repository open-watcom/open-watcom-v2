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
#include <string.h>
#include "guixutil.h"
#include "guicutil.h"
#include "guiscale.h"
#include "guicontr.h"

/*
 * GUIControlSetRedraw -- set the redraw flag for a given window control
 */
bool GUIControlSetRedraw( gui_window *wnd, unsigned control, bool redraw )
{
    HWND        hwnd;

    hwnd = _wpi_getdlgitem( wnd->hwnd, control );
    _wpi_setredraw( hwnd, ( redraw ) ? TRUE : FALSE );

    return( true );
}

/*
 * GUIAddText -- add the text to the given list/combo box
 */

bool GUIAddText( gui_window *wnd, unsigned id, char *text )
{
    int                 pos;
#ifdef __OS2_PM__
    control_item        *item;
#endif

    if( text != NULL ) {
        pos = LIT_END;
#ifdef __OS2_PM__
        item = GUIGetControlByID( wnd, id );
        if( item != NULL ) {
            if( item->style & GUI_CONTROL_SORTED ) {
                pos = LIT_SORTASCENDING;
            }
        }
#endif
        GUIToComboList( wnd, id, LB_ADDSTRING, CB_ADDSTRING, (WPI_PARAM1)pos,
                        (WPI_PARAM2)(LPSTR)text, (WPI_MRESULT)NULL );
        return( true );
    }
    return( false );
}

bool GUIAddTextList( gui_window *wnd, unsigned id, int items, const void *data_handle, PICKGETTEXT *getstring )
{
    int     item;

    GUIControlSetRedraw( wnd, id, false );
    for( item = 0; item < items; item++ ) {
        GUIAddText( wnd, id, (*getstring)( data_handle, item ) );
    }
    GUIControlSetRedraw( wnd, id, true );
    GUIControlDirty( wnd, id );
    return( true );
}

/*
 * GUIInsertText -- insert the text to the given list/combo box at position
 *                  indicated
 */

bool GUIInsertText( gui_window *wnd, unsigned id, int choice, char *text )
{
    if( text != NULL ) {
        GUIToComboList( wnd, id, LB_INSERTSTRING, CB_INSERTSTRING,
                        (WPI_PARAM1) choice, (WPI_PARAM2)(LPSTR)text, (WPI_MRESULT)NULL );
        return( true );
    }
    return( false );
}

/*
 * GUISetTopIndex -- change which item is at the top of the list/combo box
 */

bool GUISetTopIndex( gui_window *wnd, unsigned id, int choice )
{
    GUIToComboList( wnd, id, LB_SETTOPINDEX, LB_SETTOPINDEX,
                    (WPI_PARAM1)choice, (WPI_PARAM2)NULL, (WPI_MRESULT)NULL );
    return( true );
}

/*
 * GUIGetTopIndex -- get index of item which is at the top of the list/combo
 *                   box
 */

int GUIGetTopIndex( gui_window *wnd, unsigned id )
{
    return( (int) GUIToComboList( wnd, id, LB_GETTOPINDEX, LB_GETTOPINDEX,
                                  (WPI_PARAM1)NULL, (WPI_PARAM2)NULL,
                                  (WPI_MRESULT)-1 ) );
}

/*
 * GUISetHorizontalExtent -- set the width of items in the list box
 */

bool GUISetHorizontalExtent( gui_window *wnd, unsigned id, int extent )
{
#ifndef __OS2_PM__
    gui_coord   coord;

    coord.x = extent;
    GUIScaleToScreenR( &coord );
    GUIToComboList( wnd, id, LB_SETHORIZONTALEXTENT, LB_SETHORIZONTALEXTENT,
                    coord.x, (WPI_PARAM2)NULL, (WPI_MRESULT)NULL );
    return( true );
#else
    wnd = wnd;
    id = id;
    extent = extent;
    return( false );
#endif
}

/*
 *  GUIGetListSize
 */

int GUIGetListSize( gui_window *wnd, unsigned id )
{
    return( (int) GUIToComboList( wnd, id, LB_GETCOUNT, CB_GETCOUNT,
                                  (WPI_PARAM1)NULL, (WPI_PARAM2)NULL,
                                  (WPI_MRESULT)NULL ) );
}

bool GUIDeleteItem( gui_window *wnd, unsigned id, int choice )
{
    if( ( choice >= 0 ) && ( choice < GUIGetListSize( wnd, id ) ) ) {
        GUIToComboList( wnd, id, LB_DELETESTRING, CB_DELETESTRING,
                        (WPI_PARAM1)choice, (WPI_PARAM2)NULL,
                        (WPI_MRESULT)NULL );
        return( true );
    }
    return( false );
}

/*
 * GUIClearList -- clear the contents of the list/combo box
 */

bool GUIClearList( gui_window *wnd, unsigned id )
{
    GUIToComboList( wnd, id, LB_RESETCONTENT, CB_RESETCONTENT,
                    (WPI_PARAM1)NULL, (WPI_PARAM2)NULL,
                    (WPI_MRESULT)NULL );
    return( true );
}

/*
 * GUISetText - Set the text of the given control to the given text
 */

bool GUISetText( gui_window *wnd, unsigned control, const char *text )
{
    char                *new_text;
    gui_control_class   control_class;

    if( !GUIGetControlClass( wnd, control, &control_class ) ) {
        return( false );
    }
    if( control_class != GUI_EDIT ) {
        new_text = _wpi_menutext2pm( text );
        _wpi_setdlgitemtext( wnd->hwnd, control, new_text );
        if( new_text ) {
            _wpi_freemenutext( new_text );
        }
    } else {
        _wpi_setdlgitemtext( wnd->hwnd, control, text );
    }
    return( true );
}

/*
 * GUIGetText -- get the text of the given control
 */

char *GUIGetText( gui_window *wnd, unsigned control )
{
    LONG                length;
    char                *text;
    gui_control_class   control_class;
    HWND                hwnd;
    int                 choice;

    if( !GUIGetControlClass( wnd, control, &control_class ) ) {
        return( NULL );
    }
    switch( control_class ) {
    case GUI_LISTBOX :
        choice = GUIGetCurrSelect( wnd, control );
        if( choice == NO_SELECT ) {
            return( NULL );
        }
        return( GUIGetListItem( wnd, control, choice ) );
    default :
        hwnd = _wpi_getdlgitem( wnd->hwnd, control );
        if( hwnd == NULLHANDLE ) {
            return( NULL );
        }
        length = _wpi_getwindowtextlength( hwnd );
        if( length > 0 ) {
            text = (char *)GUIMemAlloc( length + 1 );
            if( text != NULL ) {
                _wpi_getwindowtext( hwnd, (LPSTR)text, length + 1 );
                switch( control_class ) {
                case GUI_PUSH_BUTTON:
                case GUI_DEFPUSH_BUTTON:
                case GUI_RADIO_BUTTON:
                case GUI_CHECK_BOX:
                case GUI_STATIC:
                case GUI_GROUPBOX:
                    _wpi_menutext2win( text );
                    break;
                }
            }
            return( text );
        }
        return( NULL );
    }
}

/*
 *  GUIGetCurrSelect -- return the index of the currently selected item
 *                      in the given list/combo box
 */

int GUIGetCurrSelect( gui_window *wnd, unsigned id )
{
    int             sel;

    sel = (int) GUIToComboList( wnd, id, LB_GETCURSEL, CB_GETCURSEL,
                                (WPI_PARAM1)NULL, (WPI_PARAM2)NULL,
                                (WPI_MRESULT)NO_SELECT );
    if( ( sel == LB_ERR ) || ( sel == CB_ERR ) ) {
        sel = NO_SELECT;
    }
    return( sel );
}

/*
 *  GUISetCurrSelect -- if the combo/list box contains enough items, set the
 *                      current selection to the indicated choice
 */

bool GUISetCurrSelect( gui_window * wnd, unsigned id, int choice )
{
    GUIToComboList( wnd, id, LB_SETCURSEL, CB_SETCURSEL,
                    (WPI_PARAM1)choice,
#ifndef __OS2_PM__
                    (WPI_PARAM2)false,
#else
                    (WPI_PARAM2)true,
#endif
                    (WPI_MRESULT)NO_SELECT );
    return( true );
}

char *GUIGetListItem( gui_window *wnd, unsigned id, int choice )
{
    int         length;
    char        *text;
    WPI_PARAM1  p1;

    length = (int) GUIToComboList( wnd, id, LB_GETTEXTLEN, CB_GETLBTEXTLEN,
                                   (WPI_PARAM1)choice,
                                   (WPI_PARAM2)NULL, (WPI_MRESULT)NULL );
    if( length > 0 ) {
        text = (char *)GUIMemAlloc( length + 1 );
#ifdef __OS2_PM__
            p1 = MPFROM2SHORT(choice, length+1 );
#else
            p1 = choice;
#endif
        if( text != NULL ) {
            GUIToComboList( wnd, id, LB_GETTEXT , CB_GETLBTEXT,
                            p1, (WPI_PARAM2)text, (WPI_MRESULT)NULL );
        }
    } else {
        text = NULL;
    }
    return( text );
}

bool GUISetListItemData( gui_window *wnd, unsigned id, unsigned choice, void *data )
{
    WPI_MRESULT ret;

    ret = GUIToComboList( wnd, id, LB_SETITEMDATA, CB_SETITEMDATA, (WPI_PARAM1)choice,
                          (WPI_PARAM2)data, (WPI_MRESULT)false );
#ifndef __OS2_PM__
    if( ( ret == LB_ERR ) || ( ret == CB_ERR ) ) {
        return( false );
    }
    return( true );
#else
    return( (bool)ret );
#endif
}

void *GUIGetListItemData( gui_window *wnd, unsigned id, unsigned choice )
{
    return( (void *)GUIToComboList( wnd, id, LB_GETITEMDATA, CB_GETITEMDATA,
                                    (WPI_PARAM1)choice, (WPI_PARAM2)NULL,
                                    (WPI_MRESULT)NULL ) );
}
