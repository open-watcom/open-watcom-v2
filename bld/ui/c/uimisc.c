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

void UIAPI uiscreeninit( VSCREEN *vs, SAREA *area, screen_flags flags )
/*********************************************************************/
{
    vs->event = EV_NO_EVENT;
    vs->flags = flags;
    vs->cursor_col = 0;
    vs->cursor_row = 0;
    vs->cursor_type = C_OFF;
    vs->title = NULL;
    vs->dynamic_title = false;
    if( area == NULL ) {
        vs->area.row = 0;
        vs->area.col = 0;
        vs->area.height = 0;
        vs->area.width = 0;
    } else {
        vs->area = *area;
    }
}

VSCREEN * intern uiopen( SAREA *area, const char *title, screen_flags flags )
/***************************************************************************/
{
    VSCREEN             *vs;

    vs = uimalloc( sizeof( VSCREEN ) );
    if( vs == NULL ) {
        return( vs );
    }
    uiscreeninit( vs, area, flags );
    if( title != NULL ) {
        unsigned    len;
        char        *str;

        len = strlen( title );
        str = uimalloc( len + 1 );
        memcpy( str, title, len );
        str[len] = '\0';
        vs->title = str;
        vs->dynamic_title = true;
    }
    uivopen( vs );
    return( vs );
}

void intern uiclose( VSCREEN *vs )
/********************************/
{
    uivclose( vs );
    if( vs->dynamic_title )
        uifree( (void *)vs->title );
    uifree( vs );
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

void UIAPI uinocursor( VSCREEN *vs )
/**********************************/
{
    vs->cursor_type = C_OFF;
}

void UIAPI uicursor( VSCREEN *vs, CURSORORD crow, CURSORORD ccol, CURSOR_TYPE ctype )
/***********************************************************************************/
{
    vs->cursor_type = ctype;
    vs->cursor_row = crow;
    vs->cursor_col = ccol;
}
