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
#include "wdemain.h"
#include "wdestat.h"
#include "wdemsgbx.h"
#include "rcstr.gh"
#include "wdedebug.h"
#include "wdecurr.h"
#include "wdealign.h"

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static Bool WdeCheckResizeOperation( OBJPTR, LIST **, RESIZE_ID );
static Bool WdeCheckAlignControl( LIST **, OBJPTR * );
static Bool WdeAlignControl( WdeAlignValue, OBJPTR, RECT * );

Bool WdeAlignControls( WdeAlignValue aval )
{
    LIST    *objlist;
    LIST    *olist;
    OBJPTR  obj;
    OBJPTR  parent;
    RECT    prect;
    RECT    nc_size;
    Bool    ret;

    if( aval == 0 || !WdeCheckAlignControl ( &objlist, &parent ) ) {
        return( FALSE );
    }

    if( objlist == NULL || parent == NULL ) {
        WdeWriteTrail( "WdeAlignControls: NULL value!");
        ListFree( objlist );
        return( FALSE );
    }

    Location( parent, &prect );

    if( !Forward( parent, GET_NC_SIZE, &nc_size, NULL ) ) {
        WdeWriteTrail( "WdeAlignControls: GET_NC_SIZE failed!");
        ListFree( objlist );
        return( FALSE );
    }

    prect.top += nc_size.top;
    prect.left += nc_size.left;
    prect.right -= nc_size.right;
    prect.bottom -= nc_size.bottom;

    ret = TRUE;

    HideSelectBoxes();

    for( olist = objlist; olist; olist = ListNext( olist ) ) {
        obj = ListElement( olist );
        if( !WdeAlignControl( aval, obj, &prect ) ) {
            WdeWriteTrail( "WdeAlignControls: WdeAlignControl failed!");
            ret = FALSE;
            break;
        }
    }

    ShowSelectBoxes();

    return( ret );
}

Bool WdeAlignControl( WdeAlignValue aval, OBJPTR obj, RECT *prect )
{
    RECT rect;

    if( aval == 0 || obj == NULL || prect == NULL ) {
        return( FALSE );
    }

    Location( obj, &rect );

    return( TRUE );
}

Bool WdeSameSize( RESIZE_ID resize_id )
{
    LIST    *objlist;
    LIST    *olist;
    OBJPTR  currobj;
    OBJPTR  primary;
    RECT    primrect;
    RECT    rect;

    if( !(resize_id & (RESIZE_ID)(R_RIGHT | R_BOTTOM)) ) {
        WdeWriteTrail( "WdeSameSize: Bad resize_id!");
        return( FALSE );
    }

    primary = WdeGetCurrObject();

    if( !WdeCheckResizeOperation( primary, &objlist, resize_id ) ) {
        return( FALSE );
    }

    if( objlist == NULL || ListCount( objlist ) < 2 ) {
        WdeWriteTrail( "WdeSameSize: Need at least two objects!");
        ListFree( objlist );
        return( FALSE );
    }

    Location( primary, &primrect );

    HideSelectBoxes();

    for( olist = objlist; olist; olist = ListNext( olist ) ) {
        currobj = ListElement( olist );
        if( currobj != primary ) {
            Location( currobj, &rect );
            if( resize_id & R_BOTTOM ) {
                rect.bottom = rect.top + (primrect.bottom - primrect.top);
            }
            if( resize_id & R_RIGHT ) {
                rect.right = rect.left + (primrect.right - primrect.left);
            }
            if( !Resize( currobj, &rect, TRUE ) ) {
                WdeWriteTrail( "WdeSameSize: Resize failed!");
            }
        }
    }

    ShowSelectBoxes();

    ListFree( objlist );

    return( TRUE );
}

Bool WdeGetBoundingRectFromSelect( RECT *r )
{
    LIST    *objlist;
    Bool    ret;

    if( r == NULL ) {
        return( FALSE );
    }

    objlist = WdeGetCurrObjectList();

    if( objlist == NULL ) {
        return( FALSE );
    }

    ret = WdeGetBoundingRectFromList( objlist, r );

    ListFree( objlist );

    return( ret );
}

Bool WdeGetBoundingRectFromList( LIST *objlist, RECT *r )
{
    LIST    *clist;
    OBJPTR  obj;
    RECT    r1;
    RECT    r2;

    if( r == NULL ) {
        return( FALSE );
    }

    SetRectEmpty( r );

    for( clist = objlist; clist; clist = ListNext( clist ) ) {
        obj = ListElement( clist );
        if( obj ) {
            Location( obj, &r1 );
            UnionRect( &r2, &r1, r );
            *r = r2;
        }
    }

    return( TRUE );
}

Bool WdeCheckResizeOperation( OBJPTR primary, LIST **objlist, RESIZE_ID id )
{
    LIST        *clist;
    OBJPTR      obj;
    RECT        rect;
    POINT       pt;
    RESIZE_ID   obj_resize_id;
    Bool        ret;

    if( objlist == NULL || primary == NULL ) {
        return( FALSE );
    }

    ret = TRUE;

    *objlist = WdeGetCurrObjectList();

    for( clist = *objlist; ret && clist != NULL ; clist = ListNext( clist ) ) {
        obj = ListElement( clist );
        if( obj != primary ) {
            Location( obj, &rect );
            pt.x = rect.left;
            pt.y = rect.top;
            if( !GetResizeInfo( obj, &obj_resize_id ) ) {
                WdeWriteTrail( "WdeCheckResizeOperation: GetResizeInfo failed!" );
                WdeSetStatusByID( -1, WDE_OBJECTCANTBESIZED );
                ret = FALSE;
            } else if( !(obj_resize_id & id) ) {
                WdeWriteTrail( "WdeCheckResizeOperation: "
                               "One of the objects can't be sized" );
                WdeSetStatusByID( -1, WDE_OBJECTCANTBESIZED );
                ret = FALSE;
            }
        }
    }

    if( !ret && *objlist != NULL ) {
        ListFree( *objlist );
        *objlist = NULL;
    }

    return( ret );
}

Bool WdeCheckAlignControl( LIST **objlist, OBJPTR *p )
{
    LIST        *clist;
    OBJPTR      obj;
    OBJPTR      obj_parent;
    RECT        rect;
    POINT       pt;
    Bool        ret;

    if( objlist == NULL || p == NULL ) {
        return( FALSE );
    }

    ret      = TRUE;
    *objlist = WdeGetCurrObjectList();
    *p       = NULL;

    for( clist = *objlist; ret && clist != NULL; clist = ListNext( clist ) ) {
        obj = ListElement( clist );
        GetObjectParent( obj, &obj_parent );
        if( *p != NULL ) {
            if( obj_parent != *p ) {
                WdeSetStatusByID( -1, WDE_ALLMUSTHAVESAMEPARENT );
                ret = FALSE;
                break;
            }
        } else {
            if( obj_parent == GetMainObject() ) {
                ret = FALSE;
                break;
            } else {
                *p = obj_parent;
            }
        }
        Location( obj, &rect );
        pt.x = rect.left;
        pt.y = rect.top;
        if( !ValidateAction( obj, MOVE, &pt ) ) {
            WdeWriteTrail( "WdeCheckResizeOperation: ValidateAction failed!" );
            WdeSetStatusByID( -1, WDE_OBJECTCANTBEMOVED );
            ret = FALSE;
        }
    }

    if( !ret && *objlist != NULL ) {
        ListFree( *objlist );
        *objlist = NULL;
    }

    return( ret );
}
