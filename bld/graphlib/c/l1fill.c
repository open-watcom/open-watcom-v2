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
* Description:  Call a low level fill routine.
*
****************************************************************************/


#include "gdefn.h"
#include "filldef.h"


void _L1Fill( short x, short y, short length )
//============================================

{
    gr_device _FARD     *dev_ptr;
    fill_fn near        *fill;

    if( length <= 0 ) {
        return;
    }
    dev_ptr = _CurrState->deviceptr;
    ( *dev_ptr->setup )( x, y, _CurrColor );
    if( _HaveMask == 0 ) {
        fill = dev_ptr->zap;
        ( *fill )( _Screen.mem, _CurrColor, 0,
                   length, _Screen.bit_pos << 8 );
    } else {
       x = abs( x - _PaRf_x ) & 7;
       y = abs( y - _PaRf_y ) & 7;
        fill = dev_ptr->fill;
        ( *fill )( _Screen.mem, _CurrColor, ( x << 8 ) + _FillMask[ y ],
                   length, _Screen.bit_pos << 8 );
    }
}


void _L1ClipFill( short x1, short x2, short y )
//=============================================

// fill from the point (x1,y) to (x2,y) clipping if necessary

{
    if( y < _CurrState->clip.ymin || y > _CurrState->clip.ymax ) {
        return;
    }
    if( x1 < _CurrState->clip.xmin ) {      // left clip
        x1 = _CurrState->clip.xmin;
    }
    if( x2 > _CurrState->clip.xmax ) {      // right clip
        x2 = _CurrState->clip.xmax;
    }
    _L1Fill( x1, y, x2 - x1 + 1 );
}
