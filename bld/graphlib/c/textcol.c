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


short _WCI86FAR _CGRAPH _gettextcolor( void )
/*======================================

   This function queries the current text output colour. */

{
#if defined( _NEC_PC )
    short               colour;

    colour = _SwapBits( _CharAttr >> 5 );
    if( _CharAttr & 0x04 ) colour |= 0x08;  // reverse bit
    return( colour );
#else
    if( _GrMode ) {
        return( _CharAttr );
    } else {
        return( ( _CharAttr & 0x0F ) + ( ( _CharAttr & 0x80 ) >> 3 ) );
    }
#endif
}

Entry( _GETTEXTCOLOR, _gettextcolor ) // alternate entry-point


#if defined( _NEC_PC )

short _WCI86FAR _CGRAPH _settextcolor( short pixval )
/*==============================================

   This routine sets the colour for text output by changing the
   attribute.  It returns the previous text colour. */

{
    short               prev;
    short               new_attr;

    prev = _gettextcolor();
    pixval = _SwapBits( pixval & 0x0F );
    new_attr = ( pixval << 5 ) | 1;
    if( pixval & 0x08 ) new_attr |= 0x04;  // reverse bit
    if( IsTextMode ) {
        _CharAttr = new_attr;
    } else {
        _CursorOff();
        _CharAttr = new_attr;
        _GrEpilog();            // cause the cursor to change to new colour
    }
    return( prev );
}

#else

short _WCI86FAR _CGRAPH _settextcolor( short pixval )
/*==============================================

   This routine sets the colour for text output by changing the
   attribute.  It returns the previous text colour. */

{
    short               prev;

    prev = _gettextcolor();
    if( _GrMode ) {
        _CursorOff();                        // keep XOR bit in colour
        _CharAttr = pixval & ( ( _CurrState->vc.numcolors - 1 ) | 0x80 );
        _GrEpilog();            // cause the cursor to change to new colour
    } else {
        _CharAttr &= 0x70;          // eliminate old colour
        _CharAttr |= ( pixval & 0x0f ) + ( ( pixval & 0x10 ) << 3 );                 /* insert new colour    */
    }
    return( prev );
}

#endif

Entry( _SETTEXTCOLOR, _settextcolor ) // alternate entry-point
