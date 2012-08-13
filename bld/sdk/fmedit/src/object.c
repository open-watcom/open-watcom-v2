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
* Description:  Generic routines handling interactions with various objects.
*
****************************************************************************/


#include <string.h>
#include <windows.h>

#include "global.h"
#include "fmedit.def"
#include "state.def"
#include "object.h"
#include "fmdlgs.h"
#include "scroll.def"

/* this constant must correspond to the index of the object menu item */

#define MAX_MENU 80
#define EDIT_MENU_FLAGS (MENU_DELETE | MENU_COPY | MENU_CUT | MENU_PASTE | MENU_ALIGN)


static OBJPTR  (*InternalCreate[])(OBJPTR, RECT *, OBJPTR) = {
    NULL,                       /* O_NONE     */
    EAtomCreate,                /* O_EATOM    */
    OItemCreate,                /* O_ITEM     */
    CurrObjCreate,              /* O_CURROBJ  */
    CurrItemCreate,             /* O_CURRITEM */
    NULL,                       /* O_UNUSED_4 */
    NULL,                       /* O_UNUSED_5 */
    NULL                        /* O_UNUSED_6 */
};

void WINEXP AddFMEditMenus( HMENU submenu, int bitmap )
/*****************************************************/
{
    /* insert the editor specific things into the edit menu */
    HBITMAP     hbitmap;
    HMENU       alignmenu;

    InsertMenu( submenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL );
    if( bitmap & MENU_CUT ) {
        InsertMenu( submenu, -1, MF_BYPOSITION | MF_STRING, IDM_CUTOBJECT,
                    "Cu&t\tShift+Del" );
    }
    if( bitmap & MENU_COPY ) {
        InsertMenu( submenu, -1, MF_BYPOSITION | MF_STRING, IDM_COPYOBJECT,
                    "&Copy\tCtrl+Ins" );
    }
    if( bitmap & MENU_PASTE ) {
        InsertMenu( submenu, -1, MF_BYPOSITION | MF_STRING, IDM_PASTEOBJECT,
                    "&Paste\tShift+Ins" );
    }
    if( bitmap & MENU_DELETE ) {
        InsertMenu( submenu, -1, MF_BYPOSITION | MF_STRING, IDM_DELETEOBJECT,
                    "&Delete\tDel" );
    }
    LoadAccel( bitmap );
    if( bitmap & MENU_ALIGN ) {
        alignmenu = CreateMenu();
        if( bitmap & EDIT_MENU_FLAGS != MENU_ALIGN ) {
            AppendMenu( submenu, MF_SEPARATOR, 0, NULL );
        }
        AppendMenu( submenu, MF_POPUP, (UINT)alignmenu, "Group &Align" );
        hbitmap = LoadBitmap( GetInst(), "LEFT" );
        AppendMenu( alignmenu, MF_BITMAP, IDM_FMLEFT, (LPSTR) hbitmap );
        AppendMenu( alignmenu, MF_SEPARATOR, 0, NULL );
        hbitmap = LoadBitmap( GetInst(), "HCENTRE" );
        AppendMenu( alignmenu, MF_BITMAP, IDM_FMHCENTRE, (LPSTR) hbitmap );
        AppendMenu( alignmenu, MF_SEPARATOR, 0, NULL );
        hbitmap = LoadBitmap( GetInst(), "RIGHT" );
        AppendMenu( alignmenu, MF_BITMAP, IDM_FMRIGHT, (LPSTR) hbitmap );
        AppendMenu( alignmenu, MF_SEPARATOR, 0, NULL );
        hbitmap = LoadBitmap( GetInst(), "TOP" );
        AppendMenu( alignmenu, MF_BITMAP, IDM_FMTOP, (LPSTR) hbitmap );
        AppendMenu( alignmenu, MF_SEPARATOR, 0, NULL );
        hbitmap = LoadBitmap( GetInst(), "VCENTRE" );
        AppendMenu( alignmenu, MF_BITMAP, IDM_FMVCENTRE, (LPSTR) hbitmap );
        AppendMenu( alignmenu, MF_SEPARATOR, 0, NULL );
        hbitmap = LoadBitmap( GetInst(), "BOTTOM" );
        AppendMenu( alignmenu, MF_BITMAP, IDM_FMBOTTOM, (LPSTR) hbitmap );
    }
}


static void FixMenuName( char *name, int len )
/********************************************/
{
    /* remove the '&' character from the menu name */
    char *mark;

    mark = strchr( name, '&' );
    if( mark == NULL ) {
        return;
    }
    memmove( mark, mark + 1, len - (mark - name) );
    name[len - 1] = '\0';
}


extern void InitEditMenu( HWND wnd, int bitmap )
/**********************************************/
{
    HMENU       submenu;
    int         nummenus;
    char        menuname[MAX_MENU + 1];
    int         i;
    BOOL        editfound;
    int         len;
    HMENU       mainmenu;

    if( bitmap != MENU_NONE ) {
        mainmenu = GetMenu( wnd );
        if( mainmenu == NULL ) {
            return;
        }
        nummenus = GetMenuItemCount( mainmenu );
        if( bitmap & EDIT_MENU_FLAGS ) {
            editfound = FALSE;
            for( i = 0; i < nummenus; ++i ) {
                len = GetMenuString( mainmenu, i, menuname, MAX_MENU, MF_BYPOSITION );
                FixMenuName( menuname, len );
                if( !editfound && stricmp( menuname, "EDIT" ) == 0 ) {
                    editfound = TRUE;
                    AddFMEditMenus( GetSubMenu( mainmenu, i ), bitmap );
                }
            }
            if( !editfound ) {
                submenu = CreatePopupMenu();
                AddFMEditMenus( submenu, bitmap );
                InsertMenu( mainmenu, nummenus - 1, MF_BYPOSITION | MF_POPUP,
                            (UINT) submenu, "&Edit" );
                ++nummenus;
            }
        }
        if( bitmap & MENU_SETUP ) {
            submenu = LoadMenu( GetInst(), "SetupMenu" );
            InsertMenu( mainmenu, nummenus - 1, MF_BYPOSITION | MF_POPUP,
                        (UINT) submenu, "&Setup" );
            DrawMenuBar( GetAppWnd() );
        }
    }
}


extern void InitializeObjects( CREATE_TABLE objtable )
/****************************************************/
{
    /* initialize the application specific objects */
    SetObjects( objtable );
    CreateMainObject();
}


OBJPTR WINEXP Create( OBJ_ID id, OBJPTR parent, RECT *rect, OBJPTR handle )
/*************************************************************************/
{
    /*  Create an object of the desired type at the passed location.  If
     *  there is a parent, remember it
     */
    CREATE_TABLE    *appobjs;

    appobjs = GetObjects();
    if( id < USER_OBJ ) {
        return( InternalCreate[id]( parent, rect, handle ) );
    } else {
        return( (*appobjs)[id - USER_OBJ]( parent, rect, handle ) );
    }
}


BOOL WINEXP Move( OBJECT *obj, POINT *p, BOOL user_action )
/*********************************************************/
{
    /* perform a move operation on the object */
    return( (*obj)( MOVE, obj, p, &user_action ) );
}


BOOL WINEXP Location( OBJECT *obj, RECT *rect )
/*********************************************/
{
    /* return the location of the object */
    return( (*obj)( LOCATE, obj, rect, NULL ) );
}


BOOL WINEXP Resize( OBJECT *obj, RECT *r, BOOL user_action )
/**********************************************************/
{
    /* perform a resize operation on the object */
    return( (*obj)( RESIZE, obj, r, &user_action ) );
}


BOOL WINEXP Register( OBJECT *obj )
/*********************************/
{
    /* perform a register operation on the object */
    return( (*obj)( REGISTER, obj, NULL, NULL ) );
}


BOOL WINEXP Recreate( OBJECT *obj, POINT *pt )
/********************************************/
{
    /* perform a register operation on the object */
    return( (*obj)( RECREATE, obj, pt, NULL ) );
}


BOOL WINEXP Draw( OBJECT *obj, RECT *rect, HDC hdc )
/**************************************************/
{
    /* perform a draw operation on the object */
    return( (*obj)( DRAW, obj, rect, &hdc ) );
}


BOOL WINEXP Destroy( OBJECT *obj, BOOL first )
/********************************************/
{
    /* destroy the object */
    ObjectDestroyed( obj );
    return( (*obj)( DESTROY, obj, &first, NULL ) );
}

BOOL WINEXP Notify( OBJECT *obj, NOTE_ID n, void *p )
/***************************************************/
{
    /* notify an object about it's new parent */
    return( (*obj)( NOTIFY, obj, &n, p ) );
}


BOOL WINEXP Define( OBJECT *obj, POINT *pt, void *init )
/******************************************************/
{
    /* define the characeristics of the object */
    SetState( EDITING );
    SetBaseState( EDITING );
    return( (*obj)( DEFINE, obj, pt, init ) );
}



BOOL WINEXP FindObjList( OBJECT *obj, SUBOBJ_REQUEST *req, LIST **lst )
/*********************************************************************/
{
    /*  Find the list of objects within the parent object contained within the
     *  passed rect.
     */
    return( (*obj)( FIND_SUBOBJECTS, obj, req, lst ) );
}


BOOL WINEXP Forward( OBJECT *obj, ACTION id, void *p1, void *p2 )
/***************************************************************/
{
    /* foward the specified action to the specified object */
    return( (*obj)( id, obj, p1, p2 ) );
}


BOOL WINEXP ValidateAction( OBJECT *obj, ACTION id, void *p2 )
/************************************************************/
{
    /* find out if action id is valid for object obj */
    return( (*obj)( VALIDATE_ACTION, obj, &id, p2 ) );
}

extern BOOL WINEXP AddObject( OBJPTR obj, OBJPTR member )
/*******************************************************/
{
    /*  Update the position of the passed member with in the given object. */
    BOOL ret;

    ret = Forward( obj, ADD_SUBOBJECT, member, NULL );
    UpdateScroll();
    return( ret );
}


extern BOOL WINEXP RemoveObject( OBJPTR obj, OBJPTR member )
/**********************************************************/
{
    /*  Update the position of the passed member with in the given object. */
    BOOL ret;

    ret = Forward( obj, REMOVE_SUBOBJECT, member, NULL );
    UpdateScroll();
    return( ret );
}

extern BOOL WINEXP GetResizeInfo( OBJECT *obj, RESIZE_ID *info )
/**************************************************************/
{
    /* Get information about what types of resizing are valid for the object */
    return( (*obj)( RESIZE_INFO, obj, info, NULL ) );
}

extern BOOL WINEXP CutObject( OBJECT *obj, OBJPTR *copy )
/*******************************************************/
{
    /* Ask the application to cut obj and supply a copy of obj at address copy */
    return( (*obj)( CUT, obj, copy, NULL ) );
}

extern BOOL WINEXP CopyObject( OBJECT *obj, OBJPTR *copy, OBJPTR handle )
/***********************************************************************/
{
    /* Ask the application to supply a copy of obj at address copy */
    return( (*obj)( COPY, obj, copy, handle ) );
}

extern BOOL WINEXP PasteObject( OBJECT *obj, OBJPTR parent, POINT pt )
/********************************************************************/
{
    /* Paste object obj with parent parent */
    return( (*obj)( PASTE, obj, parent, &pt ) );
}

OBJPTR WINEXP FindObject( SUBOBJ_REQUEST *req )
/*********************************************/
{
    /*  Find an object at the specified point.  This is done by asking for the
     *  component item in the highest level object.
     */
    OBJPTR  obj;
    LIST    *subobj;

    obj = GetMainObject();
    for( ;; ) {
        subobj = NULL;
        if( !FindObjList( obj, req, &subobj ) ) {
            break;
        }
        /* There can only be  one object in the list (since rect is just a */
        /* point or panelid is unique) so get the object and free the list.*/
        obj = ListElement( subobj );
        ListFree( subobj );
    }
    return( obj );
}

BOOL WINEXP FindObjectsPt( POINT pt, LIST **list )
/************************************************/
{
    OBJECT *obj;

    obj = GetMainObject();
    *list = NULL;
    return( (*obj)( FIND_OBJECTS_PT, obj, &pt, list ) );
}

OBJPTR WINEXP FindOneObjPt( POINT pt )
/************************************/
{
    LIST *list;
    if( FindObjectsPt( pt, &list ) ) {
        return( ListElement( list ) );
    } else {
        return( NULL );
    }
}

OBJPTR WINEXP GetCurrObject( void )
/*********************************/
{
    /* return pointer to the current object */
    OBJECT  *obj;
    OBJPTR  userobj;

    obj = GetPrimaryObject();
    userobj = NULL;
    if( obj != NULL ) {
        (*obj)(GET_OBJPTR, obj, &userobj, NULL );
    }
    return( userobj );
}

LIST * WINEXP GetCurrObjectList( void )
/*************************************/
{
    /* return pointer to the current object */
    OBJECT  *currobj;
    OBJPTR  userobj;
    LIST    *objlist;

    objlist = NULL;
    for( currobj = GetECurrObject(); currobj != NULL;
         currobj = GetNextECurrObject( currobj ) ) {
        userobj = NULL;
        (*currobj)( GET_OBJPTR, currobj, &userobj, NULL );
        if( userobj != NULL ) {
            ListAddElt( &objlist, userobj );
        }
    }
    return( objlist );
}


extern CURROBJPTR GetECurrObject( void )
/**************************************/
{
    /* return pointer to the current object */
    OBJPTR  obj;
    OBJECT  *currobj;

    currobj = GetCurrObj();
    (*currobj)( GET_OBJECT, currobj, &obj, NULL );
    return( obj );
}


extern CURROBJPTR GetNextECurrObject( CURROBJPTR obj )
/****************************************************/
{
    /* return pointer to the next current object in the list, after obj */
    CURROBJPTR  newobj;
    OBJECT      *currobj;

    currobj = GetCurrObj();
    (*currobj)( GET_OBJECT, currobj, &newobj, obj );
    return( newobj );
}

extern void SetCurrObject( OBJPTR obj )
/*************************************/
{
    /* make obj the only current object */
    BOOL    reset;
    OBJECT  *currobj;

    reset = TRUE;
    currobj = GetCurrObj();
    (*currobj)( ADD_OBJECT, currobj, obj, &reset );
}

extern void AddCurrObject( OBJPTR obj )
/*************************************/
{
    /* add obj to the list of current objects */
    BOOL        reset;
    OBJECT      *currobj;
    CURROBJPTR  main_currobj;

    reset = FALSE;
    currobj = GetCurrObj();
    (*currobj)( ADD_OBJECT, currobj, obj, &reset );

    /* if the main object is in the list, remove it */
    /* so the main object can't be part of a multiple selection */
    main_currobj = GetCurrObjptr( GetMainObject() );
    if( main_currobj != NULL ) {
        DeleteCurrObject( main_currobj );
    }
}

extern void DeleteCurrObject( OBJPTR obj )
/****************************************/
{
    /* remove obj from the list of current objects */
    OBJECT  *currobj;
    BOOL    curritem;

    curritem = TRUE;
    currobj = GetCurrObj();
    (*currobj)( DELETE_OBJECT, currobj, obj, &curritem );
}

extern void DeleteCurrObjptr( OBJPTR obj )
/****************************************/
{
    /* Delete the current object associated with obj from the list of current
     * objects
     */
    OBJECT  *currobj;
    BOOL    curritem;

    curritem = FALSE;
    currobj = GetCurrObj();
    if( currobj != NULL ) {
        (*currobj)( DELETE_OBJECT, currobj, obj, &curritem );
    }
}

extern CURROBJPTR GetPrimaryObject( void )
/****************************************/
{
    /* return a pointer to the primary object */
    CURROBJPTR  primary;
    OBJECT      *currobj;
    BOOL        flag;

    currobj = GetCurrObj();
    flag = TRUE;
    (*currobj)( GET_PRIMARY, currobj, &primary, &flag );
    return( primary );
}

extern void SetPrimaryObject( CURROBJPTR obj )
/********************************************/
{
    /* return a pointer to the primary object */
    OBJECT  *currobj;
    BOOL    flag;

    currobj = GetCurrObj();

    flag = FALSE;
    (*currobj)( GET_PRIMARY, currobj, &obj, &flag );
}

extern void StartCurrObjMod( void )
/*********************************/
{
    OBJPTR      currobj;

    currobj = GetCurrObj();
    Notify( currobj, CURR_OBJ_MOD_BEGIN, NULL );
}

extern void EndCurrObjMod( void )
/*******************************/
{
    OBJPTR      currobj;

    currobj = GetCurrObj();
    Notify( currobj, CURR_OBJ_MOD_END, NULL );
}

extern OBJPTR GetObjptr( OBJECT *obj )
/************************************/
{
    /* Get a pointer to the object associated with the current object obj */
    OBJPTR newobj;

    newobj = NULL;
    (*obj)( GET_OBJPTR, obj, &newobj, NULL );
    return( newobj );
}

extern CURROBJPTR GetCurrObjptr( OBJPTR obj )
/*******************************************/
{
    CURROBJPTR currobj;

    for( currobj = GetECurrObject(); currobj != NULL;
         currobj = GetNextECurrObject( currobj ) ) {
        if( GetObjptr( currobj ) == obj ) {
            return( currobj );
        }
    }
    return( NULL );
}

extern BOOL DeleteCurrItem( OBJECT *obj )
/***************************************/
{
    /* Delete the current item obj.  That is, destroy the current object window
     * but don't destroy the object associated with that current object
     */
    return( (*obj)( DELETE_OBJECT, obj, NULL, NULL ) );
}

static BOOL CurrObjExist( CURROBJPTR findobj )
/********************************************/
{
    CURROBJPTR currobj;

    currobj = GetECurrObject();
    while( currobj != NULL ) {
        if( currobj == findobj ) {
            return( TRUE );
        }
        currobj = GetNextECurrObject( currobj );
    }
    return( FALSE );
}

BOOL WINEXP ExecuteCurrObject( ACTION id, void *p1, void *p2 )
/************************************************************/
{
    /* Perform action id on each of the objects in the current object list.
     * Always ensure that each object about to have the action performed
     * on it is still valid.  Remember that the action can be destroy,
     * which will make that object no longer in the current object list,
     * and can also affect other objects in that list (ie it's children).
     */
    CURROBJPTR currobj;
    CURROBJPTR nextobj;

    currobj = GetECurrObject();
    while( currobj != NULL ) {
        nextobj = GetNextECurrObject( currobj );
        if( nextobj == NULL && !CurrObjExist( currobj ) ) {
            currobj = GetECurrObject();
            nextobj = GetNextECurrObject( currobj );
        }
        if( GetECurrObject() != NULL ) {
            Forward( currobj, id, p1, p2 );
        }
        currobj = nextobj;
    }
    return( TRUE );
}

extern void ObjMark( CURROBJPTR obj )
/***********************************/
{
    /* invalidate the objects location for painting */
    RECT   rect;
    HRGN   little;
    HRGN   large;
    POINT  offset;

    if( !IsMarkValid( obj )) {
        return;
    }
    Location( obj, &rect );
    GetOffset( &offset );
    OffsetRect( &rect, -offset.x, -offset.y );
    little = CreateRectRgn( rect.left, rect.top, rect.right, rect.bottom );
    InflateRect( &rect, SQUAREWIDTH / 2, SQUAREWIDTH / 2 );
    large = CreateRectRgn( rect.left, rect.top, rect.right, rect.bottom );
    CombineRgn( large, large, little, RGN_DIFF );
    InvalidateRgn( GetAppWnd(), large, TRUE );
    DeleteObject( large );
    DeleteObject( little );
}

void WINEXP ResetCurrObject( BOOL draw )
/**************************************/
{
    /* reset the current object */
    CURROBJPTR currobj;
    CURROBJPTR nextobj;

    currobj = GetECurrObject();
    while( currobj != NULL ) {
        nextobj = GetNextECurrObject( currobj );
        if( draw ) {
            if( GetObjptr( currobj ) != GetMainObject() ) {
                ObjMark( currobj );
            }
        }
        DeleteCurrObject( currobj );
        currobj = nextobj;
    }
    if( draw ) {
        UpdateWindow( GetAppWnd() );
    }
}

extern void MarkCurrObject( void )
/********************************/
{
    /* mark the current object */
    CURROBJPTR currobj;

    currobj = GetPrimaryObject();
    if( currobj != NULL ) {
        ObjMark( currobj );
    }
}

void WINEXP ObjectDestroyed( OBJPTR obj )
/***************************************/
{
    DeleteCurrObjptr( obj );
}

BOOL WINEXP GetObjectParent( OBJECT *obj, OBJPTR *parent )
/********************************************************/
{
    return( (*obj)( GET_PARENT, obj, parent, NULL ) );
}

extern void UndoMove( OBJECT *obj )
/*********************************/
{
    (*obj)( UNDO_MOVE, obj, NULL, NULL );
}

extern void RemoveFromParent( OBJECT *obj )
/*****************************************/
{
    (*obj)( REMOVE_FROM_PARENT, obj, NULL, NULL );
}

BOOL WINEXP GetPriority( OBJECT *obj, int *pri )
/**********************************************/
{
    return( (*obj)( GET_PRIORITY, obj, pri, NULL ) );
}

BOOL WINEXP ResizeIncrements( OBJECT *obj, POINT *pt )
/****************************************************/
{
    if( ValidateAction( obj,  GET_RESIZE_INC, pt ) ) {
        return( (*obj)( GET_RESIZE_INC, obj, pt, NULL ) );
    }
    return( FALSE );
}


void WINEXP MakeObjectCurrent( OBJPTR obj )
/*****************************************/
{
    /* make the passes object current */
    ResetCurrObject( FALSE );
    SetCurrObject( obj );
}


void WINEXP AddCurrentObject( OBJPTR obj )
/****************************************/
{
    /* make the passes object current */
    AddCurrObject( obj );
}

extern BOOL GetAnchor( OBJECT *obj, POINT *p )
/********************************************/
{
    return( (*obj)( GET_ANCHOR, obj, p, NULL ) );
}


extern BOOL RequestScrollRect( RECT *r )
/**************************************/
{
    OBJECT  *obj;

    obj = GetMainObject();
    return( (*obj)( GET_SCROLL_RECT, obj, r, NULL ) );
}


extern BOOL IsMarkValid( OBJECT *obj )
/************************************/
{
    BOOL     isvalid;

    isvalid = TRUE;
    if( ValidateAction( obj,  IS_MARK_VALID, NULL ) ) {
        (*obj)( IS_MARK_VALID, obj, &isvalid, NULL );
    }
    return( isvalid );
}

extern void WINEXP NewOffset( POINT point )
/*****************************************/
{
    // Set the offset to 'point' and reset the scrolling stuff
    SetOffset( point );
    UpdateScroll();
    InvalidateRect( GetAppWnd(), NULL, TRUE );
}
