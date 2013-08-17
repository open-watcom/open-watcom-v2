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
* Description:  Active page get/set routines.
*
****************************************************************************/


#include "gdefn.h"
#include "gbios.h"


_WCRTLINK short _WCI86FAR _CGRAPH _getactivepage( void )
/*=======================================

   This function queries the current active page.   */

{
    return( _CurrActivePage );
}

Entry1( _GETACTIVEPAGE, _getactivepage ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _setactivepage( short pagenum )
/*================================================

   This routine sets the active page for graphics output. */

{
    short               pos;
    short               prev;

    _InitState();
    if( _CurrState->vc.numvideopages == 1 ) {
        if( pagenum != 0 ) {
            _ErrorStatus = _GRINVALIDPARAMETER;
            return( -1 );
        } else {
            return( 0 );
        }
    }
    pagenum %= _CurrState->vc.numvideopages;
//    #if defined( __386__ )
        _CurrState->screen_seg = _CurrState->screen_seg_base;
        _CurrState->screen_off = _CurrState->screen_off_base +
                                 ( ( _CurrState->page_size * pagenum ) << 4 );
//    #else
//      _CurrState->screen_seg = _CurrState->screen_seg_base +
//                                  _CurrState->page_size * pagenum;
//      _CurrState->screen_off = _CurrState->screen_off_base;
//    #endif
    pos = *(short far *)_BIOS_data( CURSOR_POSN + 2 * pagenum );
    _TextPos.row = pos >> 8;                        /* cursor position  */
    _TextPos.col = pos & 0xFF;                      /* on new page      */
    prev = _CurrActivePage;
    _CurrActivePage = pagenum;
    return( prev );
}

Entry1( _SETACTIVEPAGE, _setactivepage ) // alternate entry-point
