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

#if !defined( _DEFAULT_WINDOWS )
#include "gbios.h"
#endif


static void             GraphCursor( void );
#if !defined( _DEFAULT_WINDOWS )
static void             TextCursor( short );
#endif



void _CursorOn( void )
//====================

{
    if( _GrCursor == 0 ) {      // if it isn't already on
#if defined( _DEFAULT_WINDOWS )
        GraphCursor();
#else
        if( IsTextMode ) {
            TextCursor( 1 );
        } else {
            GraphCursor();
        }
#endif
        _GrCursor = 1;          // cursor is on
    }
}


void _CursorOff( void )
//=====================

{
    short               cursor;

    if( _GrCursor != 0 ) {      // if the cursor is on
#if defined( _DEFAULT_WINDOWS )
        cursor = cursor;
        GraphCursor();
#else
        if( IsTextMode ) {
            TextCursor( 0 );
        } else {
        #if defined( _NEC_PC )
            GraphCursor();
            cursor = cursor;
        #else
            // if cursor is not where we think it is (printf), assume it is off
            cursor = *(short far *)_BIOS_data( CURSOR_POSN + 2 * _CurrActivePage );
            if( cursor == ( ( _TextPos.row << 8 ) + _TextPos.col ) ) {
                GraphCursor();
            }
        #endif
        }
#endif
        _GrCursor = 0;      // cursor is off
    }
}


#if !defined( _DEFAULT_WINDOWS )

static void TextCursor( short turning_on )
//========================================

{
    short               cursor;

    cursor = _CursorShape;
#if defined( _NEC_PC )
    if( turning_on ) {
        if( ( _CursorShape & 0x2000 ) == 0 ) {
            NECVideoInt( _BIOS_CURSOR_START, 0, 0, 0 ); // display cursor
        }
    } else {
        NECVideoInt( _BIOS_CURSOR_STOP, 0, 0, 0 ); // don't display cursor
    }
#else
    if( !turning_on ) {
        cursor |= 0x2000;       // set blank cursor bit
    }
    VideoInt( _BIOS_CURSOR_SIZE, 0, cursor, 0 );
#endif
}

#endif


static void GraphCursor( void )
//=============================
{
    short               prev_pltact;
    short               prev_color;
    short               font_width;
    short               font_height;
    int                 x1, x2;
    int                 y;

    // set up the plotaction and color
    prev_pltact = _setplotaction( _GXOR );
    prev_color = _CurrColor;
    _CurrColor = ( _CurrState->vc.numcolors - 1 ) & 15;
    font_width = _CurrState->vc.numxpixels / _CurrState->vc.numtextcols;
    font_height = _CurrState->vc.numypixels / _CurrState->vc.numtextrows;

    // draw a line at the bottom of the next character cell
    x1 = font_width * _TextPos.col;
    x2 = x1 + font_width - 1;
    y = font_height * ( _TextPos.row + 1 ) - 1;
    _L1SLine( x1, y, x2, y );

    // reset the original state
    _setplotaction( prev_pltact );
    _setcolor( prev_color );
}
