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


#include "precomp.h"

#include "fmedit.def"

#include "wdeglbl.h"
#include "wdeactn.h"
#include "wdeobjid.h"
#include "wderesin.h"
#include "wderes.h"
#include "wdelist.h"
#include "wdezordr.h"

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
static Bool     WdeReorderObjectWindows( LIST * );
static Bool     WdeFindObjectsInRect( RECT *, LIST **, LIST * );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

void WdeSOP( OBJPTR obj, OBJPTR parent )
{
    LIST       *ilist, *tlist, *clist;
    WdeResInfo *info;
    RECT        orect;
    OBJPTR      sib;
    OBJ_ID      id;
    Bool        clear;
    POINT       origin;

    info = WdeGetCurrentRes();
    if( info == NULL ) {
        return;
    }
    GetClientRect( info->edit_win, &orect );

    GetOffset( &origin );
    OffsetRect( &orect, origin.x, origin.y );

    if( parent == NULL ) {
        GetObjectParent( obj, &parent );
        if( parent == NULL ) {
            return;
        }
    }

    Forward( parent, GET_SUBOBJ_LIST, &tlist, NULL );

    if( tlist != NULL && WdeFindObjectsInRect( &orect, &ilist, tlist ) && ilist != NULL ) {
        clist = NULL;
        tlist = NULL;
        for( ; ilist != NULL; ilist = ListConsume( ilist ) ) {
            sib = ListElement( ilist );
            if( (Forward( sib, IS_OBJECT_CLEAR, &clear, NULL ) && clear) ||
                (Forward( sib, IDENTIFY, &id, NULL ) && id == DIALOG_OBJ) ) {
                WdeInsertObject( &clist, sib );
            } else {
                WdeInsertObject( &tlist, sib );
            }
        }
        if( clist != NULL ) {
            WdeListConcat( &tlist, clist, 0 );
            ListFree( clist );
        }
        if( tlist != NULL ) {
            WdeReorderObjectWindows( tlist );
            ListFree( tlist );
        }
    }
}

Bool WdeReorderObjectWindows( LIST *l )
{
    LIST    *o;
    OBJPTR  child;
    HWND    win;
    HWND    last_win;
    HDWP    h;
    int     count;

    if( l == NULL ) {
        return( FALSE );
    }

    count = ListCount( l );

    h = BeginDeferWindowPos( count );

    if( h == NULL ) {
        return( FALSE );
    }

    last_win = HWND_TOP;
    for( o = l; o != NULL; o = ListNext( o ) ) {
        child = ListElement( o );
        Forward( child, GET_WINDOW_HANDLE, &win, NULL );
        if( win != NULL ) {
            h = DeferWindowPos( h, win, last_win, 0, 0, 0, 0,
                                SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );
            if( h == NULL ) {
                return( FALSE );
            }
            last_win = win;
        }
    }

    return( EndDeferWindowPos( h ) );
}

Bool WdeFindObjectsInRect( RECT *r, LIST **obj_list, LIST *olist )
{
    OBJPTR   child;
    RECT     child_rect;
    RECT     irect;

    *obj_list = NULL;

    for( ; olist != NULL; olist = ListNext( olist ) ) {
        child = ListElement( olist );
        Location( child, &child_rect );
        if( IntersectRect( &irect, &child_rect, r ) ) {
            WdeInsertObject( obj_list, child );
        }
    }

    return( *obj_list != NULL );
}
