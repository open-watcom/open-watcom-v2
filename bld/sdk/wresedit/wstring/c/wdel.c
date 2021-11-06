/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "wglbl.h"
#include <ctype.h>
#include "wedit.h"
#include "wdel.h"
#include "sysall.rh"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

bool WDeleteStringEntry( WStringEditInfo *einfo )
{
    HWND         lbox;
    bool         ok;
    LRESULT      pos;

    ok = ( einfo != NULL && einfo->edit_dlg != NULL );

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_STREDLIST );
        ok = ( lbox != NULL );
    }

    if( ok ) {
        pos = SendMessage( lbox, LB_GETCURSEL, 0, 0 );
        ok = ( pos != LB_ERR );
    }

    if( ok ) {
        ok = WDeleteEditWinLBoxEntry( einfo, pos, TRUE );
    }

    return( ok );
}

bool WDeleteStringData( WStringEditInfo *einfo, WStringBlock *block,
                        uint_16 id, bool *bdel )
{
    bool        ok;

    ok = (einfo != NULL && einfo->tbl != NULL && block != NULL && bdel != NULL);

    if( ok ) {
        *bdel = false;
        if( block->block.String[id & 0xf] != NULL ) {
            WRMemFree( block->block.String[id & 0xf] );
            block->block.String[id & 0xf] = NULL;
            if( WIsBlockEmpty( block ) ) {
                ok = WRemoveStringBlock( einfo->tbl, block );
                *bdel = true;
            }
        }
    }

    return( ok );
}

bool WDeleteEditWinLBoxEntry( WStringEditInfo *einfo, LRESULT pos, bool free_it )
{
    HWND                lbox;
    bool                ok;
    bool                bdel;
    WStringBlock        *block;
    uint_16             string_id;
    LRESULT             count;

    ok = (einfo != NULL && einfo->edit_dlg != NULL && einfo->tbl != NULL);

    if( ok ) {
        lbox = GetDlgItem( einfo->edit_dlg, IDM_STREDLIST );
        ok = (lbox != NULL);
    }

    if( ok ) {
        count = SendMessage( lbox, LB_GETCOUNT, 0, 0 );
        ok = ( count != 0 && count != LB_ERR && pos < count );
    }

    if( ok ) {
        string_id = (uint_16 )SendMessage( lbox, LB_GETITEMDATA, (WPARAM)pos, 0 );
        block = WFindStringBlock( einfo->tbl, string_id );
        ok = (block != NULL);
    }

    if( ok ) {
        if( free_it ) {
            ok = WDeleteStringData( einfo, block, string_id, &bdel );
            if( bdel ) {
                block = NULL;
            }
        }
    }

    if( ok ) {
        einfo->info->modified = true;
        ok = (SendMessage( lbox, LB_DELETESTRING, (WPARAM)pos, 0 ) != LB_ERR);
    }

    if( ok ) {
        einfo->current_block = NULL;
        einfo->current_string = 0;
        einfo->current_pos = LB_ERR;
        if( pos > count - 2 )
            pos = count - 2;
        ok = (SendMessage( lbox, LB_SETCURSEL, (WPARAM)pos, 0 ) != LB_ERR);
        if( ok ) {
            WHandleSelChange( einfo );
        }
    }

    return( ok );
}
