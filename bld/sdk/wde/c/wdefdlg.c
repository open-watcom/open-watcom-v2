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

#include "wdeglbl.h"
#include "wderesin.h"
#include "wdedebug.h"
#include "wdefdlg.h"

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

WdeResDlgItem *WdeFindDialogInResInfo ( WdeResInfo *res_info, int dlg )
{
    LIST          *dlist;

    dlist = res_info->dlg_item_list;

    for ( ; dlist && ( dlg != -1 ); dlist=ListNext(dlist), dlg-- ) {
        if ( !dlg ) {
            return ( ListElement ( dlist ) );
        }
    }

    return ( NULL );
}

signed int WdeFindObjectPosInDialogList ( WdeResInfo *res_info, OBJPTR obj )
{
    LIST          *dlist;
    WdeResDlgItem *ditem;
    signed int     count;

    if ( !res_info || !obj ) {
        return ( -1 );
    }

    count = 0;
    dlist = res_info->dlg_item_list;
    for ( ; dlist; dlist = ListNext ( dlist ), count++ ) {
        ditem = (WdeResDlgItem *) ListElement ( dlist );
        if ( ditem->object == obj ) {
            return ( count );
        }
    }

    return ( -1 );
}

