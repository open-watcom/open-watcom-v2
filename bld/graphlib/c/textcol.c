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
* Description:  Text color get/set routines.
*
****************************************************************************/


#include "gdefn.h"


_WCRTLINK grcolor _WCI86FAR _CGRAPH _gettextcolor( void )
/*======================================

   This function queries the current text output colour. */

{
    if( _GrMode ) {
        return( _CharAttr );
    } else {
        return( ( _CharAttr & 0x0F ) + ( ( _CharAttr & 0x80 ) >> 3 ) );
    }
}

Entry( _GETTEXTCOLOR, _gettextcolor ) // alternate entry-point


_WCRTLINK grcolor _WCI86FAR _CGRAPH _settextcolor( grcolor pixval )
/*==============================================

   This routine sets the colour for text output by changing the
   attribute.  It returns the previous text colour. */

{
    grcolor             prev;

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

Entry( _SETTEXTCOLOR, _settextcolor ) // alternate entry-point
