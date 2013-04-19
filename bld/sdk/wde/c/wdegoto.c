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


#include "wdeglbl.h"
#include "wderesin.h"
#include "wderes.h"
#include "wdegoto.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/
#define GOTO_OFFSET     20

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

Bool WdeIsCurrentObjectVisible( WdeResInfo *info )
{
    OBJPTR      curr_obj;
    RECT        crect;
    RECT        brect;
    RECT        intersect;
    POINT       origin;

    if( info == NULL ) {
        return( FALSE );
    }

    curr_obj = WdeGetCurrentDialog();
    if( curr_obj == NULL ) {
        curr_obj = GetCurrObject();
        if( curr_obj == NULL ) {
            return( FALSE );
        }
    }

    Location( curr_obj, &crect );
    GetClientRect( info->edit_win, &brect );
    GetOffset( &origin );
    OffsetRect( &brect, origin.x, origin.y );

    if( IntersectRect( &intersect, &crect, &brect ) ) {
        return( TRUE );
    }

    return( FALSE );
}

Bool WdeHandleGotoCurrentObject( void )
{
    WdeResInfo  *info;
    OBJPTR      obj;
    RECT        crect;
    RECT        srect;
    RECT        rect;
    POINT       pt;

    info = WdeGetCurrentRes();
    if( info == NULL ) {
        return( FALSE );
    }
    InitState( info->forms_win );

    if( WdeIsCurrentObjectVisible( info ) ) {
        return( TRUE );
    }

    // calculate the exclusion rectangle for the new offset
    obj = GetMainObject();
    Forward( obj, GET_SCROLL_RECT, &srect, NULL );
    GetClientRect( info->forms_win, &crect );
    rect.left = srect.right - (crect.right - crect.left) + 1;
    rect.top = srect.bottom - (crect.bottom - crect.top) + 1;
    rect.right = srect.right;
    rect.bottom = srect.bottom;

    obj = GetCurrObject();
    if( obj == NULL ) {
        return( FALSE );
    }

    Location( obj, &crect );

    pt.x = crect.left - GOTO_OFFSET;
    pt.y = crect.top - GOTO_OFFSET;

    if( !PtInRect( &srect, pt ) ) {
        pt.x = srect.left;
        pt.y = srect.top;
    }

    if( PtInRect( &rect, pt ) ) {
        pt.x = rect.left - 1;
        pt.y = rect.top - 1;
    }

    NewOffset( pt );

    return( TRUE );
}
