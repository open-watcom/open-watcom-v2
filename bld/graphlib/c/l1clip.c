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


#include <gdefn.h>


#define CLIP_TOP        1
#define CLIP_BOTTOM     2
#define CLIP_RIGHT      4
#define CLIP_LEFT       8


char _L1OutCode( short x, short y )
/*=================================

    Verify that a point is inside or outside the active viewport.   */
{
    char            flag;

    flag = 0;
    if( x < _CurrState->clip.xmin ) {
        flag |= CLIP_LEFT;
    } else if( x > _CurrState->clip.xmax ) {
        flag |= CLIP_RIGHT;
    }
    if( y < _CurrState->clip.ymin ) {
        flag |= CLIP_TOP;
    } else if( y > _CurrState->clip.ymax ) {
        flag |= CLIP_BOTTOM;
    }
    if( flag ) _ErrorStatus = _GRCLIPPED;
    return( flag );
}


static void line_inter( short * x1, short * y1, short x2, short y2, short x )
/*===========================================================================

    Find the intersection of a line with a boundary of the viewport.
    (x1, y1) is outside and ( x2, y2 ) is inside the viewport.
    NOTE : the signs of denom and ( x - *x1 ) cancel out during division
           so make both of them positive before rounding.   */
{
    long            numer;
    long            denom;

    denom = abs( x2 - *x1 );
    numer = 2L * (long)( y2 - *y1 ) * abs( x - *x1 );
    if( numer > 0 ) {
        numer += denom;                     /* round to closest pixel   */
    } else {
        numer -= denom;
    }
    *y1 += numer / ( denom << 1 );
    *x1 = x;
}


short _L0LineClip( short *x1, short *y1, short *x2, short *y2 )
/*=============================================================

    Clips the line with end points (x1,y1) and (x2,y2) to the active
    viewport using the Cohen-Sutherland clipping algorithm. Return the
    clipped coordinates and a decision drawing flag.    */
{
    char            flag1;
    char            flag2;

    flag1 = _L1OutCode( *x1, *y1 );
    flag2 = _L1OutCode( *x2, *y2 );
    for( ;; ) {
        if( flag1 & flag2 ) break;                  /* trivially outside    */
        if( flag1 == flag2 ) break;                 /* completely inside    */
        if( flag1 == 0 ) {                          /* first point inside   */
            if( flag2 & CLIP_TOP ) {
                line_inter( y2, x2, *y1, *x1, _CurrState->clip.ymin );
            } else if( flag2 & CLIP_BOTTOM ) {
                line_inter( y2, x2, *y1, *x1, _CurrState->clip.ymax );
            } else if( flag2 & CLIP_RIGHT ) {
                line_inter( x2, y2, *x1, *y1, _CurrState->clip.xmax );
            } else if( flag2 & CLIP_LEFT ) {
                line_inter( x2, y2, *x1, *y1, _CurrState->clip.xmin );
            }
            flag2 = _L1OutCode( *x2, *y2 );
        } else {                                    /* second point inside  */
            if( flag1 & CLIP_TOP ) {
                line_inter( y1, x1, *y2, *x2, _CurrState->clip.ymin );
            } else if( flag1 & CLIP_BOTTOM ) {
                line_inter( y1, x1, *y2, *x2, _CurrState->clip.ymax );
            } else if( flag1 & CLIP_RIGHT ) {
                line_inter( x1, y1, *x2, *y2, _CurrState->clip.xmax );
            } else if( flag1 & CLIP_LEFT ) {
                line_inter( x1, y1, *x2, *y2, _CurrState->clip.xmin );
            }
            flag1 = _L1OutCode( *x1, *y1 );
        }
    }
    return( flag1 & flag2 );
}


static void block_inter( short *x, short *y, char flag )
/*======================================================

    Find the intersection of a block with a boundary of the viewport.   */
{
    if( flag & CLIP_TOP ) {
        *y = _CurrState->clip.ymin;
    } else if( flag & CLIP_BOTTOM ) {
        *y = _CurrState->clip.ymax;
    } else if( flag & CLIP_RIGHT ) {
        *x = _CurrState->clip.xmax;
    } else if( flag & CLIP_LEFT ) {
        *x = _CurrState->clip.xmin;
    }
}


short _L0BlockClip( short *x1, short *y1, short *x2, short* y2 )
/*==============================================================

    Clip a block with opposite corners (x1,y1) and (x2,y2) to the
    active viewport based on the Cohen-Sutherland algorithm for line
    clipping. Return the clipped coordinates and a decision drawing
    flag ( 0 draw : 1 don't draw ). */
{
    char            flag1;
    char            flag2;

    flag1 = _L1OutCode( *x1, *y1 );
    flag2 = _L1OutCode( *x2, *y2 );
    for( ;; ) {
        if( flag1 & flag2 ) break;                  /* trivially outside    */
        if( flag1 == flag2 ) break;                 /* completely inside    */
        if( flag1 == 0 ) {
            block_inter( x2, y2, flag2 );
            flag2 = _L1OutCode( *x2, *y2 );
        } else {
            block_inter( x1, y1, flag1 );
            flag1 = _L1OutCode( *x1, *y1 );
        }
    }
    return( flag1 & flag2 );
}
