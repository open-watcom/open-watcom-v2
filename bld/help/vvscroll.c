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


#include "stdui.h"


void vvscroll( win, use, scroll )
    VSCREEN             *win;
    SAREA               use;
    int                 scroll;
{
    SAREA               area;
    ORD                 newrow;

    area.width = use.width;
    area.col = use.col;
    if( scroll > 0  &&  scroll < use.height ){
        area.height = use.height - scroll;
        area.row = use.row + scroll;
        newrow = use.row;
    } else if( scroll < 0  &&  -scroll < use.height ){
        area.height = use.height + scroll;
        area.row = use.row;
        newrow = use.row - scroll;
    } else {
        area.height = 0;
        area.row = 0;
        newrow = use.row + use.height;
    }
    if( area.height > 0 ){
        uivmoveblock( win, area, -scroll, 0 );
    }
    if( newrow == use.row ){
        area.row = newrow + area.height;
    } else {
        area.row = use.row;
    }
    area.height = use.height - area.height;
    if( area.height > 0 ){
        uivfill( win, area, UIData->attrs[ATTR_NORMAL], ' ' );
    }
}

