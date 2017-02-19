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
#include <stdlib.h>
#include <windows.h>
#include "fmedit.def"
#include "memory.def"
#include "oitem.h"
#include "oitem.def"
#include "state.def"

/* forward references */

static bool CALLBACK OItemDispatch( ACTION, OITEM *, void *, void * );

static void OItemSetNewParent( OITEM *, ANYOBJ * );

#define pick(e,n,c) bool OItem ## n ## c
static pick_ACT_REGISTER( OITEM );
static pick_ACT_LOCATE( OITEM );
static pick_ACT_MOVE( OITEM );
static pick_ACT_RESIZE( OITEM );
static pick_ACT_NOTIFY( OITEM );
static pick_ACT_DESTROY( OITEM );
static pick_ACT_VALIDATE_ACTION( OITEM );
static pick_ACT_CUT( OITEM );
static pick_ACT_COPY( OITEM );
static pick_ACT_PASTE( OITEM );
static pick_ACT_GET_PARENT( OITEM );
static pick_ACT_GET_PRIORITY( OITEM );
static pick_ACT_FIND_OBJECTS_PT( OITEM );
#undef pick

static DISPATCH_ITEM OItemActions[] = {
    #define pick(e,n,c) {e, (DISPATCH_RTN *)OItem ## n},
    pick_ACT_REGISTER( OITEM )
    pick_ACT_LOCATE( OITEM )
    pick_ACT_MOVE( OITEM )
    pick_ACT_RESIZE( OITEM )
    pick_ACT_NOTIFY( OITEM )
    pick_ACT_DESTROY( OITEM )
    pick_ACT_VALIDATE_ACTION( OITEM )
    pick_ACT_CUT( OITEM )
    pick_ACT_COPY( OITEM )
    pick_ACT_PASTE( OITEM )
    pick_ACT_GET_PARENT( OITEM )
    pick_ACT_GET_PRIORITY( OITEM )
    pick_ACT_FIND_OBJECTS_PT( OITEM )
    #undef pick
};

#define MAX_ACTIONS (sizeof( OItemActions ) / sizeof( DISPATCH_ITEM ))

static bool CALLBACK OItemDispatch( ACTION id, OITEM *obj, void *p1, void *p2 )
/*****************************************************************************/
{
    /* dispatch the desired operation to the correct place */
    int i;

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( OItemActions[i].id == id ) {
            return( (OItemActions[i].rtn)( obj, p1, p2) );
        }
    }
    return( false );
}

static bool OItemValidateAction( OITEM *obj, ACTION *idptr, void *p2 )
/********************************************************************/
{
    /* check if the desired action is valid for and OITEM */
    int     i;

    obj = obj;        /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */
    if( *idptr == NOTIFY ) {
        return( *((NOTE_ID *)p2) == NEW_PARENT );
    }
    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( OItemActions[i].id == *idptr ) {
            return( true );
        }
    }
    return( false );
}


static bool OItemMove( OITEM *oitem, POINT *offset, bool *user_action )
/*********************************************************************/
{
    /* Do the move operation  */
    RECT    temp;
    OBJPTR  newparent;
    POINT   pt;

    temp = oitem->rect;
    OffsetRect( (LPRECT) &oitem->rect, offset->x, offset->y );
    if( *user_action ) {
        pt.x = oitem->rect.left;
        pt.y = oitem->rect.top;
        newparent = FindOneObjPt( pt );
        if( newparent == NULL || !AddObject( newparent, oitem->handle ) ) {
            /*  Moving the OITEM in the parent failed, so reture FALSE
             *  so that the operation will be undone.
             */
            oitem->rect = temp;
            return( false );
        }
    }
    MarkInvalid( &temp );
    MarkInvalid( &oitem->rect );
    return( true );
}


static bool OItemNotify( OITEM *oitem, NOTE_ID *note, void *p2 )
/**************************************************************/
{
    /* process notify message for an OITEM */
    bool    ret;

    ret = false;
    switch( *note ) {
    case NEW_PARENT:
        OItemSetNewParent( oitem, p2 );
        ret = true;
        break;
    default:
        ret = false;
        break;
    }
    return( ret );
}


OBJPTR OItemCreate( OBJPTR parent, RECT *rect, OBJPTR handle )
/************************************************************/
{
    /* create an OITEM object */
    OITEM *new;

    new = EdAlloc( sizeof( OITEM ) );
    new->invoke = (FARPROC)&OItemDispatch;
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


static bool OItemRegister( OITEM *oitem, void *p1, void *p2 )
/***********************************************************/
{
    /* register the oitem by adding it to the structures */
    p1 = p1;          /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */

    if( AddObject( oitem->parent, oitem->handle ) ) {
        MarkInvalid( (LPRECT) &oitem->rect );
        return( true );
    }
    return( false );
}


static bool OItemLocation( OITEM *oitem, RECT *rect, void *p2 )
/*************************************************************/
{
    /* return the location of the atom */
    p2 = p2;          /* ref'd to avoid warning */

    *rect = oitem->rect;
    return( true );
}

static bool OItemResize( OITEM *oitem, RECT *rect, bool *user_action )
/********************************************************************/
{
    /*  Resize the OITEM */
    RECT    temp;

    temp = oitem->rect;
    if( *user_action ) {
        /* do full operation if action instigated by the user */
        RemoveObject( oitem->parent, oitem->handle );
        oitem->rect = *rect;
        if( !AddObject( oitem->parent, oitem->handle ) ) {
            /*  The Resizing failed, so reset to the old size and replace
             *  in the parent.
             */
            oitem->rect = temp;
            AddObject( oitem->parent, oitem->handle );
            return( false );
        }
    } else {
        oitem->rect = *rect;
    }
    MarkInvalid( &temp );
    MarkInvalid( &oitem->rect );
    return( true );
}

static void OItemSetNewParent( OITEM *oitem, OBJPTR p )
/*****************************************************/
{
    /* set the oitem parent pointer to the new parent */
    oitem->parent = p;
}

static bool OItemDestroy( OITEM *oitem, bool *user_action, bool *p2 )
/*******************************************************************/
{
    /* destroy the OITEM */
    p2 = p2;          /* ref'd to avoid warning */

    if( *user_action ) {
        RemoveObject( oitem->parent, oitem->handle );
    }
    EdFree( oitem );
    return( true );
}

static bool OItemCutObject( OITEM *oitem, OITEM **newitem, void *p2 )
/*******************************************************************/
{
    p2 = p2;
    RemoveObject( oitem->parent, oitem->handle );
    Notify( oitem->handle, NEW_PARENT, NULL );
    *newitem = oitem->handle;
    return( true );
}

static bool OItemCopyObject( OITEM *oitem, OITEM **newitem, OITEM *handle )
/*************************************************************************/
{
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
        return( true );
    }
    return( false );
}

static bool OItemPasteObject( OITEM *oitem, OBJPTR parent, POINT *pt )
/*********************************************************************/
{
    POINT   offset;

    offset.x = pt->x - oitem->rect.left;
    offset.y = pt->y - oitem->rect.top;
    OffsetRect( &oitem->rect, offset.x, offset.y );
    return( AddObject( parent, oitem->handle ) );
}

static bool OItemGetObjectParent( OITEM *oitem, ANYOBJ **parent, void *p2 )
/*************************************************************************/
{
    p2 = p2;
    if( parent != NULL ) {
        *parent = oitem->parent;
    }
    return( true );
}

static bool OItemGetPriority( OITEM *oitem, int *pri, void *p2 )
/**************************************************************/
{
    p2 = p2;          /* ref'd to avoid warning */
    if( pri != NULL ) {
        *pri = oitem->priority;
    }
    return( true );
}

static bool PointInRect( LPRECT rect, POINT pt )
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

static bool OItemFindObjectsPt( OITEM *oitem, LPPOINT pt, LIST **list )
/*********************************************************************/
{
    if( PointInRect( &(oitem->rect), *pt ) ) {
        ListAddElt( list, oitem->handle );
        return( true );
    } else {
        return( false );
    }
}

extern void InitOItem( void )
/***************************/
{
}
