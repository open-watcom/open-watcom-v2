/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdio.h>
#include <io.h>
#include <limits.h>

#include "watcom.h"
#include "wreglbl.h"
#include "wreres.h"
#include "wrenames.h"
#include "wreftype.h"
#include "wrefres.h"
#include "wremsg.h"
#include "wregcres.h"
#include "wre_rc.h"
#include "wrdll.h"

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

void *WREGetCurrentResData( WRECurrentResInfo *curr )
{
    void       *rdata;

    rdata = NULL;
    if( curr != NULL && curr->info != NULL && curr->lang != NULL ) {
        rdata = WRCopyResData( curr->info->info, curr->lang );
    }

    return( rdata );
}

bool WREGetCurrentResource( WRECurrentResInfo *current )
{
    WRETypeName *tn;
    bool        ok;
    HWND        resLbox;
    int         index;

    ok = (current != NULL);

    if( ok ) {
        current->type = NULL;
        current->res = NULL;
        current->lang = NULL;
        current->info = WREGetCurrentRes();
        ok = (current->info != NULL && current->info->info != NULL &&
              current->info->info->dir != NULL);
    }

    if( ok ) {
        tn = WREGetTypeNameFromRT( current->info->current_type );
        ok = (tn != NULL);
    }

    if( ok ) {
        current->type = WREFindTypeNode( current->info->info->dir, tn->type, NULL );
        ok = (current->type != NULL);
    }

    if( ok ) {
        resLbox = GetDlgItem( current->info->info_win, IDM_RNRES );
        ok = (resLbox != (HWND)NULL);
    }

    if( ok ) {
        index = (int)SendMessage( resLbox, LB_GETCURSEL, 0, 0 );
        ok = (index != LB_ERR);
    }

    if( ok ) {
        current->lang = (WResLangNode *)SendMessage( resLbox, LB_GETITEMDATA, index, 0 );
        ok = (current->lang != NULL);
    }

    if( ok ) {
        current->res = WREFindResNodeFromLangNode( current->type, current->lang );
        ok = (current->res != NULL);
    }

    return( ok );
}
