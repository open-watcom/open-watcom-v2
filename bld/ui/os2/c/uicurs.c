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
#include "doscall.h"


#define _swap(a,b)      {int i; i=a; a=b; b=i;}

static CATTR            OldCursorAttr;
static unsigned         OldCursorRow;
static unsigned         OldCursorCol;
static CURSOR_TYPE      OldCursorType;
static USHORT           length;
static USHORT           r;
static USHORT           c;

void UIAPI uioffcursor( void )
/*****************************/
{
    VIOCURSORINFO vioCursor;

    if( UIData->cursor_on ) {
        /* set cursor size */
        VioGetCurType( &vioCursor, 0 );
        vioCursor.attr = (USHORT)CATTR_OFF;        //invisible
        VioSetCurType( &vioCursor, 0 );
        UIData->cursor_on = false;
    }
    UIData->cursor_type = C_OFF;
}



void UIAPI uioncursor( void )
/****************************/
{
    CHAR                CharCellPair[2];
    VIOCURSORINFO       vioCursor;


    /* set cursor type */
    VioGetCurType( &vioCursor, 0 );
    if( UIData->cursor_type == C_INSERT ) {
        vioCursor.yStart = vioCursor.cEnd / 2;
    } else {
        vioCursor.yStart = ( vioCursor.cEnd * 7 ) / 8;
    }
    vioCursor.cx = 1;
    vioCursor.attr = 1;
    VioSetCurType( &vioCursor, 0 );

    /* set cursor position */
    VioSetCurPos( UIData->cursor_row, UIData->cursor_col, 0);

    if( UIData->cursor_attr != CATTR_VOFF ) {
        /* get current character and attribute */
        VioGetCurPos( &r, &c, 0 );
        length = sizeof( CharCellPair );
        VioReadCellStr( &CharCellPair[0], &length, UIData->cursor_row, UIData->cursor_col, 0 );

        /* write out the character and the new attribute */
        CharCellPair[1] = UIData->cursor_attr;
        VioWrtNCell( (PBYTE)&CharCellPair[0], 24, UIData->cursor_row, UIData->cursor_col, 0 );
    }
    UIData->cursor_on = true;
}


static void savecursor( void )
/****************************/
{
    CHAR                CharCellPair[2];
    USHORT              length;
    VIOCURSORINFO       vioCursor;

    /* read cursor position */

    VioGetCurType(&vioCursor,0);
    VioGetCurPos(&r, &c, 0);
         OldCursorRow = r;
         OldCursorCol = c;
    if( vioCursor.cEnd - vioCursor.yStart > 5 ) {
         OldCursorType = C_INSERT;
    }
     else {
         OldCursorType = C_NORMAL;
    }
    UIData->cursor_on = ( vioCursor.attr != (USHORT)CATTR_OFF );
    if( !UIData->cursor_on ) {
         OldCursorType = C_OFF;
    }
    /* read character and attribute */
    length = sizeof(CharCellPair);
    VioReadCellStr( &CharCellPair[0], &length, r, c, 0);
    OldCursorAttr = CharCellPair[1];
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


static void swapcursor( void )
/****************************/
{
    _swap( UIData->cursor_type, OldCursorType );
    _swap( UIData->cursor_col, OldCursorCol );
    _swap( UIData->cursor_row, OldCursorRow );
    _swap( UIData->cursor_attr, OldCursorAttr );
    UIData->cursor_on = true;
}


void UIAPI uigetcursor( ORD *row, ORD *col, CURSOR_TYPE *type, CATTR *attr )
/**************************************************************************/
{

    USHORT              length;
    CHAR                CharCellPair[2];
    VIOCURSORINFO       vioCursor;

    /* read cursor position */
    VioGetCurType(&vioCursor,0);
    VioGetCurPos( &r, &c, 0 );
    *row = r;
    *col = c;
    if( vioCursor.cEnd - vioCursor.yStart > 5 ) {
        *type = C_INSERT;
    } else {
        *type = C_NORMAL;
    }
    if( !UIData->cursor_on ) {
        *type = C_OFF;
    }
    /* read character and attribute */
    length = sizeof(CharCellPair);
    VioReadCellStr( &CharCellPair[0], &length, r, c, 0);
    *attr = CharCellPair[1];
}


void UIAPI uisetcursor( ORD row, ORD col, CURSOR_TYPE typ, CATTR attr )
/*********************************************************************/
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


void UIAPI uiswapcursor( void )
/******************************/
{
    swapcursor();
    newcursor();
}


void UIAPI uiinitcursor( void )
/******************************/
{
    savecursor();
    uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, OldCursorAttr );
    uioffcursor();
}


void UIAPI uifinicursor( void )
/******************************/
{
    uioncursor();
}
