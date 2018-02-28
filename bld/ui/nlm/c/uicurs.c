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


#include <conio.h>
#include "uidef.h"
#include "uiattrs.h"
#include "uinlm.h"


#define _swap(a,b)      {int i; i=a; a=b; b=i;}

/* NOTE:  Not sure about these constants.  Try and see! */

#define START_INSERT_CURSOR  7
#define END_INSERT_CURSOR   14
#define START_NORMAL_CURSOR 12
#define END_NORMAL_CURSOR   14

static CATTR            OldCursorAttr;
static WORD             OldCursorRow;
static WORD             OldCursorCol;
static CURSOR_TYPE      OldCursorType;

void UIAPI uioffcursor( void )
/****************************/
{
    if( UIData->cursor_on ) {
        HideInputCursor();
        UIData->cursor_on = false;
    }
    UIData->cursor_type = C_OFF;
}

void UIAPI uioncursor( void )
/***************************/
{
    BYTE startline;     /* first cursor scan line */
    BYTE endline;       /* last cursor scan line  */

    if( UIData->cursor_type == C_INSERT ) {
        startline = START_INSERT_CURSOR;
        endline   = END_INSERT_CURSOR;
    } else {
        startline = START_NORMAL_CURSOR;
        endline   = END_NORMAL_CURSOR;
    }

    /* A PROBLEM:  This sets the cursor shape of the NLM's screen, not the  */
    /* screen created in UIBios.C! Figure this out some time... until then, */
    /* the insert cursor will be small like the regular cursor. */

    SetCursorShape( startline, endline );

    SetPositionOfInputCursor( UIData->cursor_row, UIData->cursor_col );

//NYI ???    if( UIData->cursor_attr != CATTR_VOFF ) { }

    DisplayInputCursor();

    UIData->cursor_on = true;
}

static void newcursor( void )
/***************************/
{
    if( UIData->cursor_type == C_OFF ) {
        uioffcursor();
    } else {
        uioncursor();
    }
}

void UIAPI uigetcursor( ORD *row, ORD *col, CURSOR_TYPE *type, CATTR *attr )
/**************************************************************************/
{
    BYTE startline;
    BYTE endline;

    WORD roww, colw;

    /* unused parameters */ (void)attr;

    colw = wherex();
    roww = wherey();

    *row = roww;
    *col = colw;

    GetCursorShape( &startline, &endline );

    if( endline == END_INSERT_CURSOR && startline == START_INSERT_CURSOR ) {
        *type = C_INSERT;
    } else {
        *type = C_NORMAL;
    }

    if( !UIData->cursor_on ) {
        *type = C_OFF;
    }

    //NYI:  Read the attribute
}

void UIAPI uisetcursor( ORD row, ORD col, CURSOR_TYPE typ, CATTR attr )
/*********************************************************************/
{
    if( ( typ != UIData->cursor_type ) || ( row != UIData->cursor_row )  ||
        ( col != UIData->cursor_col )  || ( attr != UIData->cursor_attr ) ) {

        UIData->cursor_type = typ;
        UIData->cursor_row = row;
        UIData->cursor_col = col;
        if( attr != CATTR_OFF ) {
            UIData->cursor_attr = attr;
        }
        newcursor();
    }
}

static void savecursor( void )
/****************************/
{
    BYTE startline;
    BYTE endline;

    OldCursorCol = wherex();
    OldCursorRow = wherey();

    GetCursorShape( &startline, &endline );

    if( endline == END_INSERT_CURSOR && startline == START_INSERT_CURSOR ) {
        OldCursorType = C_INSERT;
    } else {
        OldCursorType = C_NORMAL;
    }

// NYI -- should I be setting UIData->cursor_on ??? How???

    if( !UIData->cursor_on ) {
         OldCursorType = C_OFF;
    }

// NYI:    OldCursorAttr = ???

}

static void swapcursor( void )
/****************************/
{
    _swap( UIData->cursor_type, OldCursorType );
    _swap( UIData->cursor_col,  OldCursorCol  );
    _swap( UIData->cursor_row,  OldCursorRow  );
    _swap( UIData->cursor_attr, OldCursorAttr );
    UIData->cursor_on = true;
}

void UIAPI uiswapcursor( void )
/*****************************/
{
    swapcursor();
    newcursor();
}

void UIAPI uiinitcursor( void )
/*****************************/
{
    savecursor();
    uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, OldCursorAttr );
    uioffcursor();
}

void UIAPI uifinicursor( void )
/*****************************/
{
    uioncursor();
}
