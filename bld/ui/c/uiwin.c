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


#include "uidef.h"


static bool covered( SAREA area, UI_WINDOW *wptr )
/************************************************/
{
    int             i;
    bool            flag;
    SAREA           areas[ 5 ];

    dividearea( area, wptr->area, areas );
    flag = ( areas[ 0 ].height > 0 );
    if( wptr->prev != NULL ) {
        for( i = 1; i < 5; ++i ) {
            if( areas[ i ].height > 0 ) {
                flag |= covered( areas[ i ], wptr->prev );
            }
        }
    }
    return( flag );
}

static UI_WINDOW ** intern findspot( int priority, UI_WINDOW **prev )
/*******************************************************************/
/* find the ptr to ptr of the right spot in the UI window list
   for this window */
{
    UI_WINDOW           **win;
    UI_WINDOW           *this_win;
    UI_WINDOW           *our_prev;

    if( prev == NULL ) {
        prev = &our_prev;
    }
    *prev = NULL;
    for( win = &UIData->area_head;; ) {
        this_win = *win;
        if( this_win == NULL ) {
            break;
        }
        if( this_win->priority >= priority ) {
            break;
        }

        *prev = this_win;
        win = &this_win->next;
    }

    return( win );
}


static void insert( UI_WINDOW *wptr, int priority )
/*************************************************/
{
    UI_WINDOW           **spot;
    UI_WINDOW           *prev;

    spot = findspot( priority, &prev );

    wptr->next = *spot;
    if( *spot == NULL ) {
        UIData->area_tail = wptr;
    } else {
        (*spot)->prev = wptr;
    }
    wptr->prev = prev;
    *spot = wptr;
}


static void removewindow( UI_WINDOW *wptr )
/*****************************************/
{
    if( wptr->prev != NULL ) {
        wptr->prev->next = wptr->next;
    } else {
        UIData->area_head = wptr->next;
    }
    if( wptr->next != NULL ) {
        wptr->next->prev = wptr->prev;
        dirtynext( wptr->area, wptr->next );
    } else {
        UIData->area_tail = wptr->prev;
    }
}


bool intern openwindow( UI_WINDOW *wptr )
/***************************************/
{
    wptr->dirty_area = wptr->area;
    insert( wptr, wptr->priority );
    if( wptr->prev == NULL ) {
        return( false );
    } else {
        return( covered( wptr->area, wptr->prev ) );
    }
}


void intern closewindow( UI_WINDOW *wptr )
/****************************************/
{
    removewindow( wptr );
}


void intern movewindow( UI_WINDOW *wptr, ORD row, ORD col )
/******************************************************/
{
    if( wptr->next != NULL ) {
        dirtynext( wptr->area, wptr->next );
    }
    wptr->area.row = row;
    wptr->area.col = col;
    wptr->dirty_area = wptr->area;
}


void intern frontwindow( UI_WINDOW *wptr )
/****************************************/
{
    if( wptr->prev != NULL ) {
        if( wptr->prev->priority >= wptr->priority ) {
            removewindow( wptr );
            wptr->dirty_area = wptr->area;
            /* we really only need to dirty what is covered */
            /* using a similar algorithm to dirtynext */
            insert( wptr, wptr->priority );
        }
    }
}
