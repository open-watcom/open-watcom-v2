/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#define M_PRESS                 1
#define M_RELEASE               2
#define M_DCLICK                3
#define M_HOLD                  4
#define M_DRAG                  5
#define M_REPEAT                6

MOUSEORD        MouseRow;
MOUSEORD        MouseCol;
bool            MouseOn         = false;
MOUSESTAT       MouseStatus     = 0;
bool            MouseInstalled  = false;
MOUSETIME       MouseTime       = 0L;

static ui_event     mouseevtab[6][3] = {
    { EV_MOUSE_PRESS,   EV_MOUSE_PRESS_R,       EV_MOUSE_PRESS_M },
    { EV_MOUSE_RELEASE, EV_MOUSE_RELEASE_R,     EV_MOUSE_RELEASE_M },
    { EV_MOUSE_DCLICK,  EV_MOUSE_DCLICK_R,      EV_MOUSE_DCLICK_M },
    { EV_MOUSE_HOLD,    EV_MOUSE_HOLD_R,        EV_MOUSE_HOLD_M },
    { EV_MOUSE_DRAG,    EV_MOUSE_DRAG_R,        EV_MOUSE_DRAG_M },
    { EV_MOUSE_REPEAT,  EV_MOUSE_REPEAT_R,      EV_MOUSE_REPEAT_M }
};

static  int             MouseForcedOff  = 0;
static  bool            MouseRepeat     = false;
static  mouse_func      MouseLast       = MOUSE_OFF;
static  int             MouseLastButton = -1;

bool UIAPI uimouseinstalled( void )
/*********************************/
/* call this ONLY after UI has been initialized */
{
    return( MouseInstalled );
}

static void mouse( mouse_func func )
/**********************************/
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
/********************************/
/* this function will turn off the mouse and will prevent UI from turning
   it on until uimouseforceon is called (i.e. moving the mouse will NOT
   turn it on). */
{
    uioffmouse();
    ++MouseForcedOff;
}

void UIAPI uimouseforceon( void )
/*******************************/
/* call this function after sometime after uimouseforceoff, if the default
   UI mouse behaviour is desired */
{
    --MouseForcedOff;
}

void UIAPI uionmouse( void )
/**************************/
{
    if( MouseOn ) {
        mouse( MOUSE_ON );
    } else {
        mouse( MOUSE_OFF );
    }
}


void UIAPI uioffmouse( void )
/***************************/
// turn mouse cursor off temporarily ( until next getprimeevent )
{
    mouse( MOUSE_OFF );
}


void UIAPI uihidemouse( void )
/****************************/
// turn mouse cursor off ( until user clicks or moves )
{
    MouseOn = false;
}


static int button( MOUSESTAT status )
/***********************************/
{
    status &= UI_MOUSE_PRESS_ANY;
    if( status == UI_MOUSE_PRESS ) {
        return( 0 );
    } else if( status == UI_MOUSE_PRESS_RIGHT ) {
        return( 1 );
    } else {
        return( 2 );
    }
}

ui_event intern mouseevent( void )
/********************************/
{
    ui_event        ui_ev;
    MOUSEORD        row;
    MOUSEORD        col;
    MOUSETIME       time;
    MOUSESTAT       status;
    bool            moved;
    MOUSESTAT       diff;
    int             butt;
    int             mindex;

    ui_ev = EV_NO_EVENT;
    mindex = 0;
    butt = 0;
    if( MouseInstalled ) {
        checkmouse( &status, &row, &col, &time );
        diff = (status ^ MouseStatus) & UI_MOUSE_PRESS_ANY;
        moved = ( row / UIData->mouse_yscale != MouseRow / UIData->mouse_yscale
               || col / UIData->mouse_xscale != MouseCol / UIData->mouse_xscale );
        mindex = 0;
        butt = 0;
        if( moved ) {
            if( MouseStatus & UI_MOUSE_PRESS_ANY ) {
                /* DO NOT TURN ON THE MOUSE IF YOU ARE DRAGGING */
                /* i.e. don't set MouseOn = true */
                butt = button( status );
                mindex = M_DRAG;
            } else {
                ui_ev = EV_MOUSE_MOVE;
                MouseOn = true;
            }
            MouseLastButton = -1;    /* don't double click */
        } else if( diff & UI_MOUSE_PRESS_ANY ) {
            if( (diff & status) == diff ) {
                if( button( diff ) == MouseLastButton && time - MouseTime < UIData->mouse_clk_delay ) {
                    mindex = M_DCLICK;
                } else {
                    mindex = M_PRESS;
                    MouseLastButton = button( diff );
                }
            } else {
                mindex = M_RELEASE;
                flushkey();
            }
            butt = button( diff );
            MouseRepeat = false;
            MouseTime = time;
            MouseStatus = status;
            MouseOn = true;
        } else if( status & UI_MOUSE_PRESS_ANY ) {
            if( UIData->busy_wait ) {
                mindex = M_HOLD;
                // DEN 92/3/16 - added for dbserver - menus didn't get updated
                uirefresh();
            }
            butt = button( status );
            if( !MouseRepeat ) {
                if( time - MouseTime > UIData->mouse_acc_delay ) {
                    mindex = M_REPEAT;
                    MouseRepeat = true;
                    MouseTime = time;
                }
            } else if( time - MouseTime > UIData->mouse_rpt_delay ) {
                mindex = M_REPEAT;
                MouseTime = time;
            }
        }

        if( mindex != 0 && ui_ev != EV_MOUSE_MOVE ) {
            ui_ev = mouseevtab[mindex - 1][butt];
        }
        MouseRow = row;
        MouseCol = col;
    }
    return( ui_ev );
}

VSCREEN * UIAPI uimousepos( VSCREEN *vs, int *rowptr, int *colptr )
/*****************************************************************/
{
    VSCREEN         *owner_vs;
    int             row;
    int             col;

    owner_vs = findvscreen( MouseRow / UIData->mouse_yscale, MouseCol / UIData->mouse_xscale );

    if( vs != NULL ) {
        row = MouseRow - (int)vs->area.row * UIData->mouse_yscale;
        col = MouseCol - (int)vs->area.col * UIData->mouse_xscale;
    } else {
        row = MouseRow;
        col = MouseCol;
    }
    if( row < 0 && ( row % UIData->mouse_yscale ) != 0 ) {
        row -= UIData->mouse_yscale;
    }
    if( col < 0 && ( col % UIData->mouse_xscale ) != 0 ) {
        col -= UIData->mouse_xscale;
    }
    *rowptr = row / UIData->mouse_yscale;
    *colptr = col / UIData->mouse_xscale;

    return( owner_vs );
}

VSCREEN * UIAPI uivmousepos( VSCREEN *vs, ORD *rowptr, ORD *colptr )
/******************************************************************/
{
    VSCREEN     *owner_vs;
    int         row;
    int         col;

    owner_vs = uimousepos( vs, &row, &col );
    if( vs != NULL ) {
        if( row < 0 )
            row = 0;
        if( col < 0 )
            col = 0;
        if( row >= vs->area.height )
            row = vs->area.height - 1;
        if( col >= vs->area.width ) {
            col = vs->area.width - 1;
        }
    }
    *rowptr = (ORD)row;
    *colptr = (ORD)col;
    return( owner_vs );
}

void UIAPI uiswapmouse( void )
/****************************/
{

    if( UIData->mouse_swapped ) {
        uionmouse();
        UIData->mouse_swapped = false;
    } else {
        uioffmouse();
        UIData->mouse_swapped = true;
    }
}

#if 0
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
#endif

void UIAPI uigetmouse( ORD *row, ORD *col, bool *status )
/*******************************************************/
{
    *row = MouseRow / UIData->mouse_yscale;
    *col = MouseCol / UIData->mouse_xscale;
    *status = MouseOn;
}

bool UIAPI uivmouseinstalled( void )
{
    return( MouseInstalled );
}
