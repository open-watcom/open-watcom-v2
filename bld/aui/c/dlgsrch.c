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


#include "auipvt.h"
#include "guidlg.h"
#include "wndregx.h"
#include "dlgrx.h"
#include "dlgsrch.h"
#include <string.h>


extern char NullStr[];

char                    SrchIgnoreCase = TRUE;
char                    SrchMagicChars[MAX_MAGIC_STR] = { "^$\\.[(|?+*~@" };
char                    SrchRX = FALSE;

// This next one is just for the RX processor. It want's it backwards!
char                    SrchIgnoreMagic[MAX_MAGIC_STR] = { "\0" };

#define MetaChar( i ) ("^$\\.[(|?+*~@"[i-CTL_FIRST_RX])

static  void    GetRXStatus( gui_window *gui )
{
    char        *magic;
    int         i;

    magic = SrchMagicChars;
    for( i = CTL_FIRST_RX; i <= CTL_LAST_RX; ++i ) {
        if( GUIIsChecked( gui, i ) ) {
            *magic++ = MetaChar(i);
        }
    }
    *magic = '\0';
}


static  void    SetRXStatus( gui_window *gui )
{
    int i;

    for( i = CTL_FIRST_RX; i <= CTL_LAST_RX; ++i ) {
        GUISetChecked( gui, i, strchr( SrchMagicChars, MetaChar(i) ) != NULL );
    }
}


extern bool RXEvent( gui_window * gui, gui_event event, void * param )
{
    unsigned    id;

    switch( event ) {
    case GUI_INIT_DIALOG:
        SetRXStatus( gui );
        GUISetFocus( gui, CTL_RX_OK );
        break;
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_RX_OK:
            GetRXStatus( gui );
        case CTL_RX_CANCEL:
            GUICloseDialog( gui );
            break;
        }
        return( TRUE );
    default :
        break;
    }
    return( FALSE );
}


typedef struct {
    a_window            *wnd;
    int                 direction;
    void                *history;
    unsigned            case_ignore : 1;
    unsigned            use_rx : 1;
} dlg_search;

extern void DlgClickHistory( gui_window *gui, int edit, int list )
{
    char        *cmd;

    cmd = GUIGetText( gui, list );
    GUISetText( gui, edit, cmd );
    GUIMemFree( cmd );
}

extern void DlgSetHistory( gui_window *gui, void *history, char *cmd,
                           int edit, int list )
{
    int         i;

    GUISetFocus( gui, edit );
    if( !WndPrevFromHistory( history, cmd ) ) return;
    GUISetText( gui, edit, cmd );
    GUISelectAll( gui, edit, TRUE );
    GUIClearList( gui, list );
    while( WndPrevFromHistory( history, cmd ) ) {
        /* nothing */
    }
    i = -1;
    for( ;; ) {
        if( !WndNextFromHistory( history, cmd ) ) break;
        GUIAddText( gui, list, cmd );
        ++i;
    }
    if( i >= 0 ) GUISetCurrSelect( gui, list, i );
}


static void MoveCursor( gui_window *gui, int edit, int list, int direction )
{
    int         i,size;
    char        *cmd;

    i = GUIGetCurrSelect( gui, list );
    size = GUIGetListSize( gui, list );
    if( size == 0 ) return;
    --size;
    i += direction;
    if( i < 0 ) i = 0;
    if( i > size ) i = size;
    GUISetCurrSelect( gui, list, i );
    cmd = GUIGetText( gui, list );
    GUISetText( gui, edit, cmd );
    GUIMemFree( cmd );
    GUISelectAll( gui, edit, TRUE );
}


extern bool DlgHistoryKey( gui_window *gui, void *param, int edit, int list )
{
    unsigned    id;
    gui_key     key;

    GUI_GET_KEY_CONTROL( param, id, key );
    switch( key ) {
    case GUI_KEY_UP:
        MoveCursor( gui, edit, list, -1 );
        return( TRUE );
    case GUI_KEY_DOWN:
        MoveCursor( gui, edit, list, 1 );
        return( TRUE );
    default:
        return( FALSE );
    }
}

static void     GetDlgStatus( gui_window *gui, dlg_search *dlg )
{
    GUIMemFree( dlg->wnd->searchitem );
    dlg->wnd->searchitem = GUIGetText( gui, CTL_SRCH_EDIT );
    if( dlg->wnd->searchitem == NULL ) dlg->direction = 0;
    dlg->case_ignore = GUIIsChecked( gui, CTL_SRCH_CASE );
    dlg->use_rx = GUIIsChecked( gui, CTL_SRCH_RX );
    if( dlg->history != NULL ) {
        WndSaveToHistory( dlg->history, dlg->wnd->searchitem );
    }
}

static void     SetDlgStatus( gui_window *gui, dlg_search *dlg )
{
    char        cmd[256];

    dlg->direction = 0;
    GUISetChecked( gui, CTL_SRCH_CASE, dlg->case_ignore );
    GUISetChecked( gui, CTL_SRCH_RX, dlg->use_rx );
    GUISetText( gui, CTL_SRCH_EDIT, dlg->wnd->searchitem );
    DlgSetHistory( gui, dlg->history, cmd, CTL_SRCH_EDIT, CTL_SRCH_LIST );
}


extern bool SrchEvent( gui_window * gui, gui_event event, void * param )
{
    unsigned    id;
    dlg_search  *dlg;

    dlg = GUIGetExtra( gui );
    switch( event ) {
    case GUI_INIT_DIALOG:
        SetDlgStatus( gui, dlg );
        GUISetFocus( gui, CTL_SRCH_EDIT );
        break;
    case GUI_KEY_CONTROL:
        return( DlgHistoryKey( gui, param, CTL_SRCH_EDIT, CTL_SRCH_LIST ) );
    case GUI_CONTROL_DCLICKED:
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        switch( id ) {
        case CTL_SRCH_LIST:
            DlgClickHistory( gui, CTL_SRCH_EDIT, CTL_SRCH_LIST );
            if( event == GUI_CONTROL_CLICKED ) return( TRUE );
            /* fall through */
        case CTL_SRCH_NEXT:
            dlg->direction = 1;
            GetDlgStatus( gui, dlg );
            GUICloseDialog( gui );
            break;
        case CTL_SRCH_EDIT_RX:
            ResDlgOpen( &RXEvent, NULL, DIALOG_RX );
            break;
        case CTL_SRCH_PREV:
            dlg->direction = -1;
            GetDlgStatus( gui, dlg );
            GUICloseDialog( gui );
            break;
        case CTL_SRCH_CANCEL:
            dlg->direction = 0;
            GUICloseDialog( gui );
            break;
        default :
            break;
        }
        return( TRUE );
    default :
        break;
    }
    return( FALSE );
}


static int DoDlgSearch( a_window *wnd, void *history, bool want_prev )
{
    dlg_search  *dlg;
    int direction;

    dlg = WndMustAlloc( sizeof( *dlg ) );
    dlg->wnd = wnd;
    dlg->direction = 0;
    dlg->case_ignore = SrchIgnoreCase;
    dlg->use_rx = SrchRX;
    dlg->history = history;
    ResDlgOpen( &SrchEvent, dlg, want_prev ? DIALOG_SEARCH : DIALOG_SEARCH_ALL );
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

int DlgSearch( a_window *wnd, void *history )
{
    return( DoDlgSearch( wnd, history, TRUE ) );
}


bool DlgSearchAll( char **expr, void *history )
{
    int         direction;

    direction = DoDlgSearch( WndMain, history, FALSE );
    *expr = WndSrchItem( WndMain );
    return( direction != 0 );
}
