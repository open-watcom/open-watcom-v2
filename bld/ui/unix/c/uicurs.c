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
#include "uidef.h"
#include "uiattrs.h"
#include "uivirt.h"
#define         _swap(a,b)              {int i; i=a; a=b; b=i;}

static          ORD                     OldCursorRow;
static          ORD                     OldCursorCol;
static          int                     OldCursorType;

void global uioffcursor( void )
/*****************************/
{
    UIData->cursor_type = C_OFF;
    if( UIData->cursor_on ) {
        UIData->cursor_on = FALSE;
        _physupdate( NULL );
    }
}


void global uioncursor( void )
/****************************/
{
    if( !UIData->cursor_on ) {
        UIData->cursor_on = TRUE;
        _physupdate( NULL );
    }
}


static void savecursor( void )
/****************************/
{
    int dummy;
    _uigetcursor( &OldCursorRow, &OldCursorCol, &OldCursorType, &dummy );
    UIData->cursor_on = TRUE;
}


void newcursor( void )
/********************/
{
    if( UIData->cursor_type == C_OFF ) {
        uioffcursor();
    } else {
        uioncursor();
    }
}


static void swapcursor( void )
/****************************/
{
    _swap( UIData->cursor_type, OldCursorType );
    _swap( UIData->cursor_col, OldCursorCol );
    _swap( UIData->cursor_row, OldCursorRow );
    UIData->cursor_on = TRUE;
}


void global uigetcursor( register       ORD     *row,
                         register       ORD     *col,
                         register       int     *type,
                         register       int     *attr )
/*****************************************************/
{
    _uigetcursor(row, col, type, attr);
}


void global uisetcursor( register       ORD     row,
                         register       ORD     col,
                         register       int     typ,
                         register       int     attr )
/****************************************************/
{
    _uisetcursor(row, col, typ, attr);
}


void global uiswapcursor( void )
/******************************/
{
    swapcursor();
    newcursor();
}


void global uiinitcursor( void )
/******************************/
{
    UIData->cursor_row = (ORD)-1;
    UIData->cursor_col = (ORD)-1;
    UIData->cursor_type = C_OFF;
    savecursor();
    _uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, 0 );
    uioffcursor();
}


void global uifinicursor( void )
/******************************/
{
    UIData->cursor_row = 0;
    UIData->cursor_col = 0;
    UIData->cursor_type = C_NORMAL;
    _physupdate( NULL );
}
