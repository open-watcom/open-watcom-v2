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

#define         _swap(a,b)              {int i; i=a; a=b; b=i;}

static          int                     OldCursorAttr;
static          ORD                     OldCursorRow;
static          ORD                     OldCursorCol;
static          int                     OldCursorType;

/*
    [DH DL AH AL]
     S  E  C  T
     t  n  o  y
     a  d  d  p
     r     e  e
     t
*/

extern short BIOSSetCursorForm( unsigned long );
#pragma aux BIOSSetCursorForm = \
    "mov    ah,09h"             \
    "int    91h"                \
    parm    [dx ax]             \
    value   [ax];

extern unsigned long BIOSGetCursorForm( void );
#pragma aux BIOSGetCursorForm = \
    "mov    ah,0Ah"             \
    "int    91h"                \
    value   [dx ax];

extern void BIOSSetCursorOn( void );
#pragma aux BIOSSetCursorOn =   \
    "mov    ah,0Bh"             \
    "mov    al,00h"             \
    "int    91h"                \
    value   [ax];

extern void BIOSSetCursorOff( void );
#pragma aux BIOSSetCursorOff =  \
    "mov    ah,0Bh"             \
    "mov    al,01h"             \
    "int    91h"                \
    value   [ax];

extern char BIOSGetCursorState( void );
#pragma aux BIOSGetCursorState =  \
    "mov    ah,0Ch"             \
    "int    91h"                \
    value   [al];

extern void BIOSSetCursorPos( unsigned short );
#pragma aux BIOSSetCursorPos =  \
    "push   dx"                 \
    "mov    dx,ax"              \
    "mov    ah,0Dh"             \
    "int    91h"                \
    "pop    dx"                 \
    parm    [ax]                \
    value   [ax];

extern unsigned short BIOSGetCursorPos( void );
#pragma aux BIOSGetCursorPos =  \
    "push   dx"                 \
    "mov    ah,0Eh"             \
    "int    91h"                \
    "mov    ax,dx"              \
    "pop    dx"                 \
    value   [ax];

void global uioffcursor()
/***********************/
{

    if( UIData->cursor_on ) {
        BIOSSetCursorOff();
        UIData->cursor_on = FALSE;
    }
    UIData->cursor_type = C_OFF;
}


void global uioncursor()
/**********************/
{
    unsigned short  curpos;
    unsigned long   cursor_type;

    cursor_type = BIOSGetCursorForm();
    cursor_type = cursor_type & 0xffffff00;
    if( UIData->cursor_type == C_INSERT ) {
        cursor_type |= 1;
    }
    BIOSSetCursorForm( cursor_type );
    curpos = UIData->cursor_row << 8 | UIData->cursor_col;
    curpos += 0x0101;
    BIOSSetCursorPos( curpos );
    BIOSSetCursorOn();

//        if( UIData->cursor_attr != -2 ) {
//            /* get video state */
//            r.h.ah = 15;
//            int86( BIOS_VIDEO, &r, &r );
//            /* get current character and attribute */
//            r.h.ah = 8;
//            int86( BIOS_VIDEO, &r, &r );
//            /* write out the character and the new attribute */
//            r.h.bl = UIData->cursor_attr;
//            r.w.cx = 1;
//            r.h.ah = 9;
//            int86( BIOS_VIDEO, &r, &r );
//        }
    UIData->cursor_on = TRUE;
}

static void savecursor()
/**********************/
{
    unsigned short  curpos;
    unsigned long   cursor_type;

    curpos = BIOSGetCursorPos();
    OldCursorRow = (curpos >> 8) - 1;
    OldCursorCol = (curpos & 0xff) - 1;

    cursor_type = BIOSGetCursorForm();
    OldCursorAttr = cursor_type;
    if( cursor_type & 1 ) {
        OldCursorType = C_INSERT;
    } else {
        OldCursorType = C_NORMAL;
    }

    UIData->cursor_on = BIOSGetCursorState() ^ 1;
    if( UIData->cursor_on == FALSE ) {
        OldCursorType = C_OFF;
    }
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

register        ORD*                    row;
register        ORD*                    col;
register        int*                    type;
register        int*                    attr;
{
    unsigned short  curpos;
    unsigned long   cursor_type;

    curpos = BIOSGetCursorPos();
    *row = (curpos >> 8) - 1;
    *col = (curpos & 0xff) - 1;

    cursor_type = BIOSGetCursorForm();
    *attr = cursor_type;
    if( cursor_type & 1 ) {
        *type = C_INSERT;
    } else {
        *type = C_NORMAL;
    }
    if( BIOSGetCursorState() & 1 ) {
        *type = C_OFF;
    }
}

void global uisetcursor( row, col, typ, attr )
/********************************************/

register        ORD                     row;
register        ORD                     col;
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
