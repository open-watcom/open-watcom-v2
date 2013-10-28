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
* Description:  Screen clear routine.
*
****************************************************************************/


#include "gdefn.h"
#include "gbios.h"


void _WCI86FAR _L2clearscreen( short area )
/*====================================

    This routine clears the screen in graphics mode.    */

{
    short               pix_per_col;
    short               pix_per_row;

    switch( area ) {
    case _GVIEWPORT:                    /* clear current clip region    */
        _GrClear( _CurrState->clip.xmin, _CurrState->clip.ymin,
                  _CurrState->clip.xmax, _CurrState->clip.ymax );
        break;
    case _GWINDOW:                      /* clear current text window    */
        pix_per_col = _CurrState->vc.numxpixels / _CurrState->vc.numtextcols ;
        pix_per_row = _CurrState->vc.numypixels / _CurrState->vc.numtextrows ;
        _GrClear( _Tx_Col_Min * pix_per_col, _Tx_Row_Min * pix_per_row,
                  ( _Tx_Col_Max + 1 ) * pix_per_col - 1,
                  ( _Tx_Row_Max + 1 ) * pix_per_row - 1 );
        _settextposition( 1, 1 );
        break;
    case _GCLEARSCREEN:                         /* clear entire screen  */
    default:                                    /* in all other cases  */
        _GrClear( 0, 0, _CurrState->vc.numxpixels - 1,
                        _CurrState->vc.numypixels - 1 );
        _settextposition( 1, 1 );
    }
}


_WCRTLINK void _WCI86FAR _CGRAPH _clearscreen( short area )
/*==========================================

   This routine clears either the entire screen, the current viewport, or
   the text window depending on the parameter. */

{
    _InitState();
    _CursorOff();
#if defined( _DEFAULT_WINDOWS )
    _L2clearscreen( area );
#else
    if( _GrMode ) {
        _L2clearscreen( area );
    } else {
        switch( area ) {
        case _GVIEWPORT:                            /* do nothing   */
            _ErrorStatus = _GRNOTINPROPERMODE;
            break;
        case _GWINDOW:                      /* clear current text window    */
            _TxtClear( _Tx_Row_Min, _Tx_Col_Min, _Tx_Row_Max, _Tx_Col_Max );
            _settextposition( 1, 1 );
            break;
        case _GCLEARSCREEN:                         /* clear entire screen  */
        default:                                    /* in all other cases  */
            _TxtClear( 0, 0, _CurrState->vc.numtextrows - 1,
                             _CurrState->vc.numtextcols - 1 );
            _settextposition( 1, 1 );
        }
    }
#endif
    _GrEpilog();
}

Entry1( _CLEARSCREEN, _clearscreen ) // alternate entry-point


void _GrClear( short x1, short y1, short x2, short y2 )
/*=====================================================

    Clear area of screen in graphics mode. */

{
    unsigned char       prev_mask[ MASK_LEN ];
    grcolor             prev_colour;
    short               prev_action;

    prev_action = _setplotaction( _GPSET );
    _getfillmask( prev_mask );
    _setfillmask( NULL );
    prev_colour = _getcolor();
    _setcolor( 0 );
    _L1Block( x1, y1, x2, y2 );
    _setcolor( prev_colour );
    _setfillmask( prev_mask );
    _setplotaction( prev_action );
}


#if !defined( _DEFAULT_WINDOWS )

void _TxtClear( short r1, short c1, short r2, short c2 )
/*======================================================

    Clear an area of the screen in text mode. Can't use BIOS scroll
    function when the active page is not equal to the visual page. */

{
    short               len;
    short               i;
    unsigned short      seg;
    unsigned int        offset;
    short __far         *p;
    short               blank;

    if( _CurrState->vc.mode == _TEXTMONO ) {
        seg = _MonoSeg;
        offset = _MonoOff;
    } else {
        seg = _CgaSeg;
        offset = _CgaOff;
    }
    offset += 2 * ( r1 * _CurrState->vc.numtextcols + c1 ) +
                    _CurrActivePage * *(short __far *)_BIOS_data( CRT_LEN );
    len = c2 - c1 + 1;
    blank = ( _CharAttr << 8 ) + ' ';
    for( ; r1 <= r2; ++r1 ) {
        p = MK_FP( seg, offset );
        for( i = 0; i < len; ++i ) {
            *p = blank;
            ++p;
        }
        offset += 2 * _CurrState->vc.numtextcols;
    }
}

#endif
