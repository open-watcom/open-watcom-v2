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
* Description:  DOS mouse input handling.
*
****************************************************************************/


#include <dos.h>
#include "uidos.h"
#include "uidef.h"
#include "biosui.h"
#include "charmap.h"

extern  void    (intern *DrawCursor)( void );
        unsigned short  Points;                 /* Number of lines / char  */
extern int uimousealign( void );

struct mouse_data {
    unsigned short    bx,cx,dx;
};

typedef struct mouse_data __based( __segname( "_STACK" ) ) *md_stk_ptr;
#pragma aux MouseInt2 = 0xcd BIOS_MOUSE parm [ax] [cx] [dx] [si] [di];
extern void MouseInt2( unsigned short, unsigned short,
                       unsigned short, unsigned short, unsigned short );

#ifndef __386__

#pragma aux MouseState = 0xcd BIOS_MOUSE \
                        0x36 0x89 0x1c   \
                        0x36 0x89 0x4c 0x02 \
                        0x36 0x89 0x54 0x04 \
                        parm [ax] [si] modify [bx cx dx];
extern void MouseState( unsigned, md_stk_ptr );

#else

#pragma aux MouseState = 0xcd BIOS_MOUSE \
                        0x36 0x66 0x89 0x1e   \
                        0x36 0x66 0x89 0x4e 0x02 \
                        0x36 0x66 0x89 0x56 0x04 \
                        parm [ax] [esi] modify [bx cx dx];
extern void MouseState( unsigned short, md_stk_ptr );

#endif

extern unsigned long uiclock( void );

#define         MOUSE_SCALE             8

extern          MOUSEORD                MouseRow;
extern          MOUSEORD                MouseCol;

/* MickeyRow and MickeyCol are accurate under DOS and OS2's DOS */
static          int                     MickeyRow;
static          int                     MickeyCol;

extern          bool                    MouseOn;

extern          unsigned long           MouseTime       = 0L;

extern          unsigned short          MouseStatus;
extern          bool                    MouseInstalled;

void intern checkmouse( unsigned short *status, MOUSEORD *row,
                          MOUSEORD *col, unsigned long *time )
/************************************************************/
{
    struct  mouse_data state;
    char    change;

    change = change;
    MouseState( 3, (md_stk_ptr)&state );

    *status = state.bx;

    if( DrawCursor==NULL ) {
        *col = state.cx/MOUSE_SCALE;
        *row = state.dx/MOUSE_SCALE;
    } else {
        MouseState( 0x0B, (md_stk_ptr)&state );
        MickeyCol += (short int ) state.cx; /* delta of mickeys */
        MickeyRow += (short int ) state.dx; /* delta of mickeys */
        if( MickeyRow < 0 ) {
            MickeyRow = 0;
            change = TRUE;
        } else {
            /* -2 makes sure one pixel of mouse shows */
            if( MickeyRow > ( UIData->height * Points - 2 ) ) {
                MickeyRow = UIData->height * Points - 2;
                change = TRUE;
            } else {
                change = FALSE;
            }
        }
        if( MickeyCol < 0 ) {
            MickeyCol = 0;
            change = TRUE;
        } else {
            /* -2 makes sure one pixel of mouse shows */
            if( MickeyCol > ( UIData->width * MOUSE_SCALE - 2 ) ) {
                MickeyCol = UIData->width * MOUSE_SCALE - 2;
                change = TRUE;
            } else {
                change = FALSE;
            }
        }
        *row = MickeyRow;
        *col = MickeyCol;
        if( change ) {
            MouseInt( 4, 0, *col, *row );
            MouseInt( 0x0B, 0, 0, 0 );
        }
    }

    *time = uiclock();
    uisetmouse( *row, *col );
    *col += uimousealign();
}


void uimousespeed( unsigned speed )
/*********************************/
/* set speed of mouse. 0 is fastest; the higher the number the slower
   it goes */
{
    if( (int)speed <= 0 ) {
        speed = 1;
    }

    MouseInt2( 15, speed, speed * 2, 0, 0 );
    UIData->mouse_speed = speed;
}

void intern setupmouse( void )
{
    int     dx;

    if( DrawCursor==NULL ) {
        dx = ( UIData->width - 1 )*MOUSE_SCALE;
    } else {
        dx =   UIData->width * MOUSE_SCALE - 1;
    }
    MouseInt( 7, 0, 0, dx );

    if( DrawCursor==NULL ) {
        dx = ( UIData->height - 1 )*MOUSE_SCALE;
    } else {
        dx = UIData->height * Points - 1;
    }
    MouseInt( 8, 0, 0, dx );

    uisetmouseposn( UIData->height/2 - 1, UIData->width/2 - 1 );
    MouseInstalled = TRUE;
    MouseOn = FALSE;
    UIData->mouse_swapped = FALSE;
    checkmouse( &MouseStatus, &MouseRow, &MouseCol, &MouseTime );
    if( DrawCursor != NULL ) UIData->mouse_speed /= 2;
    uimousespeed( UIData->mouse_speed );
}

bool global initmouse( int install )
/**********************************/
{
    MouseInstalled = FALSE;
    if( install > 0 && installed( BIOS_MOUSE ) ) {
        if( install > 1 ) {
            if( MouseInt( 0, 0, 0, 0 ) != -1 ) {
               install = 0; /* mouse initialization failed */
            }
        }
        if( install > 0 ) {
            UIData->mouse_yscale = 1;
            UIData->mouse_xscale = 1;
            setupmouse();
        }
    }
    return( MouseInstalled );
}


void global finimouse( void )
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
    MouseRow = row * UIData->mouse_yscale;
    MouseCol = col * UIData->mouse_xscale;
    if( DrawCursor==NULL ) {
        MouseInt( 4, 0, col * MOUSE_SCALE, row * MOUSE_SCALE );
    } else {
        MouseInt( 4, 0, MouseCol, MouseRow );
        MouseInt( 0x0B, 0, 0, 0 );
        MickeyRow = MouseRow; /* initialize these and syncronize the INT B */
        MickeyCol = MouseCol; /* because we keep a running total */
    }
}
