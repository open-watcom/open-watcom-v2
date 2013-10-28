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


#include "doscall.h"
#include <dos.h>
#include <malloc.h>
#include <stdlib.h>
#include "uidef.h"
#include "uimouse.h"

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

/* Process Type codes (local information segment typeProcess field)           */

#define _PT_FULLSCREEN              0 /* Full screen application               */
#define _PT_REALMODE                1 /* Real mode process                     */
#define _PT_WINDOWABLEVIO           2 /* VIO windowable application            */
#define _PT_PM                      3 /* Presentation Manager application      */
#define _PT_DETACHED                4 /* Detached application                  */

struct mouse_data {
    unsigned    bx,cx,dx;
};
#define BIOS_MOUSE      0x33

#pragma aux MouseInt = 0xcd BIOS_MOUSE parm [ax] [bx] [cx] [dx];
extern unsigned MouseInt( unsigned, unsigned, unsigned, unsigned );

#pragma aux MouseInt2 = 0xcd BIOS_MOUSE parm [ax] [cx] [dx] [si] [di];
extern void MouseInt2( unsigned, unsigned, unsigned, unsigned, unsigned );

#pragma aux MouseState = 0xcd BIOS_MOUSE \
                        0x36 0x89 0x1c   \
                        0x36 0x89 0x4c 0x02 \
                        0x36 0x89 0x54 0x04 \
                        parm [ax] [si] modify [bx cx dx];
extern void MouseState( unsigned, struct mouse_data __near * );

extern unsigned long uiclock( void );

#define         MOUSE_SCALE             8


extern          MOUSEORD                MouseRow;
extern          MOUSEORD                MouseCol;
extern          bool                    MouseOn;

extern          bool                    MouseInstalled;
static          HMOU                    MouHandle;
static          bool                    TwoButtonMouse = FALSE;

static          ORD                     Row;
static          ORD                     Col;
static          unsigned short          Status;



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

    if( MouGetNumQueEl( &queue, MouHandle ) != 0 ) return;
    if( queue.cEvents == 0 ) return;
    if( MouReadEventQue( &mouinfo, &readtype, MouHandle ) != 0 ) return;
    Status = 0;
    if( mouinfo.fs & 0x0006 ) Status |= MOUSE_PRESS;
    if( TwoButtonMouse ){
        if( mouinfo.fs & 0x0078 ) Status |= MOUSE_PRESS_RIGHT;
    } else {
        if( mouinfo.fs & 0x0018 ) Status |= MOUSE_PRESS_MIDDLE;
        if( mouinfo.fs & 0x0060 ) Status |= MOUSE_PRESS_RIGHT;
    }

    Row  = mouinfo.row;
    Col  = mouinfo.col;
}

void intern checkmouse( unsigned short *pstatus, MOUSEORD *prow,
                          MOUSEORD *pcol, unsigned long *ptime )
/**************************************************************/
{
    if( _osmode == DOS_MODE ) {
        struct  mouse_data state;

        MouseState( 3, (void __near *)&state );
        *pstatus = state.bx;
        *prow  = state.dx / MOUSE_SCALE;
        *pcol  = state.cx / MOUSE_SCALE;
    } else {
        GetMouseInfo();
        *pstatus = Status;
        *prow = Row;
        *pcol = Col;
    }
    *ptime = uiclock();
    uisetmouse( *prow, *pcol );
}


void uimousespeed( unsigned speed )
/*********************************/
/* set speed of mouse. 0 is fastest; the higher the number the slower
   it goes */
{
    if( (int)speed <= 0 ) {
        speed = 1;
    }

    if( _osmode == DOS_MODE ) {
        MouseInt2( 15, speed, speed * 2, 0, 0 );
        UIData->mouse_speed = speed;
    }
}


#define         IRET                    (char) 0xcf

static bool mouse_installed( void )
{
    unsigned short __far        *vector;
    char __far                  *intrtn;
    // 91/05/15 DEN - major kludge to fix code gen bug
    int                         zero = 0;

    vector = MK_FP( zero, BIOS_MOUSE * 4 );
    intrtn = MK_FP( vector[1], vector[0] );
    return( ( intrtn != NULL ) && ( *intrtn != IRET ) );
}

static void DOS_initmouse( int install )
/**************************************/
{
    int             cx,dx;
    unsigned long   time;

    if( install > 0 && mouse_installed() ) {
        if( install > 1 ) {
            if( MouseInt( 0, 0, 0, 0 ) != -1 ) {
                install = 0; /* mouse initialization failed */
            }
        }
        if( install > 0 ) {
            dx = ( UIData->width - 1 )*MOUSE_SCALE;
            MouseInt( 7, 0, 0, dx );
            dx = ( UIData->height - 1 )*MOUSE_SCALE;
            MouseInt( 8, 0, 0, dx );

            cx = ( UIData->colour == M_MONO ? 0x79ff : 0x7fff );
            dx = ( UIData->colour == M_MONO ? 0x7100 : 0x7700 );
            MouseInt( 10, 0, cx, dx );
            MouseInt2( 16, 0, 0, 0, 0 );

            UIData->mouse_swapped = FALSE;
            UIData->mouse_xscale = 1;
            UIData->mouse_yscale = 1;
            uisetmouseposn( UIData->height/2 - 1, UIData->width/2 - 1 );
            MouseInstalled = TRUE;
            MouseOn = FALSE;
            checkmouse( &Status, &MouseRow, &MouseCol, &time );
            uimousespeed( UIData->mouse_speed );
        }
    }
}

static void OS2_initmouse( int install )
/**************************************/
{
    USHORT          mouevents;
    USHORT          num_buttons;

    if( install && (MouOpen(0L, &MouHandle) == 0) ) {
        if( MouGetNumButtons( &num_buttons, MouHandle ) == 0 ) {
            if( num_buttons == 2 ) {
                TwoButtonMouse = TRUE;
                mouevents = 0x001f;
                MouSetEventMask( &mouevents, MouHandle );
            } else if( num_buttons == 3 ) {
                mouevents = 0x007f;
                MouSetEventMask( &mouevents, MouHandle );
            }
        }
        MouseInstalled = TRUE;
        {
#ifdef __386__
            PTIB        tib;
            PPIB        pib;

            DosGetInfoBlocks( &tib, &pib );
            if( pib->pib_ultype != _PT_FULLSCREEN ) {
                uimouseforceoff();      /* let PM draw the mouse cursor */
            }
#else
            SEL                 gbl;
            SEL                 lcl;
            __LINFOSEG          __far *linfo;

            DosGetInfoSeg( &gbl, &lcl );
            linfo = MK_FP( lcl, 0 );
            if( linfo->typeProcess != _PT_FULLSCREEN ) {
                uimouseforceoff();      /* let PM draw the mouse cursor */
            }
#endif
        }
    }
    MouseOn = FALSE;
    UIData->mouse_swapped = FALSE;
    UIData->mouse_xscale = 1;
    UIData->mouse_yscale = 1;
}


bool global initmouse( int install )
/**********************************/
{
    MouseInstalled = FALSE;
    if( _osmode == DOS_MODE ) {
        DOS_initmouse( install );
    } else {
        OS2_initmouse( install );
    }
    return( MouseInstalled );
}


void extern finimouse( void )
/***************************/
{
    if( MouseInstalled ) {
        uioffmouse();
        if( _osmode == OS2_MODE ) {
            MouClose( MouHandle );
        }
    }
}

void global uisetmouseposn(             /* SET MOUSE POSITION */
    ORD row,                            /* - mouse row        */
    ORD col )                           /* - mouse column     */
{
    if( _osmode == DOS_MODE ) {
        MouseRow = row;
        MouseCol = col;
        MouseInt( 4, 0, col * MOUSE_SCALE, row * MOUSE_SCALE );
    } else {
        uisetmouse( row, col );
    }
}

