/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


 /********************************************************************
 *   Copyright (C) 1987, 1990 by PACE Computing Solutions Inc. All   *
 *   rights reserved. No part of this software may be reproduced     *
 *   in any form or by any means - graphic, electronic or            *
 *   mechanical, including photocopying, recording, taping           *
 *   or information storage and retrieval systems - except           *
 *   with the written permission of PACE Computing Solutions Inc.    *
 *********************************************************************
 * Modified   By               Reason
 * --------   --               ------
 * 90-03-20   Jim Graham       Initial implementation
 */
#include <ctype.h>
#include <string.h>

#include "uidef.h"
#include "uivedit.h"
#include "uiledit.h"


a_ui_edit       *UIEdit = NULL;

extern ui_event     LineEvents[];

static bool extend( unsigned n )
{
    char    *buf;

    buf = uirealloc( UIEdit->edit_buffer, n );
    if( buf == NULL )
        return( false );
    buf[UIEdit->edit_eline.length] = '\0';
    UIEdit->edit_buffer = buf;
    UIEdit->edit_eline.buffer = buf;
    UIEdit->edit_eline.length = n;                       /* so blanks will get padded on */
    UIEdit->edit_eline.update = true;
    return( true );
}

static unsigned trim( char *s )
{
    char                *t;

    t = s + UIEdit->edit_eline.length;
    while( t > s && (t[-1] == ' ' || t[-1] == '\0') )
        --t;
    return( t - s );
}

void uiedittrim( char *s )
{
    UIEdit->edit_eline.length = trim( s );
}

void uieditmarking( bool set, unsigned anchor )
{
    UIEdit->edit_eline.mark_attr = UIData->attrs[ATTR_MARK_NORMAL];
    if( set ) {
        if( !UIEdit->edit_eline.marking ) {
            UIEdit->edit_eline.marking = true;
            UIEdit->edit_eline.update  = true;
            UIEdit->edit_eline.mark_anchor = anchor;
        }
    } else {
        UIEdit->edit_eline.marking = false;
    }
}

a_ui_edit *uibegedit( VSCREEN *vs, ORD row, ORD col, unsigned len,
                    ATTR attr, char *buffer, unsigned used,
                    unsigned i, unsigned max, bool auto_clear,
                    unsigned scroll, bool invisible )
{
    unsigned        l;
    a_ui_edit       *edit;

    edit = uimalloc( sizeof( a_ui_edit ) );
    if( edit == NULL ) {
        return( NULL );
    }
    edit->edit_maxlen = 0;
    edit->next = UIEdit;
    UIEdit = edit;

    if( max > 0 ) {
        if( i >= max ) {
            i = max - 1;
        }
//          JDG - 90/01/14 - truncated data in fixed length fields
//          if( max < used ) {
//              used = max;
//          }
        l = max;                                    /* fixed length buffer  */
        UIEdit->edit_maxlen = max;
    } else {
        l = used + 1;                       /* must have at least one slack */
        if( i >= l ) {
            l = i + 1;
        }
    }
    UIEdit->edit_vs = vs;
    UIEdit->edit_buffer = buffer;
    uiveditinit( vs, &UIEdit->edit_eline, UIEdit->edit_buffer, used, row, col, len );
    UIEdit->edit_eline.index = i;
    UIEdit->edit_eline.invisible = invisible;
    UIEdit->edit_eline.length = used;

    if( i < scroll ) {
        scroll = i;
    } else if( ( i > scroll + UIEdit->edit_eline.fldlen ) && ( max == 0 ) ) {
        scroll = i - UIEdit->edit_eline.fldlen + 1;
    }
    UIEdit->edit_eline.scroll = scroll;
    UIEdit->edit_eline.attr = attr;
    UIEdit->edit_eline.auto_clear = auto_clear;
    if( max > 0 && UIEdit->edit_eline.fldlen > UIEdit->edit_maxlen ) {
        UIEdit->edit_eline.fldlen = UIEdit->edit_maxlen;
    }
    if( l > UIEdit->edit_eline.length ) {
        extend( l );
    }
    return( UIEdit );
}

void uieditpushlist( void )
{
    static ui_event mousepress[] = {
        __rend__,
        EV_MOUSE_PRESS,
        __end__
    };

    uipushlist( LineEvents );
    uipushlist( mousepress );
}

void uieditpoplist( void )
{
    uipoplist( /* mousepress */ );
    uipoplist( /* LineEvents */ );
}

static bool mouse( int *row, int *col )
{
    return( uimousepos( UIEdit->edit_vs, row, col ) == UIEdit->edit_vs
            && *row == UIEdit->edit_eline.row
            && *col >= UIEdit->edit_eline.col
            && *col - UIEdit->edit_eline.col < UIEdit->edit_eline.fldlen );
}

ui_event uiledit( ui_event ui_ev )
{
    static ui_event full[] = {
        __rend__,
        EV_BUFFER_FULL,
        __end__
    };

    unsigned    before;
    int         row, col;
    unsigned    i;
    ui_event    new_ui_ev;

    if( UIEdit->edit_maxlen == 0 ) {
        uipushlist( full );
    }
    before = UIEdit->edit_eline.index;
    ui_ev = uiveditevent( UIEdit->edit_vs, &UIEdit->edit_eline, ui_ev );
    if( ui_ev != EV_NO_EVENT )
        UIEdit->edit_eline.update = true; // causes lots of flashing!
    if( UIEdit->edit_maxlen == 0 ) {
        uipoplist( /* full */ );
    }

    new_ui_ev = EV_NO_EVENT;
    switch( ui_ev ) {
    case EV_BUFFER_FULL:
        if( !extend( UIEdit->edit_eline.length + 10 ) ) {
            UIEdit->edit_eline.index = before;       // set cursor back to correct pos'n
        }
        break;
    case EV_MOUSE_PRESS:
        if( mouse( &row, &col ) ) {
            i = UIEdit->edit_eline.scroll + col - UIEdit->edit_eline.col;
            if( i > trim( UIEdit->edit_buffer ) ) {
                i = trim( UIEdit->edit_buffer );
            }
            if( UIEdit->edit_eline.index != i ) {
                if( UIEdit->edit_maxlen > 0 ) {
                    if( i < UIEdit->edit_maxlen ) {
                        UIEdit->edit_eline.update = true;
                        UIEdit->edit_eline.index = i;
                    }
                } else {
                    UIEdit->edit_eline.update = true;
                    UIEdit->edit_eline.index = i;
                }
            }
        }
        if( !uiinlists( ui_ev ) )
            break;
        /* fall through */
    default:
        new_ui_ev = ui_ev;
    }
    return( new_ui_ev );
}

void uieditinsert( char *str, unsigned n )
{
    char        *ins;
    unsigned    before;

    if( UIEdit == NULL || n == 0 )
        return;
    UIEdit->edit_eline.update = true;
    UIEdit->edit_eline.dirty = true;
    if( UIEdit->edit_eline.auto_clear && UIEdit->edit_eline.index == 0 ) {
        UIEdit->edit_eline.length = 0;
        UIEdit->edit_eline.auto_clear = false;
    }
    before = UIEdit->edit_eline.length;
    if( extend( UIEdit->edit_eline.length + n ) ) {
        ins = &UIEdit->edit_buffer[UIEdit->edit_eline.index];
        memmove( ins + n, ins, before - UIEdit->edit_eline.index );
        memcpy( ins, str, n );
        UIEdit->edit_eline.index += n;
    }
}

unsigned uiendedit( void )
{
    unsigned            i;
    a_ui_edit           *edit;

    uiedittrim( UIEdit->edit_buffer );
    UIEdit->edit_vs = NULL;
    i = UIEdit->edit_eline.index - UIEdit->edit_eline.scroll;
    uiveditfini( UIEdit->edit_vs, &UIEdit->edit_eline );
    edit = UIEdit;
    UIEdit = UIEdit->next;
    uifree( edit );
    return( i );
}

bool uieditisdirty( void )
{
    if( UIEdit != NULL ) {
        uiedittrim( UIEdit->edit_buffer );
        return( UIEdit->edit_eline.dirty );
    } else {
        return( false );
    }
}

bool uieditautoclear( void )
{
    return( UIEdit != NULL && UIEdit->edit_eline.auto_clear );
}

void uieditcursor( unsigned i )
{
    UIEdit->edit_eline.index = i;
    UIEdit->edit_eline.update = true;
}

void uieditdirty( void )
{
    UIEdit->edit_eline.dirty = true;
}

void uieditclean( void )
{
    UIEdit->edit_eline.dirty = false;
}
