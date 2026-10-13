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
static int      WRFreeResNodes( WResTypeNode *typenode );
static int      WRFreeLangNodes( WResResNode *resnode );

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

bool WRAPI WRRemoveLangNodeFromDir( WResDir dir, WResTypeNode **typenode,
                                       WResResNode **resnode, WResLangNode **langnode )
{
    if( dir == NULL || typenode == NULL || *typenode == NULL || resnode == NULL ||
        *resnode == NULL || langnode == NULL || *langnode == NULL ) {
        return( false );
    }

    if( (*resnode)->Head == (*resnode)->Tail ) {
        if( (*typenode)->Head == (*typenode)->Tail ) {
            if( dir->Head == dir->Tail ) {
                dir->Head = NULL;
                dir->Tail = NULL;
            } else {
                if( dir->Head == *typenode ) {
                    dir->Head = (*typenode)->next;
                }
                if( dir->Tail == *typenode ) {
                    dir->Tail = (*typenode)->prev;
                }
                if( (*typenode)->prev != NULL ) {
                    (*typenode)->prev->next = (*typenode)->next;
                }
                if( (*typenode)->next != NULL ) {
                    (*typenode)->next->prev = (*typenode)->prev;
                }
            }
            MemFree( *typenode );
            *typenode = NULL;
        } else {
            if( (*typenode)->Head == *resnode ) {
                (*typenode)->Head = (*resnode)->next;
            }
            if( (*typenode)->Tail == *resnode ) {
                (*typenode)->Tail = (*resnode)->prev;
            }
            if( (*resnode)->prev != NULL ) {
                (*resnode)->prev->next = (*resnode)->next;
            }
            if( (*resnode)->next != NULL ) {
                (*resnode)->next->prev = (*resnode)->prev;
            }
        }
        MemFree( *resnode );
        *resnode = NULL;
    } else {
        if( (*resnode)->Head == *langnode ) {
            (*resnode)->Head = (*langnode)->next;
        }
        if( (*resnode)->Tail == *langnode ) {
            (*resnode)->Tail = (*langnode)->prev;
        }
        if( (*langnode)->prev != NULL ) {
            (*langnode)->prev->next = (*langnode)->next;
        }
        if( (*langnode)->next != NULL ) {
            (*langnode)->next->prev = (*langnode)->prev;
        }
    }

    MemFree( *langnode );
    *langnode = NULL;

    if( *resnode != NULL ) {
        (*resnode)->Info.NumResources--;
    }

    if( *typenode != NULL ) {
        (*typenode)->Info.NumResources--;
    } else {
        dir->NumTypes--;
    }

    dir->NumResources--;

    return( true );
}

bool WRAPI WRRemoveTypeNodeFromDir( WResDir dir, WResTypeNode *typenode )
{
    if( dir == NULL || typenode == NULL ) {
        return( false );
    }

    if( dir->Head == typenode ) {
        dir->Head = dir->Head->next;
    }
    if( dir->Tail == typenode ) {
        dir->Tail = dir->Tail->prev;
    }
    if( typenode->next != NULL ) {
        typenode->next->prev = typenode->prev;
    }
    if( typenode->prev != NULL ) {
        typenode->prev->next = typenode->next;
    }

    dir->NumResources -= WRFreeResNodes( typenode );

    MemFree( typenode );

    dir->NumTypes--;

    return( true );
}

int WRFreeResNodes( WResTypeNode *typenode )
{
    WResResNode         *resnode;
    int                 count;

    count = 0;
    while( (resnode = typenode->Head) != NULL ) {
        typenode->Head = resnode->next;
        WRFreeLangNodes( resnode );
        MemFree( resnode );
        count++;
    }
    typenode->Tail = NULL;

    return( count );
}

int WRFreeLangNodes( WResResNode *resnode )
{
    WResLangNode        *langnode;
    int                 count;

    count = 0;
    while( (langnode = resnode->Head) != NULL ) {
        resnode->Head = langnode->next;
        MemFree( langnode );
        count++;
    }
    resnode->Tail = NULL;

    return( count );
}
