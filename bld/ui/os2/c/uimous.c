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
* Description:  OS/2 mouse input handling.
*
****************************************************************************/


#include <dos.h>
#include "uidef.h"
#include "doscall.h"
#include "uimouse.h"
#ifdef _M_I86
    #include "int33.h"
#endif

#define _osmode_REALMODE()  (_osmode == DOS_MODE)
#define _osmode_PROTMODE()  (_osmode == OS2_MODE)


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
    if( _osmode_REALMODE() ) {
        mouse_status    state;

        _BIOSMouseGetPositionAndButtonStatus( &state );
        *pstatus = state.button_status;
        *prow  = state.y / MOUSE_SCALE;
        *pcol  = state.x / MOUSE_SCALE;
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
    if( _osmode_REALMODE() ) {
        _BIOSMouseSetMickeysToPixelsRatio( speed, speed * 2 );
        UIData->mouse_speed = speed;
    }
#endif
}

#ifdef _M_I86
#define IRET        0xCF

static bool mouse_installed( void )
/*********************************/
{
    unsigned short  __far *vector;
    unsigned char   __far *intrtn;

    /* get mouse driver interrupt vector */
    vector = _MK_FP( 0, VECTOR_MOUSE * 4 );
    intrtn = _MK_FP( vector[1], vector[0] );
    return( ( intrtn != NULL ) && ( *intrtn != IRET ) );
}

static void DOS_initmouse( init_mode install )
/********************************************/
{
    int             cx,dx;
    MOUSETIME       time;

    if( install != INIT_MOUSELESS && mouse_installed() ) {
        if( install == INIT_MOUSE_INITIALIZED ) {
            if( _BIOSMouseDriverReset() != MOUSE_DRIVER_OK ) {
                install = INIT_MOUSELESS;   /* mouse initialization failed */
            }
        }
        if( install != INIT_MOUSELESS ) {
            dx = ( UIData->width - 1 ) * MOUSE_SCALE;
            _BIOSMouseSetHorizontalLimitsForPointer( 0, dx );
            dx = ( UIData->height - 1 ) * MOUSE_SCALE;
            _BIOSMouseSetVerticalLimitsForPointer( 0, dx );

            cx = ( UIData->colour == M_MONO ? 0x79ff : 0x7fff );
            dx = ( UIData->colour == M_MONO ? 0x7100 : 0x7700 );
            _BIOSMouseSetTextPointerType( SOFTWARE_CURSOR, cx, dx );
            _BIOSMouseSetPointerExclusionArea( 0, 0, 0, 0 );

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

static bool isNotFullScreen( void )
/*********************************/
{
#ifdef _M_I86
    SEL                 gbl;
    SEL                 lcl;
    LINFOSEG            __far *linfo;

    DosGetInfoSeg( &gbl, &lcl );
    linfo = _MK_FP( lcl, 0 );
    return( linfo->typeProcess != PT_FULLSCREEN );
#else
    PTIB        tib;
    PPIB        pib;

    DosGetInfoBlocks( &tib, &pib );
    return( pib->pib_ultype != PT_FULLSCREEN );
#endif
}

static void OS2_initmouse( init_mode install )
/********************************************/
{
    USHORT          mouevents;
    USHORT          num_buttons;

    if( install != INIT_MOUSELESS && ( MouOpen( 0L, &MouHandle ) == 0 ) ) {
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
        if( isNotFullScreen() ) {
            uimouseforceoff();      /* let PM draw the mouse cursor */
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
    if( _osmode_REALMODE() ) {
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
        if( _osmode_PROTMODE() ) {
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
    if( _osmode_REALMODE() ) {
        MouseRow = row;
        MouseCol = col;
        _BIOSMouseSetPointerPosition( col * MOUSE_SCALE, row * MOUSE_SCALE );
    } else {
#endif
        uisetmouse( row, col );
#ifdef _M_I86
    }
#endif
}
