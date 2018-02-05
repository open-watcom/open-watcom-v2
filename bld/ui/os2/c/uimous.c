/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS/2 mouse input handling.
*
****************************************************************************/


#include <dos.h>
#include <malloc.h>
#include "uidef.h"
#include "doscall.h"
#include "uimouse.h"
#include "biosui.h"


#define MOUSE_SCALE     8

/* Process Type codes (local information segment typeProcess field)           */

#define _PT_FULLSCREEN              0 /* Full screen application               */
#define _PT_REALMODE                1 /* Real mode process                     */
#define _PT_WINDOWABLEVIO           2 /* VIO windowable application            */
#define _PT_PM                      3 /* Presentation Manager application      */
#define _PT_DETACHED                4 /* Detached application                  */

/* Local Information Segment */

typedef struct __LINFOSEG {      /* lis */
    PID     pidCurrent;
    PID     pidParent;
    USHORT  prtyCurrent;
    TID     tidCurrent;
    USHORT  sgCurrent;
    UCHAR   rfProcStatus;
    UCHAR   dummy1;
    BOOL    fForeground;
    UCHAR   typeProcess;
    UCHAR   dummy2;
    SEL     selEnvironment;
    USHORT  offCmdLine;
    USHORT  cbDataSegment;
    USHORT  cbStack;
    USHORT  cbHeap;
    HMODULE hmod;
    SEL     selDS;
} __LINFOSEG;

static HMOU             MouHandle;
static bool             TwoButtonMouse = false;
static ORD              Row;
static ORD              Col;
static MOUSESTAT        Status;

void intern mousespawnstart( void )
/*********************************/
{
    uihidemouse();
}

void intern mousespawnend( void )
/*******************************/
{
}

static void GetMouseInfo( void )
/******************************/
{
    struct      _MOUEVENTINFO   mouinfo;
    USHORT                      readtype = 0;
    struct      _MOUQUEINFO     queue;

    if( MouGetNumQueEl( &queue, MouHandle ) != 0 )
        return;
    if( queue.cEvents == 0 )
        return;
    if( MouReadEventQue( &mouinfo, &readtype, MouHandle ) != 0 )
        return;
    Status = 0;
    if( mouinfo.fs & 0x0006 )
        Status |= UI_MOUSE_PRESS;
    if( TwoButtonMouse ) {
        if( mouinfo.fs & 0x0078 ) {
            Status |= UI_MOUSE_PRESS_RIGHT;
        }
    } else {
        if( mouinfo.fs & 0x0018 )
            Status |= UI_MOUSE_PRESS_MIDDLE;
        if( mouinfo.fs & 0x0060 ) {
            Status |= UI_MOUSE_PRESS_RIGHT;
        }
    }

    Row  = mouinfo.row;
    Col  = mouinfo.col;
}

void intern checkmouse( MOUSESTAT *pstatus, MOUSEORD *prow, MOUSEORD *pcol, MOUSETIME *ptime )
/********************************************************************************************/
{
#ifdef _M_I86
    if( _osmode == DOS_MODE ) {
        struct  mouse_data state;

        MouseDrvCallRetState( 3, &state );
        *pstatus = state.bx;
        *prow  = state.dx / MOUSE_SCALE;
        *pcol  = state.cx / MOUSE_SCALE;
    } else {
#endif
        GetMouseInfo();
        *pstatus = Status;
        *prow = Row;
        *pcol = Col;
#ifdef _M_I86
    }
#endif
    *ptime = uiclock();
    uisetmouse( *prow, *pcol );
}


void uimousespeed( unsigned speed )
/*********************************/
/* set speed of mouse. 0 is fastest; the higher the number the slower
   it goes */
{
    if( speed == 0 ) {
        speed = 1;
    }
#ifdef _M_I86
    if( _osmode == DOS_MODE ) {
        MouseDrvCall3( 0x0F, speed, speed * 2, 0, 0 );
        UIData->mouse_speed = speed;
    }
#endif
}

#ifdef _M_I86

#define IRET       '\xCF'

static bool mouse_installed( void )
/*********************************/
{
    unsigned short  __far *vector;
    char            __far *intrtn;

    /* get mouse driver interrupt vector */
    vector = MK_FP( 0, BIOS_MOUSE * 4 );
    intrtn = MK_FP( vector[1], vector[0] );
    return( ( intrtn != NULL ) && ( *intrtn != IRET ) );
}

static void DOS_initmouse( init_mode install )
/********************************************/
{
    int             cx,dx;
    MOUSETIME       time;

    if( install > INIT_MOUSELESS && mouse_installed() ) {
        if( install > INIT_MOUSE ) {
            if( MouseDrvReset() != MOUSE_DRIVER_OK ) {
                install = INIT_MOUSELESS;   /* mouse initialization failed */
            }
        }
        if( install > INIT_MOUSELESS ) {
            dx = ( UIData->width - 1 ) * MOUSE_SCALE;
            MouseDrvCall2( 7, 0, 0, dx );
            dx = ( UIData->height - 1 ) * MOUSE_SCALE;
            MouseDrvCall2( 8, 0, 0, dx );

            cx = ( UIData->colour == M_MONO ? 0x79ff : 0x7fff );
            dx = ( UIData->colour == M_MONO ? 0x7100 : 0x7700 );
            MouseDrvCall2( 0x0A, 0, cx, dx );
            MouseDrvCall3( 0x10, 0, 0, 0, 0 );

            UIData->mouse_swapped = false;
            UIData->mouse_xscale = 1;
            UIData->mouse_yscale = 1;
            uisetmouseposn( UIData->height / 2 - 1, UIData->width / 2 - 1 );
            MouseInstalled = true;
            MouseOn = false;
            checkmouse( &Status, &MouseRow, &MouseCol, &time );
            uimousespeed( UIData->mouse_speed );
        }
    }
}
#endif

static void OS2_initmouse( init_mode install )
/********************************************/
{
    USHORT          mouevents;
    USHORT          num_buttons;

    if( install > INIT_MOUSELESS && ( MouOpen( 0L, &MouHandle ) == 0 ) ) {
        if( MouGetNumButtons( &num_buttons, MouHandle ) == 0 ) {
            if( num_buttons == 2 ) {
                TwoButtonMouse = true;
                mouevents = 0x001f;
                MouSetEventMask( &mouevents, MouHandle );
            } else if( num_buttons == 3 ) {
                mouevents = 0x007f;
                MouSetEventMask( &mouevents, MouHandle );
            }
        }
        MouseInstalled = true;
        {
#ifdef _M_I86
            SEL                 gbl;
            SEL                 lcl;
            __LINFOSEG          __far *linfo;

            DosGetInfoSeg( &gbl, &lcl );
            linfo = MK_FP( lcl, 0 );
            if( linfo->typeProcess != _PT_FULLSCREEN ) {
                uimouseforceoff();      /* let PM draw the mouse cursor */
            }
#else
            PTIB        tib;
            PPIB        pib;

            DosGetInfoBlocks( &tib, &pib );
            if( pib->pib_ultype != _PT_FULLSCREEN ) {
                uimouseforceoff();      /* let PM draw the mouse cursor */
            }
#endif
        }
    }
    MouseOn = false;
    UIData->mouse_swapped = false;
    UIData->mouse_xscale = 1;
    UIData->mouse_yscale = 1;
}


bool UIAPI initmouse( init_mode install )
/***************************************/
{
    MouseInstalled = false;
#ifdef _M_I86
    if( _osmode == DOS_MODE ) {
        DOS_initmouse( install );
    } else {
#endif
        OS2_initmouse( install );
#ifdef _M_I86
    }
#endif
    return( MouseInstalled );
}


void UIAPI finimouse( void )
/**************************/
{
    if( MouseInstalled ) {
        uioffmouse();
#ifdef _M_I86
        if( _osmode == OS2_MODE ) {
#endif
            MouClose( MouHandle );
#ifdef _M_I86
        }
#endif
    }
}

void UIAPI uisetmouseposn( ORD row, ORD col )
{
#ifdef _M_I86
    if( _osmode == DOS_MODE ) {
        MouseRow = row;
        MouseCol = col;
        MouseDrvCall2( 4, 0, col * MOUSE_SCALE, row * MOUSE_SCALE );
    } else {
#endif
        uisetmouse( row, col );
#ifdef _M_I86
    }
#endif
}
