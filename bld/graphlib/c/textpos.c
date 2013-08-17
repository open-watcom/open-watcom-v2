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
* Description:  Text position get/set routines.
*
****************************************************************************/


#include "gdefn.h"
#include "gbios.h"


_WCRTLINK struct rccoord _WCI86FAR _CGRAPH _gettextposition( void )
/*==================================================

   This function returns the current text output position relative to
   the origin of the text window.   */

{
    struct rccoord      pos;

    _InitState();               /* read the current machine state   */
    pos.row = _TextPos.row - _Tx_Row_Min + 1;
    pos.col = _TextPos.col - _Tx_Col_Min + 1;
    return( pos );
}

Entry1( _GETTEXTPOSITION, _gettextposition ) // alternate entry-point


_WCRTLINK struct rccoord _WCI86FAR _CGRAPH _settextposition( short row, short col )
/*==================================================================

   This routine sets the current text output position. */

{
    struct rccoord      prev;

    _CursorOff();
    prev = _gettextposition();
    row += _Tx_Row_Min - 1;
    col += _Tx_Col_Min - 1;
    if( row < _Tx_Row_Min ) {
        row = _Tx_Row_Min;
    }
    if( row > _Tx_Row_Max ) {
        row = _Tx_Row_Max;
    }
    if( col < _Tx_Col_Min ) {
        col = _Tx_Col_Min;
    }
    if( col > _Tx_Col_Max ) {
        col = _Tx_Col_Max;
    }
    _TextPos.row = row;
    _TextPos.col = col;
#if !defined( _DEFAULT_WINDOWS )
    VideoInt( _BIOS_CURSOR_POSN, _CurrActivePage << 8, 0, ( row << 8 ) + col );
#endif
    _GrEpilog();
    return( prev );
}

Entry1( _SETTEXTPOSITION, _settextposition ) // alternate entry-point
