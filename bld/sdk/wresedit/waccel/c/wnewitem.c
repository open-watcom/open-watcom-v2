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
#include <string.h>
#include "watcom.h"
#include "wglbl.h"
#include "wmem.h"
#include "wstat.h"
#include "wsetedit.h"
#include "wedit.h"
#include "wvk2str.h"
#include "wnewitem.h"
#include "sys_rc.h"
#include "rcstr.gh"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define DEFAULT_ACC_ID  101

#define MAX_ID_CHARS    12
#define TAB_SIZE        8

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static WAccelEntry *WCreateNewAccelEntry( WAccelEditInfo * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

Bool WInsertAccelEntry( WAccelEditInfo *einfo )
{
    HWND            lbox;
    Bool            ok;
    WAccelEntry     *entry;
    WAccelEntry     *new;
    LRESULT         ret;

    new = NULL;

    ok = (einfo != NULL && einfo->edit_dlg != NULL);

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_ACCEDLIST );
        ok = (lbox != NULL);
    }

    if( ok ) {
        ret = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
        if( ret != LB_ERR ) {
            entry = (WAccelEntry *)SendMessage( lbox, LB_GETITEMDATA, (WPARAM)ret, 0 );
        } else {
            entry = NULL;
            ret = -1;
        }
        new = WCreateNewAccelEntry( einfo );
        ok = (new != NULL);
    }

    if( ok ) {
        ok = WInsertAccelTableEntry( einfo->tbl, entry, new );
    }

    if( ok ) {
        einfo->info->modified = TRUE;
        ok = WAddEditWinLBoxEntry( einfo, new, (int)ret + 1 );
    }

    if( ok ) {
        ok = (SendMessage( lbox, LB_SETCURSEL, (int)ret + 1, 0 ) != LB_ERR);
        if( ok ) {
            einfo->current_entry = NULL;
            einfo->current_pos = -1;
            WHandleSelChange( einfo );
        }
    }

    if( ok ) {
        SetFocus( GetDlgItem( einfo->edit_dlg, IDM_ACCEDKEY ) );
        SendDlgItemMessage( einfo->edit_dlg, IDM_ACCEDKEY, EM_SETSEL,
                            GET_EM_SETSEL_MPS( 0, -1 ) );
    }

    if( !ok ) {
        if( new != NULL ) {
            WMemFree( new );
        }
    }

    return( ok );
}

Bool WAddEditWinLBoxEntry( WAccelEditInfo *einfo, WAccelEntry *entry, int pos )
{
    Bool    ok;
    char    *lbtext;
    char    *keytext;
    char    idtext[35];
    uint_16 key, flags, id;
    int     klen, ilen;
    HWND    lbox;

    lbtext = NULL;

    ok = (einfo != NULL && einfo->edit_dlg != NULL && entry != NULL);

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_ACCEDLIST );
        ok = (lbox != NULL);
    }

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
        keytext = WGetKeyText( key, flags );
        ok = (keytext != NULL);
        if( !ok ) {
            WSetStatusByID( einfo->wsb, -1, W_INVALIDACCEL );
            memcpy( entry, &DefaultEntry, sizeof( WAccelEntry ) );
            key = entry->entry.Ascii;
            flags = entry->entry.Flags;
            id = (uint_16)entry->entry.Id;
            keytext = WGetKeyText( key, flags );
            ok = (keytext != NULL);
        }
    }

    if( ok ) {
        if( entry->symbol != NULL ) {
            strncpy( idtext, entry->symbol, MAX_ID_CHARS );
            idtext[MAX_ID_CHARS] = '\0';
        } else {
            utoa( (unsigned)id, idtext, 10 );
        }
        ilen = strlen( idtext );
        idtext[ilen] = '\t';
        ++ilen;
        idtext[ilen] = '\0';
        klen = strlen( keytext );
        lbtext = (char *)WMemAlloc( klen + ilen + 2 );
        ok = (lbtext != NULL);
    }

    if( ok ) {
        memcpy( lbtext, idtext, ilen );
        lbtext[ilen] = ' ';
        memcpy( lbtext + ilen + 1, keytext, klen + 1 );
        ok = WInsertLBoxWithStr( lbox, pos, lbtext, entry );
    }

    if( lbtext != NULL ) {
        WMemFree( lbtext );
    }

    return( ok );
}

WAccelEntry *WCreateNewAccelEntry( WAccelEditInfo *einfo )
{
    WAccelEntry *new;

    new = (WAccelEntry *)WMemAlloc( sizeof( WAccelEntry ) );

    if( new != NULL ) {
        memset( new, 0, sizeof( WAccelEntry ) );
        new->is32bit = einfo->tbl->is32bit;
        if( !WGetEditWindowKeyEntry( einfo, new, FALSE ) ) {
            if( new->is32bit ) {
                new->entry32.Ascii = 'A';
                new->entry32.Id = DEFAULT_ACC_ID;
            } else {
                new->entry.Ascii = 'A';
                new->entry.Id = DEFAULT_ACC_ID;
            }
        }
    }

    return( new );
}
