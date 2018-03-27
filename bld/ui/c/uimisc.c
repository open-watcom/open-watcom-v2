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


#include <string.h>
#include "uidef.h"
#include "uimenu.h"

VSCREEN intern *uiopen( SAREA *area, const char *title, screen_flags flags )
/**************************************************************************/
{
    VSCREEN             *s;

    s = uimalloc( sizeof( VSCREEN ) );
    if( s == NULL ) {
        return( s );
    }
    s->event = EV_NO_EVENT;
    s->area = *area;
    s->flags = flags;
    s->col = 0;
    s->row = 0;
    s->cursor = C_OFF;
    s->title = NULL;
    s->dynamic_title = false;
    if( title != NULL ) {
        unsigned    len;
        char        *str;

        len = strlen( title );
        str = uimalloc( len + 1 );
        memcpy( str, title, len );
        str[len] = '\0';
        s->title = str;
        s->dynamic_title = true;
    }
    uivopen( s );
    return( s );
}

void intern uiclose( VSCREEN *s )
/*******************************/
{
    uivclose( s );
    if( s->dynamic_title )
        uifree( (void *)s->title );
    uifree( s );
}

void uicntrtext( VSCREEN *vs, SAREA *area, ATTR attr, unsigned field_len, const char *text )
/******************************************************************************************/
{
    ORD                 col;

    if( field_len > 0 ) {
        col = area->col;
        if( field_len < area->width ) {
            col += ( area->width - field_len ) / 2;
        } else {
            field_len = area->width;
        }
        uivtextput( vs, area->row, col, attr, text, field_len );
    }
}

void uinocursor( VSCREEN *vs )
/****************************/
{
    vs->cursor = C_OFF;
}

void uicursor( VSCREEN *vs, ORD row, ORD col, CURSOR_TYPE type )
/**************************************************************/
{
    vs->cursor = type;
    vs->row = row;
    vs->col = col;
}
