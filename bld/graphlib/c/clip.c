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


_WCRTLINK short _WCI86FAR _CGRAPH _setclip( short mode )
/*=======================================

   This routine sets the clipping indicator. */

{
    short               prev;

    prev = _Clipping;
    _Clipping = mode;
    if( _Clipping ) {
        _CurrState->clip.xmin = _CurrState->clip_def.xmin;
        _CurrState->clip.xmax = _CurrState->clip_def.xmax;
        _CurrState->clip.ymin = _CurrState->clip_def.ymin;
        _CurrState->clip.ymax = _CurrState->clip_def.ymax;
    } else {
        _CurrState->clip.xmin = 0;
        _CurrState->clip.xmax = _CurrState->vc.numxpixels - 1;
        _CurrState->clip.ymin = 0;
        _CurrState->clip.ymax = _CurrState->vc.numypixels - 1;
    }
    _resetscalefactor();
    return( prev );
}

Entry( _SETCLIP, _setclip ) // alternate entry-point
