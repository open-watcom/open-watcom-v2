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

/* forward references */

static BOOL CALLBACK CurrObjDispatch( ACTION, CURROBJ *, void *, void * );
static BOOL CurrObjDestroy( OBJPTR, void *, void * );
static BOOL CurrObjValidateAction( OBJPTR, void *, void * );
static BOOL CurrObjGetObject( OBJPTR, void *, void * );
static BOOL CurrObjAddObject( OBJPTR, void *, void * );
static BOOL CurrObjDeleteObject( OBJPTR, void *, void * );
static BOOL CurrObjGetPrimary( OBJPTR, void *, void * );
static BOOL CurrObjShowSelBoxes( OBJPTR, void *, void * );
static BOOL CurrObjNotify( OBJPTR, void *, void * );

static DISPATCH_ITEM CurrObjActions[] = {
    { DESTROY,          CurrObjDestroy          },
    { VALIDATE_ACTION,  CurrObjValidateAction   },
    { GET_OBJECT,       CurrObjGetObject        },
    { ADD_OBJECT,       CurrObjAddObject        },
    { DELETE_OBJECT,    CurrObjDeleteObject     },
    { GET_PRIMARY,      CurrObjGetPrimary       },
    { SHOW_SEL_BOXES,   CurrObjShowSelBoxes     },
    { NOTIFY,           CurrObjNotify           }
};

#define MAX_ACTIONS (sizeof( CurrObjActions ) / sizeof( DISPATCH_ITEM ))

static BOOL CALLBACK CurrObjDispatch( ACTION id, CURROBJ *c, void *p1, void *p2 )
/*******************************************************************************/
{
    /* dispatch the desired operation to the correct place */
    int i;

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( CurrObjActions[i].id == id ) {
            return( (CurrObjActions[i].rtn)( c, p1, p2 ) );
        }
    }
    return( FALSE );
}

static BOOL CurrObjValidateAction( OBJPTR _c, void *_idptr, void *p2 )
/********************************************************************/
{
    /* check if the desired action is valid for and CURROBJ */
    CURROBJ     *c = _c;
    ACTION      *idptr = _idptr;
    int         i;
    NOTE_ID     *note;

    c = c;            /* ref'd to avoid warning */

    if( *idptr == NOTIFY ) {
        note = p2;
        return( note != NULL &&
                (*note == CURR_OBJ_MOD_BEGIN || *note == CURR_OBJ_MOD_END) );
    }

    for( i = 0; i < MAX_ACTIONS; i++ ) {
        if( CurrObjActions[i].id == *idptr ) {
            return( TRUE );
        }
    }
    return( FALSE );
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
    new->invoke = (FARPROC)&CurrObjDispatch;
    new->currobjlist = NULL;
    new->primaryobj = NULL;
    new->prevprimary = NULL;
    new->show_sel_boxes = TRUE;
    return( new );
}

static BOOL CurrObjDestroy( OBJPTR _c, void *p1, void *p2 )
/*********************************************************/
{
    /* destroy the CURROBJ */
    CURROBJ *c = _c;
    p1 = p1;          /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */

    if( c->currobjlist != NULL ) {
        ListFree( c->currobjlist );
    }
    EdFree( c );
    return( TRUE );
}

static void NotifyCurrObj( CURROBJ *c, OBJPTR obj, BOOL add_obj )
/***************************************************************/
{
    Notify( obj, CURRENT_OBJECT, &add_obj );
    if( add_obj ) {
        if( c->no_prim_notify ) {
            c->prim_notify_pending = TRUE;
        } else {
            if( c->primaryobj == obj ) {
                Notify( obj, PRIMARY_OBJECT, NULL );
            }
        }
    }
}

static BOOL CurrObjAddObject( OBJPTR _c, OBJPTR obj, void *reset )
/****************************************************************/
{
    CURROBJ *c = _c;
    OBJPTR  new;
    RECT    rect;
    OBJPTR  currobj;

    if( c->primaryobj != NULL ) {
        c->prevprimary = c->primaryobj;
    } else {
        c->prevprimary = NULL;
    }
    if( *(BOOL *)reset ) {
        while( c->currobjlist != NULL ) {
            currobj = ListElement( c->currobjlist );
            DeleteCurrObject( currobj );
            ListRemoveElt( &c->currobjlist, currobj );
        }
        c->prevprimary = NULL;
    }
    Location( obj, &rect );
    new = Create( O_CURRITEM, c, &rect, obj );
    ListAddElt( &c->currobjlist, new );
    c->primaryobj = new;
    if( c->prevprimary != NULL ) {
        ObjMark( c->prevprimary );
    }
    Forward( new, SHOW_SEL_BOXES, &c->show_sel_boxes, NULL );
    NotifyCurrObj( c, new, TRUE );
    return( TRUE );
}

static BOOL CurrObjDeleteObject( OBJPTR _c, OBJPTR obj, void *curritem )
/**********************************************************************/
{
    CURROBJ     *c = _c;
    LIST        *clist;
    CURROBJPTR  currobj;

    if( *(BOOL *)curritem ) {
        if( ListFindElt( c->currobjlist, obj ) != NULL ) {
            NotifyCurrObj( c, obj, FALSE );
            ListRemoveElt( &c->currobjlist, obj );
            if( c->primaryobj == obj ) {
                if( c->currobjlist != NULL ) {
                    c->primaryobj = c->prevprimary;
                    c->prevprimary = NULL;
                } else {
                    c->primaryobj = NULL;
                    c->prevprimary = NULL;
                }
                ObjMark( obj );
            }
            if( c->prevprimary == obj ) {
                c->prevprimary = NULL;
            }
            DeleteCurrItem( obj );
        }
    } else {
        clist = c->currobjlist;
        while( clist != NULL ) {
            currobj = ListElement( clist );
            clist = ListNext( clist );
            if( GetObjptr( currobj ) == obj ) {
                clist = NULL;
                NotifyCurrObj( c, obj, FALSE );
                ListRemoveElt( &c->currobjlist, currobj );
                if( c->primaryobj == currobj ) {
                    if( c->currobjlist != NULL ) {
                        c->primaryobj = c->prevprimary;
                        c->prevprimary = NULL;
                    } else {
                        c->primaryobj = NULL;
                        c->prevprimary = NULL;
                    }
                    ObjMark( currobj );
                }
                if( c->prevprimary == currobj ) {
                    c->prevprimary = NULL;
                }
                DeleteCurrItem( currobj );
            }
        }
    }
    return( TRUE );
}

static BOOL CurrObjGetObject( OBJPTR _c, void *_newobj, OBJPTR prevobj )
/**********************************************************************/
{
    CURROBJ *c = _c;
    OBJPTR  *newobj = _newobj;
    LIST    *newlist;

    if( c->currobjlist == NULL ) {
        *newobj = NULL;
    } else {
        if( prevobj == NULL ) {
            *newobj = ListElement( c->currobjlist );
        } else {
            newlist = ListFindElt( c->currobjlist, prevobj );
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
    return( TRUE );
}

static BOOL CurrObjGetPrimary( OBJPTR _c, void *_primary, void *_get )
/********************************************************************/
{
    CURROBJ *c = _c;
    OBJPTR  *primary = _primary;
    BOOL    *get = _get;

    if( primary != NULL && get != NULL ) {
        if( *get ) {
            /* check if the primary object is still in the list */
            if( ListFindElt( c->currobjlist, c->primaryobj ) != NULL ) {
                *primary = c->primaryobj;
            } else {
                *primary = NULL;
            }
        } else {
            if( c->primaryobj != *primary ) {
                c->prevprimary = c->primaryobj;
                c->primaryobj = *primary;
                if( c->prevprimary != NULL ) {
                    ObjMark( c->prevprimary );
                }
                ObjMark( *primary );
                NotifyCurrObj( c, *primary, TRUE );
            }
        }
        return( TRUE );
    } else {
        return( FALSE );
    }
}

static BOOL CurrObjShowSelBoxes( OBJPTR _obj, void *_show, void *p2 )
/*******************************************************************/
{
    CURROBJ *obj = _obj;
    BOOL    *show = _show;
    LIST    *item;
    OBJECT  *cobj;

    obj->show_sel_boxes = *show;
    for( item = obj->currobjlist; item != NULL; item = ListNext( item ) ) {
        cobj = ListElement( item );
        (*cobj)( SHOW_SEL_BOXES, cobj, show, p2 );
    }
    return( TRUE );
}

static BOOL CurrObjNotify( OBJPTR _c, void *_id, void *p2 )
/*********************************************************/
{
    CURROBJ *c = _c;
    NOTE_ID *id = _id;
    p2 = p2;        // unused

    switch( *id ) {
    case CURR_OBJ_MOD_BEGIN:
        c->no_prim_notify = TRUE;
        c->prim_notify_pending = FALSE;
        return( TRUE );
    case CURR_OBJ_MOD_END:
        c->no_prim_notify = FALSE;
        if( c->prim_notify_pending ) {
            Notify( c->primaryobj, PRIMARY_OBJECT, NULL );
        }
        c->prim_notify_pending = FALSE;
        return( TRUE );
    default:
        return( FALSE );
    }
}
