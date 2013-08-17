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
* Description:  Text string output.
*
****************************************************************************/


#include "gdefn.h"
#include "gbios.h"
#include <conio.h>


#if defined( __386__ )
    #define Length( text )  strlen( text )
#else
    #define Length( text )  _fstrlen( text )
#endif


short _CharLen( char c )
//======================

{
    dbcs_pair           *p;

    if( _IsDBCS ) {
        for( p = _DBCSPairs; p->start_range != 0; ++p ) {
            if( c >= p->start_range && c <= p->end_range ) return( 2 );
        }
        return( 1 );
    } else {
        return( 1 );
    }
}


static void OutputString( char _WCI86FAR *text, short length, short newline )
//======================================================================

{
    short               can_display;
    short               ch_len;

    can_display = TRUE;
    while( length > 0 ) {
        ch_len = _CharLen( *text );
        if( ch_len == 2 && length == 1 ) {  // don't go past end of string
            ch_len = 1;
        }
        if( *text == '\n' && newline ) {
            _RefreshWindow();
            _TextPos.col = _Tx_Col_Min;     // move to start of next line
            if( _TextPos.row == _Tx_Row_Max ) {
                _ScrollWindow( _SCROLL_UP, 1 );
            } else {
               ++_TextPos.row;
            }
            can_display = TRUE;
        } else if( *text == '\r' && newline ) {
            _RefreshWindow();
            _TextPos.col = _Tx_Col_Min;     // move to start of current line
            can_display = TRUE;
        } else if( can_display ) {
            if( ch_len == 1 ) {
                _PutChar( _TextPos.row, _TextPos.col, *text );
                ++_TextPos.col;
            } else {        // double-byte character
                if( text[ 1 ] == 0 ) {      // special 1-byte char
                    _PutChar( _TextPos.row, _TextPos.col, *text );
                    ++_TextPos.col;
                } else if( _TextPos.col <= _Tx_Col_Max - 1 ) { // room for both halves
                    _PutChar( _TextPos.row, _TextPos.col, text[ 0 ] );
                    _PutChar( _TextPos.row, _TextPos.col+1, text[ 1 ] );
                    _TextPos.col += 2;
                } else {         // these three lines control double byte
                    text -= 2;   // characters at the end of the text window
                    length += 2; // at present they are not printed at all
                    _TextPos.col += 2;
                }
            }
            if( _TextPos.col > _Tx_Col_Max ) {
                if( _Wrap ) {
                    _TextPos.col = _Tx_Col_Min;
                    if( _TextPos.row == _Tx_Row_Max ) {
                        _ScrollWindow( _SCROLL_UP, 1 );
                    } else {
                        ++_TextPos.row;
                    }
                } else {
                    _TextPos.col = _Tx_Col_Max;
                    can_display = FALSE;        // past right edge of window
                }
            }
        }
        text += ch_len;
        length -= ch_len;
    }
    _RefreshWindow();
    // update cursor position
#if !defined( _DEFAULT_WINDOWS )
    VideoInt( _BIOS_CURSOR_POSN, _CurrActivePage << 8, 0,
                            ( _TextPos.row << 8 ) + _TextPos.col );
#endif
}


_WCRTLINK void _WCI86FAR _CGRAPH _outtext( char _WCI86FAR *text )
/*===========================================

   This routine displays the string of text pointed to by the parameter.
   The only formatting done is with regards to the current text window. */

{
    _InitState();
    _CursorOff();
    OutputString( text, Length( text ), TRUE );     // newline's allowed
    _GrEpilog();
}

Entry1( _OUTTEXT, _outtext ) // alternate entry-point


_WCRTLINK void _WCI86FAR _CGRAPH _outmem( unsigned char _WCI86FAR * text, short length )
/*==================================================================

   This routine writes "length" characters from the buffer pointed to by
   "text" to the active text window and updates the current text position.  */

{
    _InitState();
    _CursorOff();
    OutputString( (char _WCI86FAR *)text, length, FALSE );
    _GrEpilog();
}

Entry1( _OUTMEM, _outmem ) // alternate entry-point


_WCRTLINK void _WCI86FAR _CGRAPH _scrolltextwindow( short rows )
/*===============================================

   This routine scrolls the text window up or down by "rows" lines. */

{
    short           dir;

    if( rows == 0 ) {                               /* no lines to scroll   */
        _ErrorStatus = _GRINVALIDPARAMETER;
        return;
    }
    _InitState();
    _CursorOff();
    if( rows < 0 ) {
        rows = -rows;
        dir = _SCROLL_DOWN;
    } else {
        dir = _SCROLL_UP;
    }
    if( rows >= _Tx_Row_Max - _Tx_Row_Min + 1 ) {
        rows = 0;           // 0 means clear entire window
    }
    _ScrollWindow( dir, rows );
    _GrEpilog();
}

Entry1( _SCROLLTEXTWINDOW, _scrolltextwindow ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _gettextcursor( void )
/*=======================================

   This function returns the shape of the active text cursor.   */

{
    _InitState();
    return( _CursorShape );
}

Entry1( _GETTEXTCURSOR, _gettextcursor ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _settextcursor( short shape )
/*==============================================

   This function sets the shape of the text cursor and returns the shape
   of the previous text cursor. This function works only in text modes.
   The new cursor shape is displayed if the cursor was previously visible.
   Note: high byte of shape = starting scan line for the cursor (bits 4-0)
         low byte of shape  = ending scan line for the cursor (bits 4-0)
         ... where 0 <= scan line < 32.
   Warning : If _setrows is used to go into a 43 rows mode on an EGA, then
             the emulation bit will be on, as it should be.

   Note for windows: This function does no do anything in Windows. */

{
#if defined( _DEFAULT_WINDOWS )
    shape = shape;
    _ErrorStatus = _GRNOTINPROPERMODE;
    return( -1 );
#else
    short               previous;
    if( _GrMode ) {                 // only supported in text modes
        _ErrorStatus = _GRNOTINPROPERMODE;
        return( -1 );
    }
    previous = _CursorShape;
    _CursorShape = shape;

    VideoInt( _BIOS_CURSOR_SIZE, 0, shape, 0 );     // set new shape
    return( previous );
#endif
}

Entry1( _SETTEXTCURSOR, _settextcursor ) // alternate entry-point

