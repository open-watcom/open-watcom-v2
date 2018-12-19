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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdui.h>
#include "biosui.h"


#define NoCur   0x2000      /* outside screen */

extern void VIDSetPos( unsigned, unsigned );
#pragma aux VIDSetPos = \
        "mov  al,0fh"   \
        "out  dx,al"    \
        "inc  dx"       \
        "mov  al,bl"    \
        "out  dx,al"    \
        "dec  dx"       \
        "mov  al,0eh"   \
        "out  dx,al"    \
        "inc  dx"       \
        "mov  al,bh"    \
        "out  dx,al"    \
    __parm __caller [__dx] [__bx] \
    __value         \
    __modify        [__ax]

extern void VIDSetCurTyp( unsigned, unsigned );
#pragma aux VIDSetCurTyp = \
        "push ax"       \
        "mov  al,0ah"   \
        "out  dx,al"    \
        "inc  dx"       \
        "mov  al,ah"    \
        "out  dx,al"    \
        "dec  dx"       \
        "mov  al,0bh"   \
        "out  dx,al"    \
        "inc  dx"       \
        "pop  ax"       \
        "out  dx,al"    \
    __parm __caller [__dx] [__ax] \
    __value         \
    __modify        []

extern unsigned VIDGetCurTyp( unsigned );
#pragma aux VIDGetCurTyp = \
        "mov  al,0bh"   \
        "out  dx,al"    \
        "inc  dx"       \
        "in   al,dx"    \
        "mov  ah,al"    \
        "dec  dx"       \
        "mov  al,0ah"   \
        "out  dx,al"    \
        "inc  dx"       \
        "in   al,dx"    \
        "xchg ah,al"    \
    __parm __caller [__dx] \
    __value         [__ax] \
    __modify        []


unsigned        VIDPort = VIDMONOINDXREG;

static unsigned RegCur;
static unsigned InsCur;

static char         OldRow, OldCol;
static CURSOR_TYPE  OldTyp;

void uiinitcursor( void )
{
    OldTyp = C_OFF;
    if( UIData->height == 25 ) {
        RegCur = 0x0b0c;
    } else {
        RegCur = 0x0607;
    }
    InsCur = ( ((RegCur + 0x100) >> 1 & 0xFF00) + 0x100 ) | ( RegCur & 0x00FF );
}

void uisetcursor( ORD row, ORD col, CURSOR_TYPE typ, CATTR attr )
{
    /* unused parameters */ (void)attr;

    if( typ == C_OFF ) {
        uioffcursor();
    } else {
        if( row == OldRow && col == OldCol && typ == OldTyp )
            return;
        OldTyp = typ;
        OldRow = row;
        OldCol = col;
        VIDSetPos( VIDPort, row * UIData->width + col );
        VIDSetCurTyp( VIDPort, typ == C_INSERT ? InsCur : RegCur );
    }
}


void uioffcursor( void )
{
    OldTyp = C_OFF;
    VIDSetCurTyp( VIDPort, NoCur );
}

void uiswapcursor( void )
{
}

void uifinicursor( void )
{
}
