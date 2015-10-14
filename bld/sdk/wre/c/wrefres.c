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
#include "wreglbl.h"
#include "wrefres.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* type definition                                                          */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static bool WREResNodeContainsLangNode( WResResNode *, WResLangNode * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

WResResNode *WREFindResNodeFromLangNode( WResTypeNode *tnode, WResLangNode *lnode )
{
    WResResNode *rnode;

    if( tnode == NULL || lnode == NULL ) {
        return( NULL );
    }

    rnode = tnode->Head;

    while( rnode != NULL ) {
        if( WREResNodeContainsLangNode( rnode, lnode ) ) {
            return( rnode );
        }
        if( rnode == tnode->Tail ) {
            break;
        }
        rnode = rnode->Next;
    }

    return( NULL );
}

bool WREResNodeContainsLangNode( WResResNode *rnode, WResLangNode *lnode )
{
    WResLangNode *ln;

    if( rnode == NULL || lnode == NULL ) {
        return( FALSE );
    }

    ln = rnode->Head;

    while( ln != NULL ) {
        if( ln == lnode ) {
            return( TRUE );
        }
        ln = ln->Next;
    }

    return( FALSE );
}
