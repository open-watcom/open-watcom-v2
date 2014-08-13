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
#include "wnewitem.h"
#include "wdel.h"
#include "wedit.h"
#include "wctl3d.h"
#include "wsetedit.h"
#include "wprev.h"
#include "wclip.h"
#include "ldstr.h"
#include "rcstr.gh"
#include "wmsg.h"
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
WINEXPORT BOOL CALLBACK WMenuEditProc( HWND, UINT, WPARAM, LPARAM );
WINEXPORT BOOL CALLBACK WTestProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

extern UINT WClipbdFormat;
extern UINT WItemClipbdFormat;

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool WInitEditWindow( WMenuEditInfo * );
static void WExpandEditWindowItem( HWND, HWND, RECT * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static DLGPROC     WMenuEditWinProc = NULL;
static HBRUSH      WEditWinBrush    = NULL;
static COLORREF    WEditWinColor    = 0;

int appWidth = -1;
int appHeight = -1;

void WInitEditWindows( HINSTANCE inst )
{
    _wtouch( inst );

    WEditWinColor = GetSysColor( COLOR_BTNFACE );
    WEditWinBrush = CreateSolidBrush( WEditWinColor );
    WMenuEditWinProc = (DLGPROC)MakeProcInstance( (FARPROC)WMenuEditProc, inst );
}

void WFiniEditWindows( void )
{
    if( WEditWinBrush != NULL ) {
        DeleteObject( WEditWinBrush );
    }
    FreeProcInstance( (FARPROC)WMenuEditWinProc );
}


bool WCreateMenuEditWindow( WMenuEditInfo *einfo, HINSTANCE inst )
{
    einfo->edit_dlg = JCreateDialogParam( inst, "WMenuEditDLG", einfo->win,
                                          WMenuEditWinProc, (LPARAM)einfo );

    if( einfo->edit_dlg == (HWND)NULL ) {
        return( FALSE );
    }

    if( !WCreatePrevWindow( inst, einfo ) ) {
        return( FALSE );
    }

    SetWindowPos( einfo->edit_dlg, (HWND)NULL, 0, WGetRibbonHeight(), 0, 0,
                  SWP_NOSIZE | SWP_NOZORDER );

    return( WInitEditWindow( einfo ) );
}

bool WResizeMenuEditWindow( WMenuEditInfo *einfo, RECT *prect )
{
    int     width, height, ribbon_depth;
    HWND    win;
    RECT    crect;

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
    win = GetDlgItem( einfo->edit_dlg, IDM_MENUEDBLACKLINE );
    GetWindowRect( win, &crect );
    SetWindowPos( win, (HWND)NULL, 0, 0, width, crect.bottom - crect.top,
                  SWP_NOMOVE | SWP_NOZORDER );

    // change the size of the resource name edit field
    win = GetDlgItem( einfo->edit_dlg, IDM_MENUEDRNAME );
    WExpandEditWindowItem( einfo->edit_dlg, win, prect );

    // change the size of the listbox
    win = GetDlgItem( einfo->edit_dlg, IDM_MENUEDLIST );
    WExpandEditWindowItem( einfo->edit_dlg, win, prect );

    SetWindowPos( einfo->edit_dlg, (HWND)NULL, 0, ribbon_depth,
                  width, height, SWP_NOZORDER );

    // change the size of the preview window
    WExpandEditWindowItem( einfo->edit_dlg, einfo->preview_window, prect );

    return( TRUE );
}

static void WExpandEditWindowItem( HWND hDlg, HWND win, RECT *prect )
{
    RECT crect, t;

    if( win == (HWND)NULL ) {
        return;
    }

    /* expand the child window */
    GetWindowRect( win, &crect );
    MapWindowPoints( (HWND)NULL, hDlg, (POINT *)&crect, 2 );
    t.left = 0;
    t.top = 0;
    t.right = 0;
    t.bottom = WEDIT_PAD;
    MapDialogRect( hDlg, &t );
    SetWindowPos( win, (HWND)NULL, 0, 0, prect->right - crect.left - t.bottom,
                  crect.bottom - crect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
    InvalidateRect( win, NULL, TRUE );
}

void WSetEditWindowControls( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    bool        enable;

    // can this entry be reset or changed
    enable = (entry != NULL);
    EnableWindow( GetDlgItem( einfo->edit_dlg, IDM_MENUEDRESET ), enable );
    EnableWindow( GetDlgItem( einfo->edit_dlg, IDM_MENUEDCHANGE ), enable );

    // can this entry be shifted left
    enable = (entry != NULL && entry->parent != NULL && entry->next == NULL);
    EnableWindow( GetDlgItem( einfo->edit_dlg, IDM_MENUEDSHIFTLEFT ), enable );

    // can this entry be shifted right
    enable = (entry != NULL && entry->prev != NULL && entry->prev->item->IsPopup);
    EnableWindow( GetDlgItem( einfo->edit_dlg, IDM_MENUEDSHIFTRIGHT ), enable );
}

bool WSetEditWindowMenuEntry( WMenuEditInfo *einfo, WMenuEntry *entry )
{
    bool        ok;
    bool        pop_sep;
    MenuFlags   flags;
    uint_16     id;
    char        *text;

    ok = (einfo != NULL && einfo->edit_dlg != NULL && entry != NULL);

    if( ok ) {
        if( entry->item->IsPopup ) {
            flags = entry->item->Item.Popup.ItemFlags;
            text = entry->item->Item.Popup.ItemText;
        } else {
            flags = entry->item->Item.Normal.ItemFlags;
            id = entry->item->Item.Normal.ItemID;
            text = entry->item->Item.Normal.ItemText;
        }
        ok = WSetEditWindowText( einfo->edit_dlg, flags, text );
    }

    if( ok ) {
        ok = WSetEditWindowFlags( einfo->edit_dlg, flags, FALSE );
    }

    if( ok ) {
        pop_sep = (entry->item->IsPopup || (flags & MENU_SEPARATOR));
        ok = WSetEditWindowID( einfo->edit_dlg, id, pop_sep, entry->symbol );
    }

    return( ok );
}

static bool WQueryNukePopup( WMenuEditInfo *einfo )
{
    int         ret;
    UINT        style;
    char        *title;
    char        *text;

    style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
    title = WCreateEditTitle( einfo );
    text = AllocRCString( W_QUERYNUKEPOPUP );

    ret = MessageBox( einfo->edit_dlg, text, title, style );

    if( text != NULL ) {
        FreeRCString( text );
    }
    if( title != NULL ) {
        WRMemFree( title );
    }

    if( ret == IDYES ) {
        return( TRUE );
    }

    return( FALSE );
}

bool WGetEditWindowMenuEntry( WMenuEditInfo *einfo, WMenuEntry *entry,
                              bool test_mod, bool *reset )
{
    MenuFlags   flags;
    MenuFlags   iflags;
    uint_16     id;
    char        *text;
    char        *symbol;
    bool        ok;

    flags = 0;
    id = 0;
    text = NULL;
    symbol = NULL;

    if( reset ) {
        *reset = FALSE;
    }

    ok = (einfo != NULL && einfo->edit_dlg != NULL && entry != NULL);

    if( ok ) {
        ok = WGetEditWindowFlags( einfo->edit_dlg, &flags );
    }

    if( ok ) {
        if( !(flags & MENU_SEPARATOR) ) {
            ok = WGetEditWindowText( einfo->edit_dlg, &text );
        }
    }

    if( ok ) {
        if( !(flags & MENU_POPUP) ) {
            ok = WGetEditWindowID( einfo->edit_dlg, &symbol, &id,
                                   einfo->info->symbol_table,
                                   einfo->combo_change );
        }
    }

    /* check if anything was actually modified */
    if( ok ) {
        // make sure the symbol info did not change
        ok = (entry->symbol == NULL && symbol != NULL) ||
             (entry->symbol != NULL && symbol == NULL);
        if( !ok ) {
            ok = symbol && stricmp( entry->symbol, symbol );
            if( !ok ) {
                iflags = entry->item->Item.Popup.ItemFlags;
                iflags &= ~MENU_ENDMENU;
                ok = (iflags != flags);
                if( !ok ) {
                    if( flags & MENU_POPUP ) {
                        ok = strcmp( entry->item->Item.Popup.ItemText, text );
                    } else if( flags & MENU_SEPARATOR ) {
                        ok = false;
                    } else {
                        ok = (entry->item->Item.Normal.ItemID != id ||
                              strcmp( entry->item->Item.Normal.ItemText, text ));
                    }
                }
            }
        }
        if( test_mod ) {
            return( ok );
        }
    }

    if( ok ) {
        if( entry->item->IsPopup ) {
            if( entry->child != NULL && (flags & MENU_POPUP) == 0 ) {
                ok = WQueryNukePopup( einfo );
            }
        }
    }

    if( ok ) {
        if( entry->item->IsPopup ) {
            if( (flags & MENU_POPUP) == 0 ) {
                if( reset ) {
                    *reset = TRUE;
                }
                entry->preview_popup = (HMENU)NULL;
                if( entry->child != NULL ) {
                    WFreeMenuEntries( entry->child );
                    entry->child = NULL;
                }
            }
            if( entry->item->Item.Popup.ItemText ) {
                WRMemFree( entry->item->Item.Popup.ItemText );
            }
        } else {
            // if the item is being changed from a normal item into a popup
            // or separator then reset the preview
            if( (flags & MENU_POPUP) != 0 || (flags & MENU_SEPARATOR) != 0 ) {
                if( reset ) {
                    *reset = TRUE;
                }
                entry->preview_popup = (HMENU)NULL;
            }
            if( entry->item->Item.Normal.ItemText ) {
                WRMemFree( entry->item->Item.Normal.ItemText );
            }
        }
        if( entry->symbol != NULL ) {
            WRMemFree( entry->symbol );
        }
        entry->item->IsPopup = ((flags & MENU_POPUP) != 0);
        if( entry->item->IsPopup ) {
            entry->item->Item.Popup.ItemText = text;
            entry->item->Item.Popup.ItemFlags = flags;
        } else {
            // if the flags are zero - indicating a normal item - and the
            // id is also zero then change it to the default
            // flag == id == 0 indicates a separator
            if( flags == 0 && id == 0 ) {
                id = DEFAULT_MENU_ID;
            }
            entry->item->Item.Normal.ItemText = text;
            entry->item->Item.Normal.ItemID = id;
            entry->item->Item.Normal.ItemFlags = flags;
        }
        entry->symbol = symbol;
    } else {
        if( symbol != NULL ) {
            WRMemFree( symbol );
        }
        if( text != NULL ) {
            WRMemFree( text );
        }
    }

    return( ok );
}

bool WSetEditWindowText( HWND dlg, MenuFlags flags, char *text )
{
    bool    ok;
    char    *t;
    char    *n;

    ok = (dlg != (HWND)NULL);

    if( ok ) {
        if( flags & MENU_SEPARATOR ) {
            t = "";
        } else {
            t = text;
            if( t == NULL ) {
                t = "";
            }
        }
    }

    if( ok ) {
        n = WConvertStringFrom( t, "\t\x8", "ta" );
        if( n != NULL ) {
            ok = WSetEditWithStr( GetDlgItem( dlg, IDM_MENUEDTEXT ), n );
            WRMemFree( n );
        } else {
            ok = WSetEditWithStr( GetDlgItem( dlg, IDM_MENUEDTEXT ), t );
        }
    }

    return( ok );
}

bool WGetEditWindowText( HWND dlg, char **text )
{
    bool        ok;
    char        *n;

    ok = (dlg != (HWND)NULL && text != NULL);

    if( ok ) {
        n = WGetStrFromEdit( GetDlgItem( dlg, IDM_MENUEDTEXT ), NULL );
        if( n != NULL && *n == '\0' ) {
            WRMemFree( n );
            n = NULL;
        }
        *text = WConvertStringTo( n, "\t\x8", "ta" );
        if( n != NULL ) {
            WRMemFree( n );
        }
        ok = (*text != NULL);
    }

    return( ok );
}

bool WSetEditWindowID( HWND dlg, uint_16 id, bool is_pop_sep, char *symbol )
{
    bool    ok;

    ok = (dlg != (HWND)NULL);

    if( ok ) {
        if( is_pop_sep ) {
            ok = WSetEditWithStr( GetDlgItem( dlg, IDM_MENUEDID ), "" );
        } else {
            if( symbol != NULL ) {
                ok = WSetEditWithStr( GetDlgItem( dlg, IDM_MENUEDID ), symbol );
            } else {
                ok = WSetEditWithSINT32( GetDlgItem( dlg, IDM_MENUEDID ), (int_32)id, 10 );
            }
        }
    }

    if( ok ) {
        if( is_pop_sep ) {
            ok = WSetEditWithStr( GetDlgItem( dlg, IDM_MENUEDNUM ), "" );
        } else {
            ok = WSetEditWithSINT32( GetDlgItem( dlg, IDM_MENUEDNUM ), (int_32)id, 10 );
        }
    }

    return( ok );
}

bool WGetEditWindowID( HWND dlg, char **symbol, uint_16 *id,
                       WRHashTable *symbol_table, bool combo_change )
{
    int_32      val;
    char        *ep;
    WRHashValue hv;
    WRHashEntry *new_entry;
    bool        dup;

    if( dlg == (HWND)NULL ) {
        return( false );
    }

    if( combo_change ) {
        *symbol = WGetStrFromComboLBox( GetDlgItem( dlg, IDM_MENUEDID ), -1 );
    } else {
        *symbol = WGetStrFromEdit( GetDlgItem( dlg, IDM_MENUEDID ), NULL );
    }

    if( *symbol == NULL ) {
        return( false );
    }

    if( **symbol == '\0' ) {
        *symbol = WGetStrFromEdit( GetDlgItem( dlg, IDM_MENUEDNUM ), NULL );
    }

    if( *symbol == NULL ) {
        return( false );
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
            dup = false;
            new_entry = WRAddDefHashEntry( symbol_table, *symbol, &dup );
            if( new_entry != NULL ) {
                *id = (uint_16)new_entry->value;
                if( !dup ) {
                    SendDlgItemMessage( dlg, IDM_MENUEDID, CB_ADDSTRING,
                                        0, (LPARAM)(LPSTR)new_entry->name );
                    SendDlgItemMessage( dlg, IDM_MENUEDID, CB_SETITEMDATA,
                                        0, (LPARAM)new_entry );
                }
            } else {
                *id = 0;
                WRMemFree( *symbol );
                *symbol = NULL;
                return( false );
            }
        }
    } else {
        // the string did have a numeric representation
        *id = (uint_16)val;
        WRMemFree( *symbol );
        *symbol = NULL;
    }

    return( true );
}

bool WSetEditWindowFlags( HWND dlg, MenuFlags flags, bool reset )
{
    bool ok;

    ok = (dlg != (HWND)NULL);

    if( ok ) {
#if 0
        EnableWindow( GetDlgItem( dlg, IDM_MENUEDPOPUP ), (flags & MENU_POPUP) != 0 );
        EnableWindow( GetDlgItem( dlg, IDM_MENUEDSEP ), (flags & MENU_SEPARATOR) != 0 );
        EnableWindow( GetDlgItem( dlg, IDM_MENUEDNORMAL ),
                      !(flags & MENU_POPUP) && !(flags & MENU_SEPARATOR) );
#endif

        CheckDlgButton( dlg, IDM_MENUEDPOPUP, reset || (flags & MENU_POPUP) != 0 );
        CheckDlgButton( dlg, IDM_MENUEDSEP, !reset && (flags & MENU_SEPARATOR) != 0 );
        CheckDlgButton( dlg, IDM_MENUEDNORMAL,
                        !reset && !(flags & MENU_POPUP) && !(flags & MENU_SEPARATOR) );
        CheckDlgButton( dlg, IDM_MENUEDCHECKED, !reset && (flags & MENU_CHECKED) != 0 );
        CheckDlgButton( dlg, IDM_MENUEDGRAYED, !reset && (flags & MENU_GRAYED) != 0 );
        CheckDlgButton( dlg, IDM_MENUEDINACTIVE, !reset && (flags & MENU_INACTIVE) != 0 );
        CheckDlgButton( dlg, IDM_MENUEDHELP, !reset && (flags & MENU_HELP) != 0 );
        CheckDlgButton( dlg, IDM_MENUEDMENU, !reset && (flags & MENU_MENUBREAK) != 0 );
        CheckDlgButton( dlg, IDM_MENUEDMENUBAR, !reset && (flags & MENU_MENUBARBREAK) != 0 );
    }

    return( ok );
}

bool WResetEditWindowFlags( HWND dlg )
{
    bool ok;

    ok = (dlg != (HWND)NULL);

    if( ok ) {
        ok = WSetEditWindowFlags( dlg, 0, TRUE );
    }

    return( ok );
}

bool WGetEditWindowFlags( HWND dlg, MenuFlags *flags )
{
    bool ok;

    ok = (dlg != (HWND)NULL && flags != NULL);

    if( ok ) {
        *flags = 0;
        if( IsDlgButtonChecked( dlg, IDM_MENUEDPOPUP ) ) {
            *flags |= MENU_POPUP;
        }
        if( IsDlgButtonChecked( dlg, IDM_MENUEDSEP ) ) {
            *flags |= MENU_SEPARATOR;
        }
        if( IsDlgButtonChecked( dlg, IDM_MENUEDCHECKED ) ) {
            *flags |= MENU_CHECKED;
        }
        if( IsDlgButtonChecked( dlg, IDM_MENUEDGRAYED ) ) {
            *flags |= MENU_GRAYED;
        }
        if( IsDlgButtonChecked( dlg, IDM_MENUEDINACTIVE ) ) {
            *flags |= MENU_INACTIVE;
        }
        if( IsDlgButtonChecked( dlg, IDM_MENUEDHELP ) ) {
            *flags |= MENU_HELP;
        }
        if( IsDlgButtonChecked( dlg, IDM_MENUEDMENU ) ) {
            *flags |= MENU_MENUBREAK;
        }
        if( IsDlgButtonChecked( dlg, IDM_MENUEDMENUBAR ) ) {
            *flags |= MENU_MENUBARBREAK;
        }
    }

    return( ok );
}

bool WSetEditWinResName( WMenuEditInfo *einfo )
{
    if( einfo != NULL && einfo->edit_dlg != NULL && einfo->info->res_name != NULL ) {
        return( WSetEditWithWResID( GetDlgItem( einfo->edit_dlg, IDM_MENUEDRNAME ),
                                    einfo->info->res_name ) );
    }

    return( TRUE );
}

bool WInitEditWindowListBox( WMenuEditInfo *einfo )
{
    bool        ok;
    HWND        lbox;
    int         pos;

    ok = (einfo != NULL && einfo->edit_dlg != NULL && einfo->menu != NULL);

    if( ok ) {
        pos = 0;
        lbox = GetDlgItem( einfo->edit_dlg, IDM_MENUEDLIST );
        ok = (lbox != (HWND)NULL);
    }

    if( ok ) {
        SendMessage( lbox, WM_SETREDRAW, FALSE, 0 );
        SendMessage( lbox, LB_RESETCONTENT, FALSE, 0 );
        ok = WAddMenuEntriesToLBox( lbox, einfo->menu->first_entry, &pos );
        SendMessage( lbox, WM_SETREDRAW, TRUE, 0 );
        InvalidateRect( lbox, NULL, TRUE );
    }

    if( ok ) {
        ok = WResetPrevWindowMenu( einfo );
    }

    return( ok );
}

static bool WInitEditWindow( WMenuEditInfo *einfo )
{
    HWND    lbox;
    bool    ok;

    ok = (einfo != NULL && einfo->edit_dlg != NULL);

    if( ok ) {
        ok = WSetEditWinResName( einfo );
    }

    if( ok ) {
        ok = WInitEditWindowListBox( einfo );
    }

    if( ok ) {
        WSetEditWindowControls( einfo, einfo->menu->first_entry );
        if( einfo->menu->first_entry != NULL ) {
            ok = WSetEditWindowMenuEntry( einfo, einfo->menu->first_entry );
            if( ok ) {
                lbox = GetDlgItem( einfo->edit_dlg, IDM_MENUEDLIST );
                ok = (SendMessage( lbox, LB_SETCURSEL, 0, 0 ) != LB_ERR);
                einfo->current_entry = einfo->menu->first_entry;
                einfo->current_pos = 0;
            }
        } else {
            CheckDlgButton( einfo->edit_dlg, IDM_MENUEDPOPUP, TRUE );
        }
    }

    return( ok );
}

bool WPasteMenuItem( WMenuEditInfo *einfo )
{
    WMenuEntry  *entry;
    void        *data;
    uint_32     dsize;
    bool        ok;

    data = NULL;
    ok = (einfo != NULL);

    if( ok ) {
        ok = WGetClipData( einfo->win, WItemClipbdFormat, &data, &dsize );
    }

    if( ok ) {
        entry = WMakeMenuEntryFromClipData( data, dsize );
        ok = (entry != NULL);
    }

    if( ok ) {
        ok = WResolveEntries( entry, einfo->info->symbol_table );
    }

    if( ok ) {
        ok = WInsertMenuEntry( einfo, entry, TRUE );
    }

    if( data != NULL ) {
        WRMemFree( data );
    }

    return( ok );
}

bool WClipMenuItem( WMenuEditInfo *einfo, bool cut )
{
    HWND        lbox;
    LRESULT     index;
    void        *data;
    uint_32     dsize;
    WMenuEntry  *entry;
    bool        ok;

    data = NULL;
    ok = (einfo != NULL);

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_MENUEDLIST );
        ok = (lbox != (HWND)NULL);
    }

    if( ok ) {
        index = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
        ok = (index != LB_ERR);
    }

    if( ok ) {
        entry = (WMenuEntry *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)index, 0 );
        ok = (entry != NULL);
    }

    if( ok ) {
        ok = WMakeClipDataFromMenuEntry( entry, &data, &dsize );
    }

    if( ok ) {
        ok = WCopyClipData( einfo->win, WItemClipbdFormat, data, dsize );
    }

    if( ok ) {
        if( cut ) {
            ok = WDeleteMenuEntry( einfo );
        }
    }

    if( data != NULL ) {
        WRMemFree( data );
    }

    return( ok );
}

static bool WQueryChangeEntry( WMenuEditInfo *einfo )
{
    int         ret;
    UINT        style;
    char        *title;
    char        *text;

    style = MB_YESNO | MB_APPLMODAL | MB_ICONEXCLAMATION;
    title = WCreateEditTitle( einfo );
    text = AllocRCString( W_CHANGEMODIFIEDMENUITEM );

    ret = MessageBox( einfo->edit_dlg, text, title, style );

    if( text != NULL ) {
        FreeRCString( text );
    }
    if( title != NULL ) {
        WRMemFree( title );
    }

    if( ret == IDYES ) {
        return( TRUE );
    }

    return( FALSE );
}

void WDoHandleSelChange( WMenuEditInfo *einfo, bool change, bool reset )
{
    HWND        lbox;
    LRESULT     index;
    WMenuEntry  *entry;
    bool        reinit;
    bool        mod;

    if( einfo == NULL ) {
        return;
    }

    reinit = FALSE;

    lbox = GetDlgItem( einfo->edit_dlg, IDM_MENUEDLIST );
    if( lbox == (HWND)NULL ) {
        return;
    }

    index = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
    if( index != LB_ERR ) {
        entry = (WMenuEntry *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)index, 0 );
    } else {
        entry = NULL;
    }

    if( einfo->current_entry != NULL && !reset ) {
        mod = WGetEditWindowMenuEntry( einfo, einfo->current_entry, TRUE, NULL );
        if( mod && einfo->current_pos != -1 ) {
            if( change || WQueryChangeEntry( einfo ) ) {
                WGetEditWindowMenuEntry( einfo, einfo->current_entry, FALSE, &reinit );
                einfo->info->modified = true;
                if( reinit ) {
                    WInitEditWindowListBox( einfo );
                } else {
                    SendMessage( lbox, LB_DELETESTRING, einfo->current_pos, 0 );
                    WAddEditWinLBoxEntry( lbox, einfo->current_entry, einfo->current_pos );
                    WModifyEntryInPreview( einfo, einfo->current_entry );
                }
            }
        }
    }

    WSetEditWindowControls( einfo, entry );
    if( entry != NULL ) {
        if( !change || reinit ) {
            WSetEditWindowMenuEntry( einfo, entry );
        } else {
            uint_16     id;
            bool        pop_sep;
            MenuFlags   flags;
            if( entry->item->IsPopup ) {
                flags = entry->item->Item.Popup.ItemFlags;
            } else {
                flags = entry->item->Item.Normal.ItemFlags;
                id = entry->item->Item.Normal.ItemID;
            }
            pop_sep = (entry->item->IsPopup || (flags & MENU_SEPARATOR));
            WSetEditWindowID( einfo->edit_dlg, id, pop_sep, entry->symbol );
        }
    }

    einfo->current_entry = entry;
    einfo->current_pos = (index == LB_ERR ? -1 : index);
    if( index != LB_ERR ) {
        SendMessage( lbox, LB_SETCURSEL, (WPARAM)index, 0 );
    }
}

void WHandleSelChange( WMenuEditInfo *einfo )
{
    WDoHandleSelChange( einfo, FALSE, FALSE );
}

static bool WShiftEntry( WMenuEditInfo *einfo, bool left )
{
    WMenuEntry  *prev;
    WMenuEntry  *parent;
    WMenuEntry  *entry;
    LRESULT     ret;
    HWND        lbox;
    bool        entry_removed;
    bool        ok;

    entry_removed = FALSE;

    ok = (einfo != NULL && einfo->edit_dlg != NULL);

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_MENUEDLIST );
        ok = (lbox != NULL);
    }

    if( ok ) {
        ret = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
        ok = (ret != LB_ERR);
    }

    if( ok ) {
        entry = (WMenuEntry *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)ret, 0 );
        ok = (entry != NULL);
    }

    if( ok ) {
        parent = entry->parent;
        prev = entry->prev;
        ok = WRemoveMenuEntry( einfo->menu, entry );
        if( ok ) {
            entry_removed = TRUE;
        }
    }

    if( ok ) {
        if( left ) {
            ok = WInsertEntryIntoMenu( einfo, parent, parent->parent, entry, FALSE );
        } else {
            if( prev->child != NULL ) {
                parent = prev;
                for( prev = parent->child; prev != NULL && prev->next != NULL;
                     prev = prev->next );
                ok = WInsertEntryIntoMenu( einfo, prev, parent, entry, FALSE );
            } else {
                ok = WInsertEntryIntoMenu( einfo, prev, parent, entry, TRUE );
            }
        }
    }

    if( ok ) {
        ok = WInitEditWindowListBox( einfo );
    }

    if( ok ) {
        einfo->info->modified = true;
        einfo->current_entry = NULL;
        einfo->current_pos = -1;
        ret = SendMessage( lbox, LB_SETCURSEL, (WPARAM)ret, 0 );
        ok = (ret != LB_ERR);
        if( ok ) {
            WHandleSelChange( einfo );
        }
    }

    if( !ok ) {
        if( entry_removed ) {
            WFreeMenuEntries( entry->child );
            WFreeMenuEntry( entry );
            WInitEditWindowListBox( einfo );
        }
    }

    return( ok );
}

WINEXPORT BOOL CALLBACK WMenuEditProc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    WMenuEditInfo       *einfo;
    HWND                win;
    RECT                r;
    POINT               p;
    LRESULT             ret;
    WORD                wp, cmd;

    ret = FALSE;
    einfo = (WMenuEditInfo *)GET_DLGDATA( hDlg );

    switch( message ) {
    case WM_INITDIALOG:
        einfo = (WMenuEditInfo *)lParam;
        einfo->edit_dlg = hDlg;
        SET_DLGDATA( hDlg, (LONG_PTR)einfo );
        WRAddSymbolsToComboBox( einfo->info->symbol_table, hDlg,
                                IDM_MENUEDID, WR_HASHENTRY_ALL );
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
        win = GetDlgItem( hDlg, IDM_MENUEDRNAME );
        GetWindowRect( win, &r );
        MapWindowPoints( HWND_DESKTOP, hDlg, (POINT *)&r, 2 );
        if( PtInRect( &r, p ) ) {
            WHandleRename( einfo );
        }
        ret = TRUE;
        break;

    case WM_SETFOCUS:
        if( einfo != NULL && einfo->preview_window != (HWND)NULL ) {
            SendMessage( einfo->preview_window, WM_NCACTIVATE, (WPARAM)TRUE, (LPARAM)NULL );
        }
        break;

#if 0
    case WM_PARENTNOTIFY:
        cmd = GET_WM_PARENTNOTIFY_EVENT( wParam, lParam );
        switch( cmd ) {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
            MAKE_POINT( p, lParam );
            win = GetDlgItem( hDlg, IDM_MENUEDLIST );
            GetClientRect( win, &r );
            MapWindowPoints( win, hDlg, (POINT *)&r, 2 );
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
        case IDM_MENUEDGRAYED:
            if( IsDlgButtonChecked( hDlg, wp ) ) {
                CheckDlgButton( hDlg, IDM_MENUEDINACTIVE, 0 );
            }
            break;
        case IDM_MENUEDINACTIVE:
            if( IsDlgButtonChecked( hDlg, wp ) ) {
                CheckDlgButton( hDlg, IDM_MENUEDGRAYED, 0 );
            }
            break;
#if 0
        case IDM_MENUEDID:
            if( cmd == CBN_SELCHANGE ) {
                einfo->combo_change = TRUE;
                WHandleSelChange( einfo );
                einfo->combo_change = FALSE;
            }
            break;
#endif
        case IDM_MENUEDINSERT:
            WInsertNew( einfo );
            break;
        case IDM_MENUEDCHANGE:
            WDoHandleSelChange( einfo, TRUE, FALSE );
            break;
        case IDM_MENUEDRESET:
            WDoHandleSelChange( einfo, FALSE, TRUE );
            break;
        case IDM_MENUEDSHIFTLEFT:
            WShiftEntry( einfo, TRUE );
            break;
        case IDM_MENUEDSHIFTRIGHT:
            WShiftEntry( einfo, FALSE );
            break;
        case IDM_MENUEDLIST:
            if( cmd == LBN_SELCHANGE ) {
                WHandleSelChange( einfo );
            }
            break;
        case IDM_MENUEDSETMENU:
            if( einfo->menu != NULL && einfo->menu->first_entry != NULL ) {
                WHandleSelChange( einfo );
            } else {
                WInsertNew( einfo );
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
    JCreateDialog( inst, "WMenuEditDLG", parent, (DLGPROC)lpProc );
    FreeProcInstance( lpProc );
}
