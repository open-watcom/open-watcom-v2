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


#include <windows.h>
#include <ctype.h>
#include <string.h>
#include "win1632.h"

#include "wglbl.h"
#include "wribbon.h"
#include "wmain.h"
#include "wmem.h"
#include "wnewitem.h"
#include "wdel.h"
#include "wedit.h"
#include "wctl3d.h"
#include "wsetedit.h"
#include "wmsg.h"
#include "wmsgfile.h"
#include "wstrdup.h"
#include "widn2str.h"
#include "wclip.h"
#include "sys_rc.h"
#include "jdlg.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define WEDIT_PAD 4

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
LRESULT WINEXPORT WStringEditProc ( HWND, UINT, WPARAM, LPARAM );

extern UINT     WClipbdFormat;
extern UINT     WItemClipbdFormat;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static Bool  WInitEditWindow        ( WStringEditInfo * );
static void  WExpandEditWindowItem  ( HWND, int, RECT *, int );
static Bool  WIsCurrentModified     ( WStringEditInfo *einfo, char *text,
                                      uint_16 id, char *symbol );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static DLGPROC     WStringEditWinProc = NULL;
static HBRUSH      WEditWinBrush     = NULL;
static COLORREF    WEditWinColor     = NULL;

int appWidth = -1;
int appHeight = -1;

void WInitEditWindows ( HINSTANCE inst )
{
    _wtouch(inst);

    WEditWinColor = GetSysColor( COLOR_BTNFACE );
    WEditWinBrush = CreateSolidBrush ( WEditWinColor );
    WStringEditWinProc = (DLGPROC)
        MakeProcInstance ( (FARPROC) WStringEditProc, inst );
}

void WFiniEditWindows ( void )
{
    if ( WEditWinBrush ) {
        DeleteObject ( WEditWinBrush );
    }
    FreeProcInstance ( (FARPROC) WStringEditWinProc );
}


Bool WCreateStringEditWindow ( WStringEditInfo *einfo, HINSTANCE inst )
{
    int tabstop;

    einfo->edit_dlg = JCreateDialogParam( inst, "WStringEditDLG", einfo->win,
                                          WStringEditWinProc, (LPARAM) einfo );

    if( einfo->edit_dlg == (HWND) NULL ) {
        return( FALSE );
    }

    tabstop = 105;
    SendDlgItemMessage( einfo->edit_dlg, IDM_STREDLIST, LB_SETTABSTOPS,
                        (WPARAM)1, (LPARAM) &tabstop );

    SetWindowPos( einfo->edit_dlg, (HWND)NULL, 0, WGetRibbonHeight(), 0, 0,
                  SWP_NOSIZE | SWP_NOZORDER );

    return( WInitEditWindow( einfo ) );
}

Bool WResizeStringEditWindow ( WStringEditInfo *einfo, RECT *prect )
{
    int   width, height, ribbon_depth;

    if ( !einfo || !einfo->edit_dlg || !prect  ) {
        return ( FALSE );
    }

    if ( einfo->show_ribbon ) {
        ribbon_depth = WGetRibbonHeight();
    } else {
        ribbon_depth = 0;
    }

    width  = prect->right - prect->left;
    height = prect->bottom - prect->top - ribbon_depth - WGetStatusDepth();

    /* change the size of the listbox */
    WExpandEditWindowItem ( einfo->edit_dlg, IDM_STREDLIST, prect, height );

    /* change the size of the edit field */
    WExpandEditWindowItem ( einfo->edit_dlg, IDM_STREDTEXT, prect, 0 );

    SetWindowPos ( einfo->edit_dlg, (HWND)NULL, 0, ribbon_depth,
                   width, height, SWP_NOZORDER );

    return ( TRUE );
}

void WExpandEditWindowItem ( HWND hDlg, int id, RECT *prect, int height )
{
    HWND        win;
    RECT        crect, t;
    int         new_height;

    /* expand the child window */
    win = GetDlgItem ( hDlg, id );
    GetWindowRect ( win, &crect );
    MapWindowPoints ( (HWND)NULL, hDlg, (POINT *)&crect, 2 );
    t.left   = 0;
    t.top    = 0;
    t.right  = 0;
    t.bottom = WEDIT_PAD;
    MapDialogRect ( hDlg, &t );
    new_height = (height) ? (height - (crect.top - prect->top + t.bottom))
                          : (crect.bottom - crect.top) ;
    SetWindowPos ( win, (HWND) NULL, 0, 0,
                   prect->right - crect.left - t.bottom,
                   new_height, SWP_NOMOVE | SWP_NOZORDER );
    InvalidateRect ( win, NULL, TRUE );

}

Bool WSetEditWindowStringData( WStringEditInfo *einfo, WStringBlock *block,
                               uint_16 string_id )
{
    Bool        ok;
    char        *text;

    text = NULL;

    ok = ( einfo && einfo->edit_dlg && block );

    if( ok ) {
        text = WResIDNameToStr( block->block.String[ string_id & 0xf ] );
        ok = ( text != NULL );
    }

    if( ok ) {
        ok = WSetEditWindowText( einfo->edit_dlg, text );
    }

    if( ok ) {
        ok = WSetEditWindowID( einfo->edit_dlg, string_id,
                               block->symbol[ string_id & 0xf ] );
    }

    if( text ) {
        WMemFree( text );
    }

    return( ok );
}

Bool WGetEditWindowStringData( WStringEditInfo *einfo, char **text,
                               char **symbol, uint_16 *string_id )
{
    Bool        ok;

    ok = ( einfo && einfo->edit_dlg && text && symbol && string_id );

    if( ok ) {
        *text = NULL;
        *symbol = NULL;
        ok = WGetEditWindowText( einfo->edit_dlg, text );
    }

    if( ok ) {
        ok = WGetEditWindowID( einfo->edit_dlg, symbol, string_id,
                               einfo->info->symbol_table,
                               einfo->combo_change );
    }

    if( !ok ) {
        if( text && *text ) {
            WMemFree( *text );
            *text = NULL;
        }
        if( symbol && *symbol ) {
            WMemFree( *symbol );
            *symbol = NULL;
        }
    }

    return( ok );
}

Bool WGetEditWindowStringEntry( WStringEditInfo *einfo, WStringBlock *block,
                                uint_16 string_id )
{
    Bool                ok;
    uint_16             id;
    char                *symbol;
    char                *text;
    char                *oldtext;
    WStringBlock        *new_block;

    text = NULL;
    symbol = NULL;
    oldtext = NULL;

    ok = ( einfo && einfo->tbl && einfo->edit_dlg );

    if( ok ) {
        ok = WGetEditWindowText( einfo->edit_dlg, &text );
    }

    if( ok ) {
        ok = WGetEditWindowID( einfo->edit_dlg, &symbol, &id,
                               einfo->info->symbol_table,
                               einfo->combo_change );
    }

    if ( ok ) {
        new_block = WGetOrMakeStringBlock( einfo->tbl, id );
        ok = ( new_block != NULL );
    }

    /* check if anything was actually modified */
    if ( ok ) {
        ok = TRUE;
        if( ( block == new_block ) && ( id == string_id ) ) {
            oldtext = WResIDNameToStr( block->block.String[ string_id & 0xf ] );
            if( text && oldtext ) {
                ok = ( strcmp( text, oldtext ) != 0 );
            }
        }
    }

    if ( ok ) {
        if( id == string_id ) {
            // text was modified
            WMemFree( block->block.String[ string_id & 0xf ] );
            block->block.String[ string_id & 0xf ] = WResIDNameFromStr(text);
        } else {
            // identifier was modified
            if( block == new_block ) {
            } else {
            }
        }
    }

    if( text != NULL ) {
        WMemFree( text );
    }

    if( oldtext != NULL ) {
        WMemFree( oldtext );
    }

    return ( ok );
}

Bool WSetEditWindowText( HWND dlg, char *text )
{
    char        *t;
    char        *n;
    Bool        ok;

    ok = ( ( dlg != (HWND) NULL ) && ( text != NULL ) );

    if( ok ) {
        t = text;
        if( t == NULL ) {
            t = "";
        }
    }

    if( ok ) {
        n = WConvertStringFrom( t, "\t\n", "tn" );
        if( n ) {
            ok = WSetEditWithStr( GetDlgItem( dlg, IDM_STREDTEXT ), n );
            WMemFree( n );
        } else {
            ok = WSetEditWithStr( GetDlgItem( dlg, IDM_STREDTEXT ),  t );
        }
    }

    return( ok );
}

Bool WGetEditWindowText ( HWND dlg, char **text )
{
    Bool        ok;
    char        *n;

    ok = ( ( dlg != (HWND) NULL ) && ( text != NULL ) );

    if( ok ) {
        n = WGetStrFromEdit( GetDlgItem( dlg, IDM_STREDTEXT ), NULL );
        *text = WConvertStringTo( n, "\t\n", "tn" );
        if( n ) {
            WMemFree( n );
        }
        ok = ( *text != NULL );
    }

    return ( ok );
}

Bool WSetEditWindowID( HWND dlg, uint_16 id, char *symbol )
{
    Bool  ok;

    ok = ( dlg != (HWND) NULL );

    if( ok ) {
        if( symbol ) {
            ok = WSetEditWithStr( GetDlgItem( dlg, IDM_STREDCMDID ), symbol );
        } else {
            ok = WSetEditWithSINT32( GetDlgItem( dlg, IDM_STREDCMDID ),
                                     (int_32) id, 10 );
        }
    }

    if ( ok ) {
        ok = WSetEditWithSINT32( GetDlgItem( dlg, IDM_STREDCMDNUM ),
                                 (int_32) id, 10 );
    }

    return ( ok );
}

Bool WGetEditWindowID( HWND dlg, char **symbol, uint_16 *id,
                       WRHashTable *symbol_table, Bool combo_change )
{
    int_32      val;
    char        *ep;
    WRHashValue hv;
    WRHashEntry *new_entry;
    BOOL        dup;

    if( dlg == (HWND)NULL ) {
        return( FALSE );
    }

    if( combo_change ) {
        *symbol = WGetStrFromComboLBox( GetDlgItem( dlg, IDM_STREDCMDID ), -1 );
    } else {
        *symbol = WGetStrFromEdit( GetDlgItem( dlg, IDM_STREDCMDID ), NULL );
    }

    if( *symbol == NULL ) {
        return( FALSE );
    }

    if( !**symbol ) {
        *symbol = WGetStrFromEdit( GetDlgItem( dlg, IDM_STREDCMDNUM ), NULL );
    }

    if( *symbol == NULL ) {
        return( FALSE );
    }

    strupr( *symbol );

    // check if the string has a numeric representation
    val = (int_32)strtol( *symbol, &ep, 0 );
    if( *ep ) {
        // the string did not have a numeric representation
        // so lets look it up in the hash table
        if( WRLookupName( symbol_table, *symbol, &hv ) ) {
            *id = (uint_16)hv;
        } else {
            dup = FALSE;
            new_entry = WRAddDefHashEntry( symbol_table, *symbol, &dup );
            if( new_entry != NULL ) {
                *id = (uint_16)new_entry->value;
                if( !dup ) {
                    SendDlgItemMessage( dlg, IDM_STREDCMDID, CB_ADDSTRING,
                                        0, (LPARAM)(LPCSTR)new_entry->name );
                    SendDlgItemMessage( dlg, IDM_STREDCMDID, CB_SETITEMDATA,
                                        0, (LPARAM)new_entry );
                }
            } else {
                *id = 0;
                WMemFree( *symbol );
                *symbol = NULL;
                return( FALSE );
            }
        }
    } else {
        // the string did have a numeric representation
        *id = (uint_16)val;
        WMemFree( *symbol );
        *symbol = NULL;
    }

    return( TRUE );
}

Bool WInitEditWindowListBox ( WStringEditInfo *einfo )
{
    Bool         ok;
    HWND         lbox;
    WStringBlock *block;

    ok = ( einfo && einfo->edit_dlg && einfo->tbl );

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_STREDLIST );
        SendMessage( lbox, WM_SETREDRAW, FALSE, 0 );
        SendMessage( lbox, LB_RESETCONTENT, 0, 0 );
        block = einfo->tbl->first_block;
        while( block && ok ) {
            ok = WAddEditWinLBoxBlock( einfo, block, -1 );
            block = block->next;
        }
        SendMessage( lbox, WM_SETREDRAW, TRUE, 0 );
    }

    return ( ok );
}

Bool WInitEditWindow( WStringEditInfo *einfo )
{
    HWND        lbox;
    Bool        ok;
    uint_16     first_id;

    ok = ( einfo && einfo->edit_dlg );

    if ( ok ) {
        ok = WInitEditWindowListBox ( einfo );
    }

    if ( ok ) {
        if ( einfo->tbl->first_block ) {
            ok = WGetFirstStringInBlock( einfo->tbl->first_block, &first_id );
        }
    }

    if ( ok ) {
        if ( einfo->tbl->first_block ) {
            ok = WSetEditWindowStringData( einfo, einfo->tbl->first_block, first_id );
            if ( ok ) {
                lbox = GetDlgItem ( einfo->edit_dlg, IDM_STREDLIST );
                ok = ( SendMessage ( lbox, LB_SETCURSEL, 0, 0 ) != LB_ERR );
                einfo->current_block  = einfo->tbl->first_block;
                einfo->current_string = first_id;
                einfo->current_pos    = 0;
            }
        }
    }

    return ( ok );
}

Bool WIsCurrentModified( WStringEditInfo *einfo, char *text, uint_16 id,
                         char *symbol )
{
    char        *current_text;
    char        *s;
    Bool        mod;

    mod = ( einfo && einfo->current_block && text );

    if( mod ) {
        current_text = WResIDNameToStr( einfo->current_block->block.String[ einfo->current_string & 0xf ] );
    }

    if( mod ) {
        // make sure the symbol info did not change
        s = einfo->current_block->symbol[ id & 0xf ];
        mod = ( !s && symbol ) || ( s && !symbol );
        if( !mod ) {
            mod = symbol && stricmp( s, symbol );
            if( !mod ) {
                mod = ( ( id != einfo->current_string ) ||
                        ( strcmp( text, current_text ) ) );
            }
        }
    }

    if( current_text ) {
        WMemFree( current_text );
    }

    return( mod );
}

void WResetEditWindow( WStringEditInfo *einfo )
{
    if( einfo ) {
        WSetEditWithStr( GetDlgItem(einfo->edit_dlg, IDM_STREDTEXT), "" );
        WSetEditWithStr( GetDlgItem(einfo->edit_dlg, IDM_STREDCMDID), "" );
        WSetEditWithStr( GetDlgItem(einfo->edit_dlg, IDM_STREDCMDNUM), "" );
    }
}

Bool WPasteStringItem( WStringEditInfo *einfo )
{
    char                *text;
    char                *symbol;
    WRHashValueList     *vlist;
    uint_32             len;
    uint_16             id;
    Bool                ok;

    text = NULL;
    symbol = NULL;
    ok = ( einfo && einfo->tbl );

    if( ok ) {
        ok = WGetClipData( einfo->win, CF_TEXT, &text, &len );
    }

    if( ok ) {
        id = WFindLargestStringID( einfo->tbl ) + 1;
    }

    if( ok ) {
        ok = WSetEditWindowText( einfo->edit_dlg, text );
    }

    if( ok ) {
        vlist = WRLookupValue( einfo->info->symbol_table, id );
        if( vlist ) {
            if( vlist->next == NULL ) {
                symbol = WStrDup( vlist->entry->name );
            }
            WRValueListFree( vlist );
        }
    }

    if( ok ) {
        ok = WSetEditWindowID( einfo->edit_dlg, id, symbol );
    }

    if( ok ) {
        ok = WInsertStringEntry( einfo );
    }

    if( text != NULL ) {
        WMemFree( text );
    }

    return( ok );
}

Bool WClipStringItem( WStringEditInfo *einfo, Bool cut )
{
    HWND                lbox;
    LRESULT             pos;
    WStringBlock        *block;
    uint_16             id;
    char                *text;
    uint_32             len;
    Bool                ok;

    block = NULL;
    text = NULL;
    id = -1;

    ok = ( einfo && einfo->tbl );

    if( ok ) {
        lbox = GetDlgItem ( einfo->edit_dlg, IDM_STREDLIST );
        ok = ( lbox != (HWND) NULL );
    }

    if( ok ) {
        pos = SendMessage ( lbox, LB_GETCURSEL, 0, 0 );
        ok = ( pos != LB_ERR );
    }

    if( ok ) {
        id = (uint_16 )(void *)
            SendMessage ( lbox, LB_GETITEMDATA, (WPARAM) pos, 0 );
        block = WFindStringBlock( einfo->tbl, id );
        ok = ( block != NULL );
    }

    if ( ok ) {
        text = WResIDNameToStr( block->block.String[ id & 0xf ] );
        ok = ( text != NULL );
    }

    if( ok ) {
        len = strlen( text ) + 1;
        ok = WCopyClipData( einfo->win, CF_TEXT, text, len );
    }

    if( ok ) {
        if( cut ) {
            ok = WDeleteStringEntry( einfo );
        }
    }

    if( text ) {
        WMemFree( text );
    }

    return( ok );
}

static Bool WQueryChangeEntry( WStringEditInfo *einfo )
{
    int         ret;
    UINT        style;
    char        *title;
    char        *text;

    style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
    title = WCreateEditTitle( einfo );
    text = WAllocRCString( W_CHANGEMODIFIEDMENUITEM );

    ret = MessageBox( einfo->edit_dlg, text, title, style );

    if( text ) {
        WFreeRCString( text );
    }
    if( title ) {
        WMemFree( title );
    }

    if( ret == IDYES ) {
        return( TRUE );
    }

    return( FALSE );
}

void WDoHandleSelChange( WStringEditInfo *einfo, Bool change, Bool reset )
{
    HWND                lbox;
    int                 pos;
    char                *text;
    char                *symbol;
    uint_16             id;
    WStringBlock        *block;
    Bool                mod;
    Bool                ok;
    Bool                bdel;
    Bool                replace;

    mod  = FALSE;
    block = NULL;
    text = NULL;
    symbol = NULL;
    id = -1;
    pos = -1;
    ok = ( einfo != NULL );

    if( ok ) {
        lbox = GetDlgItem ( einfo->edit_dlg, IDM_STREDLIST );
        ok = ( lbox != (HWND) NULL );
    }

    if( ok ) {
        if( einfo->current_block && !reset ) {
            mod = WGetEditWindowStringData( einfo, &text, &symbol, &id );
            if( mod ) {
                mod = WIsCurrentModified( einfo, text, id, symbol );
            }
        }
        if( mod && ( einfo->current_pos != -1 ) ) {
            if( change || WQueryChangeEntry( einfo ) ) {
                einfo->info->modified = TRUE;
                pos = einfo->current_pos;
                SendMessage( lbox, LB_DELETESTRING, pos, 0 );
                if( WDeleteStringData( einfo, einfo->current_block,
                                       einfo->current_string, &bdel ) ) {
                    block = WInsertStringData( einfo, id, text, symbol, &replace );
                    if( block ) {
                        pos = WFindStringPos( einfo->tbl, id );
                        if( pos != -1 ) {
                            WAddEditWinLBoxEntry( einfo, block, id, pos );
                        }
                    }
                }
            }
        }
    }

    if( !block || ( id == (uint_16)-1 ) || ( pos == -1 ) ) {
        pos = (int) SendMessage( lbox, LB_GETCURSEL, 0, 0 );
        if( pos != LB_ERR ) {
            id = (uint_16) SendMessage( lbox, LB_GETITEMDATA, (WPARAM) pos, 0 );
            block = WFindStringBlock( einfo->tbl, id );
            if( block == NULL ) {
                return;
            }
        } else {
            block = NULL;
            id = -1;
        }
    }

    if( block ) {
        if( change ) {
            WSetEditWindowID( einfo->edit_dlg, id,
                              block->symbol[ id & 0xf ] );
        } else {
            WSetEditWindowStringData( einfo, block, id );
        }
    }

    if( ( einfo->current_block != block ) ||
        ( einfo->current_string != id ) ||
        ( einfo->current_pos != pos ) ) {
        einfo->current_block  = block;
        einfo->current_string = id;
        einfo->current_pos    = (pos == LB_ERR) ? -1 : pos;
        if( pos != LB_ERR ) {
            SendMessage( lbox, LB_SETCURSEL, (WPARAM) pos, 0 );
        }
    }

    if( symbol ) {
        WMemFree( symbol );
    }

    if( text ) {
        WMemFree( text );
    }
}

void WHandleSelChange( WStringEditInfo *einfo )
{
    WDoHandleSelChange( einfo, FALSE, FALSE );
}

LRESULT WINEXPORT WStringEditProc ( HWND hDlg, UINT message,
                                    WPARAM wParam, LPARAM lParam )
{
    WStringEditInfo     *einfo;
    LRESULT             ret;
    WORD                wp;
    WORD                cmd;

    ret   = FALSE;
    einfo = (WStringEditInfo *) GetWindowLong ( hDlg, DWL_USER );

    switch ( message ) {
        case WM_INITDIALOG:
            einfo = (WStringEditInfo *) lParam;
            einfo->edit_dlg = hDlg;
            SetWindowLong ( hDlg, DWL_USER, (LONG) einfo );
            WRAddSymbolsToComboBox( einfo->info->symbol_table, hDlg,
                                    IDM_STREDCMDID, WR_HASHENTRY_ALL );
            ret = TRUE;
            break;

        case WM_SYSCOLORCHANGE:
            WCtl3dColorChange ();
            break;

#if 0
#ifdef __NT__
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
#else
        case WM_CTLCOLOR:
#endif
            return( (LRESULT)WCtl3dCtlColorEx( message, wParam, lParam ) );
#endif

        #if 0
        case WM_PARENTNOTIFY:
        {
            HWND                win;
            RECT                r;
            POINT               p;
            cmd = GET_WM_PARENTNOTIFY_EVENT(wParam,lParam);
            switch( cmd ) {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
                MAKE_POINT( p, lParam );
                win = GetDlgItem ( hDlg, IDM_STREDLIST );
                GetWindowRect( win, &r );
                MapWindowPoints( HWND_DESKTOP, hDlg, (POINT *)&r, 2 );
                if( PtInRect( &r, p ) ) {
                    WHandleSelChange( einfo );
                }
                break;
            }
            ret = TRUE;
            break;
        }
        #endif

        case WM_COMMAND:
            wp = LOWORD(wParam);
            cmd = GET_WM_COMMAND_CMD(wParam,lParam);
            switch( wp ) {
                case IDM_STREDINSERT:
                    WInsertStringEntry( einfo );
                    break;
                case IDM_STREDCHANGE:
                    WDoHandleSelChange( einfo, TRUE, FALSE );
                    break;
                case IDM_STREDRESET:
                    WDoHandleSelChange( einfo, FALSE, TRUE );
                    break;
                case IDM_STREDSETSTR:
                    if( einfo->tbl && einfo->tbl->first_block ) {
                        WHandleSelChange( einfo );
                    } else {
                        WInsertStringEntry( einfo );
                    }
                    break;
                #if 0
                case IDM_STREDCMDID:
                    if( cmd == CBN_SELCHANGE ) {
                        einfo->combo_change = TRUE;
                        WHandleSelChange( einfo );
                        einfo->combo_change = FALSE;
                    }
                    break;
                #endif
                case IDM_STREDLIST:
                    if( cmd == LBN_SELCHANGE ) {
                        WHandleSelChange( einfo );
                    }
                    break;
            }
            break;
    }

    return( ret );
}

LRESULT WINEXPORT WTestProc( HWND hDlg, UINT message,
                             WPARAM wParam, LPARAM lParam )
{
    RECT        r;

    _wtouch(wParam);
    _wtouch(lParam);

    if( message == WM_INITDIALOG ) {
        GetWindowRect( hDlg, &r );
        appWidth = r.right - r.left + 10;
        appHeight = r.bottom - r.top + 85;
        DestroyWindow( hDlg );
        return( TRUE );
    }

    return( FALSE );
}

void WInitEditDlg( HINSTANCE inst, HWND parent )
{
    FARPROC     lpProc;

    lpProc = MakeProcInstance( (FARPROC) WTestProc, inst );
    JCreateDialog( inst, "WStringEditDLG", parent, (DLGPROC) lpProc );
    FreeProcInstance( lpProc );

    return;
}

