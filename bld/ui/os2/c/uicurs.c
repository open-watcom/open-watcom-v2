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
#include "uidef.h"
#include "uiattrs.h"
#include "doscall.h"

#define         _swap(a,b)              {int i; i=a; a=b; b=i;}

static          int                     OldCursorAttr;
static          unsigned                OldCursorRow;
static          unsigned                OldCursorCol;
static          int                     OldCursorType;
static          USHORT                  length;
static          USHORT                  r;
static          USHORT                  c;


void global uioffcursor()
/***********************/
{
    VIOCURSORINFO vioCursor;

    if( UIData->cursor_on ) {
        /* set cursor size */
            VioGetCurType( &vioCursor, 0 );

           vioCursor.attr   = (USHORT)-1;        //invisible

           VioSetCurType(&vioCursor,0);

        UIData->cursor_on = FALSE;
    }
    UIData->cursor_type = C_OFF;
}



void global uioncursor()
/**********************/
{
    unsigned char       CharCellPair[2];
    VIOCURSORINFO       vioCursor;


    VioGetCurType( &vioCursor, 0 );
    if( UIData->cursor_type == C_INSERT ) {
        vioCursor.yStart = vioCursor.cEnd / 2;
    } else {
        vioCursor.yStart = (vioCursor.cEnd * 7) / 8;
    }
    vioCursor.cx = 1;
    vioCursor.attr = 1;

    VioSetCurType(&vioCursor, 0);


    /* set cursor position */

    VioSetCurPos( UIData->cursor_row, UIData->cursor_col, 0);

    if( UIData->cursor_attr != -2 ) {
    /* get current character and attribute */

        VioGetCurPos( &r, &c, 0 );
        length = sizeof(CharCellPair);
        VioReadCellStr(&CharCellPair[0],
                       &length,
                       UIData->cursor_row,
                       UIData->cursor_col,
                       0);

        /* write out the character and the new attribute */
        CharCellPair[1] = UIData->cursor_attr;
        VioWrtNCell(&CharCellPair[0],
                    24,
                    UIData->cursor_row,
                    UIData->cursor_col,
                    0);

    }
    UIData->cursor_on = TRUE;
}


static void savecursor()
/**********************/
{
    unsigned char       CharCellPair[2];
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
    UIData->cursor_on = (  vioCursor.attr != (USHORT) -1 );
    if( UIData->cursor_on == FALSE ) {
         OldCursorType = C_OFF;
    }
    /* read character and attribute */
    length = sizeof(CharCellPair);
    VioReadCellStr( &CharCellPair[0], &length, r, c, 0);
    OldCursorAttr = CharCellPair[1];
}


static void newcursor()
/*********************/
{
    if( UIData->cursor_type == C_OFF ) {
        uioffcursor();
    } else {
        uioncursor();
    }
}


static void swapcursor()
/**********************/
{
    _swap( UIData->cursor_type, OldCursorType );
    _swap( UIData->cursor_col, OldCursorCol );
    _swap( UIData->cursor_row, OldCursorRow );
    _swap( UIData->cursor_attr, OldCursorAttr );
    UIData->cursor_on = TRUE;
}


void global uigetcursor( row, col, type, attr )
/*********************************************/
    ORD*             row;
    ORD*             col;
    int*             type;
    int*             attr;
{

    USHORT              length;
    unsigned char       CharCellPair[2];
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
    if( UIData->cursor_on == FALSE ) {
        *type = C_OFF;
    }
    /* read character and attribute */
    length = sizeof(CharCellPair);
    VioReadCellStr( &CharCellPair[0], &length, r, c, 0);
    *attr = CharCellPair[1];
}


void global uisetcursor( row, col, typ, attr )
/********************************************/

register        unsigned                row;
register        unsigned                col;
register        int                     typ;
register        int                     attr;
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


void global uiswapcursor()
/************************/
{
    swapcursor();
    newcursor();
}


void global uiinitcursor()
/************************/
{
    savecursor();
    uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, OldCursorAttr );
    uioffcursor();
}


void global uifinicursor()
/************************/
{
    uioncursor();
}
