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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "_aui.h"
#include "guidlg.h"
#include "wndregx.h"
#include "dlgrx.rh"
#include "dlgsrch.rh"
#include <string.h>


#define METACHARACTERS  "^$\\.[(|?+*~@"

#define MetaChar( i )   (METACHARACTERS[i - CTL_FIRST_RX])

extern char NullStr[];

bool                    SrchIgnoreCase = true;
char                    SrchMagicChars[MAX_MAGIC_STR + 1] = { METACHARACTERS };
bool                    SrchRX = false;

// This next one is just for the RX processor. It want's it backwards!
char                    SrchIgnoreMagic[MAX_MAGIC_STR + 1] = { "\0" };

static void MoveCursor( gui_window *gui, int edit, int list, int direction )
{
    int         i,size;
    char        *cmd;

    i = -1;
    GUIGetCurrSelect( gui, list, &i );
    size = GUIGetListSize( gui, list );
    if( size == 0 )
        return;
    --size;
    i += direction;
    if( i < 0 )
        i = 0;
    if( i > size )
        i = size;
    GUISetCurrSelect( gui, list, i );
    cmd = GUIGetText( gui, list );
    GUISetText( gui, edit, cmd );
    GUIMemFree( cmd );
    GUISelectAll( gui, edit, true );
}

static void DlgClickHistory( gui_window *gui, int edit, int list )
{
    char        *cmd;

    cmd = GUIGetText( gui, list );
    GUISetText( gui, edit, cmd );
    GUIMemFree( cmd );
}

static void DlgSetHistory( gui_window *gui, void *history, char *cmd, int edit, int list )
{
    int         i;

    GUISetFocus( gui, edit );
    if( !WndPrevFromHistory( history, cmd ) )
        return;
    GUISetText( gui, edit, cmd );
    GUISelectAll( gui, edit, true );
    GUIClearList( gui, list );
    while( WndPrevFromHistory( history, cmd ) ) {
        /* nothing */
    }
    i = -1;
    for( ; WndNextFromHistory( history, cmd ); ) {
        GUIAddText( gui, list, cmd );
        ++i;
    }
    if( i >= 0 ) {
        GUISetCurrSelect( gui, list, i );
    }
}

static bool DlgHistoryKey( gui_window *gui, void *param, int edit, int list )
{
    gui_ctl_id  id;
    gui_key     key;

    GUI_GET_KEY_CONTROL( param, id, key );
    switch( key ) {
    case GUI_KEY_UP:
        MoveCursor( gui, edit, list, -1 );
        return( true );
    case GUI_KEY_DOWN:
        MoveCursor( gui, edit, list, 1 );
        return( true );
    default:
        return( false );
    }
}

static  void    GetRXStatus( gui_window *gui )
{
    char        *magic;
    int         i;

    magic = SrchMagicChars;
    for( i = CTL_FIRST_RX; i <= CTL_LAST_RX; ++i ) {
        if( GUIIsChecked( gui, i ) == GUI_CHECKED ) {
            *magic++ = MetaChar(i);
        }
    }
    *magic = '\0';
}


static  void    SetRXStatus( gui_window *gui )
{
    int i;

    for( i = CTL_FIRST_RX; i <= CTL_LAST_RX; ++i ) {
        GUISetChecked( gui, i, ( strchr( SrchMagicChars, MetaChar(i) ) != NULL ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    }
}


static bool RXGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;

    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        SetRXStatus( gui );
        GUISetFocus( gui, CTL_RX_OK );
        return( true );
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_RX_OK:
            GetRXStatus( gui );
            /* fall through */
        case CTL_RX_CANCEL:
            GUICloseDialog( gui );
            return( true );
        }
        break;
    default :
        break;
    }
    return( false );
}


typedef struct {
    a_window            wnd;
    int                 direction;
    void                *history;
    bool                case_ignore;
    bool                use_rx;
} dlg_search;

static void     GetDlgStatus( gui_window *gui, dlg_search *dlg )
{
    GUIMemFree( dlg->wnd->searchitem );
    dlg->wnd->searchitem = GUIGetText( gui, CTL_SRCH_EDIT );
    if( dlg->wnd->searchitem == NULL )
        dlg->direction = 0;
    dlg->case_ignore = ( GUIIsChecked( gui, CTL_SRCH_CASE ) == GUI_CHECKED );
    dlg->use_rx = ( GUIIsChecked( gui, CTL_SRCH_RX ) == GUI_CHECKED );
    if( dlg->history != NULL ) {
        WndSaveToHistory( dlg->history, dlg->wnd->searchitem );
    }
}

static void     SetDlgStatus( gui_window *gui, dlg_search *dlg )
{
    char        cmd[256];

    dlg->direction = 0;
    GUISetChecked( gui, CTL_SRCH_CASE, ( dlg->case_ignore ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    GUISetChecked( gui, CTL_SRCH_RX, ( dlg->use_rx ) ? GUI_CHECKED : GUI_NOT_CHECKED );
    GUISetText( gui, CTL_SRCH_EDIT, dlg->wnd->searchitem );
    DlgSetHistory( gui, dlg->history, cmd, CTL_SRCH_EDIT, CTL_SRCH_LIST );
}


static bool SrchGUIEventProc( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;
    dlg_search  *dlg;

    dlg = GUIGetExtra( gui );
    switch( gui_ev ) {
    case GUI_INIT_DIALOG:
        SetDlgStatus( gui, dlg );
        GUISetFocus( gui, CTL_SRCH_EDIT );
        return( true );
    case GUI_KEY_CONTROL:
        return( DlgHistoryKey( gui, param, CTL_SRCH_EDIT, CTL_SRCH_LIST ) );
    case GUI_CONTROL_DCLICKED:
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_SRCH_LIST:
            DlgClickHistory( gui, CTL_SRCH_EDIT, CTL_SRCH_LIST );
            if( gui_ev == GUI_CONTROL_CLICKED )
                return( true );
            /* fall through */
        case CTL_SRCH_NEXT:
            dlg->direction = 1;
            GetDlgStatus( gui, dlg );
            GUICloseDialog( gui );
            return( true );
        case CTL_SRCH_EDIT_RX:
            ResDlgOpen( &RXGUIEventProc, NULL, DIALOG_RX );
            return( true );
        case CTL_SRCH_PREV:
            dlg->direction = -1;
            GetDlgStatus( gui, dlg );
            GUICloseDialog( gui );
            return( true );
        case CTL_SRCH_CANCEL:
            dlg->direction = 0;
            GUICloseDialog( gui );
            return( true );
        default :
            break;
        }
        break;
    default :
        break;
    }
    return( false );
}


static int DoDlgSearch( a_window wnd, void *history, bool want_prev )
{
    dlg_search  *dlg;
    int direction;

    dlg = WndMustAlloc( sizeof( *dlg ) );
    dlg->wnd = wnd;
    dlg->direction = 0;
    dlg->case_ignore = SrchIgnoreCase;
    dlg->use_rx = SrchRX;
    dlg->history = history;
    ResDlgOpen( &SrchGUIEventProc, dlg, want_prev ? DIALOG_SEARCH : DIALOG_SEARCH_ALL );
    direction = dlg->direction;
    SrchRX = dlg->use_rx;
    SrchIgnoreCase = dlg->case_ignore;
    if( dlg->use_rx ) {
        WndSetMagicStr( SrchMagicChars );
    } else {
        WndSetMagicStr( "" );
    }
    WndFree( dlg );
    return( direction );
}

int DlgSearch( a_window wnd, void *history )
{
    return( DoDlgSearch( wnd, history, true ) );
}


bool DlgSearchAll( char **expr, void *history )
{
    int         direction;

    direction = DoDlgSearch( WndMain, history, false );
    *expr = WndSrchItem( WndMain );
    return( direction != 0 );
}
