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


#include <dos.h>
#include "uidef.h"
#include "biosui.h"
#include "charmap.h"

// extern       void    (intern *DrawCursor)(void);

static int mouseType;
#define _NEC_MOUSE_HIGH 1
#define _NEC_MOUSE_LOW  2

typedef unsigned short int mouse_t;

#if 0
// This definition of a lo-res mouse is no longer used.
#define _IS_LOW_RES_MOUSE \
    ( UIData->colour == M_NEC_NORM || UIData->colour == M_NEC_NORMHI )
#else
#define _IS_LOW_RES_MOUSE \
    ( mouseType == _NEC_MOUSE_LOW )
#endif
#define _IS_HIGH_RES_MOUSE \
    !(_IS_LOW_RES_MOUSE)

struct mouse_data {
    mouse_t ax,bx,cx,dx;
};

/*
 * The following real-mode type interrupt functions now have protect-mode
 * counterparts, which make use of DPMI function call 0x300 to simulate
 * real-mode interrupts.
 *
 * They all use TinyDPMISimulateRealInt, out of tinyio.h
 */

#ifdef __386__
/*
 * Protect mode versions
 */
#include <string.h>
#include "tinyio.h"

mouse_t MouseInt( mouse_t a, mouse_t b, mouse_t c, mouse_t d )
{
    call_struct r;
    memset( &r, 0, sizeof( call_struct ) );
    r.eax = a;
    r.ebx = b;
    r.ecx = c;
    r.edx = d;
    TinyDPMISimulateRealInt( BIOS_MOUSE, 0, 0, (call_struct far *)(&r) );
    return( (mouse_t)r.eax );
}

mouse_t MouseInt2( mouse_t a, mouse_t b, mouse_t c, mouse_t d, mouse_t e )
{
    call_struct r;
    memset( &r, 0, sizeof( call_struct ) );
    r.eax = a;
    r.ecx = b;
    r.edx = c;
    r.esi = d;
    r.edi = e;
    TinyDPMISimulateRealInt( BIOS_MOUSE, 0, 0, (call_struct far *)(&r) );
    return( (mouse_t)r.eax );
}

void MouseState( mouse_t a, struct mouse_data near * b )
{
    call_struct r;
    memset( &r, 0, sizeof( call_struct ) );
    r.eax = a;
    TinyDPMISimulateRealInt( BIOS_MOUSE, 0, 0, (call_struct far *)(&r) );
    b->ax = (mouse_t)r.eax;
    b->bx = (mouse_t)r.ebx;
    b->cx = (mouse_t)r.ecx;
    b->dx = (mouse_t)r.edx;
}
#else

/*
 * real mode versions
 */

#pragma aux MouseInt = 0xcd BIOS_MOUSE parm [ax] [bx] [cx] [dx];
extern mouse_t MouseInt( mouse_t, mouse_t, mouse_t, mouse_t );

#pragma aux MouseInt2 = 0xcd BIOS_MOUSE parm [ax] [cx] [dx] [si] [di];
extern void MouseInt2( mouse_t, mouse_t, mouse_t, mouse_t, mouse_t );

#pragma aux MouseState =                                \
    "int    33h                                 "       \
    "mov    ss:[si], ax                         "       \
    "mov    ss:+2H[si], bx                      "       \
    "mov    ss:+4H[si], cx                      "       \
    "mov    ss:+6H[si], dx                      "       \
        parm    [ax] [si]                               \
        modify  [ax bx cx dx];
extern void MouseState( mouse_t, struct mouse_data near * );
#endif

extern unsigned long uiclock( void );

#define         MOUSE_SCALE             8
#define         MOUSE_PRESS             0x0001
#define         MOUSE_PRESS_RIGHT       0x0002

extern          MOUSEORD                MouseRow;
extern          MOUSEORD                MouseCol;
extern          bool                    MouseOn;

extern          unsigned long           MouseTime       = 0L;

extern          unsigned short          MouseStatus;
extern          bool                    MouseInstalled;

//  #define _DEBUG_MOUSE
#ifdef _DEBUG_MOUSE
extern          LP_ZEN_CODE             code_pages[];
extern          LP_ZEN_ATTR             attr_pages[];
extern          int                     BIOSCurPage;
#endif

void intern checkmouse( unsigned short  *status,
                        MOUSEORD        *row,
                        MOUSEORD        *col,
                        unsigned long   *time )
{

    struct  mouse_data      state;

#ifdef _DEBUG_MOUSE
    LP_ZEN_CODE             codedst;
    LP_ZEN_ATTR             attrdst;
    char buf[256];
    int i;
#endif

    MouseState( 3, (void near *)&state );

#ifdef _DEBUG_MOUSE
    codedst  = CODE_LOOKUP( 1, 1 );
    attrdst  = ATTR_LOOKUP( 1, 1 );
    sprintf( buf, "mouse : ax=%d; bx=%d; cx=%d; dx=%d\0", state.ax, state.bx, state.cx, state.dx );
    for( i = 0; i < 60; i++ ) {
        codedst[ i ].left = buf[ i ]; codedst[ i ].right = 0;
        attrdst[ i ].left = 0xe1;
    }
#endif

    if( _IS_LOW_RES_MOUSE ) {
        *status = 0x0000;
        if ( state.ax == (mouse_t)(-1) ) {
            *status |= MOUSE_PRESS;
        } else if ( state.bx == (mouse_t)(-1) ) {
            *status |= MOUSE_PRESS_RIGHT;
        }
    } else {
        *status = state.bx;
    }

    *col = state.cx/MOUSE_SCALE;
    *row = state.dx/MOUSE_SCALE;

    *time = uiclock();
    uisetmouse( *row, *col );
}


void uimousespeed( unsigned speed )
/*********************************/
/* set speed of mouse. 0 is fastest; the higher the number the slower
   it goes */
{
    if( speed <= 0 ) {
        speed = 1;
    }

    MouseInt2( 15, speed, speed * 2, 0, 0 );
    UIData->mouse_speed = speed;
}

extern unsigned int getrows();
extern unsigned int getcolumns();

void intern setupmouse( void )
{
    int     dx;
    int     value;
    char    set_horizontal      = _IS_LOW_RES_MOUSE ? 16 : 7;
    char    set_vertical        = _IS_LOW_RES_MOUSE ? 17 : 8;

    /*
     * Set vertical extent:
     */
    value = getrows();
    switch( value ) {
    case 20:    dx = 159;       break;
    case 25:    dx = 199;       break;
    case 31:    dx = 246;       break;
    default:    dx = 199;       break;
    }
    MouseInt( set_vertical, 0, 0, dx );

    /*
     * Set horizontal extent:
     */
    value = getcolumns();
    switch( value ) {
    case 40:    dx = 319;       break;
    case 80:    dx = 639;       break;
    default:    dx = 639;       break;
    }
    MouseInt( set_horizontal, 0, 0, dx );

    uisetmouseposn( UIData->height/2 - 1, UIData->width/2 - 1 );
    MouseInstalled = TRUE;
    MouseOn = FALSE;
    UIData->mouse_swapped = FALSE;
    checkmouse( &MouseStatus, &MouseRow, &MouseCol, &MouseTime );
    uimousespeed( UIData->mouse_speed );
}

bool global initmouse( int install )
{
    struct  mouse_data      state;

    MouseInstalled = FALSE;
    if( install > 0 && installed( BIOS_MOUSE ) ) {
        if( install > 1 ) {
            if( MouseInt( 0, 0, 0, 0 ) != (mouse_t)-1 ) {
               install = 0; /* mouse initialization failed */
            }
        }
        if( install > 0 ) {
// UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY
            /*
             * Okay - I tried the mouse thing on both NECs. From the
             * differences in implementation, I think I found a way to
             * differentiate the two drivers.  It's ugly, but it might
             * work.  We do a checkmouse.  On the high res, ax always
             * equals 3.  On the low res, it indicates the status of the
             * left button, but is never three.  So we have a test....
             */
            MouseState( 3, (void near *)&state );
            if( state.ax == 3 ) {
                mouseType = _NEC_MOUSE_HIGH;
            } else {
                mouseType = _NEC_MOUSE_LOW;
            }
// UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY UGLY

            UIData->mouse_yscale = 1;
            UIData->mouse_xscale = 1;
            setupmouse();
        }
    }

    return( MouseInstalled );
}

bool global uiinitgmouse( int install )
{
    return( initmouse( install ) );
}


void global finimouse()
{
    if( MouseInstalled ) {
        uioffmouse();
    }
}

void global uifinigmouse()
{
    finimouse();
}

void global uisetmouseposn(             /* SET MOUSE POSITION */
    ORD row,                            /* - mouse row        */
    ORD col )                           /* - mouse column     */
{
    MouseRow = row;
    MouseCol = col;
    MouseInt( 4, 0, col * MOUSE_SCALE, row * MOUSE_SCALE );
}

int global FlipCharacterMap()
{
    return 0;
}
