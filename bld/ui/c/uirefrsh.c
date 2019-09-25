/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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


/*
    _uirefresh UI internal function is used directly by OW Debugger.
    Don't use it directly in UI project, call uirefresh instead.
*/

#include "uidef.h"
#include "uirefrhk.h"
#include "uidebug.h"


typedef struct update_area {
    unsigned        start;
    unsigned        end;
} update_area;


static void _dorefresh( update_area *total, SAREA area, UI_WINDOW *wptr, UI_WINDOW *cover )
/*****************************************************************************************/
{
    int             i;
    SAREA           areas[5];
    unsigned        start;
    unsigned        end;

    if( cover == NULL ) {
        start = area.row * UIData->width + area.col;
        end = start + ( area.height - 1 ) * UIData->width + area.width;
        if( total->start > start )
            total->start = start;
        if( total->end < end )
            total->end = end;
        if( wptr->update_func != NULL ) {
            (*wptr->update_func)( area, wptr->update_parm );
        }
    } else {
        dividearea( area, cover->area, areas );
        for( i = 1; i < 5; ++i ) {
            if( areas[i].height > 0 ) {
                _dorefresh( total, areas[i], wptr, cover->prev );
            }
        }
    }
}

#if defined( __DOS__ ) || defined( __WINDOWS__ )
void UIHOOK _uirefresh( void )
#else
void UIAPI  uirefresh( void )
#endif
/****************************/
{
    UI_WINDOW       *wptr;
    update_area     total;
    SAREA           area;
    uisize          start;
    uisize          end;

    _uicheckuidata();

    total.start = 0;
    total.end = 0;
    for( wptr = UIData->area_tail; wptr != NULL; wptr = wptr->prev ) {
        if( wptr->dirty_area.height > 0 ) {
            _dorefresh( &total, wptr->dirty_area, wptr, wptr->prev );
            wptr->dirty_area.height = 0;
        }
    }
    if( total.start < total.end ) {
        start = total.start / UIData->width;
        end = ( total.end - 1 ) / UIData->width;
        area.row = start;
        area.col = 0;
        area.height = end - start + 1;
        area.width = UIData->width;
        physupdate( &area );
    }
    uionmouse();               /* redisplay mouse cursor (if it was on) */
}
