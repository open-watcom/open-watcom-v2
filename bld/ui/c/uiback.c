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


static void backblank_update_fn( SAREA area, void *dummy )
/********************************************************/
{
    uisize      row;

    /* unused parameters */ (void)dummy;

    for( row = area.row; row < area.row + area.height; ++row ) {
        bfill( &UIData->screen, row, area.col, UIData->attrs[ATTR_NORMAL], ' ', area.width );
    }
}


void UIAPI uirestorebackground( void )
/************************************/
{
    ATTR        hold;

    hold = UIData->attrs[ATTR_NORMAL];
    UIData->attrs[ATTR_NORMAL] = 0x07;
    uidirty( UIData->blank_window.area );
    uirefresh();
    UIData->attrs[ATTR_NORMAL] = hold;
}


void intern openbackground( void )
/********************************/
{
    UIData->blank_window.area.row = 0;
    UIData->blank_window.area.col = 0;
    UIData->blank_window.area.height = UIData->height;
    UIData->blank_window.area.width = UIData->width;
    UIData->blank_window.priority = P_BACKGROUND;
    UIData->blank_window.update_func = backblank_update_fn;
    UIData->blank_window.update_parm = NULL;
    openwindow( &UIData->blank_window );
}


void intern closebackground( void )
/*********************************/
{
    closewindow( &UIData->blank_window );
}
