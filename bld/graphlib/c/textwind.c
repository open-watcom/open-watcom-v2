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


#include "gdefn.h"


void _WCI86FAR _CGRAPH _gettextwindow( short _WCI86FAR * row_top, short _WCI86FAR * col_left,
/*=============================*/ short _WCI86FAR * row_bot, short _WCI86FAR * col_right )

/* This functions returns the boundaries of the active text window. */

{
    _InitState();
    *row_top = _Tx_Row_Min + 1;
    *row_bot = _Tx_Row_Max + 1;
    *col_left = _Tx_Col_Min + 1;
    *col_right = _Tx_Col_Max + 1;
}

Entry( _GETTEXTWINDOW, _gettextwindow ) // alternate entry-point


void _WCI86FAR _CGRAPH _settextwindow( short row1, short col1,
/*=============================*/ short row2, short col2 )

/* This routine sets a window for text output. */

{
    short               t;

    _InitState();
    if( row1 > row2 ) {         // ensure row1 < row2
        t = row1;
        row1 = row2;
        row2 = t;
    }
    if( col1 > col2 ) {         // ensure col1 < col2
        t = col1;
        col1 = col2;
        col2 = t;
    }
    row1--;
    row2--;
    col1--;
    col2--;
    if( row1 < 0 ) {
        row1 = 0;
    }
    if( col1 < 0 ) {
        col1 = 0;
    }
    if( row2 >= _CurrState->vc.numtextrows ) {
        row2 = _CurrState->vc.numtextrows - 1;
    }
    if( col2 >= _CurrState->vc.numtextcols ) {
        col2 = _CurrState->vc.numtextcols - 1;
    }
    _Tx_Row_Min = row1;
    _Tx_Row_Max = row2;
    _Tx_Col_Min = col1;
    _Tx_Col_Max = col2;
    _settextposition( 1, 1 );
}

Entry( _SETTEXTWINDOW, _settextwindow ) // alternate entry-point
