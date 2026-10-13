/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "wreglbl.h"
#include <io.h>
#include <limits.h>
#include "wreres.h"
#include "wrenames.h"
#include "wreftype.h"
#include "wrefres.h"
#include "wremsg.h"
#include "wregcres.h"
#include "wre.rh"
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

char *WREGetCopyResData( WRECurrentResInfo *curr )
{
    char       *rdata;

    rdata = NULL;
    if( curr != NULL && curr->info != NULL && curr->langnode != NULL ) {
        rdata = WRAllocCopyResData( curr->info->info, curr->langnode );
    }

    return( rdata );
}

bool WREGetCurrentResource( WRECurrentResInfo *curr )
{
    WRETypeName *tn;
    bool        ok;
    HWND        resLbox;
    LRESULT     pos;

    ok = (curr != NULL);

    if( ok ) {
        curr->typenode = NULL;
        curr->resnode = NULL;
        curr->langnode = NULL;
        curr->info = WREGetCurrentRes();
        ok = (curr->info != NULL && curr->info->info != NULL && curr->info->info->dir != NULL);
    }

    if( ok ) {
        tn = WREGetTypeNameFromRT( curr->info->current_type );
        ok = (tn != NULL);
    }

    if( ok ) {
        curr->typenode = WREFindTypeNode( curr->info->info->dir, tn->type, NULL );
        ok = (curr->typenode != NULL);
    }

    if( ok ) {
        resLbox = GetDlgItem( curr->info->info_win, IDM_RNRES );
        ok = (resLbox != (HWND)NULL);
    }

    if( ok ) {
        pos = SendMessage( resLbox, LB_GETCURSEL, 0, 0 );
        ok = (pos != LB_ERR);
    }

    if( ok ) {
        curr->langnode = (WResLangNode *)SendMessage( resLbox, LB_GETITEMDATA, (WPARAM)pos, 0 );
        ok = (curr->langnode != NULL);
    }

    if( ok ) {
        curr->resnode = WREFindResNodeFromLangNode( curr->typenode, curr->langnode );
        ok = (curr->resnode != NULL);
    }

    return( ok );
}
