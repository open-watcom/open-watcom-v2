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
* Description:  UI library mouse event processing.
*
****************************************************************************/


#include "uidef.h"
#include "uimouse.h"

static          EVENT                   mouseevtab[6][3] = {
    { EV_MOUSE_PRESS,   EV_MOUSE_PRESS_R,       EV_MOUSE_PRESS_M },
    { EV_MOUSE_RELEASE, EV_MOUSE_RELEASE_R,     EV_MOUSE_RELEASE_M },
    { EV_MOUSE_DCLICK,  EV_MOUSE_DCLICK_R,      EV_MOUSE_DCLICK_M },
    { EV_MOUSE_HOLD,    EV_MOUSE_HOLD_R,        EV_MOUSE_HOLD_M },
    { EV_MOUSE_DRAG,    EV_MOUSE_DRAG_R,        EV_MOUSE_DRAG_M },
    { EV_MOUSE_REPEAT,  EV_MOUSE_REPEAT_R,      EV_MOUSE_REPEAT_M }
    };

                MOUSEORD          MouseRow;
                MOUSEORD          MouseCol;
                bool                    MouseOn;
                unsigned short          MouseStatus;
                bool                    MouseInstalled;

static          int                     MouseForcedOff = 0;

static          bool                    MouseRepeat;
static          unsigned long           MouseTime       = 0L;
static          int                     MouseLast       = MOUSE_OFF;
static          unsigned short          MouseLastButton = (unsigned short)~0;


int UIAPI uimouseinstalled( void )
/*********************************/
/* call this ONLY after UI has been initialized */

{
    return( MouseInstalled );
}

static void mouse( int func )
/***************************/
{
    if( MouseInstalled ) {
        if( MouseForcedOff == 0 ) {
            if( MouseLast != func ) {
                uimouse( func );
                MouseLast = func;
            }
        }
    }
}

void UIAPI uimouseforceoff( void )
/*********************************/
/* this function will turn off the mouse and will prevent UI from turning
   it on until uimouseforceon is called (i.e. moving the mouse will NOT
   turn it on). */
{
    uioffmouse();
    ++MouseForcedOff;
}

void UIAPI uimouseforceon( void )
/********************************/
/* call this function after sometime after uimouseforceoff, if the default
   UI mouse behaviour is desired */
{
    --MouseForcedOff;
}

void UIAPI uionmouse( void )
/***************************/
{
    if( MouseOn ) {
        mouse( MOUSE_ON );
    } else {
        mouse( MOUSE_OFF );
    }
}


void UIAPI uioffmouse( void )
/****************************/
// turn mouse cursor off temporarily ( until next getprimeevent )
{
    mouse( MOUSE_OFF );
}


void UIAPI uihidemouse( void )
/*****************************/
// turn mouse cursor off ( until user clicks or moves )
{
    MouseOn = FALSE;
}


static unsigned short button( unsigned short status )
/***************************************************/
{
    status &= MOUSE_PRESS_ANY;
    if( status == MOUSE_PRESS ){
        return( 0 );
    } else if( status == MOUSE_PRESS_RIGHT ){
        return( 1 );
    } else {
        return( 2 );
    }
}

EVENT intern mouseevent( void )
/*****************************/
{
    register    EVENT                   ev;
    auto        MOUSEORD                     row;
    auto        MOUSEORD                     col;
    auto        unsigned long           time;
    auto        unsigned short          status;
    auto        bool                    moved;
    auto        unsigned short          diff;
    auto        signed short            butt = 0;

    ev = EV_NO_EVENT;
    if( MouseInstalled ) {

        checkmouse( &status, &row, &col, &time );
        diff = (status ^ MouseStatus) & MOUSE_PRESS_ANY;

        moved = ( row/UIData->mouse_yscale != MouseRow/UIData->mouse_yscale
               || col/UIData->mouse_xscale != MouseCol/UIData->mouse_xscale );

        if( moved ){
            if( MouseStatus & MOUSE_PRESS_ANY ){
                /* DO NOT TURN ON THE MOUSE IF YOU ARE DRAGGING */
                /* i.e. don't set MouseOn = TRUE */
                butt = button( status );
                ev = M_DRAG;
            } else {
                ev = EV_MOUSE_MOVE;
                MouseOn = TRUE;
            }
            MouseLastButton = -1;    /* don't double click */
        } else if( diff & MOUSE_PRESS_ANY ){
            if( (diff & status) == diff ){
                if( button(diff) == MouseLastButton  &&
                    time - MouseTime < UIData->mouse_clk_delay ){
                    ev = M_DCLICK;
                } else {
                    ev = M_PRESS;
                    MouseLastButton = button( diff );
                }
            } else {
                ev = M_RELEASE;
                flushkey();
            }
            butt = button( diff );
            MouseRepeat = FALSE;
            MouseTime = time;
            MouseStatus = status;
            MouseOn = TRUE;
        } else if( status & MOUSE_PRESS_ANY ){
            if( UIData->busy_wait ) {
                ev = M_HOLD;
                // DEN 92/3/16 - added for dbserver - menus didn't get updated
                uirefresh();
            }
            butt = button( status );
            if( !MouseRepeat ){
                if( time - MouseTime > UIData->mouse_acc_delay ){
                    ev = M_REPEAT;
                    MouseRepeat = TRUE;
                    MouseTime = time;
                }
            } else if( time - MouseTime > UIData->mouse_rpt_delay ){
                ev = M_REPEAT;
                MouseTime = time;
            }
        }

        if( ev != EV_NO_EVENT && ev != EV_MOUSE_MOVE ){
            ev = mouseevtab[ ev-1 ][ butt ];
        }
        MouseRow = row;
        MouseCol = col;
    }
    return( ev );
}

VSCREEN* UIAPI uimousepos( VSCREEN *vptr, int *rowptr, int *colptr )
/*******************************************************************/
{
    register    VSCREEN*                owner;

    owner = findvscreen( MouseRow/UIData->mouse_yscale, MouseCol/UIData->mouse_xscale );

    if( vptr != NULL ) {
        *rowptr = (int) MouseRow - (int) vptr->area.row * UIData->mouse_yscale;
        *colptr = (int) MouseCol - (int) vptr->area.col * UIData->mouse_xscale;
    } else {
        *rowptr = MouseRow;
        *colptr = MouseCol;
    }
    if( *rowptr < 0  &&  ( *rowptr % UIData->mouse_yscale ) != 0 ) {
        *rowptr -= UIData->mouse_yscale;
    }
    if( *colptr < 0  &&  ( *colptr % UIData->mouse_xscale ) != 0 ) {
        *colptr -= UIData->mouse_xscale;
    }
    *rowptr /= UIData->mouse_yscale;
    *colptr /= UIData->mouse_xscale;

    return( owner );
}

VSCREEN* UIAPI uivmousepos( VSCREEN *vptr, ORD *rowptr, ORD *colptr )
/********************************************************************/
{
    VSCREEN*                            owner;
    int                      row;
    int                      col;

    owner = uimousepos( vptr, &row, &col );
    if( vptr != NULL ) {
        if( row < 0 ) row = 0;
        if( col < 0 ) col = 0;
        if( row >= vptr->area.height ) row = vptr->area.height - 1;
        if( col >= vptr->area.width ) col = vptr->area.width - 1;
    }
    *rowptr = (ORD) row;
    *colptr = (ORD) col;
    return( owner );
}

void UIAPI uiswapmouse( void )
/*****************************/
{

    if( UIData->mouse_swapped ) {
        uionmouse();
        UIData->mouse_swapped = FALSE;
    } else {
        uioffmouse();
        UIData->mouse_swapped = TRUE;
    }
}


MOUSEORD UIAPI uigetmrow( void )
/*******************************/
{
    return( MouseRow );
}


MOUSEORD UIAPI uigetmcol( void )
/*******************************/
{
    return( MouseCol );
}


void UIAPI uigetmouse( ORD *row, ORD *col, int *status )
/*******************************************************/
{
    *row = MouseRow/UIData->mouse_yscale;
    *col = MouseCol/UIData->mouse_xscale;
    *status = MouseOn;
}

int UIAPI uivmouseinstalled( void )
{
    return( MouseInstalled );
}

