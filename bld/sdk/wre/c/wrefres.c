/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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

WResResNode *WREFindResNodeFromLangNode( WResTypeNode *typenode, WResLangNode *langnode )
{
    WResResNode *resnode;

    if( typenode == NULL || langnode == NULL ) {
        return( NULL );
    }

    for( resnode = typenode->Head; resnode != NULL; resnode = resnode->next ) {
        if( WREResNodeContainsLangNode( resnode, langnode ) ) {
            return( resnode );
        }
        if( resnode == typenode->Tail ) {
            break;
        }
    }

    return( NULL );
}

bool WREResNodeContainsLangNode( WResResNode *resnode, WResLangNode *langnode )
{
    WResLangNode *langnode2;

    if( resnode == NULL || langnode == NULL ) {
        return( false );
    }

    for( langnode2 = resnode->Head; langnode2 != NULL; langnode2 = langnode2->next ) {
        if( langnode2 == langnode ) {
            return( true );
        }
    }

    return( false );
}
