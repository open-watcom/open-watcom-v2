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


#include <math.h>
#include "gdefn.h"


extern float            sqrtf( float );
#pragma aux             sqrtf "*_";


/*  Definitions of local functions  */

static void     GetVectors( struct xycoord *, struct xycoord * );
static void     VecNormalize( short, float, float, struct xycoord * );
static void     GetAlignment( short *, short * );
static void     CalcSpacing( struct xycoord *, struct xycoord *,
                             struct xycoord * );
static void     CalcSides( struct xycoord *, struct xycoord *, struct xycoord *,
                           struct xycoord *, struct xycoord *, char _WCI86FAR * );
static void     CalcTranslation( struct xycoord *, struct xycoord *,
                           struct xycoord *, struct xycoord *, short, short );
static void     CalcCorners( struct xycoord _WCI86FAR *, struct xycoord *,
                             struct xycoord *, struct xycoord *, short, short );
static void     CalcNominal( struct xycoord *, struct xycoord *,
                             struct xycoord *, struct xycoord *,
                             struct xycoord *, char );
static short    Round( short, short );
static void     CalcConcat( struct xycoord _WCI86FAR *, struct xycoord *,
                            struct xycoord *, short, short, short, short );
static short    _CharWidth( char );


void _L1Text( short xpos, short ypos, char _WCI86FAR * str )
/*=====================================================

    Draw the character string pointed to by "str" at the position
    (xpos, ypos) using the current graphics text settings.  */

{
    short               hor;        /* horizontal alignment */
    short               vert;       /* vertical alignment   */
    struct xycoord      up;         /* character up vector  */
    struct xycoord      base;       /* character base line vector   */
    struct xycoord      prop;       /* adjustment vector for prop. font */
    struct xycoord      nommove;    /* nominal displacement vector  */
    struct xycoord      space;      /* spacing between characters   */
    struct xycoord      length;     /* horizontal side of parallelogram */
    struct xycoord      height;     /* vertical side of parallelogram   */
    struct xycoord      trans;      /* translation for parallelogram    */
    struct xycoord      corner[4];  /*  0 - lower left  */
                                    /*  1 - lower right */
                                    /*  2 - upper right */
                                    /*  3 - upper left  */
    if( *str == '\0' ) {
        _ErrorStatus = _GRNOOUTPUT;
        return;
    }
    if( _TextSettings.height == 0 || _TextSettings.width == 0 ) {
        _ErrorStatus = _GRNOOUTPUT;
        return;
    }
    GetVectors( &base, &up );
    GetAlignment( &hor, &vert );
    CalcSpacing( &base, &up, &space );
    CalcSides( &length, &height, &base, &up, &space, str );
    CalcTranslation( &trans, &length, &height, &up, hor, vert );
    CalcCorners( &corner, &length, &height, &trans, xpos, ypos );
    if( _TextSettings.txpath == _PATH_UP ) {    /* select proper corner for */
        xpos = corner[ 0 ].xcoord;              /* text starting position   */
        ypos = corner[ 0 ].ycoord;
    } else {
        xpos = corner[ _TextSettings.txpath ].xcoord;
        ypos = corner[ _TextSettings.txpath ].ycoord;
    }
    if( _TextSettings.txpath == _PATH_RIGHT ||
        _TextSettings.txpath == _PATH_LEFT ) {
        while( *str != '\0' ) {
            CalcNominal( &base, &up, &prop, &nommove, &space, *str );
            if( _TextSettings.txpath == _PATH_RIGHT ) {
                _HershDraw( *str, up.xcoord, -up.ycoord,
                            prop.xcoord, -prop.ycoord, xpos, ypos );
            } else {
                _HershDraw( *str, up.xcoord, -up.ycoord,
                            prop.xcoord, -prop.ycoord,
                            xpos-prop.xcoord, ypos+prop.ycoord );
            }
            xpos += nommove.xcoord;
            ypos -= nommove.ycoord;
            str++;
        }
    } else {                                        /* path is Up or DOWN   */
        if( _TextSettings.txpath == _PATH_DOWN ) {
            xpos -= up.xcoord;                      /* special increment*/
            ypos += up.ycoord;                      /* for path down    */
        }
        while( *str != '\0' ) {
            CalcNominal( &base, &up, &prop, &nommove, &space, *str );
            _HershDraw( *str, up.xcoord, -up.ycoord,
                        prop.xcoord, -prop.ycoord,
                        xpos + ( length.xcoord-prop.xcoord ) / 2,
                        ypos - ( length.ycoord-prop.ycoord ) / 2 );
            xpos += nommove.xcoord;
            ypos -= nommove.ycoord;
            str++;
        }
    }
    _RefreshWindow();
}

void _L1TXX( short xpos, short ypos, char _WCI86FAR * str,
/*========*/ struct xycoord _WCI86FAR * concat, struct xycoord _WCI86FAR * extent )

/*  Inquire the extents of the character drawing parallelogram and
    return the concatenation point. The concatenation point is the same
    as the drawing point if it cannot be calculated exactly.    */

{
    short               hor;        /* horizontal alignment */
    short               vert;       /* vertical alignment   */
    struct xycoord      up;         /* character up vector  */
    struct xycoord      base;       /* character base line vector   */
    struct xycoord      space;      /* spacing between characters   */
    struct xycoord      length;     /* horizontal side of parallelogram */
    struct xycoord      height;     /* vertical side of parallelogram   */
    struct xycoord      trans;      /* translation for parallelogram    */

    GetVectors( &base, &up );
    GetAlignment( &hor, &vert );
    CalcSpacing( &base, &up, &space );
    CalcSides( &length, &height, &base, &up, &space, str );
    CalcTranslation( &trans, &length, &height, &up, hor, vert );
    CalcCorners( extent, &length, &height, &trans, xpos, ypos );
    if( _TextSettings.txpath == _PATH_RIGHT ||
        _TextSettings.txpath == _PATH_LEFT ) {
        CalcConcat( concat, &length, &space, xpos, ypos, hor, vert );
    } else {
        CalcConcat( concat, &height, &space, xpos, ypos, hor, vert );
    }
}

static void GetVectors( struct xycoord * base, struct xycoord * up )
/*==================================================================

    Normalize the character up and base vectors to the character height
    and width respectively. The character up vector is made visually
    perpendicular to the character base vector. The character up vector
    is corrected by the screen aspect ratio so that it looks perpendicular
    to the character base vector on all devices. Assume that the
    width : height ratio of the physical dimensions of the screen is 4 : 3. */

{
    float           aspectratio;
    float           basex;
    float           basey;
    float           upx;
    float           upy;

    aspectratio = (float)( _CurrState->vc.numypixels << 2 ) /
                     (float)( _CurrState->vc.numxpixels * 3 );
    basex = (float) _TextSettings.basevectorx;
    basey = (float) _TextSettings.basevectory * aspectratio;
    upx = - (float) _TextSettings.basevectory;
    upy = basex * aspectratio;
    VecNormalize( _TextSettings.height, upx, upy, up );
    VecNormalize( _TextSettings.width, basex, basey, base );
}

static void VecNormalize( short scale, float dx, float dy, struct xycoord * vect )
/*================================================================================

    Normalize the vector (dx,dy) to scale and return in vect.   */

{
    float           factor;

    factor = (float) scale / sqrtf( dx * dx + dy * dy );
    vect->xcoord = dx * factor + roundoff( dx );              /* round result */
    vect->ycoord = dy * factor + roundoff( dy );
}

static void GetAlignment( short * hor, short * vert )
/*===================================================

    Remap the horizontal and vertical alignments to account for the
    defaults in the case of _NORMAL horizontal and/or vertical
    alignments */
{
    if ( _TextSettings.horizalign == _NORMAL ) {
        if( _TextSettings.txpath == _PATH_LEFT ) {
            *hor = _RIGHT;
        } else {
            *hor = _LEFT;
        }
    } else {
        *hor = _TextSettings.horizalign;
    }
    if( _TextSettings.vertalign == _NORMAL ) {
        if( _TextSettings.txpath == _PATH_UP ) {
            *vert = _BOTTOM;
        } else {
            *vert = _TOP;
        }
    } else {
        *vert = _TextSettings.vertalign;
    }
}

static void CalcSpacing( struct xycoord * base, struct xycoord * up,
/*====================*/ struct xycoord * space )

/*  Calculate the text spacing based on the text path.  */

{
    if( _TextSettings.txpath == _PATH_RIGHT ||
        _TextSettings.txpath == _PATH_LEFT ) {
        space->xcoord = (float)( base->xcoord * _TextSettings.spacing ) /
                                 _TextSettings.width;
        space->ycoord = (float)( base->ycoord * _TextSettings.spacing ) /
                                 _TextSettings.width;
    } else {
        space->xcoord = (float)( up->xcoord * _TextSettings.spacing ) /
                               (float) _TextSettings.height;
        space->ycoord = (float)( up->ycoord * _TextSettings.spacing ) /
                               (float) _TextSettings.height;
    }
}

static void CalcSides( struct xycoord * length, struct xycoord * height,
/*==================*/ struct xycoord * base, struct xycoord * up,
                       struct xycoord * space, char _WCI86FAR * str )

/*  Compute the sides of the parallelogram. The length vector is along
    the orientation vector; the visual height vector is perpendicular to
    the orientation vector. */
{
    short               len;
    short               len_A;
    short               temp;
    short               width;

    len_A = _CharWidth( 'A' );
    len = 0;
    width = 0;
    if( _TextSettings.txpath == _PATH_UP ||
        _TextSettings.txpath == _PATH_DOWN ) {
        while( *str != '\0' ) {                     /* must find the    */
            len += 1;                               /* maximum width    */
            temp = _CharWidth( *str++ );
            if( temp > width ) width = temp;
        }
        length->xcoord = ( base->xcoord * width ) / len_A;
        length->ycoord = ( base->ycoord * width ) / len_A;
        height->xcoord = space->xcoord * ( len - 1 ) + len * up->xcoord;
        height->ycoord = space->ycoord * ( len - 1 ) + len * up->ycoord;
    } else {
        while( *str != '\0' ) {                     /* must find the    */
            len += 1;                               /* total width      */
            width += _CharWidth( *str++ );
        }
        length->xcoord = space->xcoord * ( len - 1 ) +
                         (short)( ( (long)width * base->xcoord ) / len_A );
        length->ycoord = space->ycoord * ( len - 1 ) +
                         (short)( ( (long)width * base->ycoord ) / len_A );
        height->xcoord = up->xcoord;
        height->ycoord = up->ycoord;
    }
}

static void CalcTranslation( struct xycoord * trans, struct xycoord * length,
/*========================*/ struct xycoord * height, struct xycoord * up,
                             short hor, short vert )

/*  Calculate the translation vector: First assume that the lower left
    corner of the text parallelogram corresponds to the text alignment
    ( _LEFT, _BOTTOM ). Second, find the translation required to move
    the point corresponding to the true text alignment to the lower left
    corner of the parallelogram. The remaining corners of the parallelogram
    all obey the same transformation.   */

{
    switch( hor ) {     /* Compute the translation for horizontal alignment */
    case _LEFT  :
        trans->xcoord = 0;
        trans->ycoord = 0;
        break;
    case _RIGHT :
        trans->xcoord = -length->xcoord;
        trans->ycoord = -length->ycoord;
        break;
    case _CENTER :
        trans->xcoord = -length->xcoord / 2;
        trans->ycoord = -length->ycoord / 2;
    }
    switch( vert ) {            /* Add translation from vertical alignment  */
    case _TOP   :
        trans->xcoord -= height->xcoord;
        trans->ycoord -= height->ycoord;
        break;
    case _CAP   :
        if( _TextSettings.txpath == _PATH_UP ||
            _TextSettings.txpath == _PATH_DOWN ) {
            trans->xcoord -= height->xcoord - up->xcoord / 4;
            trans->ycoord -= height->ycoord - up->ycoord / 4;
        } else {
            trans->xcoord -= 3 * height->xcoord / 4;
            trans->ycoord -= 3 * height->ycoord / 4;
        }
        break;
    case _HALF :
        trans->xcoord -= height->xcoord / 2;
        trans->ycoord -= height->ycoord / 2;
        break;
    case _BASE  :
        if( _TextSettings.txpath == _PATH_UP ||
            _TextSettings.txpath == _PATH_DOWN ) {
            trans->xcoord -= up->xcoord / 4;
            trans->ycoord -= up->ycoord / 4;
        } else {
            trans->xcoord -= height->xcoord / 4;
            trans->ycoord -= height->ycoord / 4;
        }
        break;
    case _BOTTOM :
        break;
    }
}

static void CalcCorners( struct xycoord _WCI86FAR * corner, struct xycoord * length,
/*====================*/ struct xycoord * height, struct xycoord * trans,
                         short xpos, short ypos )

/*  Compute the corners of the character drawing parallelogram. */

{
    short               p;                      /* loop index   */

    corner[0].xcoord = 0;                       /* initialize the corners   */
    corner[0].ycoord = 0;
    corner[1].xcoord = length->xcoord;
    corner[1].ycoord = - length->ycoord;
    corner[2].xcoord = length->xcoord + height->xcoord;
    corner[2].ycoord = - length->ycoord - height->ycoord;
    corner[3].xcoord = height->xcoord;
    corner[3].ycoord = - height->ycoord;
    trans->xcoord += xpos;                      /* final translation vector */
    trans->ycoord = ypos - trans->ycoord;
    for( p = 0; p <= 3; p++ ) {                 /* translate the 4 corners  */
        corner[p].xcoord += trans->xcoord;
        corner[p].ycoord += trans->ycoord;
    }
}

static void CalcNominal( struct xycoord * base, struct xycoord * up,
/*====================*/ struct xycoord * prop, struct xycoord * nommove,
                         struct xycoord * space, char ch )

/*  Calculate the nominal move vector for one character. It is used in
    updating the drawing point once a character has been drawn. It is
    also used in computing the concatenation point. */

{
    short           len_A, len_ch;

    len_A = _CharWidth( 'A' );
    len_ch = _CharWidth( ch );
    prop->xcoord = ( (long) base->xcoord * len_ch +
                     Round( base->xcoord, len_A ) ) / len_A;
    prop->ycoord = ( (long) base->ycoord * len_ch +
                     Round( base->ycoord, len_A ) ) / len_A;
    if( _TextSettings.txpath == _PATH_RIGHT ||
        _TextSettings.txpath == _PATH_LEFT ) {  /* increment is +base vector*/
        nommove->xcoord = prop->xcoord + space->xcoord;
        nommove->ycoord = prop->ycoord + space->ycoord;
    } else {
        nommove->xcoord = up->xcoord + space->xcoord;    /* increment is +up vector  */
        nommove->ycoord = up->ycoord + space->ycoord;
    }
    if( _TextSettings.txpath == _PATH_LEFT ||
        _TextSettings.txpath == _PATH_DOWN ) {
        nommove->xcoord = -nommove->xcoord;
        nommove->ycoord = -nommove->ycoord;
    }
}

static short Round( short sign, short val )
/*=======================================*/

{
    val = val / 2;
    if( sign >= 0 ) {
        return( val );
    } else {
        return( -val );
    }
}

static void CalcConcat( struct xycoord _WCI86FAR * concat, struct xycoord * move,
/*===================*/ struct xycoord * space, short xpos, short ypos,
                        short hor, short vert )

/*  Compute the concatenation point for only those cases where there is
    no ambiguity. If the concatenation point cannot be computed exactly,
    then simply return the character drawing point. */
{
    short           exact;

    exact = 0;                  /* assume can't compute concatenation point */
    concat->xcoord = xpos;          /* return same point if can't compute   */
    concat->ycoord = ypos;          /* the concatenation point exactly.     */
    switch( _TextSettings.txpath ) {
    case _PATH_UP :
        if( vert == _BASE || vert == _BOTTOM ) {
            exact = 1;
        }
        break;
    case _PATH_DOWN :
        if( vert == _CAP || vert == _TOP ) {
            exact = -1;
        }
        break;
    case _PATH_RIGHT :
        if( hor == _LEFT ) {
            exact = 1;
        }
        break;
    case _PATH_LEFT :
        if( hor == _RIGHT ) {
            exact = -1;
        }
    }
    if( exact ) {                           /* adjust concatenation point   */
        concat->xcoord += exact * ( move->xcoord + space->xcoord );
        concat->ycoord -= exact * ( move->ycoord + space->ycoord );
    }
}


static short _CharWidth( char ch )
/*==============================*/

/*  This routine returns the width of the character ch scaled to the
    interval [0,127].   */

{
    ch = ch;
//  if( ch >= ' ' && ch <= 0x7e ) {
//      if( _FontType == _STROKE ) {
//          if( _PropFont ) {
//              return( _CurrFont->font.stroke.width[ ch ] );
//          } else {
//              return( _CurrFont->font.stroke.width[ 'A' ] );
//          }
//      } else {
//          return( _CurrFont->font.bit.width );
//      }
        return( _TextSettings.width );
//  } else {
//      return( 0 );
//  }
}
