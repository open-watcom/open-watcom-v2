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
* Description:  Routines to handle the mouse interactions.
*
****************************************************************************/


#include <wwindows.h>
#include <limits.h>
#include "global.h"
#include "fmedit.def"
#include "object.def"
#include "state.def"
#include "currobj.def"
#include "grid.def"
#include "list.def"
#include "align.def"
#include "dlist.def"
#include "clip.def"

#include "mouse.def"


static DLIST        *OrderList( LIST * );
static DLIST_ELT    GetNextElement( DLIST * );
static void         ResetEdit( POINT, WORD, OBJPTR );

void ProcessDBLCLK( POINT point )
/*******************************/
{
    /*  Process a double click on the current object.  This implies a request
     *  to define/redefine the characteristics of the object.
     */
    POINT   pt;
    OBJPTR  currobj;

    pt = point;
    currobj = GetPrimaryObject();
    if( currobj != NULL ) {
        Define( currobj, &pt, NULL );
    }
}

static void ResizeBegin( POINT pt, WORD ks, OBJPTR d )
/****************************************************/
{
    /* Begin a resizing operation */
    OBJPTR         object;
    RECT           rect;
    RECT           offrect;
    POINT          rgrid;

    pt = pt;    /* ref'd to avoid warning */
    ks = ks;    /* ref'd to avoid warning */
    d = d;
    SetDefState();
    object = GetPrimaryObject();
    if( object != NULL ) {
        if( GetState() == EDITING ) {
            Notify( object, TERMINATE_EDIT, NULL );
        }
        SetDefState();
        if( !ValidateAction( object, RESIZE, NULL ) ) {
            return;
        }
        SetState( SIZING );
        Location( object, &rect );
        rgrid.x = GetHorizontalInc();
        rgrid.y = GetVerticalInc();
        ResizeIncrements( object, &rgrid );
        SetResizeGrid( rgrid.x, rgrid.y );
        SaveObject();
        object = GetObjptr( object );
        ResetCurrObject( false );
        object = Create( O_EATOM, object, &rect, NULL );
        SetCurrObject( object );
        offrect = rect;
        if( SnapRectToGrid( &offrect ) ) {
            offrect.top -= rect.top;
            offrect.left -= rect.left;
            offrect.right -= rect.right;
            offrect.bottom -= rect.bottom;
            Resize( object, &offrect, true );
        }
    }
}


static void UnexpectedStateRecover( POINT pt )
/********************************************/
{
    /*  A mouse press was received in a state where it should be impossible to
     *  get a mouse press. Ignore it.
     */
    pt = pt;           /* ref'd to avoid warning */
#ifdef DEBUG_ON
    MessageBox( GFileIO.hWnd, "State Error", NULL, MB_OK | MB_ICONSTOP );
#endif
}

static void UnexpectedPressStateRecover( POINT pt, WORD ks, OBJPTR d )
/********************************************************************/
{
    /*  A mouse press was received in a state where it should be impossible to
     *  get a mouse press. Ignore it.
     */
    pt = pt;           /* ref'd to avoid warning */
    ks = ks;           /* ref'd to avoid warning */
    d = d;             /* ref'd to avoid warning */
#ifdef DEBUG_ON
    MessageBox( GFileIO.hWnd, "State Error", NULL, MB_OK | MB_ICONSTOP );
#endif
}


static void CreateBegin( POINT pt, OBJPTR parent )
/************************************************/
{
    /* begin the creation of a new object */
    RECT           origin;

    SetState( CREATING );
    SnapPointToGrid( &pt );
    origin.top = pt.y;
    origin.bottom = pt.y;
    origin.left = pt.x;
    origin.right = pt.x;
    SaveObject();
    ResetCurrObject( false );
    SetCurrObject( Create( O_EATOM, parent, &origin, NULL ) );
}


static void MovePendingBegin( WORD keystate, OBJPTR object )
/**********************************************************/
{
    /* begin a move operation */

    /* If the object the mouse press was on is not in the current object list
     * then, if mulitple select, add it to the list, otherwise, make it the
     * current object */

    if( GetCurrObjptr( object ) == NULL ) {
        if( (keystate & MK_SHIFT) == 0 && (keystate & MK_CONTROL) == 0 ) {
            ResetCurrObject( false );
        }
        AddCurrObject( object );

        /* Remove the Control flag so that the added selection isn't removed
         * on the Mouse UP of thhe state stays the same as MOVE_PENDING
         */
        keystate &= ~MK_CONTROL;
    } else {
        SetPrimaryObject( GetCurrObjptr( object ) );
    }
    SetKeyState( keystate );
    SetState( MOVE_PENDING );
}

bool CheckMoveOperation( LIST **objlist )
/***************************************/
{
    LIST    *clist;
    OBJPTR  obj;
    RECT    rect;
    POINT   pt;

    *objlist = GetCurrObjectList();
    for( clist = *objlist; clist != NULL; clist = ListNext( clist ) ) {
        obj = ListElement( clist );
        Location( obj, &rect );
        pt.x = rect.left;
        pt.y = rect.top;
        if( !ValidateAction( obj, MOVE, &pt ) ) {
            SetPrimaryObject( GetCurrObjptr( obj ) );
            MessageBox( GetAppWnd(), "The current selected object cannot be moved.",
                        NULL, MB_ICONEXCLAMATION | MB_OK );

            ListFree( *objlist );
            *objlist = NULL;
            return( false );
        }
    }
    return( true );
}


static void BeginMove( POINT p )
/******************************/
{
    /* Begin the movement operation */
    LIST    *movelist;

    if( CheckMoveOperation( &movelist ) ) {
        SetState( MOVING );
        BeginMoveOperation( movelist );
        ListFree( movelist );
    } else {
        SetState( DORMANT );
    }
    SetPrevMouse( p );
}


static void SelectBegin( POINT pt, WORD keystate )
/************************************************/
{
    RECT origin;

    origin.top = pt.y;
    origin.bottom = pt.y;
    origin.left = pt.x;
    origin.right = pt.x;
    SaveObject();
    SetKeyState( keystate );
    SetState( SELECTING );
    SetSelectEatom( Create( O_EATOM, NULL, &origin, NULL ) );
}

static bool InVicinity( OBJPTR *obj, short y, short x )
/*****************************************************/
{
    /* See if we're in the vicinity of an object that can be moved */
    POINT          pt;
    OBJPTR         closeobj;

    pt.x = x;
    pt.y = y;
    closeobj = FindOneObjPt( pt );
    if( closeobj == *obj || !ValidateAction( closeobj, MOVE, &pt ) ) {
        return( false );
    } else {
        *obj = closeobj;
    }
    return( true );
}



static bool IsMoveOperation( OBJPTR obj, POINT point, WORD keystate )
/*******************************************************************/
{
    bool   ret;

    ret = false;
    if( ValidateAction( obj, MOVE, &point ) ||
        InVicinity( &obj, point.y - OBJ_VICINITY, point.x ) ||
        InVicinity( &obj, point.y + OBJ_VICINITY, point.x ) ||
        InVicinity( &obj, point.y, point.x - OBJ_VICINITY ) ||
        InVicinity( &obj, point.y, point.x + OBJ_VICINITY ) ) {
        MovePendingBegin( keystate, obj );
        ret = true;
    }
    return( ret );
}


static void ActionBegin( POINT point, WORD keystate, OBJPTR obj )
/***************************************************************/
{
    /* begin a create or move action depending on the button pressed */
    if( obj == NULL ) {
        obj = FindOneObjPt( point );
    }
    if( GetBaseObjType() != O_NONE ) {
        CreateBegin( point, obj );
    } else if( !IsMoveOperation( obj, point, keystate ) ) {
        SelectBegin( point, keystate );
    }
}


static void FinishMove( POINT pt )
/********************************/
{
    /* Finish the movement operation */
    pt = pt;
    FinishMoveOperation( true );
}


static void IgnoreMouse( POINT pt )
/*********************************/
{
    /* Ignore a mouse action */
    pt = pt;        /* ref'd to avoid warning */
}

static void IgnoreMousePress( POINT pt, WORD ks, OBJPTR obj )
/***********************************************************/
{
    /* Ignore a mouse action */
     pt = pt;           /* ref'd to avoid warning */
     ks = ks;           /* ref'd to avoid warning */
     obj = obj;         /* ref'd to avoid warning */
}


static void FinishResize( POINT pt )
/**********************************/
{
    /* finish the resize operation */
    OBJPTR currobj;

    pt = pt;
    currobj = GetEditCurrObject();
    if( currobj != NULL ) {
        if( !Register( currobj ) ) {
            RestorePrevObject();
        }
    }
    MarkCurrObject();
    SetDefState();
}

void AbortResize( void )
/**********************/
{
    OBJPTR  eobj;

    eobj = GetEditCurrObject();

    if( eobj != NULL ) {
        Destroy( eobj, false );
        RestorePrevObject();
    }
    MarkCurrObject();
    SetDefState();
}

static void DoObjectRecreate( POINT pt )
/**************************************/
{
    /* recreate the current object based on the mouse movement */
    POINT  point;
    OBJPTR currobj;

    point = pt;
    SnapPointToGrid( &point );
    currobj = GetEditCurrObject();
    if( currobj != NULL ) {
        Recreate( currobj, &point );
    }
    SetPrevMouse( pt );
}

static void DoSelectRecreate( POINT pt )
/**************************************/
{
    /* recreate the current object based on the mouse movement */
    POINT  point;
    OBJPTR eatom;

    point = pt;
    eatom = GetSelectEatom();
    if( eatom != NULL ) {
        Recreate( eatom, &point );
    }
    SetPrevMouse( pt );
}


static void FinishCreate( POINT pt )
/**********************************/
{
    /* finish the create operation */
    OBJPTR currobj;

    pt = pt;
    currobj = GetEditCurrObject();
    if( currobj != NULL ) {
        if( !Register( currobj ) ) {
            RestorePrevObject();
        }
    }
    MarkCurrObject();
    SetDefState();
}


static bool SignificantMove( POINT pt )
/*************************************/
{
    POINT prev;

    prev = GetPrevMouse();
    return( !(pt.x == prev.x && pt.y == prev.y) );
}


static bool Close( int cursor, int corner )
/*****************************************/
{
    /* decides if the cursor position is within half of the width of a sizing
     * square away from the corner
     */
    return( (cursor > corner - SQUAREWIDTH / 2) &&
            (cursor < corner + SQUAREWIDTH / 2) );
}


static bool CheckForSquare( RECT *rect, int x, char sizeid )
/**********************************************************/
{
    /* checks if the cursor is near any of the sizing squares */
    if( Close( x, rect->left ) && (sizeid & R_LEFT) ) {
        SetSize( R_LEFT );
        return( true );
    }
    if( Close( x, rect->right ) && (sizeid & R_RIGHT) ) {
        SetSize( R_RIGHT );
        return( true );
    }
    if( Close( x, (rect->left + rect->right) / 2 ) )  {
        return( true );
    }
    return( false );
}


static void CheckMousePosn( POINT pt )
/************************************/
{
    /*  Check the position of the mouse and change the mouse cursor if necessary */
    RECT        rect;
    RESIZE_ID   sizeid;
    OBJPTR      currobj;

    currobj = GetPrimaryObject();
    if( currobj == NULL ) {
        return;
    }
    ResetSize();
    Location( currobj, &rect );
    sizeid = R_ALL;
    GetResizeInfo( currobj, &sizeid );
    if( sizeid != R_NONE ) {
        if( (sizeid & R_TOP) && Close( pt.y, rect.top ) ) {
            /* check for sizing boxes along top edge */
            if( CheckForSquare( &rect, pt.x, sizeid ) ) {
                SetSize( R_TOP );
            }
        } else if( (sizeid & R_BOTTOM) && Close( pt.y, rect.bottom ) ) {
            /* check for sizing boxes along bottom edge */
            if( CheckForSquare( &rect, pt.x, sizeid ) ) {
                SetSize( R_BOTTOM );
            }
        } else if( Close( pt.y, (rect.top + rect.bottom) / 2 ) ) {
            /* check for sizing on left or right edges */
            if( (sizeid & R_LEFT) && Close( pt.x, rect.left ) ) {
                SetSize( R_LEFT );
            } else if( (sizeid & R_RIGHT) && Close( pt.x, rect.right ) ) {
                SetSize( R_RIGHT );
            }
        }
    }
    if( GetState() == OVERBOX ) {
        if( !Sizing( R_ALL ) ) {
            /* was over a sizing square but isn't anymore */
            SetDefState();
        }
    } else if( Sizing( R_ALL ) ) {
        /* wasn't over a sizing squaure but now it is */
        SetState( OVERBOX );
    }
    SetPrevMouse( pt );
}


static void DoObjectMove( POINT pt )
/**********************************/
{
    /* move the current object based on the mouse movement */
    POINT       lastmouse;
    POINT       offset;
    OBJPTR      currobj;

    lastmouse = GetPrevMouse();
    offset.x = pt.x - lastmouse.x;
    offset.y = pt.y - lastmouse.y;
    for( currobj = GetEditCurrObject(); currobj != NULL; currobj = GetNextEditCurrObject( currobj ) ) {
        if( GetObjptr( GetObjptr( currobj ) ) != NULL ) {
            Move( currobj, &offset, true );
        }
    }
    SetPrevMouse( pt );
}


static void DoObjectResize( POINT pt )
/************************************/
{
    /* resize the current object based on the mouse movement */
    RECT   newloc;
    POINT  lastmouse;
    POINT  offset;
    OBJPTR currobj;

    SnapPointToResizeGrid( &pt );
    lastmouse = GetPrevMouse();
    SnapPointToResizeGrid( &lastmouse );
    offset.x = pt.x - lastmouse.x;
    offset.y = pt.y - lastmouse.y;
    if( offset.x != 0 || offset.y != 0 ) {
        newloc.top = offset.y;
        newloc.bottom = offset.y;
        newloc.left = offset.x;
        newloc.right = offset.x;
        currobj = GetEditCurrObject();
        if( currobj != NULL ) {
            Resize( currobj, &newloc, true );
        }
        SetPrevMouse( pt );
    }
}

static void FindPasteOffset( POINT *offset, POINT mouse )
/*******************************************************/
{
    /* Figure out how much to move all of the current objects so that the one
     * that has it's top left corner fartherest to the left has the
     * cursor at it's top left corner, and all other objects are positioned
     * relative to it, to the right.
     */
    OBJPTR  obj;
    RECT    left;
    RECT    rect;
    void    *clist;

    left.left = SHRT_MAX;
    for( clist = GetClipList(); clist != NULL; clist = NextClipList( clist ) ) {
        obj = GetClipObject( clist );
        Location( obj, &rect );
        if( rect.left < left.left ) {
            CopyRect( &left, &rect );
        }
    }
    offset->x = mouse.x - left.left;
    offset->y = mouse.y - left.top;
}

static void BeginPaste( POINT pt, WORD keystate, OBJPTR d )
/*********************************************************/
{
    /* begin a paste operation */
    OBJPTR  object;
    OBJPTR  newobj;
    RECT    rect;
    POINT   offset;
    OBJPTR  eatom;
    void    *clist;

    keystate = keystate;              /* ref'd to avoid warnings */
    d = d;
    SetState( PASTEING );
    FindPasteOffset( &offset, pt );
    ResetCurrObject( false );
    StartCurrObjMod();
    for( clist = GetClipList(); clist != NULL; clist = NextClipList( clist ) ) {
        object = GetClipObject( clist );
        CopyObject( object, &newobj, NULL );
        Location( newobj, &rect );
        OffsetRect( &rect, offset.x, offset.y );
        eatom = Create( O_EATOM, newobj, &rect, NULL );
        AddCurrObject( eatom );
    }
    EndCurrObjMod();
}

static void DoPasteMove( POINT pt )
/*********************************/
{
    /* move the current object based on the mouse movement */
    POINT  lastmouse;
    POINT  offset;
    bool   flag;

    lastmouse = GetPrevMouse();
    offset.x = pt.x - lastmouse.x;
    offset.y = pt.y - lastmouse.y;
    flag = true;
    ExecuteCurrObject( MOVE, &offset, &flag );
    SetPrevMouse( pt );
}


static void FinishPaste( POINT pt )
/*********************************/
{
    /* finish a paste operation */
    OBJPTR      parent;
    POINT       loc_pt;
    OBJPTR      eatom;
    OBJPTR      object;
    OBJPTR      currobj;
    RECT        rect;
    LIST        *newcurrobj;

    newcurrobj = NULL;
    SnapPointToGrid( &pt );
    for( currobj = GetEditCurrObject(); currobj != NULL; currobj = GetEditCurrObject() ) {
        Location( currobj, &rect );
        loc_pt.x = rect.left;
        loc_pt.y = rect.top;
        SnapPointToGrid( &loc_pt );
        parent = FindOneObjPt( loc_pt );
        eatom = GetObjptr( currobj );
        object = GetObjptr( eatom );
        if( PasteObject( object, parent, loc_pt ) ) {
            ListAddElt( &newcurrobj, object );
        }
        Destroy( eatom, false );
        DeleteCurrObject( currobj );
        MarkCurrObject();
    }
    StartCurrObjMod();
    while( newcurrobj != NULL ) {
        AddCurrObject( ListElement( newcurrobj ) );
        newcurrobj = ListConsume( newcurrobj );
    }
    EndCurrObjMod();
    SetDefState();
}

static void PointSelect( POINT pt )
/*********************************/
{
    OBJPTR      currobj;
    OBJPTR      currobjptr;
    LIST        *list;

    FindObjectsPt( pt, &list );
    for( ; list != NULL; list = ListConsume( list ) ) {
        currobj = ListElement( list );
        currobjptr = GetCurrObjptr( currobj );
        if( GetShift() ) { /* Doing multiple select */
            if( currobjptr != NULL ) {
                /* Object is already current so make it primary */
                SetPrimaryObject( currobjptr );
            } else {
                AddCurrObject( currobj );
            }
        } else if( GetControl() ) {
            /* Toggle the selection status of the object */
            if( currobjptr != NULL ) {
                DeleteCurrObjptr( currobj );
                MarkCurrObject();
            } else {
                AddCurrObject( currobj );
            }
        } else {
            SetCurrObject( currobj );
        }
    }
}

typedef enum {
    RECT_EQUAL,
    RECT_A_IN_B,
    RECT_B_IN_A,
    RECT_DISJOINT,
    RECT_INTERSECT
} compare_rect_rc;

static compare_rect_rc CompareRect( LPRECT rect_a, LPRECT rect_b )
/****************************************************************/
{
    RECT    intersect;

    if( EqualRect( rect_a, rect_b ) ) {
        return( RECT_EQUAL );
    } else if( IntersectRect( &intersect, rect_a, rect_b ) ) {
        if( EqualRect( rect_a, &intersect ) ) {
            return( RECT_A_IN_B );
        } else if( EqualRect( rect_b, &intersect ) ) {
            return( RECT_B_IN_A );
        } else {
            return( RECT_INTERSECT );
        }
    } else {
        return( RECT_DISJOINT );
    }
}

static OBJPTR FindBSelectRoot( LPRECT rect )
/******************************************/
{
    RECT            obj_loc;
    OBJPTR          root;
    OBJPTR          obj;
    LIST            *list;       // children of root
    SUBOBJ_REQUEST  req;
    compare_rect_rc rc;

    root = GetMainObject();
    req.a.ty = ALL;
    for( ;; ) {
        list = NULL;
        FindObjList( root, &req, &list );
        for( ; list != NULL; list = ListConsume( list ) ) {
            /* check each child for containment */
            obj = ListElement( list );
            Location( obj, &obj_loc );
            rc = CompareRect( rect, &obj_loc );
            if( rc == RECT_A_IN_B || rc == RECT_EQUAL ) {
                /* we have containment in one of the children so set the */
                /* root to that child and start over */
                root = obj;
                break;
            }
        }
        if( list == NULL ) {
            /* we made it down the list without finding further containment */
            break;
        }
        ListFree( list );
    }
    return( root );
}

static void FindBSelectChildren( OBJPTR root, LIST **child, LIST **gchild )
/*************************************************************************/
{
    /* find the children and grand children of root */
    SUBOBJ_REQUEST  req;
    LIST            *curr;
    LIST            *new_gchild;

    req.a.ty = ALL;
    *child = NULL;
    *gchild = NULL;
    FindObjList( root, &req, child );
    for( curr = *child; curr != NULL; curr = ListNext( curr ) ) {
        new_gchild = NULL;
        FindObjList( ListElement( curr ), &req, &new_gchild );
        ListMerge( gchild, new_gchild );
    }
}

static bool BuildBSelectList( LPRECT rect, LIST *child, LIST **sel )
/******************************************************************/
{
    /* returns TRUE if at least one of the objects in sel was contained */
    bool        contained;
    RECT        obj_loc;
    LIST        *curr;
    OBJPTR      currobj;

    *sel = NULL;
    contained = false;
    for( curr = child; curr != NULL; curr = ListNext( curr ) ) {
        currobj = ListElement( curr );
        Location( currobj, &obj_loc );
        switch( CompareRect( rect, &obj_loc ) ) {
        case RECT_EQUAL:
        case RECT_B_IN_A:
            contained = true;
            /* fall through */
        case RECT_A_IN_B:
        case RECT_INTERSECT:
            ListAddElt( sel, currobj );
            break;
        }
    }
    return( contained );
}

static void BandedSelect( LPRECT rect )
/*************************************/
{
    /* Banded selection is done by finding the lowest level object that fully */
    /* contains the selection rect and calling that the root. The objects that */
    /* may be select are either children of the root or grandchildren of the root */
    /* which intersect with the rect. Preference is given to which ever of the two*/
    /* sets contains an object which is fully contained in rect with ties going */
    /* to the child set. */
    OBJPTR      root;
    LIST        *child;         // children of root
    LIST        *gchild;        // grandchildren of root
    LIST        *child_sel;
    LIST        *gchild_sel;
    LIST        *sel_list;
    bool        child_cont;     // some children were fully contained in rect
    bool        gchild_cont;    // some gchildren were fully contained in rect
    OBJPTR      currobj;
    OBJPTR      currptr;

    root = FindBSelectRoot( rect );
    FindBSelectChildren( root, &child, &gchild );
    /* if the root has no children try one level up so that, if possible, */
    /* something will be select on a banded select */
    if( ListCount( child ) == 0 ) {
        GetObjectParent( root, &root );
        if( root == NULL ) {
            /* this can happen if root was the main object */
            return;
        }
        FindBSelectChildren( root, &child, &gchild );
    }

    child_cont = BuildBSelectList( rect, child, &child_sel );
    gchild_cont = BuildBSelectList( rect, gchild, &gchild_sel );

    /* use the intersecting children if either any of them were fully */
    /* contained in rect or if no grandchildren were fully contained */
    if( child_cont || !gchild_cont ) {
        sel_list = child_sel;
    } else {
        sel_list = gchild_sel;
    }

    /* select the objects if there are any */
    if( sel_list != NULL ) {
        if( !GetShift() ) {
            ResetCurrObject( false );
        }
        StartCurrObjMod();
        for( ; sel_list != NULL; sel_list = ListNext( sel_list ) ) {
            currobj = ListElement( sel_list );
            currptr = GetCurrObjptr( currobj );
            if( currptr == NULL ) {
                AddCurrObject( currobj );
            } else {
                /* Object already in list, just make it primary */
                SetPrimaryObject( currptr );
            }
        }
        EndCurrObjMod();
    }

    if( child != NULL ) ListFree( child );
    if( gchild != NULL ) ListFree( gchild );
    if( child_sel != NULL ) ListFree( child_sel );
    if( gchild_sel != NULL ) ListFree( gchild_sel );
}

static void FinishSelect( POINT fin_pt )
/**************************************/
{
    /* Finish the select operation */
    OBJPTR          eatom;
    RECT            rect;
    POINT           pt;

    fin_pt = fin_pt;
    eatom = GetSelectEatom();
    if( eatom != NULL ) {
        GetAnchor( eatom, &pt );
        Location( eatom, &rect );
        Destroy( eatom, false );
        SetSelectEatom( NULL );
        if( IsRectEmpty( &rect ) ) {
            PointSelect( pt );
        } else {
            BandedSelect( &rect );
        }
    }
    SetDefState();
}

static void FinishMovePending( POINT pt )
/***************************************/
{
    /* Leave the MovePending state because mouse up happened before a move */
    OBJPTR          obj;

    SetDefState();
    if( GetControl() ) {
        obj = FindOneObjPt( pt );
        DeleteCurrObjptr( obj );
    }
    MarkCurrObject();
}

void FinishMoveOperation( bool change_state )
/*******************************************/
{
    /* Finish the move operation */
    OBJPTR      currobj;
    LIST        *mycurrobjlist;
    LIST        *clist;
    DLIST       *movedlist;
    bool        success;
    OBJPTR      eatom;
    OBJPTR      obj;
    OBJPTR      primary;
    DLIST       *dlist;
    DLIST_ELT   elt;
    DLIST       *newmovedlist;
    RECT        rect;

    if( change_state )
        SetState( MOVING );
    primary = GetObjptr( GetCurrObject() );
    mycurrobjlist = GetCurrObjectList();
    for( clist = mycurrobjlist; clist != NULL; clist = ListNext( clist ) ) {
        RemoveFromParent( ListElement( clist ) );
    }
    SetShowError( false ); /* Don't want error message in the middle of
                              multiple moves - paint won't work properly */
    success = true;        /* Flag to indicate whether or not any move failed */
    movedlist = NULL;      /* List of objects that have been moved */
    dlist = OrderList( mycurrobjlist );
    /* Move all objects until done or a move fails.  Move in correct order */
    while( dlist != NULL ) {
        elt = GetNextElement( dlist );
        success = Register( elt.original );
        if( !success ) {
            break;
        }
        DListRemoveElt( &dlist, elt );
        DListAddElt( &movedlist, elt );
    }
    /* If a move failed, first remove all objects from their parents and
     * then undo all moves that already happened, including the
     * one that failed. Use the correct order.
     */
    if( !success ) {
        newmovedlist = DListCopy( movedlist );
        while( newmovedlist != NULL ) {
            elt = GetNextElement( newmovedlist );
            RemoveFromParent( elt.original );
            DListRemoveElt( &newmovedlist, elt );
        }
        while( movedlist != NULL ) {
            elt = GetNextElement( movedlist );
            UndoMove( elt.original );
            DListRemoveElt( &movedlist, elt );
        }
    }
    /* Notify all objects that the move operation is done */
    for( obj = GetEditCurrObject(); obj != NULL; obj = GetNextEditCurrObject( obj ) ) {
        Notify( obj, MOVE_END, NULL );
    }
    /* Register all of the objects that got removed from the parent but
     * never got moved or moved back.  Use correct order.
     */
    ResetCurrObject( false );
    while( dlist != NULL ) {
        elt = DListElement( dlist );
        eatom = elt.original;
        obj = GetObjptr( eatom );
        Location( eatom, &rect );
        MarkInvalid( &rect );
        Register( obj );
        DListRemoveElt( &dlist, elt );
    }
    StartCurrObjMod();
    for( clist = mycurrobjlist; clist != NULL; clist = ListConsume( clist ) ) {
        obj = ListElement( clist );
        AddCurrObject( GetObjptr( obj ) );
        Destroy( obj, false );
    }
    DListFree( movedlist );
    DListFree( dlist );
    /* Set correct object to be primary */
    if( primary != NULL ) {
        currobj = GetCurrObjptr( primary );
        if( currobj != NULL ) {
            SetPrimaryObject( currobj );
        }
    }
    EndCurrObjMod();
    SetShowError( true );  /* Set flag saying it's okay to show errors now */
    if( change_state )
        SetDefState();
    /* Any error shown will cause repainting, so be sure */
    /* that we repaint the current object markers */
    UpdateWindow( GetAppWnd() );
    ReportPending();   /* Report any pending errors */
}

static DLIST *OrderList( LIST *list )
/***********************************/
{
    /* Return a DLIST that has an element for each element in list (in original)
     * and that object's priority in copy.
     */
    DLIST       *dlist;
    DLIST_ELT   elt;
    OBJPTR      currobj;
    int         priority;
    OBJPTR      obj;
    LIST        *mylist;

    dlist = NULL;
    for( mylist = list; mylist != NULL; mylist = ListNext( mylist ) ) {
        currobj = ListElement( mylist );
        elt.original = currobj;
        obj = GetObjptr( currobj );
        GetPriority( obj, &priority );
        elt.copy = (OBJPTR)priority;
        DListAddElt( &dlist, elt );
    }
    return( dlist );
}

static DLIST_ELT GetNextElement( DLIST *dlist )
/*********************************************/
{
    /* Return the lowest priority element in the list */
    DLIST_ELT   elt;
    DLIST_ELT   curr_elt;
    DLIST       *mydlist;

    elt.original = NULL;
    elt.copy = 0;
    mydlist = dlist;
    if( mydlist == NULL )
        return( elt );
    curr_elt = DListElement( mydlist );
    for( mydlist = DListNext( mydlist ); mydlist != NULL; mydlist = DListNext( mydlist ) ) {
        elt = DListElement( mydlist );
        if( (int)curr_elt.copy > (int)elt.copy ) {
            curr_elt.copy = elt.copy;
            curr_elt.original = elt.original;
        }
    }
    return( curr_elt );
}

void BeginMoveOperation( LIST *mycurrobjlist )
/********************************************/
{
    /* Prepare for the move operation */
    POINT          init;
    OBJPTR         eatom;
    OBJPTR         currobj;
    RECT           rect;
    OBJPTR         primary;

    /* Remember the primary object so that it's eatom can be set to primary */
    primary = GetCurrObject();
    ResetCurrObject( false );
    currobj = ListElement( mycurrobjlist );
    StartCurrObjMod();
    while( currobj != NULL ) {
        Location( currobj, &rect );
        eatom = Create( O_EATOM, currobj, &rect, NULL );
        AddCurrObject( eatom );
        /* Remember eatom if this is the eatom for the primary object */
        if( currobj == primary ) {
            primary = GetPrimaryObject();
        }
        init.x = rect.left;
        init.y = rect.top;
        SnapPointToGrid( &init );
        init.x -= rect.left;
        init.y -= rect.top;
        if( init.x != 0 || init.y != 0 ) {
            Move( eatom, &init, true );
        }
        mycurrobjlist = ListNext( mycurrobjlist );
        if( mycurrobjlist == NULL ) {
            break;
        }
        currobj = ListElement( mycurrobjlist );
    }
    /* Set correct eatom to be primary */
    if( primary != NULL ) {
        SetPrimaryObject( primary );
    }
    EndCurrObjMod();
    /* notify the objects of the move start */
    for( currobj = GetEditCurrObject(); currobj != NULL; currobj = GetNextEditCurrObject( currobj ) ) {
        Notify( currobj, MOVE_START, NULL );
    }
}

void AbortMoveOperation( void )
/*****************************/
{
    LIST    *objlist;
    OBJPTR  obj;
    OBJPTR  eatom;
    OBJPTR  currobj;
    OBJPTR  primary;

    objlist = GetCurrObjectList();
    primary = GetObjptr( GetCurrObject() );

    /* Notify all objects that the move operation is done */
    for( obj = GetEditCurrObject(); obj != NULL; obj = GetNextEditCurrObject( obj ) ) {
        Notify( obj, MOVE_END, NULL );
    }

    ResetCurrObject( false );

    StartCurrObjMod();
    for( ; objlist != NULL; objlist = ListConsume( objlist ) ) {
        eatom = ListElement( objlist );
        currobj = GetObjptr( eatom );
        AddCurrObject( currobj );
        Destroy( eatom, false );
    }

    /* Set correct object to be primary */
    if( primary != NULL ) {
        currobj = GetCurrObjptr( primary );
        if( currobj != NULL ) {
            SetPrimaryObject( currobj );
        }
    }
    EndCurrObjMod();
}

static void FinishActionAborted( POINT pt )
/*****************************************/
{
    pt = pt;

    SetState( DORMANT );
}


static void (*MousePressActions[])( POINT, WORD, OBJPTR ) = {
    #define pick(id,curs,kdown,kup,mpres,mmove,mrel) mpres,
    #include "_state.h"
    #undef pick
};

static void (*MouseMoveActions[])( POINT ) = {
    #define pick(id,curs,kdown,kup,mpres,mmove,mrel) mmove,
    #include "_state.h"
    #undef pick
};

static void (*MouseReleaseActions[])( POINT ) = {
    #define pick(id,curs,kdown,kup,mpres,mmove,mrel) mrel,
    #include "_state.h"
    #undef pick
};


static void ResetEdit( POINT pt, WORD keystate, OBJPTR d )
/********************************************************/
{
    /*  Reset the previous editing operation and proceed with the default
     *  action for a mouse press.
     */
    OBJPTR  currobj;

    d = d;
    currobj = GetEditCurrObject();
    if( currobj != NULL ) {
        Notify( currobj, TERMINATE_EDIT, NULL );
    }
    SetBaseState( DORMANT );
    SetDefState();
    MousePressActions[GetState()]( pt, keystate, NULL );
}

void ProcessButtonDown( POINT point, WORD keystate, OBJPTR obj )
/**************************************************************/
{
    /* responds to a button down message from the mouse */
    STATE_ID st;

    st = GetState();
    if( st != PASTE_PENDING ) {
        SetCapture( GetAppWnd() );
    }
    MousePressActions[st]( point, keystate, obj );
    SetPrevMouse( point );
}

void ProcessButtonUp( POINT point )
/*********************************/
{
    /* responds to a button up message from the mouse */
    ProcessMouseMove( point );
    MouseReleaseActions[GetState()]( point );
    CheckMousePosn( point );
    SnapPointToGrid( &point );
    SetPrevMouse( point );
    ReleaseCapture();
}


void ProcessMouseMove( POINT point )
/**********************************/
{
    /* responds to a button down message from the mouse */
    STATE_ID st;

    st = GetState();
    if( st != MOVE_PENDING || SignificantMove( point ) ) {
        MouseMoveActions[st]( point );
    }
}
