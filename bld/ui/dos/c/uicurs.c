/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
#include "uicurshk.h"
#include "uicurshk.h"
#include "int10.h"


#define _swap(t,a,b)    {t i; i=a; a=b; b=i;}

static CATTR            OldCursorAttr;
static CURSORORD        OldCursorRow;
static CURSORORD        OldCursorCol;
static CURSOR_TYPE      OldCursorType;

void UIHOOK _uioffcursor( void )
/******************************/
{
    int10_cursor_typ    ct;

    if( UIData->cursor_on ) {
        /* set OldCursor size */
        ct.value = NORM_CURSOR_OFF;
        _BIOSVideoSetCursorTyp( ct );
        UIData->cursor_on = false;
    }
    UIData->cursor_type = C_OFF;
}


void UIHOOK _uioncursor( void )
/*****************************/
{
    int10_mode_info     info;
    int10_cursor        c;
    int10_pixel_data    cursor_pixel;

    if( ( UIData->colour == M_CGA ) || ( UIData->colour == M_EGA ) ) {
        c.typ.value = CGA_CURSOR_ON;
    } else {
        c.typ.value = MONO_CURSOR_ON;
    }
    if( UIData->cursor_type == C_INSERT ) {
        c.typ.s.top_line = ( c.typ.s.bot_line + 1 ) / 2;
    }
    _BIOSVideoSetCursorTyp( c.typ );
    info = _BIOSVideoGetModeInfo();
    c.pos.s.row = UIData->cursor_row;
    c.pos.s.col = UIData->cursor_col;
    _BIOSVideoSetCursorPos( info.page, c.pos );
    if( UIData->cursor_attr != CATTR_VOFF ) {
        /* get current character and attribute */
        cursor_pixel = _BIOSVideoGetCharPixel( info.page );
        /* write out the character and the new attribute */
        cursor_pixel.s.attr = UIData->cursor_attr;
        _BIOSVideoSetCharPixel( info.page, cursor_pixel );
    }
    UIData->cursor_on = true;
}

static void savecursor( void )
/****************************/
{
    int10_mode_info     info;
    int10_cursor        c;

    info = _BIOSVideoGetModeInfo();
    c = _BIOSVideoGetCursor( info.page );
    OldCursorRow = c.pos.s.row;
    OldCursorCol = c.pos.s.col;
    if( c.typ.s.bot_line - c.typ.s.top_line > 1 ) {
        OldCursorType = C_INSERT;
    } else {
        OldCursorType = C_NORMAL;
    }
    UIData->cursor_on = ( (c.typ.s.top_line & CURSOR_INVISIBLE) == 0 );
    if( !UIData->cursor_on ) {
        OldCursorType = C_OFF;
    }
    OldCursorAttr = _BIOSVideoGetAttr( info.page );
}


void intern newcursor( void )
/***************************/
{
    if( UIData->cursor_type == C_OFF ) {
        uioffcursor();
    } else {
        _uioncursor();
    }
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


void UIHOOK _uigetcursor( CURSORORD *crow, CURSORORD *ccol, CURSOR_TYPE *ctype, CATTR *cattr )
/********************************************************************************************/
{
    int10_mode_info     info;
    int10_cursor        c;

    info = _BIOSVideoGetModeInfo();
    c = _BIOSVideoGetCursor( info.page );
    *crow = c.pos.s.row;
    *ccol = c.pos.s.col;
    if( c.typ.s.bot_line - c.typ.s.top_line > 1 ) {
        *ctype = C_INSERT;
    } else {
        *ctype = C_NORMAL;
    }
    if( !UIData->cursor_on ) {
        *ctype = C_OFF;
    }
    *cattr = _BIOSVideoGetAttr( info.page );
}


void UIHOOK _uisetcursor( CURSORORD crow, CURSORORD ccol, CURSOR_TYPE ctyp, CATTR cattr )
/***************************************************************************************/
{
    if( ( ctyp != UIData->cursor_type )
      || ( crow != UIData->cursor_row )
      || ( ccol != UIData->cursor_col )
      || ( cattr != UIData->cursor_attr ) ) {
        UIData->cursor_type = ctyp;
        UIData->cursor_row = crow;
        UIData->cursor_col = ccol;
        if( cattr != CATTR_OFF ) {
            UIData->cursor_attr = cattr;
        }
        newcursor();
    }
}


void UIHOOK _uiswapcursor( void )
/*******************************/
{
    swapcursor();
    newcursor();
}


void UIHOOK _uiinitcursor( void )
/*******************************/
{
    savecursor();
    uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, OldCursorAttr );
    uioffcursor();
}


void UIHOOK _uifinicursor( void )
/*******************************/
{
    _uioncursor();
}
