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
* Description:  Windows mouse input handling.
*
****************************************************************************/


#include <dos.h>
#include "uidos.h"
#include "uidef.h"
#include "uimouse.h"
#include "biosui.h"
#include "windows.h"

static int MouseX,MouseY,MouseStatusBits;
static int ScreenXFudge,ScreenYFudge;

struct mouse_data {
    unsigned    bx,cx,dx;
};

typedef struct mouse_data __based( __segname( "_STACK" ) ) *md_stk_ptr;

/* Invoke the mouse interrupt (33h). */
extern unsigned MouseInt( unsigned, unsigned, unsigned, unsigned );
#pragma aux MouseInt =  \
    "int 33h"           \
    parm [ax] [bx] [cx] [dx];

extern void MouseInt2( unsigned, unsigned, unsigned, unsigned, unsigned );
#pragma aux MouseInt2 = \
    "int 33h"           \
    parm [ax] [cx] [dx] [si] [di];

//extern void MouseState( unsigned, struct mouse_data near * );
extern void MouseState( unsigned, md_stk_ptr );
#pragma aux MouseState =    \
    "int 33h"               \
    "mov ss:[si+0],bx"      \
    "mov ss:[si+2],cx"      \
    "mov ss:[si+4],dx"      \
    parm [ax] [si] modify [bx cx dx];

extern unsigned long uiclock( void );

#define         MOUSE_SCALE             8


extern          MOUSEORD                MouseRow;
extern          MOUSEORD                MouseCol;
extern          bool                    MouseOn;

extern          unsigned long           MouseTime       = 0L;

extern          unsigned                MouseStatus;
extern          bool                    MouseInstalled;

void intern checkmouse( unsigned short *status, MOUSEORD *row,
                          MOUSEORD *col, unsigned long *time )
/************************************************************/
{
    struct  mouse_data state;

    MouseState( 3, (md_stk_ptr)&state );
    *status = MouseStatusBits;
    *col = MouseX;
    *row = MouseY;
    *time = uiclock();
    uisetmouse( *row, *col );
}



bool global initmouse( int install )
/**********************************/
{
    int         cx,dx;
    unsigned short  tmp;

    MouseInstalled = FALSE;
    ScreenXFudge = (WORD) ((DWORD) GetSystemMetrics( SM_CXSCREEN )/(DWORD) UIData->width);
    ScreenYFudge = (WORD) ((DWORD) GetSystemMetrics( SM_CYSCREEN )/(DWORD) UIData->height);
    if( install > 0 ) {

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
            checkmouse( &tmp, &MouseRow, &MouseCol, &MouseTime );
            MouseStatus = tmp;
        }
    }
    return( MouseInstalled );
}


void extern finimouse( void )
/***************************/
{
    if( MouseInstalled ) {
        uioffmouse();
    }
}


void global uisetmouseposn(             /* SET MOUSE POSITION */
    ORD row,                            /* - mouse row        */
    ORD col )                           /* - mouse column     */
{
    MouseRow = row;
    MouseCol = col;
//  MouseInt( 4, 0, col * MOUSE_SCALE, row * MOUSE_SCALE );
    SetCursorPos( col * ScreenXFudge, row * ScreenYFudge );
}

/*
 * WindowsMouseEvent
 */
void WindowsMouseEvent( unsigned event, unsigned info )
{
POINT p;

        info = info;    /* shut the compiler up */
        switch( event ) {
        case WM_MOUSEMOVE:
            GetCursorPos( &p );
            MouseX = (WORD)((DWORD)p.x / (DWORD) ScreenXFudge);
            if( MouseX > UIData->width-1 ) MouseX = UIData->width-1;
            MouseY = (WORD)((DWORD)p.y / (DWORD) ScreenYFudge);
            if( MouseY > UIData->height-1 ) MouseY = UIData->height-1;
            break;
        case WM_LBUTTONUP:
            MouseStatusBits &= (0xFFFF) - MOUSE_PRESS;
            break;
        case WM_RBUTTONUP:
            MouseStatusBits &= (0xFFFF) - MOUSE_PRESS_RIGHT;
            break;
        case WM_LBUTTONDOWN:
            MouseStatusBits |= MOUSE_PRESS;
            break;
        case WM_RBUTTONDOWN:
            MouseStatusBits |= MOUSE_PRESS_RIGHT;
            break;
        } /* switch */

} /* WindowsMouseEvent */
