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
#include "uiattrs.h"
#include <windows.h>
#include "uicurshk.h"


#define _swap(a,b)      {int i; i=a; a=b; b=i;}

extern HANDLE           OutputHandle;

static CURSORORD        OldCursorRow;
static CURSORORD        OldCursorCol;
static CURSOR_TYPE      OldCursorType;

void UIHOOK uioffcursor( void )
{
    CONSOLE_CURSOR_INFO ci;

    if( UIData->cursor_on ) {
        ci.bVisible = true;
        ci.dwSize = 0;
        SetConsoleCursorInfo( OutputHandle, &ci );
        UIData->cursor_on = false;
    }
    UIData->cursor_type = C_OFF;
}


void UIHOOK uioncursor( void )
{
    CONSOLE_CURSOR_INFO ci;
    COORD               cpos;

    if( UIData->cursor_type == C_INSERT ) {
        ci.dwSize = 75;
    } else {
        ci.dwSize = 25;
    }
    ci.bVisible = true;
    SetConsoleCursorInfo( OutputHandle, &ci );

    cpos.X = UIData->cursor_col;
    cpos.Y = UIData->cursor_row;
    SetConsoleCursorPosition( OutputHandle, cpos );

    UIData->cursor_on = true;
}


void intern newcursor( void )
{
    if( UIData->cursor_type == C_OFF ) {
        uioffcursor();
    } else {
        uioncursor();
    }
}


static void swapcursor( void )
{
    _swap( UIData->cursor_type, OldCursorType );
    _swap( UIData->cursor_col, OldCursorCol );
    _swap( UIData->cursor_row, OldCursorRow );
    UIData->cursor_on = true;
}


void UIHOOK uigetcursor( CURSORORD *row, CURSORORD *col, CURSOR_TYPE *type, CATTR *attr )
{
    *row = UIData->cursor_row;
    *col = UIData->cursor_col;
    *type = UIData->cursor_type;
    *attr = 0;
}


void UIHOOK uisetcursor( CURSORORD row, CURSORORD col, CURSOR_TYPE typ, CATTR attr )
{
    if( ( typ != UIData->cursor_type ) ||
        ( row != UIData->cursor_row ) ||
        ( col != UIData->cursor_col ) ||
        ( attr != UIData->cursor_attr ) ) {
        UIData->cursor_type = typ;
        UIData->cursor_row = row;
        UIData->cursor_col = col;
        if( attr != CATTR_OFF ) {
            UIData->cursor_attr = attr;
        }
        newcursor();
    }
}

void UIHOOK uiswapcursor( void )
{
    swapcursor();
    newcursor();
}


void UIHOOK uiinitcursor( void )
{
    CATTR   tmp;

    UIData->cursor_row = CURSOR_INVALID;
    UIData->cursor_col = CURSOR_INVALID;
    UIData->cursor_type = C_OFF;
    uigetcursor( &OldCursorRow, &OldCursorCol, &OldCursorType, &tmp );
    UIData->cursor_on = true;
    uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, 0 );
    uioffcursor();
}


void UIHOOK uifinicursor( void )
{
    uioncursor();
}
