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


#include "dbgdefn.h"
#include "dbgwind.h"
#include "guidlg.h"
#include "dlglist.h"
#include <string.h>

extern char             *TxtBuff;
extern char             *DupStr(char*);
extern bool             AllBrowse( char * );

static void SelectListLast( gui_window *gui )
{
    int         size;

    size = GUIGetListSize( gui, CTL_LIST_LIST );
    if( size != 0 ) {
        GUISetCurrSelect( gui, CTL_LIST_LIST, size-1 );
    }
}

static void AddText( gui_window *gui, char *add )
{
    int         size;
    int         i;
    char        *text;
    int         dup;

    size = GUIGetListSize( gui, CTL_LIST_LIST );
    dup = FALSE;
    for( i = 0; i < size; ++i ) {
        text = GUIGetListItem( gui, CTL_LIST_LIST, i );
        if( text != NULL ) {
            dup = strcmp( add, text ) == 0;
            WndFree( text );
            if( dup ) break;
        }
    }
    if( !dup ) GUIAddText( gui, CTL_LIST_LIST, add );
}

static bool SourceEvent( gui_window * gui, gui_event gui_ev, void * param )
{
    unsigned    id;
    void        *curr;
    int         i;
    int         size;
    char        *text;
    dlg_list    *dlg;

    dlg = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_DESTROY:
        WndFree( dlg->title );
        return( TRUE );
        break;
    case GUI_INIT_DIALOG:
        GUISetWindowText( gui, dlg->title );
        GUIClearList( gui, CTL_LIST_LIST );
        for(curr = dlg->next( NULL ); curr != NULL; curr = dlg->next( curr )) {
            AddText( gui, dlg->name( curr ) );
        }
        GUISetFocus( gui, CTL_LIST_EDIT );
        return( TRUE );
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_LIST_LIST:
            GUIDlgBuffGetText( gui, CTL_LIST_LIST, TxtBuff, TXT_LEN );
            GUISetText( gui, CTL_LIST_EDIT, TxtBuff );
            break;
        case CTL_LIST_DELETE:
            i = GUIGetCurrSelect( gui, CTL_LIST_LIST );
            if( i != -1 ) {
                GUIDeleteItem( gui, CTL_LIST_LIST, i );
            }
            size = GUIGetListSize( gui, CTL_LIST_LIST );
            if( i < size ) {
                GUISetCurrSelect( gui, CTL_LIST_LIST, i );
            } else {
                SelectListLast( gui );
            }
            GUISetFocus( gui, CTL_LIST_LIST );
            GUISetText( gui, CTL_LIST_EDIT, NULL );
            break;
        case CTL_LIST_ADD:
        case CTL_LIST_OK:
            GUIDlgBuffGetText( gui, CTL_LIST_EDIT, TxtBuff, TXT_LEN );
            if( TxtBuff[0] != '\0' ) {
                AddText( gui, TxtBuff );
            }
            SelectListLast( gui );
            GUIClearText( gui, CTL_LIST_EDIT );
            GUISetFocus( gui, CTL_LIST_EDIT );
            if( id == CTL_LIST_ADD ) break;
            dlg->clear();
            size = GUIGetListSize( gui, CTL_LIST_LIST );
            for( i = 0; i < size; ++i ) {
                text = GUIGetListItem( gui, CTL_LIST_LIST, i );
                if( text != NULL ) dlg->add( text );
                WndFree( text );
            }
            /* fall through */
        case CTL_LIST_CANCEL:
            GUICloseDialog( gui );
            break;
        case CTL_LIST_BROWSE:
            GUIDlgBuffGetText( gui, CTL_LIST_EDIT, TxtBuff, TXT_LEN );
            if( !AllBrowse( TxtBuff ) ) return( TRUE );
            GUISetText( gui, CTL_LIST_EDIT, TxtBuff );
            GUISetFocus( gui, CTL_LIST_EDIT );
            return( TRUE );
        }
        return( TRUE );
    default:
        return( FALSE );
    }
}

void DlgList( char *title, void (*clear)(void), void (*add)(char*),
                           void *(*next)(void*), char *(*name)(void*) )
{
    dlg_list dlg;
    dlg.clear = clear;
    dlg.add = add;
    dlg.next = next;
    dlg.name = name;
    dlg.title = DupStr( title );
    ResDlgOpen( &SourceEvent, &dlg, DIALOG_LIST );
}
