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
* Description:  Module to contain various pieces of state information.
*
****************************************************************************/


#include <string.h>
#include <dos.h>
#include <windows.h>
#include "fmedit.def"
#include "object.def"
#include "cursor.def"
#include "memory.def"

#include "state.h"

static HANDLE       FMEditInst = NULL;
static STATE        *State = NULL;
static STATE        *StateList = NULL;
static STATE_HDL    StateID = 1;

/* forward declaration */
bool ShowError( void );

void SetInst( HANDLE inst )
{
    /* save the DLL instance */
    FMEditInst = inst;
}

void NewState( void )
{
    /* initialize state info */
    State = EdAlloc( sizeof( STATE ) );
    State->currstate = DORMANT;
    State->basestate = DORMANT;
    State->currobj = NULL;
    State->selecteatom = NULL;
    State->prevobject = NULL;
    State->mainobject = NULL;
    State->gridvinc = DEF_V_GRID;
    State->gridhinc = DEF_H_GRID;
    State->vresizegrid = DEF_V_GRID;
    State->hresizegrid = DEF_H_GRID;
    State->offset.x = 0;
    State->offset.y = 0;
    State->hAccel[0] = LoadAccelerators( GetInst(), "EscAccelTable" );
    State->hAccel[1] = NULL;
    State->hAccel[2] = NULL;
    State->hAccel[3] = NULL;
    State->hAccel[4] = NULL;
    State->showerror = true;
    State->error = NULL;
    State->mouseaction = NULL;
    State->show_eatoms = true;
    State->id = StateID;
    ++StateID;
    State->next = StateList;
    StateList = State;
}

void SetStateWnd( HWND wnd )
{
    RECT   rect;

    State->appwnd = wnd;
    if( wnd != NULL ) {
        GetClientRect( wnd, &rect );
    } else {
        memset( &rect, 0, sizeof( RECT ) );
    }
    State->scrollrect = rect;
}


void FreeState( void )
{
    STATE *s;
    STATE *last;

    last = NULL;
    for( s = StateList; s != NULL; s = s->next ) {
        if( s == State ) {
            break;
        }
        last = s;
    }
    if( last == NULL ) {
        StateList = s->next;
    } else {
        last->next = s->next;
    }
    EdFree( s );
    State = NULL;
}

HWND InheritState( HWND newwnd )
{
    /* replace the window of the current state with the passed window */
    HWND old;

    old = State->appwnd;
    State->appwnd = newwnd;
    return( old );
}

bool FMEDITAPI InitState( HWND wnd )
{
    /* initialize the state from the window */
    STATE *s;

    for( s = StateList; s != NULL; s = s->next ) {
        if( s->appwnd == wnd ) {
            break;
        }
    }
    State = s;
    return( State != NULL );
}

void SetState( STATE_ID state )
{
    /* set to the specified state */
    State->currstate = state;
    SetStateCursor( state );
}

STATE_ID GetState( void )
{
    /* return the current state */
    return( State->currstate );
}

void SetBaseState( STATE_ID st )
{
    /* set the state that the system goes to when nothing is actively happening */
    State->basestate = st;
}

void SetDefState( void )
{
    /* reset to base state */
    State->currstate = State->basestate;
    SetStateCursor( State->basestate );
}

void SetSize( RESIZE_ID id )
{
    /* set the sizing state based on the passed sizing operation */
    State->sizeinfo |= id;
}

void ResetSize( void )
{
    /* reset the sizing state info */
    State->sizeinfo = R_NONE;
}

RESIZE_ID GetSizing( void )
{
    /* return the sizing info */
    return( State->sizeinfo );
}

bool Sizing( RESIZE_ID op )
{
    /* check to see if a sizing operation is valid */
    return( (State->sizeinfo & op) != R_NONE );
}

void SetPrevMouse( POINT pt )
{
    /* save the mouse position */
    State->prevmouse = pt;
}

POINT GetPrevMouse( void )
{
    /* Return the last significant the mouse position. */
    return( State->prevmouse );
}

HANDLE GetAppWnd( void )
{
    /* save the application window handle */
    return( State->appwnd );
}

HANDLE GetInst( void )
{
    /* save the instance handle */
    return( FMEditInst );
}

void CreateMainObject( void )
{
    /* create the main object */
    State->mainobject = Create( FIRST_USER_OBJ_ID, NULL, NULL, NULL );
}

void DestroyMainObject( void )
{
    /* destroy the main object */
    OBJPTR temp;

    temp = State->mainobject;
    Destroy( temp, false );
    State->mainobject = NULL;
}

void CreateCurrObject( void )
{
    /* Create the current object */
    State->currobj = Create( O_CURROBJ, NULL, NULL, NULL );
}

void DestroyCurrObject( void )
{
    /* Destroy the current object */
    Destroy( State->currobj, false );
}

OBJPTR FMEDITAPI GetMainObject( void )
{
    /* create the main object */
    return( State->mainobject );
}

void FMEDITAPI SetBaseObjType( OBJ_ID id )
{
    /* save the type of object to create */
    State->objtype = id;
}

OBJ_ID FMEDITAPI GetBaseObjType( void )
{
    /* return the type of object to build */
    return( State->objtype );
}

int FMEDITAPI GetVerticalInc( void )
{
    /* return the vertical grid increment value */
    return( State->gridvinc );
}

void FMEDITAPI SetVerticalInc( unsigned inc )
{
    /* set the vertical grid increment value */
    State->gridvinc = inc;
}

int FMEDITAPI GetHorizontalInc( void )
{
    /* return the vertical grid increment value */
    return( State->gridhinc );
}

void FMEDITAPI SetHorizontalInc( unsigned inc )
{
    /* set the vertical grid increment value */
    State->gridhinc = inc;
}

void SetObjects( void * objs )
{
    /* save a pointer to the object table */
    State->objects = objs;
}

void *GetObjects( void )
{
    /* get the object table pointer */
    return( State->objects );
}

void SaveObject( void )
{
    /* remember the previous object */
    OBJPTR  currobj;

    currobj = GetEditCurrObject();
    if( currobj != NULL ) {
        State->prevobject = GetObjptr( currobj );
    }
}

void RestorePrevObject( void )
{
    /* reset the currobject from the previous object */
    AddCurrObject( State->prevobject );
}

OBJPTR GetCurrObj( void )
{
    /* get the current object */
    if( State == NULL ) {
        return( NULL );
    } else {
        return( State->currobj );
    }
}

void FMEDITAPI GetOffset( POINT *point )
{
    /* return the offset point */
    *point = State->offset;
}

void SetOffset( POINT point )
{
    /* set the offset point */
    State->offset = point;
}

RECT GetScrollRect( void )
{
    /* return the scroll rect */
    return( State->scrollrect );
}

void SetScrollRect( RECT rect )
{
    /* set the scroll rect */
    State->scrollrect = rect;
}

void SetScrollConfig( SCR_CONFIG flag )
{
    /* Set the scroll configuration */
    State->scrollconfig = flag;
}

SCR_CONFIG GetScrollConfig( void )
{
    /* Get the scroll configuration */
    return( State->scrollconfig );
}

OBJPTR GetSelectEatom( void )
{
    /* return the banded select eatom */
    return( State->selecteatom );
}

void SetSelectEatom( OBJPTR eatom )
{
    /* set the banded select eatom */
    State->selecteatom = eatom;
}

void SetShowEatoms( bool show )
{
    State->show_eatoms = show;
}

bool GetShowEatoms( void )
{
    return( State->show_eatoms );
}


int FMEDITAPI FMTranslateAccelerator( HWND wnd, LPMSG message )
{
    bool    ret;
    int     i;

    if( !InitState( wnd ) ) {
        return( false );
    }
    if( State->currstate == EDITING ) {
        return( false );
    }
    ret = false;
    for( i = 0; i < ACCELS && !ret; i++ ) {
        if( State->hAccel[i] != NULL ) {
            ret |= ( TranslateAccelerator( wnd, State->hAccel[i], message ) != 0 );
        }
    }
    return( ret );
}

void LoadAccel( int bitmap )
{
    if( bitmap & MENU_DELETE ) {
        State->hAccel[1] = LoadAccelerators( GetInst(), "DeleteAccelTable" );
    }
    if( bitmap & MENU_COPY ) {
        State->hAccel[2] = LoadAccelerators( GetInst(), "CopyAccelTable" );
    }
    if( bitmap & MENU_PASTE ) {
        State->hAccel[3] = LoadAccelerators( GetInst(), "PasteAccelTable" );
    }
    if( bitmap & MENU_CUT ) {
        State->hAccel[4] = LoadAccelerators( GetInst(), "CutAccelTable" );
    }
}

bool GetShift( void )
{
    return( (State->keystate & MK_SHIFT) != 0 );
}

bool GetControl( void )
{
    return( (State->keystate & MK_CONTROL) != 0 );
}

void SetKeyState( WORD keystate )
{
    State->keystate = keystate;
}

void FMEDITAPI DisplayError( char * msg )
{
    /* report an error message */
    if( msg != NULL ) {
        if( ShowError() ) {
            MessageBox( GetAppWnd(), (LPSTR) msg, NULL,
                        MB_ICONEXCLAMATION | MB_OK );
        } else {
            State->error = EdAlloc( strlen( msg ) + 1 );
            strcpy( State->error, msg );
        }
    }
}

void ReportPending( void )
{
    if( ShowError() && State->error != NULL ) {
        MessageBox( GetAppWnd(), (LPSTR) State->error, NULL,
                    MB_ICONEXCLAMATION | MB_OK );
        EdFree( State->error );
        State->error = NULL;
    }
}

void FMEDITAPI ClearError( void )
{
    if( State->error != NULL ) {
        EdFree( State->error );
        State->error = NULL;
    }
}


bool ShowError( void )
{
    return( State->showerror );
}

void FMEDITAPI SetShowError( bool show )
{
    State->showerror = show;
}

bool FMEDITAPI IsEditting( HWND wnd )
{
    InitState( wnd );
    return( State->currstate == EDITING );
}

void FMEDITAPI SetMouseRtn( HWND wnd, MOUSEACTION *rtn )
{
    InitState( wnd );
    State->mouseaction = rtn;
}


void MouseAction( RECT *r )
{
    RECT safe;

    if( State->mouseaction != NULL ) {
        safe = *r;
        State->mouseaction( State->appwnd, &safe );
    }
}


void SetResizeGrid( int horz, int vert )
{
    State->hresizegrid = horz;
    State->vresizegrid = vert;
}

int GetResizeHInc( void )
{
    return( State->hresizegrid );
}


int GetResizeVInc( void )
{
    return( State->vresizegrid );
}

bool FMEDITAPI InitStateFormID( STATE_HDL st )
{
    STATE *s;

    for( s = StateList; s != NULL; s = s->next ) {
        if( s->id == st ) {
            break;
        }
    }
    State = s;
    return( State != NULL );
}

STATE_HDL FMEDITAPI GetCurrFormID( void )
{
    if( State != NULL ) {
        return( State->id );
    }
    return( 0 );
}

void FMEDITAPI HideSelectBoxes( void )
{
    OBJPTR      currobj;
    bool        show;

    currobj = GetCurrObj();
    show = false;
    OBJ_DISPATCHER( currobj )( SHOW_SEL_BOXES, currobj, &show, NULL );
}

void FMEDITAPI ShowSelectBoxes( void )
{
    OBJPTR      currobj;
    bool        show;

    currobj = GetCurrObj();
    show = true;
    OBJ_DISPATCHER( currobj )( SHOW_SEL_BOXES, currobj, &show, NULL );
}
