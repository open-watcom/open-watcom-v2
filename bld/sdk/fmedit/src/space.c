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
* Description:  Implementation of even spacing of objects.
*
****************************************************************************/


#include "windows.h"
#include "fmedit.def"
#include "state.def"
#include "space.def"
#include "mouse.def"

extern void Space( WPARAM wparam )
/********************************/
{
    /* Perform the requested spacing of current objects.
     */

    OBJPTR  currobj;
    OBJPTR  primary;
    RECT    rect;
    RECT    totalrect;
    BOOL    atleasttwo;
    POINT   offset;
    LIST    *objlist;
    int     totalsize;
    int     objsize;
    int     spacesize;
    int     objcount;
    int     currpos;

    primary = GetPrimaryObject();
    if( primary == NULL ) {
        return;
    }
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
    totalrect.left = totalrect.top = 32767;
    totalrect.right = totalrect.bottom = 0;
    objsize = 0;
    objcount = 0;
    currobj = GetECurrObject();
    while( currobj != NULL ) {
        Location( currobj, &rect );
        if( rect.left < totalrect.left ) {
            totalrect.left = rect.left;
        }
        if( rect.top < totalrect.top ) {
            totalrect.top = rect.top;
        }
        if( rect.right > totalrect.right ) {
            totalrect.right = rect.right;
        }
        if( rect.bottom > totalrect.bottom ) {
            totalrect.bottom = rect.bottom;
        }
        if( LOWORD( wparam ) == IDM_SPACE_HORZ ) {
            objsize += rect.right - rect.left;
        } else {
            objsize += rect.bottom - rect.top;
        }
        objcount++;
        currobj = GetNextECurrObject( currobj );
    }
    if( LOWORD( wparam ) == IDM_SPACE_HORZ ) {
        totalsize = totalrect.right - totalrect.left;
        currpos = totalrect.left;
    } else {
        totalsize = totalrect.bottom - totalrect.top;
        currpos = totalrect.top;
    }
    spacesize = (totalsize - objsize) / (objcount - 1);
    currobj = GetECurrObject();
    while( currobj != NULL ) {
        Location( currobj, &rect );
        if( LOWORD( wparam ) == IDM_SPACE_HORZ ) {
            offset.x = currpos - rect.left;
            offset.y = 0;
            currpos += rect.right - rect.left;
        } else {
            offset.x = 0;
            offset.y = currpos - rect.top;
            currpos += rect.bottom - rect.top;
        }
        currpos += spacesize;
        Move( currobj, &offset, TRUE );
        currobj = GetNextECurrObject( currobj );
    }
    FinishMoveOperation( TRUE );
}
