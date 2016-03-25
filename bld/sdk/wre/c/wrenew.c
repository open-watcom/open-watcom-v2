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
#include <stdlib.h>
#include "watcom.h"
#include "wreglbl.h"
#include "wretoolb.h"
#include "wreftype.h"
#include "wrenames.h"
#include "wrenew.h"

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

bool WRENewResource( WRECurrentResInfo *curr, WResID *tname, WResID *rname,
                     uint_16 memflags, uint_32 offset, uint_32 length,
                     WResLangType *lang, bool *dup, uint_16 type,
                     bool new_type )
{
    bool   ok;

    ok = (curr != NULL && curr->info != NULL && tname != NULL && rname != NULL &&
          lang != NULL);

    if( ok && ( curr->info->info->dir == NULL ) ) {
        ok = ((curr->info->info->dir = WResInitDir()) != NULL);
    }

    if( ok ) {
        ok = !WResAddResource( tname, rname, memflags, offset, length,
                               curr->info->info->dir, lang, dup );
    }

    if( ok ) {
        ok = (dup == NULL || (dup != NULL && !*dup));
    }

    if( ok ) {
        if( new_type ) {
            curr->info->current_type = 0;
            ok = WREInitResourceWindow( curr->info, type );
        } else {
            WRESetResNamesFromType( curr->info, type, TRUE, rname, 0 );
        }
    }

    if( ok && new_type ) {
        curr->type = WREFindTypeNodeFromWResID( curr->info->info->dir, tname );
        ok = (curr->type != NULL);
    }

    if( ok ) {
        curr->res = WREFindResNodeFromWResID( curr->type, rname );
        ok = (curr->res != NULL);
    }

    if( ok ) {
        curr->lang = WREFindLangNodeFromLangType( curr->res, lang );
        ok = (curr->lang != NULL);
    }

    return( ok );
}
