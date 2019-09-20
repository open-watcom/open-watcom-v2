/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include "uidebug.h"

static UI_WINDOW *BadWindow = NULL;

static void bad_window( UI_WINDOW *wptr )
/***************************************/
{
    BadWindow = wptr;
}

void intern uicheckuidata( void )
/*******************************/
{
    UI_WINDOW *curr;
    UI_WINDOW *prev;

    if( UIData->area_head == NULL || UIData->area_tail == NULL ) {
        if( UIData->area_head != NULL || UIData->area_tail != NULL ) {
            bad_window( NULL );
        }
        return;
    }

    prev = NULL;
    for( curr = UIData->area_head; curr != NULL; curr = curr->next ) {
        if( curr->prev != prev ) {
            bad_window( curr );
        }
        prev = curr;
    }
    if( prev != UIData->area_tail ) {
        bad_window( prev );
    }
}
