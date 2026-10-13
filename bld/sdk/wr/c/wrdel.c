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


#include "wrglbl.h"
#include "wresall.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static int      WRFreeResNodes( WResTypeNode *tnode );
static int      WRFreeLangNodes( WResResNode *rnode );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

bool WRAPI WRRemoveLangNodeFromDir( WResDir dir, WResTypeNode **tnode,
                                       WResResNode **rnode, WResLangNode **lnode )
{
    if( dir == NULL || tnode == NULL || *tnode == NULL || rnode == NULL ||
        *rnode == NULL || lnode == NULL || *lnode == NULL ) {
        return( false );
    }

    if( (*rnode)->Head == (*rnode)->Tail ) {
        if( (*tnode)->Head == (*tnode)->Tail ) {
            if( dir->Head == dir->Tail ) {
                dir->Head = NULL;
                dir->Tail = NULL;
            } else {
                if( dir->Head == *tnode ) {
                    dir->Head = (*tnode)->next;
                }
                if( dir->Tail == *tnode ) {
                    dir->Tail = (*tnode)->prev;
                }
                if( (*tnode)->prev != NULL ) {
                    (*tnode)->prev->next = (*tnode)->next;
                }
                if( (*tnode)->next != NULL ) {
                    (*tnode)->next->prev = (*tnode)->prev;
                }
            }
            MemFree( *tnode );
            *tnode = NULL;
        } else {
            if( (*tnode)->Head == *rnode ) {
                (*tnode)->Head = (*rnode)->next;
            }
            if( (*tnode)->Tail == *rnode ) {
                (*tnode)->Tail = (*rnode)->prev;
            }
            if( (*rnode)->prev != NULL ) {
                (*rnode)->prev->next = (*rnode)->next;
            }
            if( (*rnode)->next != NULL ) {
                (*rnode)->next->prev = (*rnode)->prev;
            }
        }
        MemFree( *rnode );
        *rnode = NULL;
    } else {
        if( (*rnode)->Head == *lnode ) {
            (*rnode)->Head = (*lnode)->next;
        }
        if( (*rnode)->Tail == *lnode ) {
            (*rnode)->Tail = (*lnode)->prev;
        }
        if( (*lnode)->prev != NULL ) {
            (*lnode)->prev->next = (*lnode)->next;
        }
        if( (*lnode)->next != NULL ) {
            (*lnode)->next->prev = (*lnode)->prev;
        }
    }

    MemFree( *lnode );
    *lnode = NULL;

    if( *rnode != NULL ) {
        (*rnode)->Info.NumResources--;
    }

    if( *tnode != NULL ) {
        (*tnode)->Info.NumResources--;
    } else {
        dir->NumTypes--;
    }

    dir->NumResources--;

    return( true );
}

bool WRAPI WRRemoveTypeNodeFromDir( WResDir dir, WResTypeNode *tnode )
{
    if( dir == NULL || tnode == NULL ) {
        return( false );
    }

    if( dir->Head == tnode ) {
        dir->Head = dir->Head->next;
    }
    if( dir->Tail == tnode ) {
        dir->Tail = dir->Tail->prev;
    }
    if( tnode->next != NULL ) {
        tnode->next->prev = tnode->prev;
    }
    if( tnode->prev != NULL ) {
        tnode->prev->next = tnode->next;
    }

    dir->NumResources -= WRFreeResNodes( tnode );

    MemFree( tnode );

    dir->NumTypes--;

    return( true );
}

int WRFreeResNodes( WResTypeNode *tnode )
{
    WResResNode         *rnode;
    int                 count;

    count = 0;
    while( (rnode = tnode->Head) != NULL ) {
        tnode->Head = rnode->next;
        WRFreeLangNodes( rnode );
        MemFree( rnode );
        count++;
    }
    tnode->Tail = NULL;

    return( count );
}

int WRFreeLangNodes( WResResNode *rnode )
{
    WResLangNode        *lnode;
    int                 count;

    count = 0;
    while( (lnode = rnode->Head) != NULL ) {
        rnode->Head = lnode->next;
        MemFree( lnode );
        count++;
    }
    rnode->Tail = NULL;

    return( count );
}
