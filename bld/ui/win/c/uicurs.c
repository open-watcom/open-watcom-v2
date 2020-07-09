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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdui.h>
#include "uicurshk.h"
#include "int10.h"


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

static int10_cursor_typ RegCur;
static int10_cursor_typ InsCur;

static CURSORORD    OldCursorRow;
static CURSORORD    OldCursorCol;
static CURSOR_TYPE  OldCursorTyp;

void UIHOOK uiinitcursor( void )
{
    OldCursorTyp = C_OFF;
    if( UIData->height == 25 ) {
        RegCur.value = MONO_CURSOR_ON;
    } else {
        RegCur.value = CGA_CURSOR_ON;
    }
    InsCur.s.bot_line = RegCur.s.bot_line;
    InsCur.s.top_line = ( RegCur.s.bot_line + 1 ) / 2;
}

void UIHOOK uisetcursor( CURSORORD crow, CURSORORD ccol, CURSOR_TYPE ctype, CATTR cattr )
{
    /* unused parameters */ (void)cattr;

    if( ctype == C_OFF ) {
        uioffcursor();
    } else {
        if( crow == OldCursorRow && ccol == OldCursorCol && ctype == OldCursorTyp )
            return;
        OldCursorTyp = ctype;
        OldCursorRow = crow;
        OldCursorCol = ccol;
        VIDSetPos( VIDPort, crow * UIData->width + ccol );
        VIDSetCurTyp( VIDPort, ( ctype == C_INSERT ) ? InsCur.value : RegCur.value );
    }
}


void UIHOOK uioffcursor( void )
{
    OldCursorTyp = C_OFF;
    VIDSetCurTyp( VIDPort, NoCur );
}

void UIHOOK uiswapcursor( void )
{
}

void UIHOOK uifinicursor( void )
{
}
