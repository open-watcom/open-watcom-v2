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
* Description:  Routines to handle the operations on a OITEM.
*
****************************************************************************/


#include <string.h>
#include <windows.h>

#include "global.h"
#include "fmedit.def"
#include "memory.def"
#include "oitem.h"
#include "oitem.def"
#include "state.def"

/* forward references */

static BOOL WINAPI OItemDispatch( ACTION, OITEM *, void *, void * );
static BOOL OItemRegister( OBJPTR, void *, void * );
static BOOL OItemLocation( OBJPTR, void *, void * );
static BOOL OItemMove( OBJPTR, void *, void * );
static BOOL OItemResize( OBJPTR, void *, void * );
static BOOL OItemNotify( OBJPTR, void *, void * );
static void OItemSetNewParent( OITEM *, OBJPTR );
static BOOL OItemDestroy( OBJPTR, void *, void * );
static BOOL OItemValidateAction( OBJPTR, void *, void * );
static BOOL OItemCutObject( OBJPTR, void *, void * );
static BOOL OItemCopyObject( OBJPTR, void *, void * );
static BOOL OItemPasteObject( OBJPTR, void *, void * );
static BOOL OItemGetObjectParent( OBJPTR, void *, void * );
static BOOL OItemGetPriority( OBJPTR, void *, void * );
static BOOL OItemFindObjectPt( OBJPTR, void *, void * );

static DISPATCH_ITEM OItemActions[] = {
    { REGISTER,        OItemRegister        },
    { LOCATE,          OItemLocation        },
    { MOVE,            OItemMove            },
    { RESIZE,          OItemResize          },
    { NOTIFY,          OItemNotify          },
    { DESTROY,         OItemDestroy         },
    { VALIDATE_ACTION, OItemValidateAction  },
    { CUT,             OItemCutObject       },
    { COPY,            OItemCopyObject      },
    { PASTE,           OItemPasteObject     },
    { GET_PARENT,      OItemGetObjectParent },
    { GET_PRIORITY,    OItemGetPriority     },
    { FIND_OBJECTS_PT,  OItemFindObjectPt   }
};

#define MAX_ACTIONS (sizeof( OItemActions ) / sizeof( DISPATCH_ITEM ))

static FARPROC DispatchRtn = NULL;

BOOL WINAPI OItemDispatch( ACTION id, OITEM *obj, void *p1, void *p2 )
/********************************************************************/
{
    /* dispatch the desired operation to the correct place */
    int i;

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( OItemActions[i].id == id ) {
            return( (OItemActions[i].rtn)( obj, p1, p2) );
        }
    }
    return( FALSE );
}

static BOOL OItemValidateAction( OBJPTR _obj, void *_idptr, void *p2 )
/********************************************************************/
{
    /* check if the desired action is valid for and OITEM */
    OITEM   *obj = _obj;
    ACTION  *idptr = _idptr;
    int     i;

    obj = obj;        /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */
    if( *idptr == NOTIFY ) {
        return( *((NOTE_ID *)p2) == NEW_PARENT );
    }
    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( OItemActions[i].id == *idptr ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


static BOOL OItemMove( OBJPTR _oitem, void *_offset, void *user_action )
/**********************************************************************/
{
    /* Do the move operation  */
    OITEM   *oitem = _oitem;
    POINT   *offset = _offset;
    RECT    temp;
    OBJPTR  newparent;
    POINT   pt;

    temp = oitem->rect;
    OffsetRect( (LPRECT) &oitem->rect, offset->x, offset->y );
    if( *(BOOL *)user_action ) {
        pt.x = oitem->rect.left;
        pt.y = oitem->rect.top;
        newparent = FindOneObjPt( pt );
        if( newparent == NULL || !AddObject( newparent, oitem->handle ) ) {
            /*  Moving the OITEM in the parent failed, so reture FALSE
             *  so that the operation will be undone.
             */
            oitem->rect = temp;
            return( FALSE );
        }
    }
    MarkInvalid( &temp );
    MarkInvalid( &oitem->rect );
    return( TRUE );
}


static BOOL OItemNotify( OBJPTR _oitem, void *p1, void *p2 )
/**********************************************************/
{
    /* process notify message for an OITEM */
    OITEM   *oitem = _oitem;
    BOOL    ret;

    ret = FALSE;
    switch( *((NOTE_ID *)p1) ) {
    case NEW_PARENT:
        OItemSetNewParent( oitem, p2 );
        ret = TRUE;
        break;
    default:
        ret = FALSE;
        break;
    }
    return( ret );
}


extern OBJPTR OItemCreate( OBJPTR parent, RECT *rect, OBJPTR handle )
/*******************************************************************/
{
    /* create an OITEM object */
    OITEM *new;

    new = EdAlloc( sizeof( OITEM ) );
    new->invoke = DispatchRtn;
    new->parent = parent;
    if( parent != NULL ) {
        GetPriority( parent, &new->priority );
        new->priority++;
    } else {
        new->priority = 0;
    }
    if( handle == NULL ) {
        new->handle = new;
    } else {
        new->handle = handle;
    }
    new->rect = *rect;
    return( new );
}


static BOOL OItemRegister( OBJPTR _oitem, void *p1, void *p2 )
/************************************************************/
{
    /* register the oitem by adding it to the structures */
    OITEM   *oitem = _oitem;
    p1 = p1;          /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */

    if( AddObject( oitem->parent, oitem->handle ) ) {
        MarkInvalid( (LPRECT) &oitem->rect );
        return( TRUE );
    }
    return( FALSE );
}


static BOOL OItemLocation( OBJPTR _oitem, void *_rect, void *p2 )
/***************************************************************/
{
    /* return the location of the atom */
    OITEM   *oitem = _oitem;
    RECT    *rect = _rect;
    p2 = p2;          /* ref'd to avoid warning */

    *rect = oitem->rect;
    return( TRUE );
}

static BOOL OItemResize( OBJPTR _oitem, void *_rect, void *user_action )
/**********************************************************************/
{
    /*  Resize the OITEM */
    OITEM   *oitem = _oitem;
    RECT    *rect = _rect;
    RECT    temp;

    temp = oitem->rect;
    if( *(BOOL *)user_action ) {
        /* do full operation if action instigated by the user */
        RemoveObject( oitem->parent, oitem->handle );
        oitem->rect = *rect;
        if( !AddObject( oitem->parent, oitem->handle ) ) {
            /*  The Resizing failed, so reset to the old size and replace
             *  in the parent.
             */
            oitem->rect = temp;
            AddObject( oitem->parent, oitem->handle );
            return( FALSE );
        }
    } else {
        oitem->rect = *rect;
    }
    MarkInvalid( &temp );
    MarkInvalid( &oitem->rect );
    return( TRUE );
}

static void OItemSetNewParent( OITEM *oitem, OBJPTR p )
/*****************************************************/
{
    /* set the oitem parent pointer to the new parent */
    oitem->parent = p;
}

static BOOL OItemDestroy( OBJPTR _oitem, void *user_action, void *p2 )
/********************************************************************/
{
    /* destroy the OITEM */
    OITEM   *oitem = _oitem;
    p2 = p2;          /* ref'd to avoid warning */

    if( *(BOOL *)user_action ) {
        RemoveObject( oitem->parent, oitem->handle );
    }
    EdFree( oitem );
    return( TRUE );
}

static BOOL OItemCutObject( OBJPTR _oitem, void *_newitem, void *p2 )
/*******************************************************************/
{
    OITEM   *oitem = _oitem;
    OITEM   **newitem = _newitem;
    p2 = p2;
    RemoveObject( oitem->parent, oitem->handle );
    Notify( oitem->handle, NEW_PARENT, NULL );
    *newitem = oitem->handle;
    return( TRUE );
}

static BOOL OItemCopyObject( OBJPTR _oitem, void *_newitem, OBJPTR handle )
/*************************************************************************/
{
    OITEM   *oitem = _oitem;
    OITEM   **newitem = _newitem;
    OITEM   *no;

    if( newitem != NULL ) {
        no = EdAlloc( sizeof( OITEM ) );
        *newitem = no;
        no->invoke = oitem->invoke;
        if( handle != NULL ) {
            no->handle = handle;
        } else {
            no->handle = no;
        }
        no->priority = oitem->priority;
        CopyRect( &no->rect, &oitem->rect );
        return( TRUE );
    }
    return( FALSE );
}

static BOOL OItemPasteObject( OBJPTR _oitem, OBJPTR parent, void *_pt )
/*********************************************************************/
{
    OITEM   *oitem = _oitem;
    POINT   *pt = _pt;
    POINT   offset;

    offset.x = pt->x - oitem->rect.left;
    offset.y = pt->y - oitem->rect.top;
    OffsetRect( &oitem->rect, offset.x, offset.y );
    return( AddObject( parent, oitem->handle ) );
}

static BOOL OItemGetObjectParent( OBJPTR _oitem, void *_parent, void *p2 )
/************************************************************************/
{
    OITEM   *oitem = _oitem;
    OBJPTR  *parent = _parent;
    p2 = p2;
    if( parent != NULL ) {
        *parent = oitem->parent;
    }
    return( TRUE );
}

static BOOL OItemGetPriority( OBJPTR _oitem, void *_pri, void *p2 )
/*****************************************************************/
{
    OITEM   *oitem = _oitem;
    int     *pri = _pri;

    p2 = p2;          /* ref'd to avoid warning */
    if( pri != NULL ) {
        *pri = oitem->priority;
    }
    return( TRUE );
}

static BOOL PointInRect( LPRECT rect, POINT pt )
/**********************************************/
{
    /* Check if pt is inside rect. Window's function PtInRect doesn't consider */
    /* the bottom and right of the rect to be "in". */
#if 0
    /* This is a temporary change so the ACME beta will work */
    return( rect->top <= pt.y && pt.y <= rect->bottom &&
            rect->left <= pt.x && pt.x <= rect->right );
#else
    return( rect->top <= pt.y && pt.y < rect->bottom - 1 &&
            rect->left <= pt.x && pt.x < rect->right - 1 );
#endif
}

static BOOL OItemFindObjectPt( OBJPTR _oitem, void *_pt, void *_list )
/********************************************************************/
{
    OITEM   *oitem = _oitem;
    LPPOINT pt = _pt;
    LIST    **list = _list;

    if( PointInRect( &(oitem->rect), *pt ) ) {
        ListAddElt( list, oitem->handle );
        return( TRUE );
    } else {
        return( FALSE );
    }
}

extern void InitOItem( void )
/***************************/
{
    /*  set up the dispatch routine address for the OITEM since this may
     *  be called from the application
     */
    DispatchRtn = MakeProcInstance( (FARPROC) OItemDispatch, GetInst() );
}
