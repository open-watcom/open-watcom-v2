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
#include "uicurshk.h"


#define _swap(t,a,b)    {t i; i=a; a=b; b=i;}

/* NOTE:  Not sure about these constants.  Try and see! */

#define START_INSERT_CURSOR  7
#define END_INSERT_CURSOR   14
#define START_NORMAL_CURSOR 12
#define END_NORMAL_CURSOR   14

static CATTR            OldCursorAttr;
static CURSORORD        OldCursorRow;
static CURSORORD        OldCursorCol;
static CURSOR_TYPE      OldCursorType;

void UIHOOK uioffcursor( void )
/*****************************/
{
    if( UIData->cursor_on ) {
        HideInputCursor();
        UIData->cursor_on = false;
    }
    UIData->cursor_type = C_OFF;
}

void UIHOOK uioncursor( void )
/****************************/
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

void intern newcursor( void )
/***************************/
{
    if( UIData->cursor_type == C_OFF ) {
        uioffcursor();
    } else {
        uioncursor();
    }
}

void UIHOOK uigetcursor( CURSORORD *crow, CURSORORD *ccol, CURSOR_TYPE *ctype, CATTR *cattr )
/*******************************************************************************************/
{
    BYTE startline;
    BYTE endline;

    /* unused parameters */ (void)cattr;

    *crow = wherey();
    *ccol = wherex();

    GetCursorShape( &startline, &endline );

    if( endline == END_INSERT_CURSOR && startline == START_INSERT_CURSOR ) {
        *ctype = C_INSERT;
    } else {
        *ctype = C_NORMAL;
    }

    if( !UIData->cursor_on ) {
        *ctype = C_OFF;
    }

    //NYI:  Read the attribute
}

void UIHOOK uisetcursor( CURSORORD crow, CURSORORD ccol, CURSOR_TYPE ctype, CATTR cattr )
/***************************************************************************************/
{
    if( ( ctype != UIData->cursor_type ) || ( crow != UIData->cursor_row ) ||
        ( ccol != UIData->cursor_col ) || ( cattr != UIData->cursor_attr ) ) {

        UIData->cursor_type = ctype;
        UIData->cursor_row = crow;
        UIData->cursor_col = ccol;
        if( cattr != CATTR_OFF ) {
            UIData->cursor_attr = cattr;
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
    _swap( CURSOR_TYPE, UIData->cursor_type, OldCursorType );
    _swap( CURSORORD, UIData->cursor_col, OldCursorCol );
    _swap( CURSORORD, UIData->cursor_row, OldCursorRow );
    _swap( CATTR, UIData->cursor_attr, OldCursorAttr );
    UIData->cursor_on = true;
}

void UIHOOK uiswapcursor( void )
/******************************/
{
    swapcursor();
    newcursor();
}

void UIHOOK uiinitcursor( void )
/******************************/
{
    savecursor();
    uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, OldCursorAttr );
    uioffcursor();
}

void UIHOOK uifinicursor( void )
/******************************/
{
    uioncursor();
}
