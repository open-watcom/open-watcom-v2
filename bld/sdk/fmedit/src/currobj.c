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
* Description:  Routines to handle the operations on a CURROBJ.
*
****************************************************************************/


#include <stdlib.h>
#include <windows.h>
#include "fmedit.def"
#include "object.def"
#include "currobj.h"
#include "memory.def"


#define pick_ACTS(o) \
    pick_ACTION_DESTROY(o,pick) \
    pick_ACTION_VALIDATE_ACTION(o,pick) \
    pick_ACTION_GET_OBJECT(o,pick) \
    pick_ACTION_ADD_OBJECT(o,pick) \
    pick_ACTION_DELETE_OBJECT(o,pick) \
    pick_ACTION_GET_PRIMARY(o,pick) \
    pick_ACTION_SHOW_SEL_BOXES(o,pick) \
    pick_ACTION_NOTIFY(o,pick)

/* forward references */
#define pick(e,n,c)    static bool CurrObj ## n ## c;
    pick_ACTS( CURROBJ )
#undef pick

static DISPATCH_ITEM CurrObjActions[] = {
    #define pick(e,n,c)    {e, (DISPATCH_RTN *)CurrObj ## n},
    pick_ACTS( CURROBJ )
    #undef pick
};

#define MAX_ACTIONS (sizeof( CurrObjActions ) / sizeof( DISPATCH_ITEM ))

static bool CALLBACK CurrObjDispatch( ACTION_ID id, OBJPTR cobj, void *p1, void *p2 )
/***********************************************************************************/
{
    /* dispatch the desired operation to the correct place */
    int i;

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( CurrObjActions[i].id == id ) {
            return( CurrObjActions[i].rtn( cobj, p1, p2 ) );
        }
    }
    return( false );
}

static bool CurrObjValidateAction( CURROBJ *cobj, ACTION_ID *idptr, void *p2 )
/****************************************************************************/
{
    /* check if the desired action is valid for and CURROBJ */
    int         i;
    NOTE_ID     *noteid;

    cobj = cobj;            /* ref'd to avoid warning */

    if( *idptr == NOTIFY ) {
        noteid = p2;
        return( noteid != NULL && (*noteid == CURR_OBJ_MOD_BEGIN || *noteid == CURR_OBJ_MOD_END) );
    }

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( CurrObjActions[i].id == *idptr ) {
            return( true );
        }
    }
    return( false );
}

OBJPTR CurrObjCreate( OBJPTR obj, RECT *loc, OBJPTR handle )
/**********************************************************/
{
    /* create an CURROBJ object */
    CURROBJ *new;

    handle = handle;     /* ref'd to avoid warning */
    obj = obj;           /* ref'd to avoid warning */
    loc = loc;           /* ref'd to avoid warning */
    new = EdAlloc( sizeof( CURROBJ ) );
    OBJ_DISPATCHER_SET( new, CurrObjDispatch );
    new->currobjlist = NULL;
    new->primaryobj = NULL;
    new->prevprimary = NULL;
    new->show_sel_boxes = true;
    return( (OBJPTR)new );
}

static bool CurrObjDestroy( CURROBJ *cobj, bool *p1, bool *p2 )
/*************************************************************/
{
    /* destroy the CURROBJ */
    p1 = p1;          /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */

    if( cobj->currobjlist != NULL ) {
        ListFree( cobj->currobjlist );
    }
    EdFree( cobj );
    return( true );
}

static void NotifyCurrObj( CURROBJ *cobj, OBJPTR obj, bool add_obj )
/******************************************************************/
{
    Notify( obj, CURRENT_OBJECT, &add_obj );
    if( add_obj ) {
        if( cobj->no_prim_notify ) {
            cobj->prim_notify_pending = true;
        } else {
            if( cobj->primaryobj == obj ) {
                Notify( obj, PRIMARY_OBJECT, NULL );
            }
        }
    }
}

static bool CurrObjAddObject( CURROBJ *cobj, OBJPTR obj, bool *reset )
/********************************************************************/
{
    OBJPTR  new;
    RECT    rect;
    OBJPTR  o;

    if( cobj->primaryobj != NULL ) {
        cobj->prevprimary = cobj->primaryobj;
    } else {
        cobj->prevprimary = NULL;
    }
    if( *reset ) {
        while( cobj->currobjlist != NULL ) {
            o = ListElement( cobj->currobjlist );
            ListRemoveElt( &cobj->currobjlist, o );
            DeleteCurrObject( o );
        }
        cobj->prevprimary = NULL;
    }
    Location( obj, &rect );
    new = Create( O_CURRITEM, (OBJPTR)cobj, &rect, obj );
    ListAddElt( &cobj->currobjlist, new );
    cobj->primaryobj = new;
    if( cobj->prevprimary != NULL ) {
        ObjMark( cobj->prevprimary );
    }
    Forward( new, SHOW_SEL_BOXES, &cobj->show_sel_boxes, NULL );
    NotifyCurrObj( cobj, new, true );
    return( true );
}

static void deleteItem( CURROBJ *cobj, OBJPTR obj )
{
    ListRemoveElt( &cobj->currobjlist, obj );
    if( cobj->primaryobj == obj ) {
        if( cobj->currobjlist != NULL ) {
            cobj->primaryobj = cobj->prevprimary;
            cobj->prevprimary = NULL;
        } else {
            cobj->primaryobj = NULL;
            cobj->prevprimary = NULL;
        }
        ObjMark( obj );
    }
    if( cobj->prevprimary == obj ) {
        cobj->prevprimary = NULL;
    }
    DeleteCurrItem( obj );
}

static bool CurrObjDeleteObject( CURROBJ *cobj, OBJPTR obj, bool *curritem )
/**************************************************************************/
{
    LIST        *clist;
    OBJPTR      o;

    if( *curritem ) {
        if( ListFindElt( cobj->currobjlist, obj ) != NULL ) {
            NotifyCurrObj( cobj, obj, false );
            deleteItem( cobj, obj );
        }
    } else {
        for( clist = cobj->currobjlist; clist != NULL; clist = ListNext( clist ) ) {
            o = ListElement( clist );
            if( GetObjptr( o ) == obj ) {
                NotifyCurrObj( cobj, obj, false );
                deleteItem( cobj, o );
                break;
            }
        }
    }
    return( true );
}

static bool CurrObjGetObject( CURROBJ *cobj, OBJPTR *newobj, OBJPTR prevobj )
/***************************************************************************/
{
    LIST    *newlist;

    if( cobj->currobjlist == NULL ) {
        *newobj = NULL;
    } else {
        if( prevobj == NULL ) {
            *newobj = ListElement( cobj->currobjlist );
        } else {
            newlist = ListFindElt( cobj->currobjlist, prevobj );
            if( newlist != NULL ) {
                newlist = ListNext( newlist );
            }
            if( newlist == NULL ) {
                *newobj = NULL;
            } else {
                *newobj = ListElement( newlist );
            }
        }
    }
    return( true );
}

static bool CurrObjGetPrimary( CURROBJ *cobj, OBJPTR *primary, bool *get )
/************************************************************************/
{
    if( primary != NULL && get != NULL ) {
        if( *get ) {
            /* check if the primary object is still in the list */
            if( ListFindElt( cobj->currobjlist, cobj->primaryobj ) != NULL ) {
                *primary = cobj->primaryobj;
            } else {
                *primary = NULL;
            }
        } else {
            if( cobj->primaryobj != *primary ) {
                cobj->prevprimary = cobj->primaryobj;
                cobj->primaryobj = *primary;
                if( cobj->prevprimary != NULL ) {
                    ObjMark( cobj->prevprimary );
                }
                ObjMark( *primary );
                NotifyCurrObj( cobj, *primary, true );
            }
        }
        return( true );
    } else {
        return( false );
    }
}

static bool CurrObjShowSelBoxes( CURROBJ *cobj, bool *show, void *p2 )
/********************************************************************/
{
    LIST    *item;
    OBJPTR  o;

    cobj->show_sel_boxes = *show;
    for( item = cobj->currobjlist; item != NULL; item = ListNext( item ) ) {
        o = ListElement( item );
        OBJ_DISPATCHER( o )( SHOW_SEL_BOXES, o, show, p2 );
    }
    return( true );
}

static bool CurrObjNotify( CURROBJ *cobj, NOTE_ID *noteid, void *p2 )
/*******************************************************************/
{
    p2 = p2;        // unused

    switch( *noteid ) {
    case CURR_OBJ_MOD_BEGIN:
        cobj->no_prim_notify = true;
        cobj->prim_notify_pending = false;
        return( true );
    case CURR_OBJ_MOD_END:
        cobj->no_prim_notify = false;
        if( cobj->prim_notify_pending ) {
            Notify( cobj->primaryobj, PRIMARY_OBJECT, NULL );
        }
        cobj->prim_notify_pending = false;
        return( true );
    default:
        return( false );
    }
}
