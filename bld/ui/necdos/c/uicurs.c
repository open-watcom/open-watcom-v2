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
#include "uidos.h"
#include "uidef.h"
#include "uiattrs.h"
#include "biosui.h"

extern unsigned inp(unsigned __port);
extern unsigned outp(unsigned __port, unsigned __value);

#define         _swap(a,b)              {int i; i=a; a=b; b=i;}

#define         NEC_BLINK_CURSOR        0x00
#define         NEC_NONE_BLINK_CURSOR   0x01

static          int                     OldCursorAttr;
static          ORD                     OldCursorRow;
static          ORD                     OldCursorCol;
static          int                     OldCursorType;

void global uioffcursor()
/***********************************************************************
 uioffcursor:
 -----------
    Input:      NONE
    Output:     NONE

    This function terminate cursor display if cursor is on and set
    some parmeters in UIData for the future use
************************************************************************/
{
        if( UIData->cursor_on ) {
            /* terminate cursor display */
            BIOSSetCursorOff();

            UIData->cursor_on = FALSE;
        }
        UIData->cursor_type = C_OFF;
}


void global uioncursor()
/***********************************************************************
 uioncursor:
 ----------
    Input:      NONE
    Output:     NONE

    This function display cursor if cursor is not on and set
    some parmeters in UIData for the future use
************************************************************************/
{
    int   CurPos;
        /* NEC automatically defines the size of the cursor box and
           the blinking speed so only thing we can set using bios call
           is whether to have blinking cursor or non-blinking cursor
        */
        BIOSSetCursorType( NEC_BLINK_CURSOR );

        /* Set cursor position, CurPos is a byte offset from 0xA000.
           Because there is a cap between first and second page we have to
           add some value to the offset of cursor position w.r.t 0xA000.
           For 25 line mode the cap is 96 bytes but how about other modes ??
           Currently I am adding 256 bytes as DOS does.
        */
        CurPos = (UIData->cursor_row * UIData->width + UIData->cursor_col) * 2;
        if ( CurPos >= ( UIData->height * UIData->width * 2 ) ) {
            if ( UIData->height == 25 ) {
                CurPos += 96;
            } else {
                CurPos += 256;
            }
        }

        if( CurrentPageNum() == 0 ) {
            BIOSSetCurPos( CurPos );
        } else {
            BIOSSetCurPos( 0x1000+CurPos );
        }
//      BIOSSetCurPos( CurPos );

        /* show cursor */
        BIOSSetCursorOn();

        UIData->cursor_on = TRUE;
}


static void GetGdcCursor( int *CurPos )
/***********************************************************************
 GetGdcCursor: ( STATIC )
 ------------
    Input:      &CurPos : pointer to CurPos
    Output:     NONE

    This function uses NEC GDC to get current cursor location regardless
    of current page and put the value at input position.
************************************************************************/
{
    int     i;
    short   ead;

    for(;;) {                                   /* read gdc status register */
        GdcMask();                              /* disable interrupts */
        if( (inp( 0x60 ) & 0x04) ) break;       /* quit if fifo empty */
        GdcDelay();
        GdcUnMask();                            /* restore interrupts */
    }
    GdcDelay();
    outp( 0x62, 0xE0 );                         /* gdc CURD command */
    for(;;) {                                   /* read gdc status register */
        GdcDelay();
        if( (inp( 0x60 ) & 0x01) ) break;       /* quit if data ready */
    }
    GdcDelay();
    ead = inp( 0x62 );                          /* read gdc EAD low */
    GdcDelay();
    ead |= (inp( 0x62 ) << 8);                  /* read gdc EAD high */
    for( i = 0; i < 3; i++ ) {                  /* skip next 3 bytes includ'g */
        GdcDelay();                             /* 2 highest bits of EAD */
        inp( 0x62 );                            /* read gdc */
    }
    GdcUnMask();                                /* restore interrupts */

    /* change to relative address from 0xA000 */
    *CurPos = ead * 2;
}


static void getcursorloc( ORD *row, ORD *col )
/***********************************************************************
 getcursorloc:
 ------------
    Input:      fprow, fpcol
    Output:     fprow, fpcol

    get far pointer to row and col location and fill in the blank
************************************************************************/
{
    int     CurPos;

    GetGdcCursor( &CurPos );

    *row = CurPos / ( UIData->width * 2 );
    *col = ( CurPos - ( *row * UIData->width * 2 ) ) / 2;
}


static void savecursor()
/***********************************************************************
 savecursor:
 ----------
    Input:      NONE
    Output:     NONE

    get current cursor informations and store into local variables
************************************************************************/
{
    getcursorloc( &OldCursorRow, &OldCursorCol );
    OldCursorType = UIData->cursor_type;

    /*  since there is no "known" way to detect whether cursor is
        displayed at the start of BIOS call for NEC, we will assume
        cursor is initially displayed for now
    */
    UIData->cursor_on = TRUE;

    if( UIData->cursor_on == FALSE ) {
        OldCursorType = C_OFF;
    }
    OldCursorAttr = UIData->cursor_attr;
}


static void newcursor()
/***********************************************************************
 newcursor:
 ---------
    Input:      NONE
    Output:     NONE

    update has been made to the cursor attributes and now we have to
    update this changes to video
************************************************************************/
{
    if( UIData->cursor_type == C_OFF ) {
        uioffcursor();
    } else {
        uioncursor();
    }
}


static void swapcursor()
/***********************************************************************
 swapcursor:
 ----------
    Input:      NONE
    Output:     NONE

    swap current UIData cursor informations with Old(?) local copies
************************************************************************/
{
    _swap( UIData->cursor_type, OldCursorType );
    _swap( UIData->cursor_col, OldCursorCol );
    _swap( UIData->cursor_row, OldCursorRow );
    _swap( UIData->cursor_attr, OldCursorAttr );
    UIData->cursor_on = TRUE;
}


void global uigetcursor( row, col, type, attr )
/***********************************************************************
 uigetcursor:
 -----------
    Input:      pointers to row, col, type, attr
    Output:     values of row, col, type, attr

    get current cursor informations and return
************************************************************************/

register        ORD*                    row;
register        ORD*                    col;
register        int*                    type;
register        int*                    attr;
{
        getcursorloc( row, col );
        *type = UIData->cursor_type;
        if( UIData->cursor_on == FALSE ) {
            *type = C_OFF;
        }
        *attr = UIData->cursor_attr;
}


void global uisetcursor( row, col, typ, attr )
/***********************************************************************
 uisetcursor:
 -----------
    Input:      row, col, type, attr
    Output:     NONE

    set UIData parameters according to the input cursor informations
************************************************************************/

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
/***********************************************************************
 uiswapcursor:
 ------------
    Input:      NONE
    Output:     NONE

    swap current UIData cursor informations with Old(?) local copies
    and update the results on the screen if necessary
************************************************************************/
{
        swapcursor();
        newcursor();
}


void global uiinitcursor()
/***********************************************************************
 uiinitcursor:
 ------------
    Input:      NONE
    Output:     NONE

    initializing cursor informations
************************************************************************/
{
        savecursor();
        uisetcursor( OldCursorRow, OldCursorCol, OldCursorType, OldCursorAttr );
        uioffcursor();
}


void global uifinicursor()
/***********************************************************************
 uifinicursor:
 ------------
    Input:      NONE
    Output:     NONE

    cursor manupulations are done so restore old cursor informations
************************************************************************/
{
        uioncursor();
}
