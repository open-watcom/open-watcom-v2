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
#include "uivedit.h"
#include "uiedit.h"


EVENT LineEvents[] = {
        EV_FIRST_EDIT_CHAR, EV_LAST_EDIT_CHAR,
        EV_HOME, EV_DELETE,
        EV_NO_EVENT,            /* end of list of ranges */
        EV_RUB_OUT,
        EV_CTRL_END,
        EV_CTRL_HOME,
        EV_CTRL_LEFT,
        EV_CTRL_RIGHT,
        EV_NO_EVENT
};


static void echoline( VSCREEN *vptr, VEDITLINE *editline )
/********************************************************/
{
    SAREA       area;
    unsigned    start;
    unsigned    trim;

    if( editline->invisible ) {
        area.row = editline->row;
        area.col = editline->col;
        area.height = 1;
        area.width = editline->fldlen;
        uivfill( vptr, area, editline->attr, ' ' );
        for( trim = editline->length ; trim > editline->index ; --trim ) {
            if( editline->buffer[ trim - 1 ] != ' ' ) break;
        }
        area.width = min( editline->fldlen, trim - editline->scroll );
        uivfill( vptr, area, editline->attr, '*' );
    } else {
        uitextfield( vptr, editline->row, editline->col, editline->fldlen,
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
        if( area.width + start >= editline->scroll  &&  area.width > 0 ) {
            if( start > editline->scroll ) {
                area.col = editline->col + start - editline->scroll;
            } else {
                area.col = editline->col;
                area.width = area.width - editline->scroll + start;
            }
            if( area.width + area.col > editline->col + editline->fldlen ) {
                area.width =    editline->fldlen + editline->col - area.col;
            }
            uivattribute( vptr, area, editline->mark_attr );
        }
    }
}


EVENT global uiveditevent( VSCREEN *vptr, VEDITLINE *editline, EVENT ev )
/***********************************************************************/
{
    register    int                     scroll;
    register    bool                    scrollable;
    register    bool                    growing;
    auto        VBUFFER                 buffer;


    if( editline->update ) {
        if( vptr->cursor == C_OFF ) {
            vptr->cursor = C_NORMAL;
        }
        uipadblanks( editline->buffer, editline->length );
        vptr->row = editline->row;
        scroll = min( editline->scroll, editline->index );
        scroll = max( scroll, editline->index - editline->fldlen + 1 );
        editline->scroll = scroll;
        vptr->col = editline->col + editline->index - editline->scroll;
        echoline( vptr, editline );
//      uirefresh();                    not needed for QNX or DOS ??? jimg
        editline->update = FALSE;
    }
    if( ev > EV_NO_EVENT ) {
        if( uiinlist( ev ) == FALSE ) {
            growing = uiinlist( EV_BUFFER_FULL );
            scrollable = growing || ( editline->length > editline->fldlen );
            buffer.content = editline->buffer;
            buffer.length = editline->length;
            buffer.index = editline->index;
            buffer.insert = ( vptr->cursor == C_INSERT );
            buffer.dirty = FALSE;
            buffer.auto_clear = editline->auto_clear;
            ev = uieditevent( ev, &buffer );
            editline->auto_clear = buffer.auto_clear;
            editline->dirty |= buffer.dirty;
            editline->index = buffer.index;
            vptr->cursor = buffer.insert ? C_INSERT : C_NORMAL ;
            if( scrollable ) {
                scroll = min( editline->scroll, editline->index );
                scroll = max( scroll, editline->index - editline->fldlen + 1 );
            } else {
                scroll = 0;
            }
            vptr->col = editline->col + editline->index - scroll;
            if( ( scroll != editline->scroll ) || buffer.dirty ) {
                editline->scroll = scroll;
                echoline( vptr, editline );
            }
            if( ( editline->index == editline->length ) ||
                ( *(editline->buffer + editline->length - 1) != ' ' ) ) {
                if( growing ) {
                    ev = EV_BUFFER_FULL;   /* may clobber EV_BUMP_RIGHT */
                }
            }
//            if( ev != EV_NO_EVENT && !uiinlist( ev ) ) {   /* 891206 */
//                ev = EV_NO_EVENT;
//            }
        }
    }
    return( ev );
}


EVENT global uiveditline( VSCREEN *vptr, VEDITLINE *editline )
/************************************************************/
{
    register    EVENT                   ev;

    uipushlist( LineEvents );
    ev = uivgetevent( vptr );
    uipoplist();
    ev = uiveditevent( vptr, editline, ev );
    return( ev );
}

bool global uiveditinit( VSCREEN *vptr, VEDITLINE *editline, char *buffer,
                                  int bufflen, ORD row, ORD col, int len )
/************************************************************************/
{
    editline->index = 0;
    editline->dirty = FALSE;
    editline->row = row;
    editline->col = col;
    editline->fldlen = len;
    editline->scroll = 0;
    editline->attr = UIData->attrs[ ATTR_CURR_EDIT ];
    editline->update = TRUE;
    editline->auto_clear = FALSE;
    editline->buffer = buffer;
    editline->length = bufflen;
    editline->marking = FALSE;
    return( TRUE );
}

bool global uiveditfini( VSCREEN *vptr, VEDITLINE *editline )
/***********************************************************/
{
    return( TRUE );
}

