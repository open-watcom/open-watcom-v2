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
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgmem.h"
#include "dbglog.h"
#include "dbgwdlg.h"
#include "wndsys.h"
#include "dbgupdt.h"
#include "dbgwglob.h"
#include "wndmenu.h"


#define MAX_DLG_LINES 30000U

extern int              PageSize( int );

typedef struct dlg_entry {
    struct dlg_entry        *prev;
    struct dlg_entry        *next;
    unsigned                line;
    wnd_attr_wv             wndattr;
    char                    data[1];
} dlg_entry;

static a_window         *WndDlg;
static dlg_entry        *DlgListTop = NULL;
static dlg_entry        *DlgListBot = NULL;
static unsigned         DlgListLineNum = 0;
static unsigned         DlgLines = 0;


static void DlgListPush( const char *buff, size_t len, wnd_attr_wv wndattr )
{
    dlg_entry   *entry;

    entry = DbgMustAlloc( sizeof( dlg_entry ) + len + 1 );
    SET_SYM_NAME_LEN( entry->data, len );
    memcpy( SYM_NAME_NAME( entry->data ), buff, len );
    SYM_NAME_NAME( entry->data )[len] = '\0';
    entry->wndattr = wndattr;
    entry->next = NULL;
    entry->prev = DlgListBot;
    entry->line = DlgListLineNum++;
    if( DlgListBot == NULL ) {
        DlgListTop = entry;
    } else {
        DlgListBot->next = entry;
    }
    DlgListBot = entry;
    DlgLines++;
}


static void DlgListPopLine( void )
{
    dlg_entry  *temp;

    temp = DlgListTop;
    DlgListTop = temp->next;
    DlgListTop->prev = NULL;
    DlgLines--;
    DbgUpdate( UP_DLG_WRITTEN );
    _Free( temp );
}


bool DlgInfoRelease( void )
{
    if( DlgListBot == NULL || DlgListTop == NULL)
        return( false );
    if( (DlgListBot->line - DlgListTop->line) <= 1 )
        return( false );
    DlgListPopLine();
    return( true );
}

static void WndDlgLine( const char *buff, wnd_attr_wv wndattr )
{
    size_t  len;

    len = strlen( buff );
    DlgListPush( buff, len, wndattr );
    while( (DlgListBot->line - DlgListTop->line) > MAX_DLG_LINES ) {
        DlgListPopLine();
    }
}


static bool WndDlgTxtAttr( const char *buff, wnd_attr_wv wndattr )
{
    char        ch, *p;
    bool        multi = false;

    for( ;; ) {
        p = strchr( buff, '\n' );
        if( p == NULL )
            break;
        ch = *p;
        *p = '\0';
        LogLine( buff );
        WndDlgLine( buff, wndattr );
        multi = true;
        *p = ch;
        buff = p + 1;
    }
    if( !multi || buff[0] != '\0' ) {
        LogLine( buff );
        WndDlgLine( buff, wndattr );
    }
    if( WndDlg == NULL )
        return( false );
    DbgUpdate( UP_DLG_WRITTEN );
    return( true );
}


static int DlgNumRows( a_window *wnd )
{
    wnd = wnd;
    return( DlgLines );
}


static void DlgRefresh( a_window *wnd )
{
    if( DlgLines != 0 )
        WndMoveCurrent( wnd, DlgLines - 1, 0 );
    WndNoSelect( wnd );
    WndRepaint( wnd );
}


bool WndDlgTxt( const char *buff )
{
    return( WndDlgTxtAttr( buff, WND_PLAIN ) );
}


static  bool    DlgGetLine( a_window *wnd, int row, int piece, wnd_line_piece *line )
{
    int         i;
    dlg_entry   *curr;

    wnd = wnd;
    curr = DlgListTop;
    if( piece != 0 )
        return( false );
    i = 0;
    while( i < row && curr != NULL ) {
        curr = curr->next;
        ++i;
    }
    if( curr == NULL )
        return( false );
    line->text = SYM_NAME_NAME( curr->data );
    line->attr = curr->wndattr;
    return( true );
}


void WndDlgFini( void )
{
    dlg_entry   *old;

    while( DlgListTop ) {
        old = DlgListTop;
        DlgListTop = DlgListTop->next;
        _Free( old );
    }
}

static bool DlgEventProc( a_window * wnd, gui_event gui_ev, void *parm )
{
    parm=parm;
    switch( gui_ev ) {
    case GUI_NOW_ACTIVE:
        SetLogMenuItems( true );
        break;
    case GUI_NOT_ACTIVE:
        SetLogMenuItems( false );
        break;
    case GUI_INIT_WINDOW:
        WndDlg = wnd;
        SetLogMenuItems( true );
        return( true );
    case GUI_DESTROY :
        WndDlg = NULL;
        SetLogMenuItems( false );
        return( true );
    }
    return( false );
}

wnd_info LogInfo = {
    DlgEventProc,
    DlgRefresh,
    DlgGetLine,
    NoMenuItem,
    NoScroll,
    NoBegPaint,
    NoEndPaint,
    NoModify,
    DlgNumRows,
    NoNextRow,
    NoNotify,
    ChkFlags,
    UP_DLG_WRITTEN,
    NoPopUp
};


a_window *WndDlgOpen( void )
{
    if( WndDlg != NULL )
        WndClose( WndDlg );
    WndDlg = DbgWndCreate( LIT_DUI( WindowLog ), &LogInfo, WND_DIALOGUE, NULL, &LogIcon );
    return( WndDlg );
}
