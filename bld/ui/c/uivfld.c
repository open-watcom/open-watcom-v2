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
#include "uivfld.h"
#include "uivedit.h"
#include "uiedit.h"

#include "clibext.h"


static ui_event livefieldevents[] = {
    ' ',            '~',
    EV_HOME,        EV_DELETE,
    0x0001,         0x00ff,     /* Alt Keypad number */
    __rend__,
    EV_RUB_OUT,
    EV_CTRL_END,
    EV_CTRL_HOME,
    EV_TAB_FORWARD,
    EV_TAB_BACKWARD,
    __end__
};


static ui_event deadfieldevents[] = {
    EV_HOME,        EV_INSERT,
    __rend__,
    EV_TAB_FORWARD,
    EV_TAB_BACKWARD,
    __end__
};


static ui_event setfield( VSCREEN *vs, VFIELDEDIT *header, VFIELD_EDIT *cur, ORD col )
/************************************************************************************/
{
    ui_event        ui_ev;
    VFIELD_EDIT     *prev;

    if( cur != header->curfield ) {
        prev = header->curfield;
        if( prev != NULL ) {
            /* change attribute on field being left */
            uivtextput( vs, prev->row, prev->col, header->exit_attr, header->buffer, prev->length );
        }
        header->prevfield = prev;
        header->curfield = cur;
        header->fieldpending = true;
        ui_ev = EV_FIELD_CHANGE;
    } else {
        ui_ev = EV_NO_EVENT;
    }
    if( cur != NULL ) {
        vs->cursor_row = cur->row;
        vs->cursor_col = cur->col + col;
    }
    return( ui_ev );
}


static ui_event movecursor( VSCREEN *vs, VFIELDEDIT *header, int row, int col )
/*****************************************************************************/
{
    unsigned            cursor;
    unsigned            field;
    VFIELD_EDIT         *cur;
    CURSORORD           crow;
    CURSORORD           ccol;

    col += vs->cursor_col;
    row += vs->cursor_row;
    if( col < 0 ) {
        ccol = col + vs->area.width;
        row -= 1;
    } else if( col >= vs->area.width ) {
        ccol = col - vs->area.width;
        row += 1;
    } else {
        ccol = col;
    }
    if( row < 0 ) {
        crow = 0;
    } else if( row > vs->area.height - 1 ) {
        crow = vs->area.height - 1;
    } else {
        crow = row;
    }
    cursor = crow * vs->area.width + ccol;
    field = 0;
    vs->cursor_row = crow;
    vs->cursor_col = ccol;
    for( cur = header->fieldlist; cur != NULL; cur = cur->link ) {
        field = cur->row * vs->area.width + cur->col;
        if( ( field <= cursor ) && ( field + cur->length > cursor ) ) {
            break;
        }
    }
    return( setfield( vs, header, cur, cursor - field ) );
}


static VFIELD_EDIT *tabfield( VSCREEN *vs, VFIELD_EDIT *fieldlist, bool forward )
/*******************************************************************************/
{
    VFIELD_EDIT         *chase;
    VFIELD_EDIT         *cur;
    int                 diff;
    int                 closest;

    cur = fieldlist;
    closest = vs->area.height * vs->area.width;
    for( chase = fieldlist; chase != NULL; chase = chase->link ) {
        if( forward ) {
            diff = ( chase->row - vs->cursor_row ) * vs->area.width + ( chase->col - vs->cursor_col );
        } else {
            diff = ( vs->cursor_row - chase->row ) * vs->area.width + ( vs->cursor_col - chase->col );
        }
        if( diff <= 0 ) {
            diff = diff + vs->area.height * vs->area.width;
        }
        if( diff < closest ) {
            cur = chase;
            closest = diff;
        }
    }
    return( cur );
}


ui_event UIAPI uivfieldedit( VSCREEN *vs, VFIELDEDIT *header )
/************************************************************/
{
    ui_event           ui_ev;
    VFIELD_EDIT        *cur;
    VBUFFER            buffer;
    SAREA              area;

    buffer.content = NULL;
    buffer.length = 0;
    buffer.index = 0;
    buffer.insert = false;
    buffer.dirty = false;
    buffer.auto_clear = false;
    if( header->reset ) {
        header->reset = false;
        header->prevfield = NULL;
        header->curfield = NULL;
        header->cursor = true;
        area.height = 1;
        for( cur = header->fieldlist; cur != NULL; cur = cur->link ) {
            area.row = cur->row;
            area.col = cur->col;
            area.width = cur->length;
            uivattribute( vs, area, header->exit_attr );
        }
    }
    if( header->cursor ) {
        header->cursor = false;
        header->delpending = false;
        header->fieldpending = false;
        header->cancel = false;
        if( vs->cursor_type == C_OFF ) {
            vs->cursor_type = C_NORMAL;
        }
        return( movecursor( vs, header, 0, 0 ) );
    }
    if( header->fieldpending ) {
        header->update = true;
        if( header->cancel ) {
            header->cancel = false;
            header->curfield = NULL;
            setfield( vs, header, header->prevfield, 0 );
        }
        header->fieldpending = false;
    }
    cur = header->curfield;
    if( header->update ) {
        header->update = false;
        if( cur != NULL ) {     /* this should always be non-NULL */
            uipadblanks( header->buffer, cur->length );
            if( header->delpending ) {
                buffer.content = header->buffer;
                buffer.length = cur->length;
                buffer.index = vs->cursor_col - cur->col;
                uieditevent( EV_DELETE, &buffer );
                header->dirty = true;
                header->delpending = false;
            }
            uivtextput( vs, cur->row, cur->col, header->enter_attr, header->buffer, cur->length );
        }
    }
    if( header->oktomodify ) {
        uipushlist( livefieldevents );
    } else {
        uipushlist( deadfieldevents );
    }
    ui_ev = uivgetevent( vs );
    if( ui_ev > EV_NO_EVENT ) {
        if( uiintoplist( ui_ev ) ) {
            if( cur != NULL ) {
                buffer.content = header->buffer;
                buffer.length = cur->length;
                buffer.index = vs->cursor_col - cur->col;
                buffer.insert = ( vs->cursor_type == C_INSERT );
                buffer.dirty = false;
                uieditevent( ui_ev, &buffer );
                header->dirty |= buffer.dirty;
            }
            switch( ui_ev ) {
            case EV_HOME:
                if( cur != NULL )   /* home is within field */
                    break;
                /* fall through */
            case EV_TAB_FORWARD:
            case EV_TAB_BACKWARD:
                cur = tabfield( vs, header->fieldlist, ui_ev == EV_TAB_FORWARD );
                /* WARNING: the EV_HOME case falls through */
                if( cur != NULL ) {
                    ui_ev = setfield( vs, header, cur, 0 );
                    cur = NULL; /* kludge - avoid calling movecursor */
                }
                break;
            case EV_INSERT:
                if( vs->cursor_type == C_INSERT ) {
                    vs->cursor_type = C_NORMAL ;
                } else {
                    vs->cursor_type = C_INSERT ;
                }
                break;
            case EV_CURSOR_UP:
                ui_ev = movecursor( vs, header, -1, 0 );
                break;
            case EV_CURSOR_DOWN:
                ui_ev = movecursor( vs, header, 1, 0 );
                break;
            case EV_RUB_OUT:
                header->delpending = true;
                /* fall through */
            case EV_CURSOR_LEFT:
                if( cur != NULL ) {
                    if( vs->cursor_col > cur->col ) {
                        break; /* cursor movement within field */
                    }
                }
                ui_ev = movecursor( vs, header, 0, -1 );
                break;
            case EV_CURSOR_RIGHT:
            case ' ':
                if( header->curfield ) {
                    if( vs->cursor_col < cur->col + cur->length - 1 ) {
                        break; /* cursor movement within field */
                    }
                }
                ui_ev = movecursor( vs, header, 0, 1 );
                break;
            }
            if( ui_ev != EV_FIELD_CHANGE ) {
                if( cur != NULL ) {
                    ui_ev = movecursor( vs, header, 0, cur->col + buffer.index - vs->cursor_col );
                    if( buffer.dirty && ( ui_ev == EV_NO_EVENT ) ) {
                        uivtextput( vs, cur->row, cur->col, header->enter_attr, header->buffer, cur->length );
                    }
                } else {
                    ui_ev = EV_NO_EVENT;
                }
                header->delpending = false;
            }
        }
    }
    uipoplist( /* livefieldevents or deadfieldevents */ );
    return( ui_ev );
}
