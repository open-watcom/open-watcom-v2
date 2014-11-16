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
#include "uivfld.h"
#include "uivedit.h"
#include "uiedit.h"
#include "clibext.h"


static EVENT livefieldevents[] = {
        ' ', '~',
        EV_HOME, EV_DELETE,
        EV_ALT_KEYPAD( 1 ), EV_ALT_KEYPAD( 0xff ),
        EV_NO_EVENT,            /* end of list of ranges */
        EV_RUB_OUT,
        EV_CTRL_END,
        EV_CTRL_HOME,
        EV_TAB_FORWARD,
        EV_TAB_BACKWARD,
        EV_NO_EVENT
};


static EVENT deadfieldevents[] = {
        EV_HOME, EV_INSERT,
        EV_NO_EVENT,            /* end of list of ranges */
        EV_TAB_FORWARD,
        EV_TAB_BACKWARD,
        EV_NO_EVENT
};


static EVENT setfield( VSCREEN *vptr, VFIELDEDIT *header, VFIELD *cur, ORD col )
/*********************************************/
{
    register    EVENT                   ev;
    register    VFIELD*                 prev;

    if( cur != header->curfield ) {
        prev = header->curfield;
        if( prev ) {
            /* change attribute on field being left */
            uivtextput( vptr, prev->row, prev->col, header->exit,
                header->buffer, prev->length );
        }
        header->prevfield = prev;
        header->curfield = cur;
        header->fieldpending = TRUE;
        ev = EV_FIELD_CHANGE;
    } else {
        ev = EV_NO_EVENT;
    }
    if( cur != NULL ) {
        vptr->row = cur->row;
        vptr->col = cur->col + col;
    }
    return( ev );
}


static EVENT movecursor( VSCREEN *vptr, VFIELDEDIT *header, int row, int col )
/***********************************************/
{
    register    int                      cursor;
    register    int                      field = 0; // GCC wrongly thinks this might be uninited
    register    VFIELD*                  cur;

    if( col < 0 ) {
        col += vptr->area.width;
        row -= 1;
    } else if( col >= vptr->area.width ) {
        col -= vptr->area.width;
        row += 1;
    }
    if( row > vptr->area.height - 1 )
        row = vptr->area.height - 1;
    if( row < 0 )
        row = 0;
    cursor = row * vptr->area.width + col;
    vptr->row = row;
    vptr->col = col;
    for( cur = header->fieldlist; cur != NULL; cur = cur->link ) {
        field = cur->row * vptr->area.width + cur->col;
        if( ( field <= cursor ) && ( field + cur->length > cursor ) ) {
            break;
        }
    }
    return( setfield( vptr, header, cur, cursor - field ) );
}


static VFIELD *tabfield( VSCREEN *vptr, VFIELD *fieldlist, bool forward )
/*************************************************/
{
    register    VFIELD*                 chase;
    register    VFIELD*                 cur;
    register    int                     diff;
    register    int                     closest;

    chase = fieldlist;
    cur = chase;
    closest = vptr->area.height * vptr->area.width;
    while( chase != NULL ) {
        if( forward ) {
            diff = ( chase->row - vptr->row ) * vptr->area.width +
                   ( chase->col - vptr->col );
        } else {
            diff = ( vptr->row - chase->row ) * vptr->area.width +
                   ( vptr->col - chase->col );
        }
        if( diff <= 0 ) {
            diff = diff + vptr->area.height * vptr->area.width;
        }
        if( diff < closest ) {
            cur = chase;
            closest = diff;
        }
        chase = chase->link;
    }
    return( cur );
}


EVENT UIAPI uivfieldedit( VSCREEN *vptr, VFIELDEDIT *header )
/************************************************************/
{
    register    EVENT                   ev;
    register    VFIELD*                 cur;
    auto        VBUFFER                 buffer;
    auto        SAREA                   area;

    if( header->reset ) {
        header->reset = FALSE;
        header->prevfield = NULL;
        header->curfield = NULL;
        header->cursor = TRUE;
        area.height = 1;
        for( cur = header->fieldlist ; cur != NULL ; cur = cur->link ) {
            area.row = cur->row;
            area.col = cur->col;
            area.width = cur->length;
            uivattribute( vptr, area, header->exit );
        }
    }
    if( header->cursor ) {
        header->cursor = FALSE;
        header->delpending = FALSE;
        header->fieldpending = FALSE;
        header->cancel = FALSE;
        if( vptr->cursor == C_OFF ) {
            vptr->cursor = C_NORMAL;
        }
        return( movecursor( vptr, header, vptr->row, vptr->col ) );
    }
    if( header->fieldpending ) {
        header->update = TRUE;
        if( header->cancel ) {
            header->cancel = FALSE;
            header->curfield = NULL;
            setfield( vptr, header, header->prevfield, 0 );
        }
        header->fieldpending = FALSE;
    }
    cur = header->curfield;
    if( header->update ) {
        header->update = FALSE;
        if( cur ) {     /* this should always be non-NULL */
            uipadblanks( header->buffer, cur->length );
            if( header->delpending ) {
                buffer.content = header->buffer;
                buffer.length = cur->length;
                buffer.index = vptr->col - cur->col;
                uieditevent( EV_DELETE, &buffer );
                header->dirty = TRUE;
                header->delpending = FALSE;
            }
            uivtextput( vptr, cur->row, cur->col, header->enter,
                    header->buffer, cur->length );
        }
    }
    if( header->oktomodify ) {
        uipushlist( livefieldevents );
    } else {
        uipushlist( deadfieldevents );
    }
    ev = uivgetevent( vptr );
    if( ev > EV_NO_EVENT ) {
        if( uiintoplist( ev ) ) {
            if( cur ) {
                buffer.content = header->buffer;
                buffer.length = cur->length;
                buffer.index = vptr->col - cur->col;
                buffer.insert = ( vptr->cursor == C_INSERT );
                buffer.dirty = FALSE;
                uieditevent( ev, &buffer );
                header->dirty |= buffer.dirty;
            }
            switch( ev ) {
            case EV_HOME:
                if( cur != NULL ) break; /* home is within field */
                /* WARNING: this case falls through to the next */
            case EV_TAB_FORWARD:
            case EV_TAB_BACKWARD:
                cur = tabfield( vptr, header->fieldlist, ev == EV_TAB_FORWARD );
                /* WARNING: the EV_HOME case falls through */
                if( cur != NULL ) {
                    ev = setfield( vptr, header, cur, 0 );
                    cur = NULL; /* kludge - avoid calling movecursor */
                }
                break;
            case EV_INSERT:
                if( vptr->cursor == C_INSERT ) {
                    vptr->cursor = C_NORMAL ;
                } else {
                    vptr->cursor = C_INSERT ;
                }
                break;
            case EV_CURSOR_UP:
                ev = movecursor( vptr, header, vptr->row - 1, vptr->col );
                break;
            case EV_CURSOR_DOWN:
                ev = movecursor( vptr, header, vptr->row + 1, vptr->col );
                break;
            case EV_RUB_OUT:
                header->delpending = TRUE;
                /* WARNING: this case falls through to the next !!!! */
            case EV_CURSOR_LEFT:
                if( cur ) {
                    if( vptr->col > cur->col ) {
                        break; /* cursor movement within field */
                    }
                }
                ev = movecursor( vptr, header, vptr->row, vptr->col - 1 );
                break;
            case EV_CURSOR_RIGHT:
            case ' ':
                if( header->curfield ) {
                    if( vptr->col < cur->col + cur->length - 1 ) {
                        break; /* cursor movement within field */
                    }
                }
                ev = movecursor( vptr, header, vptr->row, vptr->col + 1 );
                break;
            }
            if( ev != EV_FIELD_CHANGE ) {
                if( cur ) {
                    ev = movecursor( vptr, header,
                           vptr->row, cur->col + buffer.index );
                    if( buffer.dirty && ( ev == EV_NO_EVENT ) ) {
                        uivtextput( vptr, cur->row, cur->col, header->enter,
                            header->buffer, cur->length );
                    }
                } else {
                    ev = EV_NO_EVENT;
                }
                header->delpending = FALSE;
            }
        }
    }
    uipoplist();
    return( ev );
}
