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
#include "uimenu.h"

SAREA *uisetscreenarea( SAREA *area, bool all, bool framed )
/**********************************************************/
{
    unsigned    height;

    area->col = framed;
    area->width = UIData->width - 2*framed;
    area->row = framed;
    area->height = UIData->height - 2*framed;
    if( !all ) {
        height = uimenuheight();
        area->row += height;
        area->height -= height;
    }
    return( area );
}


SAREA *uisetarea( SAREA *area, VSCREEN *s )
/*****************************************/
{
    area->row = 0;
    area->col = 0;
    area->height = s->area.height;
    area->width = s->area.width;
    return( area );
}

static void window_pos( ORD *start, ORD *size, int slack, int pos )
/*****************************************************************/
{
    ORD         bump;

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

void uiposition( SAREA *a, ORD h, ORD w, int rpos, int cpos, bool overmenus )
/***************************************************************************/
{
    uisetscreenarea( a, overmenus, TRUE );
    if( h > 0 ) {
        window_pos( &a->row, &a->height, a->height - h, rpos );
    }
    if( w > 0 ) {
        window_pos( &a->col, &a->width, a->width - w, cpos );
    }
}


VSCREEN *uiopen( SAREA *area, char *title, unsigned flags )
/********************************************************/
{
    VSCREEN             *s;

    s = uicalloc( 1, sizeof( VSCREEN ) );
    if( s == NULL ) {
        return( s );
    }
    s->area = *area;
    s->title = title;
    s->flags = flags;
    s->col = 0;
    s->row = 0;
    s->cursor = C_OFF;
    uivopen( s );
    return( s );
}


void uiclose( VSCREEN *s )
/************************/
{
    uivclose( s );
    uifree( s );
}

void uicntrtext( VSCREEN        *vs,
                 SAREA          *area,
                 ATTR           attr,
                 unsigned       length,
                 const char     *text )
/*************************************/
{
    ORD                 col;

    if( length > 0 ) {
        col = area->col;
        if( length < area->width ) {
            col += ( area->width - length ) / 2;
        } else {
            length = area->width;
        }
        uivtextput( vs, area->row, col, attr, text, length );
    }
}

void uinocursor( VSCREEN *vs )
/****************************/
{
    vs->cursor = C_OFF;
}

void uicursor( VSCREEN *vs,
               ORD      row,
               ORD      col,
               int      type )
/****************************/
{
    vs->cursor = type;
    vs->row = row;
    vs->col = col;
}
