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


/* CURROBJ.C - routines to handle the operations on a CURROBJ */

#include <windows.h>

#include "fmedit.def"
#include "object.def"
#include "currobj.h"
#include "global.h"
#include "memory.def"

/* forward references */

static BOOL PASCAL CurrObjDispatch( ACTION, CURROBJ *, void *, void * );
static BOOL CurrObjDestroy( CURROBJ *, void *, void * );
static BOOL CurrObjValidateAction( CURROBJ *, ACTION *, void * );
static BOOL CurrObjGetObject( CURROBJ *, OBJPTR *, OBJPTR );
static BOOL CurrObjAddObject( CURROBJ *, OBJPTR, BOOL * );
static BOOL CurrObjDeleteObject( CURROBJ * , OBJPTR, BOOL * );
static BOOL CurrObjGetPrimary( CURROBJ *, OBJPTR *, BOOL * );
static BOOL CurrObjShowSelBoxes( CURROBJ * obj, BOOL * show, void * p2 );
static BOOL CurrObjNotify( CURROBJ * c, NOTE_ID * id, void * p2 );

static DISPATCH_ITEM CurrObjActions[] = {
    { DESTROY,          CurrObjDestroy          }
,   { VALIDATE_ACTION,  CurrObjValidateAction   }
,   { GET_OBJECT,       CurrObjGetObject        }
,   { ADD_OBJECT,       CurrObjAddObject        }
,   { DELETE_OBJECT,    CurrObjDeleteObject     }
,   { GET_PRIMARY,      CurrObjGetPrimary       }
,   { SHOW_SEL_BOXES,   CurrObjShowSelBoxes     }
,   { NOTIFY,           CurrObjNotify           }
};

#define MAX_ACTIONS (sizeof(CurrObjActions)/sizeof(DISPATCH_ITEM))

extern BOOL PASCAL CurrObjDispatch( ACTION id, CURROBJ * c,
                                        void * p1, void * p2 )
/************************************************************/

/* dispatch the desired operation to the correct place */

  {
    int i;

    for(i=0; i<MAX_ACTIONS; i++ ) {
        if( CurrObjActions[i].id == id ) {
            return((CurrObjActions[i].rtn)( c, p1, p2));
        }
    }
    return( FALSE );
  }

static BOOL CurrObjValidateAction( CURROBJ * c, ACTION * idptr, void * p2 )
/*************************************************************************/

/* check if the desired action is valid for and CURROBJ */

  {
    int         i;
    NOTE_ID *   note;

    c = c;            /* ref'd to avoid warning */

    if( *idptr == NOTIFY ) {
        note = p2;
        return( note != NULL &&
                (*note == CURR_OBJ_MOD_BEGIN || *note == CURR_OBJ_MOD_END) );
    }

    for(i=0; i<MAX_ACTIONS; i++ ) {
        if( CurrObjActions[i].id == *idptr ) {
            return( TRUE );
        }
    }
    return( FALSE );
  }

extern OBJPTR CurrObjCreate( OBJPTR * obj, RECT * loc, OBJPTR handle )
/********************************************************************/

/* create an CURROBJ object */

  {
    CURROBJ * new;

    handle = handle;     /* ref'd to avoid warning */
    obj = obj;           /* ref'd to avoid warning */
    loc = loc;           /* ref'd to avoid warning */
    new = EdAlloc( sizeof( CURROBJ ) );
    new->invoke = &CurrObjDispatch;
    new->currobjlist = NULL;
    new->primaryobj = NULL;
    new->prevprimary = NULL;
    new->show_sel_boxes = TRUE;
    return( new );
  }

static BOOL CurrObjDestroy( CURROBJ * c, void * p1, void * p2 )
/*************************************************************/

/* destroy the CURROBJ */

  {
    p1 = p1;          /* ref'd to avoid warning */
    p2 = p2;          /* ref'd to avoid warning */

    if( c->currobjlist != NULL ) {
        ListFree( c->currobjlist );
    }
    EdFree( c );
    return( TRUE );
  }

static void NotifyCurrObj( CURROBJ * c, OBJPTR obj, BOOL add_obj )
/****************************************************************/
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
} /* NotifyCurrObj */

static BOOL CurrObjAddObject( CURROBJ * c, OBJPTR obj, BOOL * reset )
/*******************************************************************/

  {
    OBJPTR *  new;
    RECT      rect;
    OBJPTR    currobj;

    if( c->primaryobj != NULL ) {
        c->prevprimary = c->primaryobj;
    } else {
        c->prevprimary = NULL;
    }
    if( *reset ) {
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

static BOOL CurrObjDeleteObject( CURROBJ * c, OBJPTR obj, BOOL * curritem )
/*************************************************************************/

  {
    LIST *     clist;
    CURROBJPTR currobj;

    if( *curritem ) {
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

static BOOL CurrObjGetObject( CURROBJ * c, OBJPTR * newobj, OBJPTR prevobj )
/**************************************************************************/

  {
    LIST * newlist;

    if( c->currobjlist == NULL ) {
        *newobj = NULL;
    } else {
        if( prevobj == NULL ) {
            *newobj = ListElement( c->currobjlist );
        } else {
            newlist = ListFindElt( c->currobjlist, prevobj );
            if( newlist != NULL ) {
                newlist= ListNext( newlist );
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

static BOOL CurrObjGetPrimary( CURROBJ * c, OBJPTR * primary, BOOL * get )
/***********************************************************************/

  {
    if( ( primary != NULL ) && ( get != NULL ) ) {
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

static BOOL CurrObjShowSelBoxes( CURROBJ * obj, BOOL * show, void * p2 )
/**********************************************************************/
{
    LIST *      item;
    OBJECT *    cobj;

    obj->show_sel_boxes = *show;
    for( item = obj->currobjlist; item != NULL; item = ListNext( item ) ) {
        cobj = ListElement( item );
        (*cobj)( SHOW_SEL_BOXES, cobj, show, p2 );
    }
    return( TRUE );
} /* CurrObjShowSelBoxes */

static BOOL CurrObjNotify( CURROBJ * c, NOTE_ID * id, void * p2 )
/***************************************************************/
{
    p2 = p2;        // unused

    switch( *id ) {
    case CURR_OBJ_MOD_BEGIN:
        c->no_prim_notify = TRUE;
        c->prim_notify_pending = FALSE;
        return( TRUE );
        break;
    case CURR_OBJ_MOD_END:
        c->no_prim_notify = FALSE;
        if( c->prim_notify_pending ) {
            Notify( c->primaryobj, PRIMARY_OBJECT, NULL );
        }
        c->prim_notify_pending = FALSE;
        return( TRUE );
        break;
    default:
        return( FALSE );
        break;
    }
} /* CurrObjNotify */

