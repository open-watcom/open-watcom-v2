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
#include "uidos.h"
#include "uiattrs.h"
#include "uidbg.h"
#include "biosui.h"

#define         _swap(a,b)              {int i; i=a; a=b; b=i;}

#define         BIOS_CURSOR_OFF         0x20

static          int                     OldCursorAttr;
static          ORD                     OldCursorRow;
static          ORD                     OldCursorCol;
static          int                     OldCursorType;

void UIDBG _uioffcursor( void )
/******************************/
{
    union REGS      r;

    if( UIData->cursor_on ) {
        /* set OldCursor size */
        r.h.ah = 1;
        r.h.ch = BIOS_CURSOR_OFF;
        r.h.cl = 0;
        int86( BIOS_VIDEO, &r, &r );
        UIData->cursor_on = FALSE;
    }
    UIData->cursor_type = C_OFF;
}


void UIDBG _uioncursor( void )
/*****************************/
{
    union REGS      r;

    /* set OldCursor type */
    r.h.ah = 1;
    if( ( UIData->colour == M_CGA ) || ( UIData->colour == M_EGA ) ) {
        r.h.cl = 0x07;
    } else {
        r.h.cl = 0x0c;
    }
    if( UIData->cursor_type == C_INSERT ) {
        r.h.ch = (char) ( r.h.cl / 2 );
    } else {
        r.h.ch = (char) ( r.h.cl - 1 );
    }
    int86( BIOS_VIDEO, &r, &r );
    /* get video state */
    r.h.ah = 15;
    int86( BIOS_VIDEO, &r, &r );
    /* set OldCursor position */
    r.h.ah = 2;
    r.h.dh = (signed char) UIData->cursor_row;
    r.h.dl = (signed char) UIData->cursor_col;
    int86( BIOS_VIDEO, &r, &r );
    if( UIData->cursor_attr != -2 ) {
        /* get video state */
        r.h.ah = 15;
        int86( BIOS_VIDEO, &r, &r );
        /* get current character and attribute */
        r.h.ah = 8;
        int86( BIOS_VIDEO, &r, &r );
        /* write out the character and the new attribute */
        r.h.bl = UIData->cursor_attr;
        r.w.cx = 1;
        r.h.ah = 9;
        int86( BIOS_VIDEO, &r, &r );
    }
    UIData->cursor_on = TRUE;
}

static void savecursor( void )
/****************************/
{
    union REGS      r;

    /* get current video state */
    r.h.ah = 15;
    int86( BIOS_VIDEO, &r, &r );
    /* read OldCursor position */
    r.h.ah = 3;
    int86( BIOS_VIDEO, &r, &r );
    OldCursorRow = (ORD) r.h.dh;
    OldCursorCol = (ORD) r.h.dl;
    if( r.h.cl - r.h.ch > 1 ) {
        OldCursorType = C_INSERT;
    } else {
        OldCursorType = C_NORMAL;
    }
    UIData->cursor_on = ( ( r.h.ch & BIOS_CURSOR_OFF ) == 0 );
    if( UIData->cursor_on == FALSE ) {
        OldCursorType = C_OFF;
    }
    /* read character and attribute */
    r.h.ah = 8;
    int86( BIOS_VIDEO, &r, &r );
    OldCursorAttr = r.h.ah;
}


static void newcursor( void )
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
    _swap( UIData->cursor_type, OldCursorType );
    _swap( UIData->cursor_col, OldCursorCol );
    _swap( UIData->cursor_row, OldCursorRow );
    _swap( UIData->cursor_attr, OldCursorAttr );
    UIData->cursor_on = TRUE;
}


void UIDBG _uigetcursor( ORD *row, ORD *col, int *type, int *attr )
/*****************************************************************/
{
    union REGS      r;

    /* get current video state */
    r.h.ah = 15;
    int86( BIOS_VIDEO, &r, &r );
    /* read OldCursor position */
    r.h.ah = 3;
    int86( BIOS_VIDEO, &r, &r );
    *row = (ORD) r.h.dh;
    *col = (ORD) r.h.dl;
    if( r.h.cl - r.h.ch > 1 ) {
        *type = C_INSERT;
    } else {
        *type = C_NORMAL;
    }
    if( UIData->cursor_on == FALSE ) {
        *type = C_OFF;
    }
    /* read character and attribute */
    r.h.ah = 8;
    int86( BIOS_VIDEO, &r, &r );
    *attr = r.h.ah;
}


void UIDBG _uisetcursor( ORD row, ORD col, int typ, int attr )
/************************************************************/
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


void UIDBG _uiswapcursor( void )
/******************************/
{
    swapcursor();
    newcursor();
}


void UIDBG _uiinitcursor( void )
/******************************/
{
    savecursor();
    uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, OldCursorAttr );
    uioffcursor();
}


void UIDBG _uifinicursor( void )
/******************************/
{
    _uioncursor();
}
