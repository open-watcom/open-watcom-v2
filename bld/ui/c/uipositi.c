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


#include <string.h>
#include "uidef.h"
#include "uimenu.h"

SAREA *uisetarea( SAREA *area, VSCREEN *vs )
/******************************************/
{
    area->row = 0;
    area->col = 0;
    area->height = vs->area.height;
    area->width = vs->area.width;
    return( area );
}

SAREA *uisetscreenarea( SAREA *area, bool all, bool framed )
/**********************************************************/
{
    uisize      height;

    area->col = framed;
    area->width = 0;
    if( UIData->width > 2 * framed )
        area->width = UIData->width - 2 * framed;
    area->row = framed;
    area->height = 0;
    if( UIData->height > 2 * framed )
        area->height = UIData->height - 2 * framed;
    if( !all ) {
        height = uimenuheight();
        area->row += height;
        if( area->height > height ) {
            area->height -= height;
        } else {
            area->height = 0;
        }
    }
    return( area );
}

static void window_pos( ORD *start, uisize *size, uisize slack, int pos )
/***********************************************************************/
{
    uisize      bump;

    if( slack > 0 ) {
        if( pos == 0 ) {
            bump = slack / 2;
        } else if( pos > 0 ) {
            if( --pos > slack ) {
                pos = slack;
            }
            bump = pos;
        } else {
            pos = -pos;
            if( --pos > slack ) {
                pos = slack;
            }
            bump = slack - pos;
        }
        *start += bump;
        *size -= slack;
    }
}

void uiposition( SAREA *a, unsigned h, unsigned w, int rpos, int cpos, bool overmenus )
/*************************************************************************************/
{
    uisetscreenarea( a, overmenus, true );
    if( h > 0 && a->height > h ) {
        window_pos( &a->row, &a->height, a->height - h, rpos );
    }
    if( w > 0 && a->width > w ) {
        window_pos( &a->col, &a->width, a->width - w, cpos );
    }
}
