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


#include <stdlib.h>
#include <dos.h>
#include "uidos.h"
#include "uidef.h"
#include "uiattrs.h"
#include "biosui.h"

extern void _uifinicursor( void );
extern void _uiinitcursor( void );
extern void _uiswapcursor( void );
extern void _uisetcursor( ORD row, ORD col, int typ, int attr );
extern void _uioffcursor( void );
extern void _uioncursor( void );
extern void _uigetcursor( ORD *row, ORD *col, int *typ, int *attr );

// these function get overridden by the debugger if it's doing its own
// hardware cursor on a monochrome screen, but the debugger needs
// to call the original version if it's on the main screen, so I've changed
// the original versions into _uixxx().  In a normal UI app, these stubs kick
// in so the world is back to normal

void UIAPI uioffcursor( void )
{
    _uioffcursor();
}

void UIAPI uioncursor( void )
{
    _uioncursor();
}

void UIAPI uigetcursor( ORD *row, ORD *col, int *type, int *attr )
{
    _uigetcursor( row, col, type, attr );
}

void UIAPI uisetcursor( ORD row, ORD col, int typ, int attr ) {
    _uisetcursor( row, col, typ, attr );
}

void UIAPI uiswapcursor( void ) {
    _uiswapcursor();
}

void UIAPI uiinitcursor( void ) {
    _uiinitcursor();
}

void UIAPI uifinicursor( void ) {
    _uifinicursor();
}
