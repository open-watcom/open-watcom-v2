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
#include "uivedit.h"
#include "uiedit.h"

#include "clibext.h"


ui_event LineEvents[] = {
    EV_FIRST_EDIT_CHAR, EV_LAST_EDIT_CHAR,
    EV_HOME,            EV_DELETE,
    __rend__,
    EV_RUB_OUT,
    EV_CTRL_END,
    EV_CTRL_HOME,
    EV_CTRL_CURSOR_LEFT,
    EV_CTRL_CURSOR_RIGHT,
    __end__
};


static void echoline( VSCREEN *vs, VEDITLINE *editline )
/******************************************************/
{
    SAREA       area;
    unsigned    start;
    unsigned    trim;

    if( editline->invisible ) {
        area.row = editline->row;
        area.col = editline->col;
        area.height = 1;
        area.width = editline->fldlen;
        uivfill( vs, area, editline->attr, ' ' );
        for( trim = editline->length; trim > editline->index; --trim ) {
            if( editline->buffer[trim - 1] != ' ' ) {
                break;
            }
        }
        area.width = editline->fldlen;
        if( area.width > trim - editline->scroll )
            area.width = trim - editline->scroll;
        uivfill( vs, area, editline->attr, '*' );
    } else {
        uitextfield( vs, editline->row, editline->col, editline->fldlen,
            editline->attr,
            editline->buffer + editline->scroll,
            editline->length - editline->scroll );
    }
    if( editline->marking ) {
        area.row = editline->row;
        area.height = 1;
        if( editline->mark_anchor < editline->index ) {
            start = editline->mark_anchor;
            area.width = editline->index - editline->mark_anchor;
        } else {
            start = editline->index;
            area.width = editline->mark_anchor - editline->index;
        }
        if( area.width + start >= editline->scroll && area.width > 0 ) {
            if( start > editline->scroll ) {
                area.col = editline->col + start - editline->scroll;
            } else {
                area.col = editline->col;
                area.width = area.width - editline->scroll + start;
            }
            if( area.width + area.col > editline->col + editline->fldlen ) {
                area.width = editline->fldlen + editline->col - area.col;
            }
            uivattribute( vs, area, editline->mark_attr );
        }
    }
}


ui_event UIAPI uiveditevent( VSCREEN *vs, VEDITLINE *editline, ui_event ui_ev )
/*****************************************************************************/
{
    int                     scroll;
    bool                    scrollable;
    bool                    growing;
    VBUFFER                 buffer;


    if( editline->update ) {
        if( vs->cursor_type == C_OFF ) {
            vs->cursor_type = C_NORMAL;
        }
        uipadblanks( editline->buffer, editline->length );
        vs->cursor_row = editline->row;
        scroll = editline->scroll;
        if( scroll > editline->index )
            scroll = editline->index;
        if( scroll < (int)( editline->index - editline->fldlen + 1 ) )
            scroll = editline->index - editline->fldlen + 1;
        editline->scroll = scroll;
        vs->cursor_col = editline->col + editline->index - editline->scroll;
        echoline( vs, editline );
//      uirefresh();                    not needed for QNX or DOS ??? jimg
        editline->update = false;
    }
    if( ui_ev > EV_NO_EVENT ) {
        if( !uiinlists( ui_ev ) ) {
            growing = uiinlists( EV_BUFFER_FULL );
            scrollable = growing || ( editline->length > editline->fldlen );
            buffer.content = editline->buffer;
            buffer.length = editline->length;
            buffer.index = editline->index;
            buffer.insert = ( vs->cursor_type == C_INSERT );
            buffer.dirty = false;
            buffer.auto_clear = editline->auto_clear;
            ui_ev = uieditevent( ui_ev, &buffer );
            editline->auto_clear = buffer.auto_clear;
            editline->dirty |= buffer.dirty;
            editline->index = buffer.index;
            vs->cursor_type = ( buffer.insert ) ? C_INSERT : C_NORMAL ;
            if( scrollable ) {
                scroll = editline->scroll;
                if( scroll > editline->index )
                    scroll = editline->index;
                if( scroll < (int)( editline->index - editline->fldlen + 1 ) ) {
                    scroll = editline->index - editline->fldlen + 1;
                }
            } else {
                scroll = 0;
            }
            vs->cursor_col = editline->col + editline->index - scroll;
            if( ( scroll != editline->scroll ) || buffer.dirty ) {
                editline->scroll = scroll;
                echoline( vs, editline );
            }
            if( ( editline->index == editline->length ) ||
                ( *(editline->buffer + editline->length - 1) != ' ' ) ) {
                if( growing ) {
                    ui_ev = EV_BUFFER_FULL;   /* may clobber EV_BUMP_RIGHT */
                }
            }
//            if( ui_ev != EV_NO_EVENT && !uiinlists( ui_ev ) ) {
//                ui_ev = EV_NO_EVENT;
//            }
        }
    }
    return( ui_ev );
}


ui_event UIAPI uiveditline( VSCREEN *vs, VEDITLINE *editline )
/************************************************************/
{
    ui_event        ui_ev;

    uipushlist( LineEvents );
    ui_ev = uivgetevent( vs );
    uipoplist( /* LineEvents */ );
    ui_ev = uiveditevent( vs, editline, ui_ev );
    return( ui_ev );
}

bool UIAPI uiveditinit( VSCREEN *vs, VEDITLINE *editline, char *buffer,
                             unsigned bufflen, ORD row, ORD col, unsigned len )
/*****************************************************************************/
{
    /* unused parameters */ (void)vs;

    editline->index = 0;
    editline->dirty = false;
    editline->row = row;
    editline->col = col;
    editline->fldlen = len;
    editline->scroll = 0;
    editline->attr = UIData->attrs[ATTR_CURR_EDIT];
    editline->update = true;
    editline->auto_clear = false;
    editline->buffer = buffer;
    editline->length = bufflen;
    editline->marking = false;
    return( true );
}

bool UIAPI uiveditfini( VSCREEN *vs, VEDITLINE *editline )
/***********************************************************/
{
    /* unused parameters */ (void)vs; (void)editline;

    return( true );
}
