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
* Description:  Cursor style functions for RDOS
*
****************************************************************************/


#include <rdos.h>
#include "uidef.h"
#include "uiattrs.h"
#include "uicurshk.h"


#define _swap(t,a,b)    {t i; i=a; a=b; b=i;}

static CURSORORD        OldCursorRow;
static CURSORORD        OldCursorCol;
static CURSOR_TYPE      OldCursorType;

void UIHOOK uioffcursor( void )
{
    UIData->cursor_on = false;
    UIData->cursor_type = C_OFF;
}

void UIHOOK uioncursor( void )
{
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
    _swap( CURSOR_TYPE, UIData->cursor_type, OldCursorType );
    _swap( CURSORORD, UIData->cursor_col, OldCursorCol );
    _swap( CURSORORD, UIData->cursor_row, OldCursorRow );
    UIData->cursor_on = true;
}


void UIHOOK uigetcursor( CURSORORD *crow, CURSORORD *ccol, CURSOR_TYPE *ctype, CATTR *cattr )
{
    *crow = UIData->cursor_row;
    *ccol = UIData->cursor_col;
    *ctype = UIData->cursor_type;
    *cattr = CATTR_NONE;
}


void UIHOOK uisetcursor( CURSORORD crow, CURSORORD ccol, CURSOR_TYPE ctype, CATTR cattr )
{
    if( ( ctype != UIData->cursor_type ) ||
        ( crow != UIData->cursor_row ) ||
        ( ccol != UIData->cursor_col ) ||
        ( cattr != UIData->cursor_attr ) ) {
        UIData->cursor_type = ctype;
        UIData->cursor_row = crow;
        UIData->cursor_col = ccol;
        if( cattr != CATTR_OFF ) {
            UIData->cursor_attr = cattr;
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
    CATTR   cattr;

    UIData->cursor_row = CURSOR_INVALID;
    UIData->cursor_col = CURSOR_INVALID;
    UIData->cursor_type = C_OFF;
    uigetcursor( &OldCursorRow, &OldCursorCol, &OldCursorType, &cattr );
    UIData->cursor_on = true;
    uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, CATTR_NONE );
    uioffcursor();
}


void UIHOOK uifinicursor( void )
{
    uioncursor();
}
