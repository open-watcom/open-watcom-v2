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
* Description:  Visual page get/set routines.
*
****************************************************************************/


#include "gdefn.h"
#include "gbios.h"


_WCRTLINK short _WCI86FAR _CGRAPH _getvisualpage( void )
/*=======================================

   This function queries the current visual page.   */

{
    return( _CurrVisualPage );
}

Entry( _GETVISUALPAGE, _getvisualpage ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _setvisualpage( short pagenum )
/*================================================

   This routine sets the visual display page. */

{
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
    VideoInt( _BIOS_VIDEO_PAGE + pagenum, 0, 0, 0 );
    prev = _CurrVisualPage;
    _CurrVisualPage = pagenum;
    return( prev );
}

Entry( _SETVISUALPAGE, _setvisualpage ) // alternate entry-point
