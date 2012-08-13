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
#include "global.h"
#include "memory.def"

#include "state.h"

static HANDLE       FMEditInst = NULL;
static STATE        *State = NULL;
static STATE        *StateList = NULL;
static STATE_HDL    StateID = 1;

/* forward declaration */
extern BOOL ShowError( void );

extern void SetInst( HANDLE inst )
{
    /* save the DLL instance */
    FMEditInst = inst;
}

extern void NewState( void )
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
    State->showerror = TRUE;
    State->error = NULL;
    State->mouseaction = NULL;
    State->show_eatoms = TRUE;
    State->id = StateID;
    ++StateID;
    State->next = StateList;
    StateList = State;
}

extern void SetStateWnd( HWND wnd )
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


extern void FreeState( void )
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

extern HWND InheritState( HWND newwnd )
{
    /* replace the window of the current state with the passed window */
    HWND old;

    old = State->appwnd;
    State->appwnd = newwnd;
    return( old );
}

BOOL WINEXP InitState( HWND wnd )
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

extern void SetState( STATE_ID state )
{
    /* set to the specified state */
    State->currstate = state;
    SetStateCursor( state );
}

extern STATE_ID GetState( void )
{
    /* return the current state */
    return( State->currstate );
}

extern void SetBaseState( STATE_ID st )
{
    /* set the state that the system goes to when nothing is actively happening */
    State->basestate = st;
}

extern void SetDefState( void )
{
    /* reset to base state */
    State->currstate = State->basestate;
    SetStateCursor( State->basestate );
}

extern void SetSize( RESIZE_ID id )
{
    /* set the sizing state based on the passed sizing operation */
    State->sizeinfo |= id;
}

extern void ResetSize( void )
{
    /* reset the sizing state info */
    State->sizeinfo = R_NONE;
}

extern unsigned char GetSizing( void )
{
    /* return the sizing info */
    return( State->sizeinfo );
}

extern BOOL Sizing( char op )
{
    /* check to see if a sizing operation is valid */
    return( (State->sizeinfo & op) != R_NONE );
}

extern void SetPrevMouse( POINT pt )
{
    /* save the mouse position */
    State->prevmouse = pt;
}

extern POINT GetPrevMouse( void )
{
    /* Return the last significant the mouse position. */
    return( State->prevmouse );
}

extern HANDLE GetAppWnd( void )
{
    /* save the application window handle */
    return( State->appwnd );
}

extern HANDLE GetInst( void )
{
    /* save the instance handle */
    return( FMEditInst );
}

extern void CreateMainObject( void )
{
    /* create the main object */
    State->mainobject = Create( USER_OBJ, NULL, NULL, NULL );
}

extern void DestroyMainObject( void )
{
    /* destroy the main object */
    OBJPTR temp;

    temp = State->mainobject;
    Destroy( temp, FALSE );
    State->mainobject = NULL;
}

extern void CreateCurrObject( void )
{
    /* Create the current object */
    State->currobj = Create( O_CURROBJ, NULL, NULL, NULL );
}

extern void DestroyCurrObject( void )
{
    /* Destroy the current object */
    Destroy( State->currobj, FALSE );
}

OBJPTR WINEXP GetMainObject( void )
{
    /* create the main object */
    return( State->mainobject );
}

void WINEXP SetBaseObjType( OBJ_ID id )
{
    /* save the type of object to create */
    State->objtype = id;
}

OBJ_ID WINEXP GetBaseObjType( void )
{
    /* return the type of object to build */
    return( State->objtype );
}

unsigned WINEXP GetVerticalInc( void )
{
    /* return the vertical grid increment value */
    return( State->gridvinc );
}

void WINEXP SetVerticalInc( unsigned inc )
{
    /* set the vertical grid increment value */
    State->gridvinc = inc;
}

unsigned WINEXP GetHorizontalInc( void )
{
    /* return the vertical grid increment value */
    return( State->gridhinc );
}

void WINEXP SetHorizontalInc( unsigned inc )
{
    /* set the vertical grid increment value */
    State->gridhinc = inc;
}

extern void SetObjects( void * objs )
{
    /* save a pointer to the object table */
    State->objects = objs;
}

extern void *GetObjects( void )
{
    /* get the object table pointer */
    return( State->objects );
}

extern void SaveObject( void )
{
    /* remember the previous object */
    CURROBJPTR currobj;

    currobj = GetECurrObject();
    if( currobj != NULL ) {
        State->prevobject = GetObjptr( currobj );
    }
}

extern void RestorePrevObject( void )
{
    /* reset the currobject from the previous object */
    AddCurrObject( State->prevobject );
}

extern OBJPTR GetCurrObj( void )
{
    /* get the current object */
    if( State == NULL ) {
        return( NULL );
    } else {
        return( State->currobj );
    }
}

extern void WINEXP GetOffset( POINT *point )
{
    /* return the offset point */
    *point = State->offset;
}

extern void SetOffset( POINT point )
{
    /* set the offset point */
    State->offset = point;
}

extern RECT GetScrollRect( void )
{
    /* return the scroll rect */
    return( State->scrollrect );
}

extern void SetScrollRect( RECT rect )
{
    /* set the scroll rect */
    State->scrollrect = rect;
}

extern void SetScrollConfig( SCR_CONFIG flag )
{
    /* Set the scroll configuration */
    State->scrollconfig = flag;
}

extern SCR_CONFIG GetScrollConfig( void )
{
    /* Get the scroll configuration */
    return( State->scrollconfig );
}

extern OBJPTR GetSelectEatom( void )
{
    /* return the banded select eatom */
    return( State->selecteatom );
}

extern void SetSelectEatom( OBJPTR eatom )
{
    /* set the banded select eatom */
    State->selecteatom = eatom;
}

extern void SetShowEatoms( BOOL show )
{
    State->show_eatoms = show;
}

extern BOOL GetShowEatoms( void )
{
    return( State->show_eatoms );
}


int WINEXP FMTranslateAccelerator( HWND wnd, LPMSG message )
{
    int ret;
    int i;

    if( !InitState( wnd ) ) {
        return( FALSE );
    }

    ret = FALSE;

    if( State->currstate == EDITING ) {
        return( ret );
    }
    for( i = 0; i < ACCELS && ret == FALSE; i++ ) {
        if( State->hAccel[i] != NULL ) {
            ret |= TranslateAccelerator( wnd, State->hAccel[i], message );
        }
    }
    return( ret );
}

extern void LoadAccel( int bitmap )
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

extern BOOL GetShift( void )
{
    return( State->keystate & MK_SHIFT );
}

extern BOOL GetControl( void )
{
    return( State->keystate & MK_CONTROL );
}

extern void SetKeyState( WORD keystate )
{
    State->keystate = keystate;
}

void WINEXP DisplayError( char * msg )
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

extern void ReportPending( void )
{
    if( ShowError() && State->error != NULL ) {
        MessageBox( GetAppWnd(), (LPSTR) State->error, NULL,
                    MB_ICONEXCLAMATION | MB_OK );
        EdFree( State->error );
        State->error = NULL;
    }
}

void WINEXP ClearError( void )
{
    if( State->error != NULL ) {
        EdFree( State->error );
        State->error = NULL;
    }
}


extern BOOL ShowError( void )
{
    return( State->showerror );
}

void WINEXP SetShowError( BOOL show )
{
    State->showerror = show;
}

BOOL WINEXP IsEditting( HWND wnd )
{
    InitState( wnd );
    return( State->currstate == EDITING );
}

void WINEXP SetMouseRtn( HWND wnd, void (FM_EXPORT *rtn)( HWND, RECT * ) )
{
    InitState( wnd );
    State->mouseaction = rtn;
}


extern void MouseAction( RECT *r )
{
    RECT safe;

    if( State->mouseaction != NULL ) {
        safe = *r;
        State->mouseaction( State->appwnd, &safe );
    }
}


extern void SetResizeGrid( unsigned horz, unsigned vert )
{
    State->hresizegrid = horz;
    State->vresizegrid = vert;
}

extern unsigned GetResizeHInc( void )
{
    return( State->hresizegrid );
}


extern unsigned GetResizeVInc( void )
{
    return( State->vresizegrid );
}

BOOL WINEXP InitStateFormID( STATE_HDL st )
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

STATE_HDL WINEXP GetCurrFormID( void )
{
    if( State != NULL ) {
        return( State->id );
    }
    return( 0 );
}

void WINEXP HideSelectBoxes( void )
{
    OBJECT      *currobj;
    BOOL        show;

    currobj = GetCurrObj();
    show = FALSE;
    (*currobj)( SHOW_SEL_BOXES, currobj, &show, NULL );
}

void WINEXP ShowSelectBoxes( void )
{
    OBJECT      *currobj;
    BOOL        show;

    currobj = GetCurrObj();
    show = TRUE;
    (*currobj)( SHOW_SEL_BOXES, currobj, &show, NULL );
}
