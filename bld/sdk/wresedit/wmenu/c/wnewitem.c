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
#include <string.h>
#include "win1632.h"
#include "wglbl.h"
#include "wmem.h"
#include "wmsg.h"
#include "wmsgfile.h"
#include "wsetedit.h"
#include "wedit.h"
#include "wstrdup.h"
#include "wnewitem.h"
#include "sys_rc.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define DEPTH_MULT      4
#define NULL_STRING     "Item"

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WMenuEntry *WCreateNewMenuEntry ( WMenuEditInfo *, Bool, Bool );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

Bool WInsertNew( WMenuEditInfo *einfo )
{
    Bool        ret;

    ret = FALSE;

    if( einfo && einfo->edit_dlg != (HWND)NULL ) {
        if( IsDlgButtonChecked( einfo->edit_dlg, IDM_MENUEDPOPUP ) ) {
            ret = WInsertNewMenuEntry ( einfo, TRUE, FALSE );
        } else if( IsDlgButtonChecked( einfo->edit_dlg, IDM_MENUEDSEP ) ) {
            ret = WInsertNewMenuEntry ( einfo, FALSE, TRUE );
        } else {
            ret = WInsertNewMenuEntry( einfo, FALSE, FALSE );
        }
    }

    return( ret );
}

Bool WInsertNewMenuEntry( WMenuEditInfo *einfo, Bool popup, Bool sep )
{
    WMenuEntry  *new;
    Bool        ok;

    new = NULL;

    ok = ( einfo && einfo->edit_dlg );

    if( ok ) {
        new = WCreateNewMenuEntry( einfo, popup, sep );
        ok = ( new != NULL );
    }

    if( ok ) {
        ok = WInsertMenuEntry( einfo, new, FALSE );
    }

    if( !ok ) {
        if( new ) {
            WFreeMenuEntry( new );
        }
    }

    return ( ok );
}

Bool WInsertMenuEntry( WMenuEditInfo *einfo, WMenuEntry *new,
                       Bool reset_lbox )
{
    HWND        lbox;
    WMenuEntry  *parent;
    WMenuEntry  *entry;
    LRESULT     ret;
    int         new_kids;
    Bool        ok;
    Bool        is_popup;
    Bool        insert_before;
    Bool        insert_subitems;

    entry = NULL;
    parent = NULL;
    is_popup = FALSE;

    ok = ( einfo && einfo->edit_dlg && new );

    if( ok ) {
        insert_before = einfo->insert_before;
        insert_subitems = insert_before || einfo->insert_subitems;
        new_kids = WCountMenuChildren( new->child );
        if( new_kids == 0 ) {
            reset_lbox = FALSE;
        }
        lbox = GetDlgItem( einfo->edit_dlg, IDM_MENUEDLIST );
        ok = ( lbox != NULL );
    }

    if( ok ) {
        ret = SendMessage ( lbox, LB_GETCURSEL, 0, 0 );
        if( insert_before ) {
            if( ret == 0 ) {
                ret = LB_ERR;
            } else if( ret != LB_ERR ) {
                ret = ret - 1;
            }
        }
        if( ret != LB_ERR ) {
            entry = (WMenuEntry *)
                SendMessage( lbox, LB_GETITEMDATA, (WPARAM) ret, 0 );
        } else {
            ret = -1;
        }
    }

    if( ok ) {
        if( entry ) {
            ret += 1;
            if( entry->item->IsPopup ) {
                insert_subitems = TRUE;
            }
            if( !insert_subitems ) {
                ret += WCountMenuChildren( entry->child );
            }
            parent = entry->parent;
            is_popup = ( entry->item->IsPopup && insert_subitems );
        } else {
            ret = ret + 1;
        }
        ok = WInsertEntryIntoMenu( einfo, entry, parent, new, is_popup );
    }

    if( ok ) {
        einfo->info->modified = TRUE;
        if( reset_lbox ) {
            ok = WInitEditWindowListBox( einfo );
        } else {
            ok = WAddEditWinLBoxEntry( lbox, new, (int) ret );
        }
    }

    if( ok ) {
        ok = ( SendMessage( lbox, LB_SETCURSEL, (int)ret, 0 ) != LB_ERR );
        if( ok ) {
            einfo->current_entry = NULL;
            einfo->current_pos   = -1;
            WHandleSelChange( einfo );
        }
    }

    if( ok ) {
        SetFocus( GetDlgItem( einfo->edit_dlg, IDM_MENUEDTEXT ) );
        SendDlgItemMessage( einfo->edit_dlg, IDM_MENUEDTEXT, EM_SETSEL, GET_EM_SETSEL_MPS( 0, -1 ) );
    }

    return ( ok );
}

Bool WAddMenuEntriesToLBox( HWND lbox, WMenuEntry *entry, int *pos )
{
    Bool        ok;

    ok = ( ( lbox != (HWND)NULL ) && pos );

    while ( entry && ok ) {
        ok = WAddEditWinLBoxEntry ( lbox, entry, *pos );
        *pos += 1;
        if( ok && entry->item->IsPopup ) {
            ok = WAddMenuEntriesToLBox ( lbox, entry->child, pos );
        }
        entry = entry->next;
    }

    return( ok );
}

Bool WAddEditWinLBoxEntry ( HWND lbox, WMenuEntry *entry, int pos )
{
    Bool        ok;
    char        *text;
    char        *lbtext;
    char        *lbtext1;
    int         depth;
    int         tlen;

    lbtext = NULL;

    ok = ( ( lbox != (HWND)NULL ) && entry );

    if ( ok ) {
        depth = WGetMenuEntryDepth ( entry );
        ok = ( depth != -1 );
    }

    if ( ok ) {
        if( entry->item->Item.Normal.ItemFlags & MENU_SEPARATOR ) {
            text = "SEPARATOR";
            tlen = 14;
        } else {
            text = WGETMENUITEMTEXT(entry->item);
            if( text == NULL ) {
                text = NULL_STRING;
            }
            tlen = strlen (text) + 1;
        }
        lbtext = (char *) WMemAlloc( depth*DEPTH_MULT + tlen + 14 );
        ok = ( lbtext != NULL );
    }

    if( ok ) {
        memset( lbtext, ' ', depth*DEPTH_MULT );
        lbtext[depth*DEPTH_MULT] = '\0';
        if( entry->item->Item.Normal.ItemFlags & MENU_POPUP ) {
            strcat( lbtext, "POPUP    \"" );
        } else if( entry->item->Item.Normal.ItemFlags & MENU_SEPARATOR ) {
            strcat( lbtext, "MENUITEM    " );
        } else {
            strcat( lbtext, "MENUITEM    \"" );
        }
        strcat( lbtext, text );
        if( !( entry->item->Item.Normal.ItemFlags & MENU_SEPARATOR ) ) {
            strcat( lbtext, "\"" );
        }
        lbtext1 = WConvertStringFrom( lbtext, "\t\x8\"\\", "ta\"\\" );
        ok = WInsertLBoxWithStr( lbox, pos, lbtext, entry );
    }

    if( lbtext1 ) {
        WMemFree( lbtext1 );
    }

    if( lbtext ) {
        WMemFree( lbtext );
    }

    return( ok );
}

WMenuEntry *WCreateNewMenuEntry( WMenuEditInfo *einfo, Bool popup, Bool sep )
{
    WMenuEntry  *new;
    char        *text;
    char        *symbol;
    MenuFlags   flags;
    uint_16     id;

    text = NULL;
    symbol = NULL;
    flags = 0;

    WGetEditWindowText( einfo->edit_dlg, &text );
    WGetEditWindowID( einfo->edit_dlg, &symbol, &id,
                      einfo->info->symbol_table, FALSE );
    if( id == 0 ) {
        id = DEFAULT_MENU_ID;
    }
    WGetEditWindowFlags( einfo->edit_dlg, &flags );
    flags &= ~( MENU_POPUP | MENU_SEPARATOR );

    new = (WMenuEntry *) WMemAlloc( sizeof(WMenuEntry) );

    if( new ) {
        memset( new, 0, sizeof(WMenuEntry) );
        new->is32bit = einfo->menu->is32bit;
        new->item = ResNewMenuItem();
        if( popup ) {
            new->item->IsPopup = TRUE;
            new->item->Item.Popup.ItemFlags = flags | MENU_POPUP;
            if( text ) {
                new->item->Item.Popup.ItemText = WStrDup( text );
            } else {
                new->item->Item.Popup.ItemText =
                    WAllocRCString( W_MENUPOPUP );
            }
        } else {
            if( sep ) {
                new->item->Item.Normal.ItemFlags = MENU_SEPARATOR;
            } else {
                new->item->Item.Normal.ItemID = id;
                new->item->Item.Normal.ItemFlags = flags;
                if( text ) {
                    new->item->Item.Normal.ItemText = WStrDup(text);
                } else {
                    new->item->Item.Normal.ItemText =
                        WAllocRCString( W_MENUITEM );
                }
            }
        }
    }

    if( !popup && !sep ) {
        new->symbol = symbol;
    } else {
        if( symbol ) {
            WMemFree( symbol );
        }
    }

    if( text ) {
        WMemFree( text );
    }

    return( new );
}
