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


#include "guiwind.h"
#include "guizlist.h"

static gui_window *GUIHead = NULL;

static bool InList( gui_window *wnd )
{
    gui_window *curr;

    for( curr = GUIHead; curr != NULL; curr = curr->next ) {
        if( curr == wnd ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

/*
 * GUIDeleteFromList
 */

void GUIDeleteFromList( gui_window *wnd )
{
    gui_window  *curr;
    gui_window  *prev;

    prev = NULL;
    for( curr = GUIHead; curr != NULL; prev = curr, curr=curr->next ) {
        if( curr == wnd ) break;
    }
    if( curr != NULL ) {
        if( prev != NULL ) {
            prev->next = curr->next;
        } else {
            GUIHead = curr->next;
        }
    }
}

/*
 * GUIFrontOfList
 */

void GUIFrontOfList( gui_window *wnd )
{
    GUIDeleteFromList( wnd );
    wnd->next = GUIHead;
    GUIHead = wnd;
}

/*
 * GUIGetFront
 */

gui_window *GUIGetFront( void )
{
    return( GUIHead );
}

/*
 * GUIGetNextWindow
 */

gui_window *GUIGetNextWindow( gui_window *wnd )
{
    if( InList( wnd ) ) {
        return( wnd->next );
    }
    return( NULL );
}

bool GUIIsValidWindow( gui_window *wnd )
{
    gui_window  *curr;

    for( curr = GUIGetFront(); curr != NULL; curr = GUIGetNextWindow( curr ) ) {
        if( curr == wnd ) {
            return( TRUE );
        }
    }
    return( FALSE );
}
