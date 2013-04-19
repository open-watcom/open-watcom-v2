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


#include "precomp.h"
#include <ctype.h>
#include <string.h>

#include "watcom.h"
#include "wglbl.h"
#include "wribbon.h"
#include "wmain.h"
#include "wmsg.h"
#include "wstat.h"
#include "wmem.h"
#include "wnewitem.h"
#include "wdel.h"
#include "wvk2str.h"
#include "wedit.h"
#include "wctl3d.h"
#include "wsetedit.h"
#include "wclip.h"
#include "sys_rc.h"
#include "rcstr.gh"
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
WINEXPORT BOOL CALLBACK WAcccelEditProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT BOOL CALLBACK WTestProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static Bool WInitEditWindow( WAccelEditInfo * );
static void WExpandEditWindowItem( HWND, int, RECT * );

static Bool WSetEditWindowKey( HWND, uint_16, uint_16 );
static Bool WSetEditWindowID( HWND, char *, uint_16 );
static Bool WSetEditWindowFlags( HWND, uint_16 );

static Bool WGetEditWindowKey( HWND, uint_16 *, uint_16 *, Bool * );
static Bool WGetEditWindowID( HWND dlg, char **symbol, uint_16 *id, WRHashTable *symbol_table, Bool combo_change );
static Bool WGetEditWindowFlags( HWND, uint_16 * );

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/
extern UINT             WClipbdFormat;
extern UINT             WItemClipbdFormat;
extern WAccelEntry      DefaultEntry = { FALSE, { ACCEL_ASCII, 'A', 101 }, NULL, NULL, NULL };

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static DLGPROC  WAccelEditWinProc = NULL;
static HBRUSH   WEditWinBrush     = NULL;
static COLORREF WEditWinColor     = 0;

int appWidth = -1;
int appHeight = -1;

void WInitEditWindows( HINSTANCE inst )
{
    _wtouch( inst );

    WEditWinColor = GetSysColor( COLOR_BTNFACE );
    WEditWinBrush = CreateSolidBrush( WEditWinColor );
    WAccelEditWinProc = (DLGPROC)MakeProcInstance( (FARPROC)WAcccelEditProc, inst );
}

void WFiniEditWindows( void )
{
    if( WEditWinBrush != NULL ) {
        DeleteObject( WEditWinBrush );
    }
    FreeProcInstance( (FARPROC)WAccelEditWinProc );
}


Bool WCreateAccelEditWindow( WAccelEditInfo *einfo, HINSTANCE inst )
{
    int tabstop;

    einfo->edit_dlg = JCreateDialogParam( inst, "WAccelEditDLG", einfo->win,
                                          WAccelEditWinProc, (LPARAM)einfo );

    if( einfo->edit_dlg == (HWND)NULL ) {
        return( FALSE );
    }

    tabstop = 85;
    SendDlgItemMessage( einfo->edit_dlg, IDM_ACCEDLIST, LB_SETTABSTOPS,
                        (WPARAM)1, (LPARAM)&tabstop );

    SetWindowPos( einfo->edit_dlg, (HWND)NULL, 0, WGetRibbonHeight(), 0, 0,
                  SWP_NOSIZE | SWP_NOZORDER );

    return( WInitEditWindow( einfo ) );
}

Bool WResizeAccelEditWindow( WAccelEditInfo *einfo, RECT *prect )
{
    int   width, height, ribbon_depth;
    HWND  win;
    RECT  crect;

    if( einfo == NULL || einfo->edit_dlg == NULL || prect == NULL ) {
        return( FALSE );
    }

    if( einfo->show_ribbon ) {
        ribbon_depth = WGetRibbonHeight();
    } else {
        ribbon_depth = 0;
    }

    width = prect->right - prect->left;
    height = prect->bottom - prect->top - ribbon_depth - WGetStatusDepth();

    /* change the size of the divider */
    win = GetDlgItem( einfo->edit_dlg, IDM_ACCEDBLACKLINE );
    GetWindowRect( win, &crect );
    SetWindowPos( win, (HWND)NULL, 0, 0, width, crect.bottom - crect.top,
                  SWP_NOMOVE | SWP_NOZORDER );

    /* change the size of the resource name edit field */
    WExpandEditWindowItem( einfo->edit_dlg, IDM_ACCEDRNAME, prect );

    /* change the size of the listbox */
    WExpandEditWindowItem( einfo->edit_dlg, IDM_ACCEDLIST, prect );

    SetWindowPos( einfo->edit_dlg, (HWND)NULL, 0, ribbon_depth,
                  width, height, SWP_NOZORDER );

    return( TRUE );
}

static void WExpandEditWindowItem( HWND hDlg, int id, RECT *prect )
{
    HWND    win;
    RECT    crect, t;

    /* expand the child window */
    win = GetDlgItem( hDlg, id );
    GetWindowRect( win, &crect );
    MapWindowPoints( (HWND)NULL, hDlg, (POINT *)&crect, 2 );
    t.left = 0;
    t.top = 0;
    t.right = 0;
    t.bottom = WEDIT_PAD;
    MapDialogRect( hDlg, &t );
    SetWindowPos( win, (HWND) NULL, 0, 0, prect->right - crect.left - t.bottom,
                  crect.bottom - crect.top, SWP_NOMOVE | SWP_NOZORDER );
    InvalidateRect( win, NULL, TRUE );
}

void WResetEditWindow( WAccelEditInfo *einfo )
{
    if( einfo != NULL ) {
        WSetEditWithStr( GetDlgItem( einfo->edit_dlg, IDM_ACCEDKEY ), "" );
        WSetEditWithStr( GetDlgItem( einfo->edit_dlg, IDM_ACCEDCMDID ), "" );
        WSetEditWithStr( GetDlgItem( einfo->edit_dlg, IDM_ACCEDCMDNUM ), "" );
        CheckDlgButton( einfo->edit_dlg, IDM_ACCEDVIRT, FALSE );
        CheckDlgButton( einfo->edit_dlg, IDM_ACCEDASCII, TRUE );
        CheckDlgButton( einfo->edit_dlg, IDM_ACCEDCNTL, FALSE );
        CheckDlgButton( einfo->edit_dlg, IDM_ACCEDSHFT, FALSE );
        CheckDlgButton( einfo->edit_dlg, IDM_ACCEDALT, FALSE );
        CheckDlgButton( einfo->edit_dlg, IDM_ACCEDFLASH, FALSE );
    }
}

Bool WSetEditWindowKeyEntry( WAccelEditInfo *einfo, WAccelEntry *entry )
{
    Bool    ok;
    uint_16 key, flags, id;

    ok = (einfo != NULL && einfo->edit_dlg != NULL && entry != NULL);

    if( ok ) {
        if( entry->is32bit ) {
            key = entry->entry32.Ascii;
            flags = entry->entry32.Flags;
            id = entry->entry32.Id;
        } else {
            key = entry->entry.Ascii;
            flags = entry->entry.Flags;
            id = (uint_16)entry->entry.Id;
        }
        ok = WSetEditWindowKey( einfo->edit_dlg, key, flags );
        if( !ok ) {
            WSetStatusByID( einfo->wsb, -1, W_INVALIDACCEL );
            memcpy( entry, &DefaultEntry, sizeof( WAccelEntry ) );
            key = entry->entry.Ascii;
            flags = entry->entry.Flags;
            id = (uint_16)entry->entry.Id;
            ok = WSetEditWindowKey( einfo->edit_dlg, key, flags );
        }
    }

    if( ok ) {
        ok = WSetEditWindowID( einfo->edit_dlg, entry->symbol, id );
    }

    if( ok ) {
        ok = WSetEditWindowFlags( einfo->edit_dlg, flags );
    }

    return( ok );
}

Bool WGetEditWindowKeyEntry( WAccelEditInfo *einfo, WAccelEntry *entry,
                             Bool check_mod )
{
    Bool        ok;
    Bool        force_ascii;
    uint_16     key, flags, id;
    char        *symbol;

    symbol = NULL;
    flags = 0;
    force_ascii = FALSE;

    ok = (einfo != NULL && einfo->edit_dlg != NULL && entry != NULL);

    if( ok ) {
        ok = WGetEditWindowFlags( einfo->edit_dlg, &flags );
    }

    if( ok ) {
        ok = WGetEditWindowKey( einfo->edit_dlg, &key, &flags, &force_ascii );
        if( !ok ) {
            WSetStatusByID( einfo->wsb, -1, W_INVALIDACCELKEY );
        }
    }

    if( ok ) {
        ok = WGetEditWindowID( einfo->edit_dlg, &symbol, &id,
                               einfo->info->symbol_table, einfo->combo_change );
    }

    if( ok ) {
        if( force_ascii ) {
            flags &= ~(ACCEL_VIRTKEY | ACCEL_SHIFT | ACCEL_CONTROL | ACCEL_ALT);
        }
    }

    /* check if anything was actually modified */
    if( ok ) {
        // make sure the symbol info did not change
        ok = (entry->symbol == NULL && symbol != NULL) ||
             (entry->symbol != NULL && symbol == NULL);
        if( !ok ) {
            ok = (symbol != NULL && stricmp( entry->symbol, symbol ));
            if( !ok ) {
                if( entry->is32bit ) {
                    ok = (entry->entry32.Ascii != key || entry->entry32.Flags != flags ||
                          entry->entry32.Id != id);
                } else {
                    ok = (entry->entry.Ascii != key || entry->entry.Flags != (uint_8)flags ||
                          entry->entry.Id != id);
                }
            }
        }
        if( check_mod ) {
            return( ok );
        }
    }

    if( ok ) {
        if( entry->is32bit ) {
            entry->entry32.Ascii = key;
            entry->entry32.Flags = flags;
            entry->entry32.Id = id;
        } else {
            entry->entry.Ascii = key;
            entry->entry.Flags = (uint_8)flags;
            entry->entry.Id = id;
        }
        if( entry->symbol != NULL ) {
            WMemFree( entry->symbol );
        }
        entry->symbol = symbol;
    } else {
        if( symbol != NULL ) {
            WMemFree( symbol );
        }
    }

    return( ok );
}

static Bool WSetEditWindowKey( HWND dlg, uint_16 key, uint_16 flags )
{
    char        *text;
    HWND        edit;
    Bool        ok;

    ok = (dlg != (HWND)NULL);

    if( ok ) {
        text = WGetKeyText( key, flags );
        ok = (text != NULL);
    }

    if( ok ) {
        edit = GetDlgItem( dlg, IDM_ACCEDKEY );
        ok = WSetEditWithStr( edit, text );
    }

    return( ok );
}

Bool WGetEditWindowKey( HWND dlg, uint_16 *key, uint_16 *flags, Bool *force_ascii )
{
    Bool    ok;
    char    *text;

    text = NULL;

    ok = (dlg != (HWND)NULL && key != NULL && flags != NULL && force_ascii != NULL);

    if( ok ) {
        text = WGetStrFromEdit( GetDlgItem( dlg, IDM_ACCEDKEY ), NULL );
        ok = (text != NULL);
    }

    if( ok ) {
        ok = WGetKeyFromText( text, key, flags, force_ascii );
    }

    if( text != NULL ) {
        WMemFree( text );
    }

    return( ok );
}

Bool WSetEditWindowID( HWND dlg, char *symbol, uint_16 id )
{
    Bool ok;

    ok = (dlg != (HWND)NULL);

    if( ok ) {
        if( symbol != NULL ) {
            ok = WSetEditWithStr( GetDlgItem( dlg, IDM_ACCEDCMDID ), symbol );
        } else {
            ok = WSetEditWithSINT32( GetDlgItem( dlg, IDM_ACCEDCMDID ), (int_32)id, 10 );
        }
    }

    if( ok ) {
        ok = WSetEditWithSINT32( GetDlgItem( dlg, IDM_ACCEDCMDNUM ), (int_32)id, 10 );
    }

    return( ok );
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
        *symbol = WGetStrFromComboLBox( GetDlgItem( dlg, IDM_ACCEDCMDID ), -1 );
    } else {
        *symbol = WGetStrFromEdit( GetDlgItem( dlg, IDM_ACCEDCMDID ), NULL );
    }

    if( *symbol == NULL ) {
        return( FALSE );
    }

    if( **symbol == '\0' ) {
        *symbol = WGetStrFromEdit( GetDlgItem( dlg, IDM_ACCEDCMDNUM ), NULL );
    }

    if( *symbol == NULL ) {
        return( FALSE );
    }

    strupr( *symbol );

    // check if the string has a numeric representation
    val = (int_32)strtol( *symbol, &ep, 0 );
    if( *ep != '\0' ) {
        // the string did not have a numeric representation
        // so let's look it up in the hash table
        if( WRLookupName( symbol_table, *symbol, &hv ) ) {
            *id = (uint_16)hv;
        } else {
            dup = FALSE;
            new_entry = WRAddDefHashEntry( symbol_table, *symbol, &dup );
            if( new_entry != NULL ) {
                *id = (uint_16)new_entry->value;
                if( !dup ) {
                    SendDlgItemMessage( dlg, IDM_ACCEDCMDID, CB_ADDSTRING,
                                        0, (LPARAM)(LPSTR)new_entry->name );
                    SendDlgItemMessage( dlg, IDM_ACCEDCMDID, CB_SETITEMDATA,
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

Bool WSetEditWindowFlags( HWND dlg, uint_16 flags )
{
    Bool ok, is_virt;

    ok = (dlg != (HWND)NULL);

    if( ok ) {
        is_virt = ((flags & ACCEL_VIRTKEY) != 0);
        WSetVirtKey( dlg, is_virt );
        if( is_virt ) {
            if( flags & ACCEL_CONTROL ) {
                CheckDlgButton( dlg, IDM_ACCEDCNTL, 1 );
            } else {
                CheckDlgButton( dlg, IDM_ACCEDCNTL, 0 );
            }

            if( flags & ACCEL_SHIFT ) {
                CheckDlgButton( dlg, IDM_ACCEDSHFT, 1 );
            } else {
                CheckDlgButton( dlg, IDM_ACCEDSHFT, 0 );
            }
        }

        if( flags & ACCEL_ALT ) {
            CheckDlgButton( dlg, IDM_ACCEDALT, 1 );
        } else {
            CheckDlgButton( dlg, IDM_ACCEDALT, 0 );
        }

        if( flags & ACCEL_NOINVERT ) {
            CheckDlgButton( dlg, IDM_ACCEDFLASH, 0 );
        } else {
            CheckDlgButton( dlg, IDM_ACCEDFLASH, 1 );
        }
    }

    return( ok );
}

Bool WGetEditWindowFlags( HWND dlg, uint_16 *flags )
{
    Bool ok, is_virt;

    ok = (dlg != (HWND)NULL);

    if( ok ) {
        if( IsDlgButtonChecked( dlg, IDM_ACCEDVIRT ) ) {
            *flags |= ACCEL_VIRTKEY;
            is_virt = TRUE;
        }

        if( !IsDlgButtonChecked( dlg, IDM_ACCEDFLASH ) ) {
            *flags |= ACCEL_NOINVERT;
        }

        if( IsDlgButtonChecked( dlg, IDM_ACCEDALT ) ) {
            *flags |= ACCEL_ALT;
        }

        if( is_virt ) {
            if( IsDlgButtonChecked( dlg, IDM_ACCEDCNTL ) ) {
                *flags |= ACCEL_CONTROL;
            }

            if( IsDlgButtonChecked( dlg, IDM_ACCEDSHFT ) ) {
                *flags |= ACCEL_SHIFT;
            }
        }
    }

    return( ok );
}

Bool WSetEditWinResName( WAccelEditInfo *einfo )
{
    if( einfo != NULL && einfo->edit_dlg != NULL && einfo->info->res_name != NULL ) {
        return( WSetEditWithWResID( GetDlgItem( einfo->edit_dlg, IDM_ACCEDRNAME ),
                                    einfo->info->res_name ) );
    }

    return( TRUE );
}

void WSetVirtKey( HWND hDlg, Bool is_virt )
{
    CheckDlgButton( hDlg, IDM_ACCEDVIRT, is_virt );
    CheckDlgButton( hDlg, IDM_ACCEDASCII, !is_virt );

    EnableWindow( GetDlgItem( hDlg, IDM_ACCEDSHFT ), is_virt );
    EnableWindow( GetDlgItem( hDlg, IDM_ACCEDCNTL ), is_virt );
    EnableWindow( GetDlgItem( hDlg, IDM_ACCEDALT ), is_virt );

    if( !is_virt ) {
        CheckDlgButton( hDlg, IDM_ACCEDSHFT, FALSE );
        CheckDlgButton( hDlg, IDM_ACCEDCNTL, FALSE );
        CheckDlgButton( hDlg, IDM_ACCEDALT, FALSE );
    }
}

Bool WInitEditWindowListBox( WAccelEditInfo *einfo )
{
    Bool        ok;
    HWND        lbox;
    WAccelEntry *entry;

    ok = (einfo != NULL && einfo->edit_dlg != NULL && einfo->tbl != NULL);

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_ACCEDLIST );
        SendMessage( lbox, WM_SETREDRAW, FALSE, 0 );
        SendMessage( lbox, LB_RESETCONTENT, 0, 0 );
        entry = einfo->tbl->first_entry;
        while( entry != NULL && ok ) {
            ok = WAddEditWinLBoxEntry( einfo, entry, -1 );
            entry = entry->next;
        }
        SendMessage( lbox, WM_SETREDRAW, TRUE, 0 );
    }

    return( ok );
}

static Bool WInitEditWindow( WAccelEditInfo *einfo )
{
    HWND    lbox;
    Bool    ok;

    ok = (einfo != NULL && einfo->edit_dlg != NULL);

    if( ok ) {
        ok = WSetEditWinResName( einfo );
    }

    if( ok ) {
        ok = WInitEditWindowListBox( einfo );
    }

    if( ok ) {
        if( einfo->tbl->first_entry != NULL ) {
            ok = WSetEditWindowKeyEntry( einfo, einfo->tbl->first_entry );
            if( ok ) {
                lbox = GetDlgItem( einfo->edit_dlg, IDM_ACCEDLIST );
                ok = (SendMessage( lbox, LB_SETCURSEL, 0, 0 ) != LB_ERR);
                einfo->current_entry = einfo->tbl->first_entry;
                einfo->current_pos = 0;
            }
        }
    }

    return( ok );
}

Bool WPasteAccelItem( WAccelEditInfo *einfo )
{
    WAccelEntry entry;
    void        *data;
    uint_32     dsize;
    Bool        ok;

    data = NULL;
    entry.symbol = NULL;

    ok = (einfo != NULL);

    if( ok ) {
        ok = WGetClipData( einfo->win, WItemClipbdFormat, &data, &dsize );
    }

    if( ok ) {
        ok = WMakeEntryFromClipData( &entry, data, dsize );
    }

    if( ok ) {
        WResolveEntrySymbol( &entry, einfo->info->symbol_table );
        ok = WSetEditWindowKeyEntry( einfo, &entry );
    }

    if( ok ) {
        ok = WInsertAccelEntry( einfo );
    }

    if( entry.symbol != NULL ) {
        WMemFree( entry.symbol );
    }

    if( data != NULL ) {
        WMemFree( data );
    }

    return( ok );
}

Bool WClipAccelItem( WAccelEditInfo *einfo, Bool cut )
{
    HWND        lbox;
    LRESULT     index;
    WAccelEntry *entry;
    void        *data;
    uint_32     dsize;
    Bool        ok;

    data = NULL;
    ok = (einfo != NULL);

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_ACCEDLIST );
        ok = (lbox != (HWND)NULL);
    }

    if( ok ) {
        index = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
        ok = (index != LB_ERR);
    }

    if( ok ) {
        entry = (WAccelEntry *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)index, 0 );
        ok = (entry != NULL);
    }

    if( ok ) {
        ok = WMakeEntryClipData( entry, &data, &dsize );
    }

    if( ok ) {
        ok = WCopyClipData( einfo->win, WItemClipbdFormat, data, dsize );
    }

    if( ok ) {
        if( cut ) {
            ok = WDeleteAccelEntry( einfo );
        }
    }

    if( data != NULL ) {
        WMemFree( data );
    }

    return( ok );
}

static Bool WQueryChangeEntry( WAccelEditInfo *einfo )
{
    int         ret;
    UINT        style;
    char        *title;
    char        *text;

    style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
    title = WCreateEditTitle( einfo );
    text = WAllocRCString( W_CHANGEMODIFIEDMENUITEM );

    ret = MessageBox( einfo->edit_dlg, text, title, style );

    if( text != NULL ) {
        WFreeRCString( text );
    }
    if( title != NULL ) {
        WMemFree( title );
    }

    if( ret == IDYES ) {
        return( TRUE );
    }

    return( FALSE );
}

void WDoHandleSelChange( WAccelEditInfo *einfo, Bool change, Bool reset )
{
    HWND        lbox;
    LRESULT     index;
    WAccelEntry *entry;
    Bool        mod;

    if( einfo == NULL ) {
        return;
    }

    lbox = GetDlgItem( einfo->edit_dlg, IDM_ACCEDLIST );
    if( lbox == (HWND)NULL ) {
        return;
    }

    index = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
    if( index != LB_ERR ) {
        entry = (WAccelEntry *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)index, 0 );
    } else {
        entry = NULL;
    }

    if( einfo->current_entry != NULL && !reset ) {
        mod = WGetEditWindowKeyEntry( einfo, einfo->current_entry, TRUE );
        if( mod && einfo->current_pos != -1 ) {
            if( change || WQueryChangeEntry( einfo ) ) {
                WGetEditWindowKeyEntry( einfo, einfo->current_entry, FALSE );
                einfo->info->modified = TRUE;
                SendMessage( lbox, LB_DELETESTRING, einfo->current_pos, 0 );
                WAddEditWinLBoxEntry( einfo, einfo->current_entry, einfo->current_pos );
            }
        }
    }

    if( entry != NULL ) {
        if( change ) {
            uint_16 id;
            if( entry->is32bit ) {
                id = entry->entry32.Id;
            } else {
                id = (uint_16)entry->entry.Id;
            }
            WSetEditWindowID( einfo->edit_dlg, entry->symbol, id );
        } else {
            WSetEditWindowKeyEntry( einfo, entry );
        }
    }

    einfo->current_entry = entry;
    einfo->current_pos = (index == LB_ERR) ? -1 : index;
    if ( index != LB_ERR ) {
        SendMessage ( lbox, LB_SETCURSEL, (WPARAM)index, 0 );
    }
}

void WHandleChange( WAccelEditInfo *einfo )
{
    WDoHandleSelChange( einfo, TRUE, FALSE );
}

void WHandleSelChange( WAccelEditInfo *einfo )
{
    WDoHandleSelChange( einfo, FALSE, FALSE );
}

WINEXPORT BOOL CALLBACK WAcccelEditProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WAccelEditInfo      *einfo;
    HWND                win;
    RECT                r;
    POINT               p;
    LRESULT             ret;
    WORD                wp;
    WORD                cmd;

    ret = FALSE;
    einfo = (WAccelEditInfo *)GET_DLGDATA( hDlg );

    switch( message ) {
    case WM_INITDIALOG:
        einfo = (WAccelEditInfo *)lParam;
        einfo->edit_dlg = hDlg;
        SET_DLGDATA( hDlg, (LONG_PTR)einfo );
        WRAddSymbolsToComboBox( einfo->info->symbol_table, hDlg,
                                IDM_ACCEDCMDID, WR_HASHENTRY_ALL );
        ret = TRUE;
        break;

    case WM_SYSCOLORCHANGE:
        WCtl3dColorChange();
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

    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONUP:
        MAKE_POINT( p, lParam );
        win = GetDlgItem( hDlg, IDM_ACCEDRNAME );
        GetClientRect( win, &r );
        MapWindowPoints( win, hDlg, (POINT *)&r, 2 );
        if( PtInRect( &r, p ) ) {
            WHandleRename( einfo );
        }
        ret = TRUE;
        break;

#if 0
    case WM_PARENTNOTIFY:
        cmd = GET_WM_PARENTNOTIFY_EVENT( wParam, lParam );
        switch( cmd ) {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            MAKE_POINT( p, lParam );
            win = GetDlgItem( hDlg, IDM_ACCEDLIST );
            GetWindowRect( win, &r );
            MapWindowPoints( HWND_DESKTOP, hDlg, (POINT *)&r, 2 );
            if( PtInRect( &r, p ) ) {
                WHandleSelChange( einfo );
            }
            break;
        }
        ret = TRUE;
        break;
#endif

    case WM_COMMAND:
        wp = LOWORD( wParam );
        cmd = GET_WM_COMMAND_CMD( wParam, lParam );
        switch( wp ) {
        case IDM_ACCEDINSERT:
            WInsertAccelEntry( einfo );
            break;
        case IDM_ACCEDCHANGE:
            WHandleChange( einfo );
            break;
        case IDM_ACCEDRESET:
            WDoHandleSelChange( einfo, FALSE, TRUE );
            break;
        case IDM_ACCEDSETACC:
            if( einfo->tbl != NULL && einfo->tbl->num != 0 ) {
                WHandleSelChange( einfo );
            } else {
                WInsertAccelEntry( einfo );
            }
            break;
#if 0
        case IDM_ACCEDCMDID:
            if( cmd == CBN_SELCHANGE ) {
                einfo->combo_change = TRUE;
                WHandleSelChange( einfo );
                einfo->combo_change = FALSE;
            }
            break;
#endif
        case IDM_ACCEDLIST:
            if( cmd == LBN_SELCHANGE ) {
                WHandleSelChange( einfo );
            }
            break;
        case IDM_ACCEDVIRT:
        case IDM_ACCEDASCII:
            if( cmd == BN_CLICKED ) {
                WSetVirtKey( hDlg, wp == IDM_ACCEDVIRT );
            }
            break;
        }
        break;
    }

    return( ret );
}

WINEXPORT BOOL CALLBACK WTestProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    RECT        r;

    _wtouch( wParam );
    _wtouch( lParam );

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

    lpProc = MakeProcInstance( (FARPROC)WTestProc, inst );
    JCreateDialog( inst, "WAccelEditDLG", parent, (DLGPROC)lpProc );
    FreeProcInstance( lpProc );
}
