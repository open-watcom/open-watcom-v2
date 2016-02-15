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

#define  Eline          (UIEdit->edit_eline)

a_ui_edit       *UIEdit = NULL;

extern EVENT LineEvents[];

static bool extend( unsigned n )
{
    char    *buf;

    buf = uirealloc( UIEdit->edit_buffer, n );
    if( buf == NULL ) {
        return( FALSE );
    } else {
        UIEdit->edit_buffer = buf;
    }
    *( UIEdit->edit_buffer + Eline.length ) = '\0';
    Eline.buffer = UIEdit->edit_buffer;
    Eline.length = n;                       /* so blanks will get padded on */
    Eline.update = TRUE;
    return( TRUE );
}

static unsigned trim( char *s )
{
    char                *t;

    t = s + Eline.length;
    while( t > s && (t[-1] == ' ' || t[-1] == '\0') ) --t;
    return( t - s );
}

void uiedittrim( char *s )
{
    Eline.length = trim( s );
}

void uieditmarking( bool set, unsigned anchor )
{
    Eline.mark_attr = UIData->attrs[ ATTR_MARK_NORMAL ];
    if( set ) {
        if( Eline.marking == FALSE ) {
            Eline.marking = TRUE;
            Eline.update  = TRUE;
            Eline.mark_anchor = anchor;
        }
    } else {
        Eline.marking = FALSE;
    }
}

a_ui_edit *uibegedit( VSCREEN *vs, ORD row, ORD col, ORD len,
                    ATTR attr, char *buffer, unsigned used,
                    unsigned i, unsigned max, bool auto_clear,
                    unsigned scroll, bool invisible )
{
    unsigned            l;
    a_ui_edit         *edit;

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
    UIEdit->edit_screen = vs;
    UIEdit->edit_buffer = buffer;
    uiveditinit( vs, &Eline, UIEdit->edit_buffer, used, row, col, len );
    Eline.index = i;
    Eline.invisible = invisible;
    Eline.length = used;

    if( i < scroll ) {
        scroll = i;
    } else if( ( i > scroll + Eline.fldlen ) && ( max == 0 ) ) {
        scroll = i - Eline.fldlen + 1;
    }
    Eline.scroll = scroll;
    Eline.attr = attr;
    Eline.auto_clear = auto_clear;
    if( max > 0  &&  Eline.fldlen > UIEdit->edit_maxlen ) {
        Eline.fldlen = UIEdit->edit_maxlen;
    }
    if( l > Eline.length ) {
        extend( l );
    }
    return( UIEdit );
}

void uieditpushlist( void )
{
    static EVENT mousepress[] = {
        EV_NO_EVENT,
        EV_MOUSE_PRESS,
        EV_NO_EVENT
    };

    uipushlist( LineEvents );
    uipushlist( mousepress );
}

void uieditpoplist( void )
{
    uipoplist();
    uipoplist();
}

static int mouse( int *row, int *col )
{
    return( uimousepos( UIEdit->edit_screen, row, col )
            == UIEdit->edit_screen
            && *row == Eline.row
            && *col >= Eline.col
            && *col < Eline.col + Eline.fldlen );
}

EVENT uiledit( EVENT ev )
{
    static EVENT full[] = {
        EV_NO_EVENT,
        EV_BUFFER_FULL,
        EV_NO_EVENT
    };

    unsigned    before;
    int         row, col, i;
    EVENT       new;

    if( UIEdit->edit_maxlen == 0 ) {
        uipushlist( full );
    }
    before = Eline.index;
    ev = uiveditevent( UIEdit->edit_screen, &Eline, ev );
    if( ev != EV_NO_EVENT ) Eline.update = TRUE; // causes lots of flashing!
    if( UIEdit->edit_maxlen == 0 ) {
        uipoplist();
    }

    new = EV_NO_EVENT;
    switch( ev ){
    case EV_BUFFER_FULL:
        if( !extend( Eline.length + 10 ) ) {
            Eline.index = before;       // set cursor back to correct pos'n
        }
        break;
    case EV_MOUSE_PRESS:
        if( mouse( &row, &col ) ) {
            i = Eline.scroll + col - Eline.col;
            if( i > trim( UIEdit->edit_buffer ) ) {
                i = trim( UIEdit->edit_buffer );
            }
            if( Eline.index != i ) {
                if( UIEdit->edit_maxlen > 0 ) {
                    if( i < UIEdit->edit_maxlen ) {
                        Eline.update = TRUE;
                        Eline.index = i;
                    }
                } else {
                    Eline.update = TRUE;
                    Eline.index = i;
                }
            }
        }
        if( !uiinlist( ev ) ) break;
        /* THIS CASE FALLS INTO THE DEFAULT */
    default:
        new = ev;
    }
    return( new );
}

void uieditinsert( char *str, unsigned n )
{
    char        *ins;
    unsigned    before;

    if( UIEdit == NULL || n == 0 ) return;
    Eline.update = TRUE;
    Eline.dirty = TRUE;
    if( Eline.auto_clear && Eline.index == 0 ) {
        Eline.length = 0;
        Eline.auto_clear = FALSE;
    }
    before = Eline.length;
    if( extend( Eline.length + n ) ) {
        ins = &UIEdit->edit_buffer[ Eline.index ];
        memmove( ins + n, ins, before - Eline.index );
        memcpy( ins, str, n );
        Eline.index += n;
    }
}

unsigned uiendedit( void )
{
    unsigned            i;
    a_ui_edit           *edit;

    uiedittrim( UIEdit->edit_buffer );
    UIEdit->edit_screen = NULL;
    i = Eline.index - Eline.scroll;
    uiveditfini( UIEdit->edit_screen, &Eline );
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
        return( FALSE );
    }
}

bool uieditautoclear( void )
{
    return( UIEdit != NULL && UIEdit->edit_eline.auto_clear );
}

void uieditcursor( unsigned i )
{
    UIEdit->edit_eline.index = i;
    UIEdit->edit_eline.update = TRUE;
}

void uieditdirty( void )
{
    UIEdit->edit_eline.dirty = TRUE;
}

void uieditclean( void )
{
    UIEdit->edit_eline.dirty = FALSE;
}
