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
* Description:  Routines to handle keyboard input.
*
****************************************************************************/


#include <windows.h>

#include "fmedit.def"
#include "object.def"
#include "state.def"
#include "currobj.def"
#include "mouse.def"

/* Forward References */
static BOOL IgnoreKbd( int keycode );
static BOOL CheckKbdMove( int keycode );
static BOOL ContinueKbdMove( int keycode );
static BOOL EndKbdMove( int keycode );

static BOOL (*KeyDownActions[])( int keycode ) = {
    CheckKbdMove,                   /* DORMANT          */
    CheckKbdMove,                   /* OVERBOX          */
    IgnoreKbd,                      /* MOVING           */
    IgnoreKbd,                      /* EDITING          */
    IgnoreKbd,                      /* SIZING           */
    IgnoreKbd,                      /* CREATING         */
    IgnoreKbd,                      /* ALIGNING         */
    IgnoreKbd,                      /* PASTE_PENDING    */
    IgnoreKbd,                      /* PASTEING         */
    IgnoreKbd,                      /* SELECTING        */
    IgnoreKbd,                      /* MOVE_PENDING     */
    IgnoreKbd,                      /* ACTION_ABORTED   */
    ContinueKbdMove                 /* KBD_MOVING       */
};

static BOOL (*KeyUpActions[])( int keycode ) = {
    IgnoreKbd,                      /* DORMANT          */
    IgnoreKbd,                      /* OVERBOX          */
    IgnoreKbd,                      /* MOVING           */
    IgnoreKbd,                      /* EDITING          */
    IgnoreKbd,                      /* SIZING           */
    IgnoreKbd,                      /* CREATING         */
    IgnoreKbd,                      /* ALIGNING         */
    IgnoreKbd,                      /* PASTE_PENDING    */
    IgnoreKbd,                      /* PASTEING         */
    IgnoreKbd,                      /* SELECTING        */
    IgnoreKbd,                      /* MOVE_PENDING     */
    IgnoreKbd,                      /* ACTION_ABORTED   */
    EndKbdMove                      /* KBD_MOVING       */
};

extern BOOL ProcessKeyDown( int keycode )
/***************************************/
{
    return( KeyDownActions[GetState()]( keycode ) );
}

extern BOOL ProcessKeyUp( int keycode )
/*************************************/
{
    return( KeyUpActions[GetState()]( keycode ) );
}


static BOOL IgnoreKbd( int keycode )
/**********************************/
{
    keycode = keycode;
    return( FALSE );
}

static void MoveCurrObj( LPPOINT pt )
/***********************************/
{
    CURROBJPTR  curr;

    curr = GetECurrObject();
    while( curr != NULL ) {
        Move( curr, pt, TRUE );
        curr = GetNextECurrObject( curr );
    }
}

static BOOL FilterMoveKeys( int keycode, LPPOINT pt )
/***************************************************/
{
    BOOL    ret;

    ret = TRUE;
    switch( keycode ) {
    case VK_LEFT:
        pt->x = -GetHorizontalInc();
        pt->y = 0;
        break;
    case VK_UP:
        pt->x = 0;
        pt->y = -GetVerticalInc();
        break;
    case VK_RIGHT:
        pt->x = GetHorizontalInc();
        pt->y = 0;
        break;
    case VK_DOWN:
        pt->x = 0;
        pt->y = GetVerticalInc();
        break;
    default:
        ret = FALSE;
        break;
    }
    return( ret );
}

static void DoKbdMove( LPPOINT pt )
/*********************************/
{
    LIST    *list;

    list = GetCurrObjectList();
    SetShowEatoms( FALSE );
    BeginMoveOperation( list );
    ListFree( list );
    MoveCurrObj( pt );
    FinishMoveOperation( FALSE );
    SetShowEatoms( TRUE );
}

static void SetKbdMoveGrid( void )
/********************************/
{
    CURROBJPTR  curr;
    unsigned    hinc;
    unsigned    vinc;
    POINT       pt;

    hinc = 0;
    vinc = 0;
    curr = GetECurrObject();
    while( curr != NULL ) {
        if( ResizeIncrements( curr, &pt ) ) {
            hinc = max( hinc, pt.x );
            vinc = max( vinc, pt.y );
        }
        curr = GetNextECurrObject( curr );
    }
    if( hinc != 0 && vinc != 0 ) {
        SetResizeGrid( hinc, vinc );
    }
}

static BOOL CheckKbdMove( int keycode )
/*************************************/
{
    POINT   pt;

    if( FilterMoveKeys( keycode, &pt ) ) {
        SetState( KBD_MOVING );
        SetKbdMoveGrid();
        DoKbdMove( &pt );
        return( TRUE );
    } else {
        return( FALSE );
    }
}

static BOOL ContinueKbdMove( int keycode )
/****************************************/
{
    POINT   pt;

    if( FilterMoveKeys( keycode, &pt ) ) {
        DoKbdMove( &pt );
        return( TRUE );
    } else {
        return( FALSE );
    }
}

static BOOL EndKbdMove( int keycode )
/***********************************/
{
    POINT   dummy;

    if( FilterMoveKeys( keycode, &dummy ) ) {
        SetDefState();
        return( TRUE );
    } else {
        return( FALSE );
    }
}
