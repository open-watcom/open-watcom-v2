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
* Description:  Implementation of alignment relative to other objects.
*
****************************************************************************/


#include <windows.h>
#include "fmedit.def"
#include "state.def"
#include "align.def"
#include "grid.def"
#include "mouse.def"

extern void Align( WPARAM wparam )
/********************************/
{
    /* Perform the requested alignment of current objects relative to the primary
     * object
     */
    OBJPTR   currobj;
    OBJPTR   primary;
    RECT     primrect;
    RECT     rect;
    BOOL     atleasttwo;
    POINT    offset;
    LIST     *objlist;

    primary = GetPrimaryObject();
    if( primary == NULL ) {
        return;
    }
    atleasttwo = FALSE;
    currobj = GetECurrObject();
    while( currobj != NULL && !atleasttwo ) {
        if( currobj != primary ) {
            atleasttwo = TRUE;
        } else {
            currobj = GetNextECurrObject( currobj );
        }
    }
    if( !atleasttwo ) {
        return;
    }
    if( !CheckMoveOperation( &objlist ) ) {
        return;
    }
    BeginMoveOperation( objlist );
    ListFree( objlist );
    currobj = GetECurrObject();
    primary = GetPrimaryObject();
    Location( primary, &primrect );
    while( currobj != NULL ) {
        if( currobj != primary ) {
            Location( currobj, &rect );
            switch( LOWORD( wparam ) ) {
            case IDM_FMLEFT:
                offset.x = primrect.left - rect.left;
                offset.y = 0;
                break;
            case IDM_FMHCENTRE:
                offset.x = ((primrect.right + primrect.left) / 2) -
                           ((rect.right + rect.left) / 2);
                offset.y = 0;
                break;
            case IDM_FMRIGHT:
                offset.x = primrect.right - rect.right;
                offset.y = 0;
                break;
            case IDM_FMTOP:
                offset.x = 0;
                offset.y = primrect.top - rect.top;
                break;
            case IDM_FMVCENTRE:
                offset.x = 0;
                offset.y = ((primrect.bottom + primrect.top) / 2) -
                           ((rect.bottom + rect.top) / 2);
                break;
            case IDM_FMBOTTOM:
                offset.x = 0;
                offset.y = primrect.bottom - rect.bottom;
                break;
            }
            Move( currobj, &offset, TRUE );
        }
        currobj = GetNextECurrObject( currobj );
    }
    FinishMoveOperation( TRUE );
}
