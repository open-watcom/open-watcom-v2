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


#include <wwindows.h>
#include "watcom.h"
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

int WRAPI WRRemoveLangNodeFromDir( WResDir dir, WResTypeNode **tnode,
                                       WResResNode **rnode, WResLangNode **lnode )
{
    if( dir == NULL || tnode == NULL || *tnode == NULL || rnode == NULL ||
        *rnode == NULL || lnode == NULL || *lnode == NULL ) {
        return( FALSE );
    }

    if( (*rnode)->Head == (*rnode)->Tail ) {
        if( (*tnode)->Head == (*tnode)->Tail ) {
            if( dir->Head == dir->Tail ) {
                dir->Head = NULL;
                dir->Tail = NULL;
            } else {
                if( dir->Head == *tnode ) {
                    dir->Head = (*tnode)->Next;
                }
                if( dir->Tail == *tnode ) {
                    dir->Tail = (*tnode)->Prev;
                }
                if( (*tnode)->Prev != NULL ) {
                    (*tnode)->Prev->Next = (*tnode)->Next;
                }
                if( (*tnode)->Next != NULL ) {
                    (*tnode)->Next->Prev = (*tnode)->Prev;
                }
            }
            MemFree( *tnode );
            *tnode = NULL;
        } else {
            if( (*tnode)->Head == *rnode ) {
                (*tnode)->Head = (*rnode)->Next;
            }
            if( (*tnode)->Tail == *rnode ) {
                (*tnode)->Tail = (*rnode)->Prev;
            }
            if( (*rnode)->Prev != NULL ) {
                (*rnode)->Prev->Next = (*rnode)->Next;
            }
            if( (*rnode)->Next != NULL ) {
                (*rnode)->Next->Prev = (*rnode)->Prev;
            }
        }
        MemFree( *rnode );
        *rnode = NULL;
    } else {
        if( (*rnode)->Head == *lnode ) {
            (*rnode)->Head = (*lnode)->Next;
        }
        if( (*rnode)->Tail == *lnode ) {
            (*rnode)->Tail = (*lnode)->Prev;
        }
        if( (*lnode)->Prev != NULL ) {
            (*lnode)->Prev->Next = (*lnode)->Next;
        }
        if( (*lnode)->Next != NULL ) {
            (*lnode)->Next->Prev = (*lnode)->Prev;
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

    return( TRUE );
}

int WRAPI WRRemoveTypeNodeFromDir( WResDir dir, WResTypeNode *tnode )
{
    if( dir == NULL || tnode == NULL ) {
        return( FALSE );
    }

    if( dir->Head == tnode ) {
        dir->Head = dir->Head->Next;
    }
    if( dir->Tail == tnode ) {
        dir->Tail = dir->Tail->Prev;
    }
    if( tnode->Next != NULL ) {
        tnode->Next->Prev = tnode->Prev;
    }
    if( tnode->Prev != NULL ) {
        tnode->Prev->Next = tnode->Next;
    }

    dir->NumResources -= WRFreeResNodes( tnode );

    MemFree( tnode );

    dir->NumTypes--;

    return( TRUE );
}

int WRFreeResNodes( WResTypeNode *tnode )
{
    WResResNode         *oldnode;
    WResResNode         *currnode;
    int                 count;

    count = 0;
    currnode = tnode->Head;
    while( currnode != NULL ) {
        oldnode = currnode;
        currnode = currnode->Next;
        WRFreeLangNodes( oldnode );
        MemFree( oldnode );
        count++;
    }

    tnode->Head = NULL;
    tnode->Tail = NULL;

    return( count );
}

int WRFreeLangNodes( WResResNode *rnode )
{
    WResLangNode        *oldnode;
    WResLangNode        *currnode;
    int                 count;

    count = 0;
    currnode = rnode->Head;
    while( currnode != NULL ) {
        oldnode = currnode;
        currnode = currnode->Next;
        MemFree( oldnode );
        count++;
    }

    rnode->Head = NULL;
    rnode->Tail = NULL;

    return( count );
}
