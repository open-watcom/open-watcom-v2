/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "wwindows.h"
#include <string.h>
#include "watcom.h"
#include "fmedit.def"
#include "state.def"
#include "object.h"
#include "fmdlgs.rh"
#include "scroll.def"


/* this constant must correspond to the index of the object menu item */

#define MAX_MENU 80
#define EDIT_MENU_FLAGS (MENU_DELETE | MENU_COPY | MENU_CUT | MENU_PASTE | MENU_ALIGN)


static INT_CREATE_RTN *InternalCreate[] = {
    #define pick(e,p) p,
    pick_OBJECTS()
    #undef pick
};

void FMEDITAPI AddFMEditMenus( HMENU hsubmenu, int bitmap )
/*********************************************************/
{
    /* insert the editor specific things into the edit menu */
    HBITMAP     hbitmap;
    HMENU       halignmenu;

    InsertMenu( hsubmenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL );
    if( bitmap & MENU_CUT ) {
        InsertMenu( hsubmenu, -1, MF_BYPOSITION | MF_STRING, IDM_CUTOBJECT, "Cu&t\tShift+Del" );
    }
    if( bitmap & MENU_COPY ) {
        InsertMenu( hsubmenu, -1, MF_BYPOSITION | MF_STRING, IDM_COPYOBJECT, "&Copy\tCtrl+Ins" );
    }
    if( bitmap & MENU_PASTE ) {
        InsertMenu( hsubmenu, -1, MF_BYPOSITION | MF_STRING, IDM_PASTEOBJECT, "&Paste\tShift+Ins" );
    }
    if( bitmap & MENU_DELETE ) {
        InsertMenu( hsubmenu, -1, MF_BYPOSITION | MF_STRING, IDM_DELETEOBJECT, "&Delete\tDel" );
    }
    LoadAccel( bitmap );
    if( bitmap & MENU_ALIGN ) {
        halignmenu = CreateMenu();
        if( (bitmap & EDIT_MENU_FLAGS) != MENU_ALIGN ) {
            AppendMenu( hsubmenu, MF_SEPARATOR, 0, NULL );
        }
        AppendMenu( hsubmenu, MF_POPUP, (UINT_PTR)halignmenu, "Group &Align" );
        hbitmap = LoadBitmap( GetInst(), "LEFT" );
        AppendMenu( halignmenu, MF_BITMAP, IDM_FMLEFT, (LPSTR) hbitmap );
        AppendMenu( halignmenu, MF_SEPARATOR, 0, NULL );
        hbitmap = LoadBitmap( GetInst(), "HCENTRE" );
        AppendMenu( halignmenu, MF_BITMAP, IDM_FMHCENTRE, (LPSTR) hbitmap );
        AppendMenu( halignmenu, MF_SEPARATOR, 0, NULL );
        hbitmap = LoadBitmap( GetInst(), "RIGHT" );
        AppendMenu( halignmenu, MF_BITMAP, IDM_FMRIGHT, (LPSTR) hbitmap );
        AppendMenu( halignmenu, MF_SEPARATOR, 0, NULL );
        hbitmap = LoadBitmap( GetInst(), "TOP" );
        AppendMenu( halignmenu, MF_BITMAP, IDM_FMTOP, (LPSTR) hbitmap );
        AppendMenu( halignmenu, MF_SEPARATOR, 0, NULL );
        hbitmap = LoadBitmap( GetInst(), "VCENTRE" );
        AppendMenu( halignmenu, MF_BITMAP, IDM_FMVCENTRE, (LPSTR) hbitmap );
        AppendMenu( halignmenu, MF_SEPARATOR, 0, NULL );
        hbitmap = LoadBitmap( GetInst(), "BOTTOM" );
        AppendMenu( halignmenu, MF_BITMAP, IDM_FMBOTTOM, (LPSTR) hbitmap );
    }
}


static void FixMenuName( char *name, int len )
/********************************************/
{
    /* remove the '&' character from the menu name */
    char        *mark;

    mark = strchr( name, '&' );
    if( mark == NULL ) {
        return;
    }
    memmove( mark, mark + 1, len - (mark - name) );
    name[len - 1] = '\0';
}


void InitEditMenu( HWND wnd, int bitmap )
/***************************************/
{
    HMENU       hsubmenu;
    int         nummenus;
    char        menuname[MAX_MENU + 1];
    int         i;
    bool        editfound;
    int         len;
    HMENU       hmenu;

    if( bitmap != MENU_NONE ) {
        hmenu = GetMenu( wnd );
        if( hmenu == NULL ) {
            return;
        }
        nummenus = GetMenuItemCount( hmenu );
        if( bitmap & EDIT_MENU_FLAGS ) {
            editfound = false;
            for( i = 0; i < nummenus; ++i ) {
                len = GetMenuString( hmenu, i, menuname, MAX_MENU, MF_BYPOSITION );
                FixMenuName( menuname, len );
                if( !editfound && stricmp( menuname, "EDIT" ) == 0 ) {
                    editfound = true;
                    AddFMEditMenus( GetSubMenu( hmenu, i ), bitmap );
                }
            }
            if( !editfound ) {
                hsubmenu = CreatePopupMenu();
                AddFMEditMenus( hsubmenu, bitmap );
                InsertMenu( hmenu, nummenus - 1, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hsubmenu, "&Edit" );
                ++nummenus;
            }
        }
        if( bitmap & MENU_SETUP ) {
            hsubmenu = LoadMenu( GetInst(), "SetupMenu" );
            InsertMenu( hmenu, nummenus - 1, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hsubmenu, "&Setup" );
            DrawMenuBar( GetAppWnd() );
        }
    }
}


void InitializeObjects( CREATE_TABLE objtable )
/*********************************************/
{
    /* initialize the application specific objects */
    SetObjects( objtable );
    CreateMainObject();
}


OBJPTR FMEDITAPI Create( OBJ_ID id, OBJPTR parent, RECT *rect, OBJPTR handle )
/****************************************************************************/
{
    /*  Create an object of the desired type at the passed location.  If
     *  there is a parent, remember it
     */
    CREATE_TABLE    appobjs;

    if( id < FIRST_USER_OBJ_ID ) {
        return( InternalCreate[id]( parent, rect, handle ) );
    } else {
        appobjs = GetObjects();
        return( appobjs[id - FIRST_USER_OBJ_ID]( parent, rect, handle ) );
    }
}


bool FMEDITAPI Move( OBJPTR obj, POINT *p, bool user_action )
/***********************************************************/
{
    /* perform a move operation on the object */
    return( OBJ_DISPATCHER( obj )( MOVE, obj, p, &user_action ) );
}


bool FMEDITAPI Location( OBJPTR obj, RECT *rect )
/***********************************************/
{
    /* return the location of the object */
    return( OBJ_DISPATCHER( obj )( LOCATE, obj, rect, NULL ) );
}


bool FMEDITAPI Resize( OBJPTR obj, RECT *r, bool user_action )
/************************************************************/
{
    /* perform a resize operation on the object */
    return( OBJ_DISPATCHER( obj )( RESIZE, obj, r, &user_action ) );
}


bool FMEDITAPI Register( OBJPTR obj )
/***********************************/
{
    /* perform a register operation on the object */
    return( OBJ_DISPATCHER( obj )( REGISTER, obj, NULL, NULL ) );
}


bool FMEDITAPI Recreate( OBJPTR obj, POINT *pt )
/**********************************************/
{
    /* perform a register operation on the object */
    return( OBJ_DISPATCHER( obj )( RECREATE, obj, pt, NULL ) );
}


bool FMEDITAPI Draw( OBJPTR obj, RECT *rect, HDC hdc )
/****************************************************/
{
    /* perform a draw operation on the object */
    return( OBJ_DISPATCHER( obj )( DRAW, obj, rect, &hdc ) );
}


bool FMEDITAPI Destroy( OBJPTR obj, bool first )
/**********************************************/
{
    /* destroy the object */
    ObjectDestroyed( obj );
    return( OBJ_DISPATCHER( obj )( DESTROY, obj, &first, NULL ) );
}

bool FMEDITAPI Notify( OBJPTR obj, NOTE_ID noteid, void *p2 )
/***********************************************************/
{
    /* notify an object about it's new parent */
    return( OBJ_DISPATCHER( obj )( NOTIFY, obj, &noteid, p2 ) );
}


bool FMEDITAPI Define( OBJPTR obj, POINT *pt, void *init )
/********************************************************/
{
    /* define the characeristics of the object */
    SetState( EDITING );
    SetBaseState( EDITING );
    return( OBJ_DISPATCHER( obj )( DEFINE, obj, pt, init ) );
}



bool FMEDITAPI FindObjList( OBJPTR obj, SUBOBJ_REQUEST *req, LIST **lst )
/***********************************************************************/
{
    /*  Find the list of objects within the parent object contained within the
     *  passed rect.
     */
    return( OBJ_DISPATCHER( obj )( FIND_SUBOBJECTS, obj, req, lst ) );
}


bool FMEDITAPI Forward( OBJPTR obj, ACTION_ID id, void *p1, void *p2 )
/********************************************************************/
{
    /* foward the specified action to the specified object */
    return( OBJ_DISPATCHER( obj )( id, obj, p1, p2 ) );
}


bool FMEDITAPI ValidateAction( OBJPTR obj, ACTION_ID id, void *p2 )
/*****************************************************************/
{
    /* find out if action id is valid for object obj */
    return( OBJ_DISPATCHER( obj )( VALIDATE_ACTION, obj, &id, p2 ) );
}

bool FMEDITAPI AddObject( OBJPTR obj, OBJPTR member )
/***************************************************/
{
    /*  Update the position of the passed member with in the given object. */
    bool        ret;

    ret = Forward( obj, ADD_SUBOBJECT, member, NULL );
    UpdateScroll();
    return( ret );
}


bool FMEDITAPI RemoveObject( OBJPTR obj, OBJPTR member )
/******************************************************/
{
    /*  Update the position of the passed member with in the given object. */
    bool        ret;

    ret = Forward( obj, REMOVE_SUBOBJECT, member, NULL );
    UpdateScroll();
    return( ret );
}

bool FMEDITAPI GetResizeInfo( OBJPTR obj, RESIZE_ID *info )
/*********************************************************/
{
    /* Get information about what types of resizing are valid for the object */
    return( OBJ_DISPATCHER( obj )( RESIZE_INFO, obj, info, NULL ) );
}

bool FMEDITAPI CutObject( OBJPTR obj, OBJPTR *copy )
/**************************************************/
{
    /* Ask the application to cut obj and supply a copy of obj at address copy */
    return( OBJ_DISPATCHER( obj )( CUT, obj, copy, NULL ) );
}

bool FMEDITAPI CopyObject( OBJPTR obj, OBJPTR *copy, OBJPTR handle )
/******************************************************************/
{
    /* Ask the application to supply a copy of obj at address copy */
    return( OBJ_DISPATCHER( obj )( COPY, obj, copy, handle ) );
}

bool FMEDITAPI PasteObject( OBJPTR obj, OBJPTR parent, POINT pt )
/***************************************************************/
{
    /* Paste object obj with parent parent */
    return( OBJ_DISPATCHER( obj )( PASTE, obj, parent, &pt ) );
}

OBJPTR FMEDITAPI FindObject( SUBOBJ_REQUEST *req )
/************************************************/
{
    /*  Find an object at the specified point.  This is done by asking for the
     *  component item in the highest level object.
     */
    OBJPTR      obj;
    LIST        *subobj;

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

bool FMEDITAPI FindObjectsPt( POINT pt, LIST **list )
/***************************************************/
{
    OBJPTR      obj;

    obj = GetMainObject();
    *list = NULL;
    return( OBJ_DISPATCHER( obj )( FIND_OBJECTS_PT, obj, &pt, list ) );
}

OBJPTR FMEDITAPI FindOneObjPt( POINT pt )
/***************************************/
{
    LIST        *list;

    if( FindObjectsPt( pt, &list ) ) {
        return( ListElement( list ) );
    } else {
        return( NULL );
    }
}

OBJPTR FMEDITAPI GetCurrObject( void )
/************************************/
{
    /* return pointer to the current object */
    OBJPTR      obj;
    OBJPTR      userobj;

    obj = GetPrimaryObject();
    userobj = NULL;
    if( obj != NULL ) {
        OBJ_DISPATCHER( obj )( GET_OBJPTR, obj, &userobj, NULL );
    }
    return( userobj );
}

LIST *FMEDITAPI GetCurrObjectList( void )
/***************************************/
{
    /* return pointer to the current object */
    OBJPTR      currobj;
    OBJPTR      userobj;
    LIST        *objlist;

    objlist = NULL;
    for( currobj = GetEditCurrObject(); currobj != NULL; currobj = GetNextEditCurrObject( currobj ) ) {
        userobj = NULL;
        OBJ_DISPATCHER( currobj )( GET_OBJPTR, currobj, &userobj, NULL );
        if( userobj != NULL ) {
            ListAddElt( &objlist, userobj );
        }
    }
    return( objlist );
}


OBJPTR GetEditCurrObject( void )
/******************************/
{
    /* return pointer to the current object */
    OBJPTR      obj;
    OBJPTR      currobj;

    currobj = GetCurrObj();
    OBJ_DISPATCHER( currobj )( GET_OBJECT, currobj, &obj, NULL );
    return( obj );
}


OBJPTR GetNextEditCurrObject( OBJPTR obj )
/****************************************/
{
    /* return pointer to the next current object in the list, after obj */
    OBJPTR      newobj;
    OBJPTR      currobj;

    currobj = GetCurrObj();
    OBJ_DISPATCHER( currobj )( GET_OBJECT, currobj, &newobj, obj );
    return( newobj );
}

void SetCurrObject( OBJPTR obj )
/******************************/
{
    /* make obj the only current object */
    bool        reset;
    OBJPTR      currobj;

    reset = true;
    currobj = GetCurrObj();
    OBJ_DISPATCHER( currobj )( ADD_OBJECT, currobj, obj, &reset );
}

void AddCurrObject( OBJPTR obj )
/******************************/
{
    /* add obj to the list of current objects */
    bool        reset;
    OBJPTR      currobj;
    OBJPTR      main_currobj;

    reset = false;
    currobj = GetCurrObj();
    OBJ_DISPATCHER( currobj )( ADD_OBJECT, currobj, obj, &reset );

    /* if the main object is in the list, remove it */
    /* so the main object can't be part of a multiple selection */
    main_currobj = GetCurrObjptr( GetMainObject() );
    if( main_currobj != NULL ) {
        DeleteCurrObject( main_currobj );
    }
}

void DeleteCurrObject( OBJPTR obj )
/*********************************/
{
    /* remove obj from the list of current objects */
    OBJPTR      currobj;
    bool        curritem;

    curritem = true;
    currobj = GetCurrObj();
    OBJ_DISPATCHER( currobj )( DELETE_OBJECT, currobj, obj, &curritem );
}

void DeleteCurrObjptr( OBJPTR obj )
/*********************************/
{
    /* Delete the current object associated with obj from the list of current
     * objects
     */
    OBJPTR      currobj;
    bool        curritem;

    curritem = false;
    currobj = GetCurrObj();
    if( currobj != NULL ) {
        OBJ_DISPATCHER( currobj )( DELETE_OBJECT, currobj, obj, &curritem );
    }
}

OBJPTR GetPrimaryObject( void )
/*****************************/
{
    /* return a pointer to the primary object */
    OBJPTR      primary;
    OBJPTR      currobj;
    bool        flag;

    currobj = GetCurrObj();
    flag = true;
    OBJ_DISPATCHER( currobj )( GET_PRIMARY, currobj, &primary, &flag );
    return( primary );
}

void SetPrimaryObject( OBJPTR obj )
/*********************************/
{
    /* return a pointer to the primary object */
    OBJPTR      currobj;
    bool        flag;

    currobj = GetCurrObj();

    flag = false;
    OBJ_DISPATCHER( currobj )( GET_PRIMARY, currobj, &obj, &flag );
}

void StartCurrObjMod( void )
/**************************/
{
    OBJPTR      currobj;

    currobj = GetCurrObj();
    Notify( currobj, CURR_OBJ_MOD_BEGIN, NULL );
}

void EndCurrObjMod( void )
/************************/
{
    OBJPTR      currobj;

    currobj = GetCurrObj();
    Notify( currobj, CURR_OBJ_MOD_END, NULL );
}

OBJPTR GetObjptr( OBJPTR obj )
/****************************/
{
    /* Get a pointer to the object associated with the current object obj */
    OBJPTR      newobj;

    newobj = NULL;
    OBJ_DISPATCHER( obj )( GET_OBJPTR, obj, &newobj, NULL );
    return( newobj );
}

OBJPTR GetCurrObjptr( OBJPTR obj )
/********************************/
{
    OBJPTR      currobj;

    for( currobj = GetEditCurrObject(); currobj != NULL; currobj = GetNextEditCurrObject( currobj ) ) {
        if( GetObjptr( currobj ) == obj ) {
            return( currobj );
        }
    }
    return( NULL );
}

bool DeleteCurrItem( OBJPTR obj )
/*******************************/
{
    /* Delete the current item obj.  That is, destroy the current object window
     * but don't destroy the object associated with that current object
     */
    return( OBJ_DISPATCHER( obj )( DELETE_OBJECT, obj, NULL, NULL ) );
}

static bool CurrObjExist( OBJPTR findobj )
/****************************************/
{
    OBJPTR      currobj;

    for( currobj = GetEditCurrObject(); currobj != NULL; currobj = GetNextEditCurrObject( currobj ) ) {
        if( currobj == findobj ) {
            return( true );
        }
    }
    return( false );
}

bool FMEDITAPI ExecuteCurrObject( ACTION_ID id, void *p1, void *p2 )
/******************************************************************/
{
    /* Perform action id on each of the objects in the current object list.
     * Always ensure that each object about to have the action performed
     * on it is still valid.  Remember that the action can be destroy,
     * which will make that object no longer in the current object list,
     * and can also affect other objects in that list (ie it's children).
     */
    OBJPTR      currobj;
    OBJPTR      nextobj;

    for( currobj = GetEditCurrObject(); currobj != NULL; currobj = nextobj ) {
        nextobj = GetNextEditCurrObject( currobj );
        if( nextobj == NULL && !CurrObjExist( currobj ) ) {
            currobj = GetEditCurrObject();
            nextobj = GetNextEditCurrObject( currobj );
        }
        if( GetEditCurrObject() != NULL ) {
            Forward( currobj, id, p1, p2 );
        }
    }
    return( true );
}

void ObjMark( OBJPTR obj )
/************************/
{
    /* invalidate the objects location for painting */
    RECT        rect;
    HRGN        little;
    HRGN        large;
    POINT       offset;

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

void FMEDITAPI ResetCurrObject( bool draw )
/*****************************************/
{
    /* reset the current object */
    OBJPTR      currobj;
    OBJPTR      nextobj;

    for( currobj = GetEditCurrObject(); currobj != NULL; currobj = nextobj ) {
        nextobj = GetNextEditCurrObject( currobj );
        if( draw ) {
            if( GetObjptr( currobj ) != GetMainObject() ) {
                ObjMark( currobj );
            }
        }
        DeleteCurrObject( currobj );
    }
    if( draw ) {
        UpdateWindow( GetAppWnd() );
    }
}

void MarkCurrObject( void )
/*************************/
{
    /* mark the current object */
    OBJPTR      currobj;

    currobj = GetPrimaryObject();
    if( currobj != NULL ) {
        ObjMark( currobj );
    }
}

void FMEDITAPI ObjectDestroyed( OBJPTR obj )
/******************************************/
{
    DeleteCurrObjptr( obj );
}

bool FMEDITAPI GetObjectParent( OBJPTR obj, OBJPTR *parent )
/**********************************************************/
{
    return( OBJ_DISPATCHER( obj )( GET_PARENT, obj, parent, NULL ) );
}

void UndoMove( OBJPTR obj )
/*************************/
{
    OBJ_DISPATCHER( obj )( UNDO_MOVE, obj, NULL, NULL );
}

void RemoveFromParent( OBJPTR obj )
/*********************************/
{
    OBJ_DISPATCHER( obj )( REMOVE_FROM_PARENT, obj, NULL, NULL );
}

bool FMEDITAPI GetPriority( OBJPTR obj, int *pri )
/************************************************/
{
    return( OBJ_DISPATCHER( obj )( GET_PRIORITY, obj, pri, NULL ) );
}

bool FMEDITAPI ResizeIncrements( OBJPTR obj, POINT *pt )
/******************************************************/
{
    if( ValidateAction( obj,  GET_RESIZE_INC, pt ) ) {
        return( OBJ_DISPATCHER( obj )( GET_RESIZE_INC, obj, pt, NULL ) );
    }
    return( false );
}


void FMEDITAPI MakeObjectCurrent( OBJPTR obj )
/********************************************/
{
    /* make the passes object current */
    ResetCurrObject( false );
    SetCurrObject( obj );
}


void FMEDITAPI AddCurrentObject( OBJPTR obj )
/*******************************************/
{
    /* make the passes object current */
    AddCurrObject( obj );
}

bool GetAnchor( OBJPTR obj, POINT *p )
/************************************/
{
    return( OBJ_DISPATCHER( obj )( GET_ANCHOR, obj, p, NULL ) );
}


bool RequestScrollRect( RECT *r )
/*******************************/
{
    OBJPTR      obj;

    obj = GetMainObject();
    return( OBJ_DISPATCHER( obj )( GET_SCROLL_RECT, obj, r, NULL ) );
}


bool IsMarkValid( OBJPTR obj )
/****************************/
{
    bool        isvalid;

    isvalid = true;
    if( ValidateAction( obj,  IS_MARK_VALID, NULL ) ) {
        OBJ_DISPATCHER( obj )( IS_MARK_VALID, obj, &isvalid, NULL );
    }
    return( isvalid );
}

void FMEDITAPI NewOffset( POINT point )
/*************************************/
{
    // Set the offset to 'point' and reset the scrolling stuff
    SetOffset( point );
    UpdateScroll();
    InvalidateRect( GetAppWnd(), NULL, TRUE );
}
