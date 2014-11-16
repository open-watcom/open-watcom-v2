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
#include <stdlib.h>
#include <dos.h>
#include "uidef.h"
#include "uiattrs.h"

#define         _swap(a,b)              {int i; i=a; a=b; b=i;}

static ORD      OldCursorRow;
static ORD      OldCursorCol;
static int      OldCursorType;

void UIAPI uioffcursor( void )
{
    UIData->cursor_on = FALSE;
    UIData->cursor_type = C_OFF;
}

void UIAPI uioncursor( void )
{
    UIData->cursor_on = TRUE;
}


static void newcursor( void )
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
    UIData->cursor_on = TRUE;
}


void UIAPI uigetcursor( ORD *row, ORD *col, int *type, int *attr )
{
    *row = UIData->cursor_row;
    *col = UIData->cursor_col;
    *type = UIData->cursor_type;
    *attr = 0;
}


void UIAPI uisetcursor( ORD row, ORD col, int typ, int attr )
{
    if( ( typ != UIData->cursor_type ) ||
        ( row != UIData->cursor_row ) ||
        ( col != UIData->cursor_col ) ||
        ( attr != UIData->cursor_attr ) ) {
        UIData->cursor_type = typ;
        UIData->cursor_row = row;
        UIData->cursor_col = col;
        if( attr != -1 ) {
            UIData->cursor_attr = attr;
        }
        newcursor();
    }
}

void UIAPI uiswapcursor( void )
{
    swapcursor();
    newcursor();
}


void UIAPI uiinitcursor( void )
{
    int tmp;

    UIData->cursor_row = (ORD)-1;
    UIData->cursor_col = (ORD)-1;
    UIData->cursor_type = C_OFF;
    uigetcursor( &OldCursorRow, &OldCursorCol, &OldCursorType, &tmp );
    UIData->cursor_on = TRUE;
    uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, 0 );
    uioffcursor();
}


void UIAPI uifinicursor( void )
{
    uioncursor();
}
