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


static void _LineUpdate( struct line_entry *curr )
//================================================

{
    if( curr->two_dx > curr->two_dy ) {         // moves faster along x
        while( curr->slope > curr->two_dy ) {       // can increment by
            curr->curr_x += curr->sdx;              // more that 1 x pixel
            curr->slope -= curr->two_dy;
        }
        if( curr->sdx < 0 ) {
            curr->left_x = curr->curr_x;
        } else {
            curr->right_x = curr->curr_x;
        }
        curr->slope += curr->two_dx;
    } else {
        if( curr->slope <= 0 ) {                // moves faster along y
            curr->curr_x += curr->sdx;              // can increment by at
            curr->slope += curr->two_dy;            // most 1 x pixel
        }
        curr->left_x = curr->curr_x;
        curr->right_x = curr->curr_x;
        curr->slope -= curr->two_dx;
    }
}


void _LineMove( struct line_entry *curr )
//=======================================

{
    if( curr->two_dx > curr->two_dy ) {     // moves faster along x
        if( curr->sdx < 0 ) {                   // slope is negative
            curr->right_x = curr->curr_x - 1;
        } else {                                // slope is positive
            curr->left_x = curr->curr_x + 1;
        }
    }
    _LineUpdate( curr );
    // stop values off of the screen from wrapping around and changing sign
    if( curr->sdx > 0 && curr->curr_x > _CurrState->vc.numxpixels ) {
        curr->curr_x = _CurrState->vc.numxpixels;
    } else if( curr->sdx < 0 && curr->curr_x < 0 ) {
        curr->curr_x = 0;
    }
}


void _LineInit( short x1, short y1, short x2, short y2,
                struct line_entry *curr )
//=====================================================

{
    short               deltax;
    short               deltay;

    deltax = x2 - x1;
    if( deltax < 0 ) {
        deltax = -deltax;
        curr->sdx = -1;
    } else if( deltax == 0 ) {
        curr->sdx = 0;
    } else {
        curr->sdx = 1;
    }
    deltay = y2 - y1;
    if( deltay < 0 ) {
        deltay = -deltay;
    }
    curr->two_dx = deltax << 1;
    curr->two_dy = deltay << 1;
    curr->curr_x = x1;
    curr->left_x = x1;
    curr->right_x = x1;
    curr->slope = max( deltax, deltay );
    _LineUpdate( curr );
}
