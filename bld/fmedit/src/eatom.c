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


/* EATOM.C - routines to handle the operations on an EATOM */

#include <windows.h>

#include "fmedit.def"
#include "memory.def"
#include "paint.def"
#include "object.def"
#include "state.def"
#include "currobj.def"
#include "scroll.def"
#include "global.h"
#include "grid.def"

#include "eatom.h"

/* This boolean DEFINE controls where eatom's get each their own window */
/* use the app window. */
#define USE_OWN_WINDOW 1

/* forward references */

static BOOL PASCAL EAtomDispatch( ACTION, EATOM *, void *, void * );
static BOOL EAtomLocation( EATOM *, RECT *, void * );
static BOOL EAtomMove( EATOM *, POINT *, void * );
static BOOL EAtomResize( EATOM *, RECT *, void * );
static BOOL EAtomRegister( EATOM *, void *, void * );
static BOOL EAtomRecreate( EATOM *, POINT *, void * );
static BOOL EAtomDestroy( EATOM *, void *, void * );
static BOOL EAtomValidateAction( EATOM *, ACTION *, void * );
static BOOL EAtomDraw( EATOM *, RECT *, HDC * );
static BOOL EAtomGetObjptr( EATOM *, OBJPTR *, void * );
static BOOL EAtomUndoMove( EATOM *, void *, void * );
static BOOL EAtomRemoveFromParent( EATOM *, void *, void * );
static BOOL EAtomGetAnchor( EATOM *, POINT *, void * );
static BOOL EAtomNotify( EATOM * obj, NOTE_ID * id, void * p2 );
static BOOL EAtomIsMarkValid( EATOM * obj, BOOL * valid, void * p2 );

static void ShowEAtomRect( EATOM * obj );

static DISPATCH_ITEM EAtomActions[] = {
    { LOCATE,               EAtomLocation           }
,   { MOVE,                 EAtomMove               }
,   { RESIZE,               EAtomResize             }
,   { REGISTER,             EAtomRegister           }
,   { RECREATE,             EAtomRecreate           }
,   { DESTROY,              EAtomDestroy            }
,   { VALIDATE_ACTION,      EAtomValidateAction     }
,   { DRAW,                 EAtomDraw               }
,   { GET_OBJPTR,           EAtomGetObjptr          }
,   { UNDO_MOVE,            EAtomUndoMove           }
,   { REMOVE_FROM_PARENT,   EAtomRemoveFromParent   }
,   { GET_ANCHOR,           EAtomGetAnchor          }
,   { NOTIFY,               EAtomNotify             }
,   { IS_MARK_VALID,        EAtomIsMarkValid        }
};

#define MAX_ACTIONS (sizeof(EAtomActions)/sizeof(DISPATCH_ITEM))

extern BOOL PASCAL EAtomDispatch( ACTION id, EATOM * obj,
                                      void * p1, void * p2 )
/**********************************************************/

/* dispatch the desired operation to the correct place */

  {
    int i;

    if( id >= APP_ACTIONS  ) {
        return( Forward( obj->obj, id, p1, p2 ));
    }

    for(i=0; i<MAX_ACTIONS; i++ ) {
        if( EAtomActions[i].id == id ) {
            return((EAtomActions[i].rtn)( obj, p1, p2));
        }
    }
    return( FALSE );
  }

static BOOL EAtomValidateAction( EATOM * obj, ACTION * idptr, void * p2 )
/*********************************************************************/

/* check if the desired action is valid for and EATOM */

  {
    int i;

    obj = obj;        /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */

    if( *idptr >= APP_ACTIONS  ) {
        return( Forward( obj->obj, VALIDATE_ACTION, idptr, p2 ));
    }
    for(i=0; i<MAX_ACTIONS; i++ ) {
        if( EAtomActions[i].id == *idptr ) {
            return( TRUE );
        }
    }
    return( FALSE );
  }


extern OBJPTR EAtomCreate( OBJPTR parent, RECT * loc, OBJPTR handle )
/*******************************************************************/

/* create an EATOM object */

  {
    EATOM * new;
    POINT   offset;
    STATE_ID st;

    new = EdAlloc( sizeof( EATOM ) );
    new->invoke = &EAtomDispatch;
    new->obj = parent;
    new->parent = NULL;
    if( handle == NULL ) {
        new->handle = new;
    } else {
        new->handle = handle;
    }
    new->rect = *loc;
    new->anchor.x = new->rect.left;
    new->anchor.y = new->rect.top;
    new->show = GetShowEatoms();
    GetOffset( &offset );
    if( new->show ) {
        #if USE_OWN_WINDOW
            new->hwnd = CreateWindow( "EAtomClass",
                                      NULL,
                                      WS_VISIBLE | WS_CHILD | WS_DISABLED,
                                      new->rect.left - offset.x,
                                      new->rect.top - offset.y,
                                      new->rect.right - new->rect.left,
                                      new->rect.bottom - new->rect.top,
                                      GetAppWnd(),
                                      NULL,
                                      GetInst(),
                                      NULL );
        #else
            new->hwnd = GetAppWnd();
        #endif
    } else {
        new->hwnd = NULL;
    }
    new->displayed = FALSE;
    st = GetState();
    if( st != SELECTING ) {
        MouseAction( &new->rect );
    }
    ShowEAtomRect( new );
    return( new );
  }


static BOOL EAtomLocation( EATOM * obj, RECT * rect, void * p2 )
/**************************************************************/

/* return the location of the atom */

  {
    p2 = p2;          /* ref'd to avoid warning */
    *rect = obj->rect;
    return( TRUE );
  }

static void CalcDrawRect( RECT * rect, RECT * drawrect )
/****************************************************/
/* Calculate the rectangle for this eatom's client area */
{
#if USE_OWN_WINDOW
    drawrect->left = 0;
    drawrect->top = 0;
    drawrect->right = rect->right - rect->left;
    if( drawrect->right < 0 ) {
        drawrect->right = 0;
    }
    drawrect->bottom = rect->bottom - rect->top;
    if( drawrect->bottom < 0 ) {
        drawrect->bottom = 0;
    }
#else
    POINT   offset;

    GetOffset( &offset );
    drawrect->left = rect->left - offset.x;
    drawrect->right = rect->right - offset.x;
    drawrect->top = rect->top - offset.y;
    drawrect->bottom = rect->bottom - offset.y;
#endif
}

static void DrawEAtomRect( EATOM * obj )
/**************************************/
{
    RECT  drawrect;
    HDC   hdc;

    if( obj->show ) {
        CalcDrawRect( &obj->rect, &drawrect );
        hdc = GetDC( obj->hwnd );
        DrawFocusRect( hdc, &drawrect );
        ReleaseDC( obj->hwnd, hdc );
    }
} /* DrawEAtomRect */

static void ShowEAtomRect( EATOM * obj )
/**************************************/
{
    if( !obj->displayed ) {
        DrawEAtomRect( obj );
        obj->displayed = TRUE;
    }
} /* ShowEAtomRect */

static void HideEAtomRect( EATOM * obj )
/**************************************/
{
    if( obj->displayed ) {
        DrawEAtomRect( obj );
        obj->displayed = FALSE;
    }
} /* HideEAtomRect */

static BOOL EAtomMove( EATOM * obj, POINT * offset, void * p2 )
/*************************************************************/

/*  do the move operation.  The amount to move is stored in the left and
 *  top values of the passed rect.
 */

  {
    POINT scrolloffset;

    p2 = p2;          /* ref'd to avoid warning */
    HideEAtomRect( obj );
    AutoScroll( obj->rect, *offset );
    GetOffset( &scrolloffset );
    OffsetRect( &obj->rect, offset->x, offset->y );
    #if USE_OWN_WINDOW
        if( obj->show ) {
            InvalidateRect( obj->hwnd, NULL, TRUE );
            MoveWindow( obj->hwnd, obj->rect.left - scrolloffset.x,
                                   obj->rect.top - scrolloffset.y,
                                   obj->rect.right - obj->rect.left,
                                   obj->rect.bottom - obj->rect.top,
                                   FALSE );
        }
    #endif
    MouseAction( &obj->rect );
    ShowEAtomRect( obj );
    return( TRUE );
  }


static BOOL EAtomResize( EATOM * obj, RECT * info, void * p2 )
/************************************************************/

/*  do the resize operation.  The amount to resize is stored in the left and
 *  top values of the passed rect.
 */

  {
    POINT point;
    POINT scrolloffset;
    RECT  drawrect;
    RECT  newrect;

    point.x = 0;
    point.y = 0;
    p2 = p2;          /* ref'd to avoid warning */
    CalcDrawRect( &obj->rect, &drawrect );
    CopyRect( &newrect, &obj->rect );
    if( Sizing( R_TOP ) ) {
        newrect.top = obj->rect.top + info->top;
        point.y =  info->top;
    }
    if( Sizing( R_BOTTOM ) ) {
        newrect.bottom = obj->rect.bottom + info->bottom;
        point.y = info->bottom;
    }
    if( Sizing( R_LEFT ) ) {
        newrect.left = obj->rect.left + info->left;
        point.x = info->left;
    }
    if( Sizing( R_RIGHT ) ) {
        newrect.right = obj->rect.right + info->right;
        point.x = info->right;
    }
    if( point.x || point.y  ) {
        HideEAtomRect( obj );
        AutoScroll( obj->rect, point );
        GetOffset( &scrolloffset );
        CopyRect( &obj->rect, &newrect );
        #if USE_OWN_WINDOW
            if( obj->show ) {
                InvalidateRect( obj->hwnd, NULL, TRUE );
            }
        #endif
        point.x = obj->rect.right - obj->rect.left;
        point.y = obj->rect.bottom - obj->rect.top;
        if( point.x < 0 ) {
            point.x = 0;
        }
        if( point.y < 0 ) {
            point.y = 0;
        }
        #if USE_OWN_WINDOW
            if( obj->show ) {
                MoveWindow( obj->hwnd, obj->rect.left - scrolloffset.x,
                                       obj->rect.top - scrolloffset.y,
                                       point.x,
                                       point.y,
                                       FALSE );
            }
        #endif
        MouseAction( &obj->rect );
        ShowEAtomRect( obj );
    }
    return( TRUE );
  }


static BOOL EAtomRecreate( EATOM * obj, POINT * pt, void * p2 )
/*************************************************************/

/*  Recreate the passed object by using the point in anchor as one
 *  corner and the passed point as the other
 */

  {
    POINT scrolloffset;
    RECT  drawrect;
    RECT  newrect;
    POINT delta;


    p2 = p2;          /* ref'd to avoid warning */
    CalcDrawRect( &obj->rect, &drawrect );
    CopyRect( &newrect, &obj->rect );
    if( pt->x < obj->anchor.x ) {
        delta.x = pt->x - obj->rect.left;
        newrect.left = pt->x;
        newrect.right = obj->anchor.x;
    }  else {
        delta.x = pt->x - obj->rect.right;
        newrect.left = obj->anchor.x;
        newrect.right = pt->x;
    }
    if( pt->y < obj->anchor.y ) {
        delta.y = pt->y - obj->rect.top;
        newrect.top = pt->y;
        newrect.bottom = obj->anchor.y;
    }  else {
        delta.y = pt->y - obj->rect.bottom;
        newrect.top = obj->anchor.y;
        newrect.bottom = pt->y;
    }
    if( !EqualRect( &obj->rect, &newrect ) ) {
        HideEAtomRect( obj );
        AutoScroll( newrect, delta );
        GetOffset( &scrolloffset );
        CopyRect( &obj->rect, &newrect );
        #if USE_OWN_WINDOW
            if( obj->show ) {
                InvalidateRect( obj->hwnd, NULL, TRUE );
                MoveWindow( obj->hwnd, obj->rect.left - scrolloffset.x,
                                       obj->rect.top - scrolloffset.y,
                                       obj->rect.right - obj->rect.left,
                                       obj->rect.bottom - obj->rect.top,
                                       FALSE );
            }
        #endif
        if( GetState() != SELECTING ) {
            MouseAction( &obj->rect );
        }
        ShowEAtomRect( obj );
    }
    return( TRUE );
  }


static BOOL EAtomRegister( EATOM * obj, void * p1, void * p2 )
/************************************************************/

/* register the object */


  {
    BOOL       ret;
    OBJPTR     new;
    CURROBJPTR currobj;
    STATE_ID   state;
    POINT      off;

    p1 = p1;          /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */
    ret = TRUE;
    currobj = GetCurrObjptr( obj );
    state = GetState();
    if( !IsRectEmpty( (LPRECT) &obj->rect ) || state == CREATING ) {
        /* register the completed action with the appropriate object */
        switch( state ) {
        case CREATING :
            DeleteCurrObject( currobj );
            new = Create( GetBaseObjType(), obj->obj, &obj->rect, NULL  );
            if( new != NULL ) {
                if( Register( new ) ) {
                    AddCurrObject( new );
                } else {
                    ret = FALSE;
                    Destroy( new, FALSE );
                }
            } else {
                ret = FALSE;
            }
            Destroy( obj, FALSE );
            break;
        case MOVING :
            off.x = obj->rect.left;
            off.y = obj->rect.top;
            SnapPointToGrid( &off );
            off.x -= obj->anchor.x;
            off.y -= obj->anchor.y;
            ret = Move( obj->obj, &off, TRUE );
            if( ret ) {
                obj->offset = off;
                currobj = GetCurrObjptr( obj->obj );
                if( currobj != NULL ) {
                    SetPrimaryObject( currobj );
                } else {
                    AddCurrObject( obj->obj );
                }
            }
            break;
        case SIZING :
            DeleteCurrObject( currobj );
            Resize( obj->obj, &obj->rect, TRUE );
            AddCurrObject( obj->obj );
            Destroy( obj, FALSE );
            break;
#ifdef DEBUG_ON
        default :
            MessageBox( GFileIO.hWnd, ( LPSTR ) "MOVE\RESIZE Error",
                        NULL, MB_OK | MB_ICONSTOP );
#endif
        }
        UpdateScroll();
    } else {
        /* a single click on the object implies making it current */
        AddCurrObject( obj->obj );
        Destroy( obj, FALSE );
    }
    return( ret );
  }

static BOOL EAtomDestroy( EATOM * obj, void * p1, void * p2 )
/***********************************************************/
/* destroy the EATOM */
{
    p1 = p1;          /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */

    #if USE_OWN_WINDOW
        if( obj->show ) {
            SendMessage( obj->hwnd, WM_KILLFOCUS, 0, 0 );
            DestroyWindow( obj->hwnd );
        }
    #else
        HideEAtomRect( obj );
    #endif
    EdFree( obj );
    return( TRUE );
}

static BOOL EAtomDraw( EATOM * obj, RECT * rect, HDC * hdc )
/**********************************************************/

  {
    RECT intersect;

    hdc = hdc;            /* ref'd to avoid warning */

    if( obj->displayed && IntersectRect( &intersect, rect, &obj->rect ) ) {
        DrawEAtomRect( obj );
    }
    return( TRUE );
  }

static BOOL EAtomGetObjptr( EATOM * obj, OBJPTR * newobj, void * p2 )
/******************************************************************/

/* Get the OBJPTR of the object associated with this EATOM */

  {
    p2 = p2;          /* ref'd to avoid warning */

    if( newobj != NULL ) {
        *newobj = obj->obj;
    }
    return( TRUE );
  }

static BOOL EAtomUndoMove( EATOM * obj, void * p1, void * p2 )
/************************************************************/

  {
    p1 = p1;          /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */

    GetObjectParent( obj->obj, &obj->parent );
    if( obj->parent != NULL ) {
        RemoveObject( obj->parent, obj->obj );
        obj->offset.x = - obj->offset.x;
        obj->offset.y = - obj->offset.y;
        return( Move( obj->obj, &obj->offset, TRUE ));
    } else {
        return( FALSE );
    }
  }

static BOOL EAtomRemoveFromParent( EATOM * obj, void * p1, void * p2 )
/********************************************************************/

  {
    p1 = p1;          /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */

    GetObjectParent( obj->obj, &obj->parent );
    if( obj->parent != NULL ) {
        RemoveObject( obj->parent, obj->obj );
        return( TRUE );
    } else  {
        return( TRUE );
    }
  }


static BOOL EAtomGetAnchor( EATOM * obj, POINT * pt, void * p2 )
/**************************************************************/

  {
    p2 = p2;
    *pt = obj->anchor;
    return( TRUE );
  }

static BOOL EAtomIsMarkValid( EATOM * obj, BOOL * valid, void * p2 )
/******************************************************************/
{
    obj = obj;
    p2 = p2;

    *valid = FALSE;
    return( TRUE );
} /* EAtomIsMarkValid */

static BOOL EAtomNotify( EATOM * obj, NOTE_ID * id, void * p2 )
/*************************************************************/
{
    switch( *id ) {
    case MOVE_START:
    case MOVE_END:
        if( obj->obj != NULL ) {
            return( Forward( obj->obj, NOTIFY, id, p2 ) );
        } else {
            return( FALSE );
        }
        break;
    default:
        return( FALSE );
        break;
    }
} /* EAtomNotify */


long WINIEXP EAtomWndProc( HWND wnd, unsigned message,
                           WPARAM wparam, LPARAM lparam )
/****************************************************/

/* processes messages */

  {
    PAINTSTRUCT    ps;

    switch( message ) {
        case WM_PAINT :
            BeginPaint( wnd, ( LPPAINTSTRUCT ) &ps );
            EndPaint( wnd, ( LPPAINTSTRUCT ) &ps );
            break;
        default :
            return( DefWindowProc( wnd, message, wparam, lparam ));
        break;

    }
    return( NULL );
  }

extern void InitEAtom()
/*********************/

/* Initialization for EATOM objects - register the EAtomClass */

  {
    WNDCLASS wc;

    wc.style = NULL;
    wc.lpfnWndProc = EAtomWndProc;      /* Function to retrieve messages for*/
                                        /* windows of this class.           */
    wc.cbClsExtra = 0;                  /* No per-class extra data.         */
    wc.cbWndExtra = 0;                  /* No extra data for each window    */
    wc.hInstance = GetInst();           /* Application that owns the class. */
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;            /* NULL background-invisible window */
    wc.lpszMenuName =  NULL;
    wc.lpszClassName = "EAtomClass";    /* Name used in call to CreateWindow*/
    RegisterClass( &wc );
  }

