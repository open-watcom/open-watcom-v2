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


#include "commonui.h"
#include <string.h>
#include "watcom.h"
#include "wglbl.h"
#include "ldstr.h"
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

bool WInsertAccelEntry( WAccelEditInfo *einfo )
{
    HWND            lbox;
    bool            ok;
    WAccelEntry     *entry;
    WAccelEntry     *new;
    box_pos         pos;

    lbox = NULLHANDLE;
    ok = (einfo != NULL && einfo->edit_dlg != NULL);

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_ACCEDLIST );
        ok = (lbox != NULL);
    }

    new = NULL;
    entry = NULL;
    pos = -1;
    if( ok ) {
        pos = (box_pos)SendMessage( lbox, LB_GETCURSEL, 0, 0 );
        if( pos != LB_ERR ) {
            entry = (WAccelEntry *)SendMessage( lbox, LB_GETITEMDATA, pos, 0 );
        } else {
            pos = -1;
        }
        new = WCreateNewAccelEntry( einfo );
        ok = (new != NULL);
    }

    if( ok ) {
        ok = WInsertAccelTableEntry( einfo->tbl, entry, new );
    }

    if( ok ) {
        einfo->info->modified = true;
        ok = WAddEditWinLBoxEntry( einfo, new, pos + 1 );
    }

    if( ok ) {
        ok = (SendMessage( lbox, LB_SETCURSEL, pos + 1, 0 ) != LB_ERR);
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
            WRMemFree( new );
        }
    }

    return( ok );
}

bool WAddEditWinLBoxEntry( WAccelEditInfo *einfo, WAccelEntry *entry, box_pos pos )
{
    bool    ok;
    char    *lbtext;
    char    *keytext;
    char    idtext[35];
    uint_16 key, flags, id;
    size_t  klen, ilen;
    HWND    lbox;

    lbox = NULL;
    ok = (einfo != NULL && einfo->edit_dlg != NULL && entry != NULL);

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_ACCEDLIST );
        ok = (lbox != NULL);
    }

    id = 0;
    keytext = NULL;
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

    lbtext = NULL;
    ilen = 0;
    klen = 0;
    if( ok ) {
        if( entry->symbol != NULL ) {
            strncpy( idtext, entry->symbol, MAX_ID_CHARS );
            idtext[MAX_ID_CHARS] = '\0';
        } else {
            sprintf( idtext, "%u", (unsigned)id );
        }
        ilen = strlen( idtext );
        idtext[ilen] = '\t';
        ++ilen;
        idtext[ilen] = '\0';
        klen = strlen( keytext );
        lbtext = (char *)WRMemAlloc( klen + ilen + 2 );
        ok = (lbtext != NULL);
    }

    if( ok ) {
        memcpy( lbtext, idtext, ilen );
        lbtext[ilen] = ' ';
        memcpy( lbtext + ilen + 1, keytext, klen + 1 );
        ok = WInsertLBoxWithStr( lbox, pos, lbtext, entry );
    }

    if( lbtext != NULL ) {
        WRMemFree( lbtext );
    }

    return( ok );
}

WAccelEntry *WCreateNewAccelEntry( WAccelEditInfo *einfo )
{
    WAccelEntry *new;

    new = (WAccelEntry *)WRMemAlloc( sizeof( WAccelEntry ) );

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
