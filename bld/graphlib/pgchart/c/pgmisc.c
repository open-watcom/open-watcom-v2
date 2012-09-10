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


#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "pgvars.h"


void _pg_line( short x1, short y1, short x2, short y2, short color, short style )
//===============================================================================

/*  Draws a line.  Line style and colour are passed as parameters.  */

{
    _setcolor( _PGPalette[ color ].color );
    _setlinestyle( _PGPalette[ style ].style );
    _moveto( x1, y1 );
    _lineto( x2, y2 );
}


void _pg_window( windowtype _WCI86FAR *window )
//=======================================

/*  _pg_window draws a filled in rectangle in the current background
    colour, then draws an empty rectangle with the current border colour. */

{
    _setcolor( _PGPalette[ window->background ].color );
    if( !window->background ) {
        _setfillmask( _PGPalette[ 1 ].fill );
    } else {
        _setfillmask( _PGPalette[ window->background ].fill );
    }
    _rectangle( _GFILLINTERIOR, window->x1, window->y1, window->x2, window->y2 );
    if( window->border ) {
        _setcolor( _PGPalette[ window->bordercolor ].color );
        _setlinestyle( _Style[ window->borderstyle ] );
        _rectangle( _GBORDER, window->x1, window->y1, window->x2, window->y2 );
    }
}


short _MaxLabelLength( char _WCI86FAR *_WCI86FAR *labels, short n, short dim )
//==================================================================

/*  Calculates the length (in pixels) of the longest label.
    Note: dim can be _XVECTOR or _YVECTOR.  */

{
    short               i;
    short               max_length;
    short               curr;

    max_length = 0;
    for( i = 0; i < n; ++i ) {
        curr = _sLabelLength( *labels, dim );
        max_length = max( max_length, curr );
        ++labels;
    }
    return( max_length );
}


short _sLabelLength( char _WCI86FAR *label, short dim )
//================================================

/*  Calculates the length of a single label in pixels. */

{
    short               length;

    if( dim ) {
        length = StringLen( label ) * _CharHeight;
    } else {
        length = _getgtextextent( label );
    }
    return( length );
}


short _MaxLabelWidth( char _WCI86FAR *_WCI86FAR *labels, short n, short dim )
//=================================================================

/*  Calculates the maximum label width for a set of labels in pixels.
    Note: dim can be _XVECTOR or _YVECTOR.  */

{
    short               i;
    short               max_width;
    short               curr;

    if( dim ) {
        max_width = _CharHeight;
    } else {
        max_width = 0;
        for( i = 0; i < n; ++i ) {
            curr = _sMaxLabelWidth( *labels, dim );
            max_width = max( max_width, curr );
            ++labels;
        }
    }
    return( max_width );
}


short _sMaxLabelWidth( char _WCI86FAR *label, short dim )
//==================================================

/*  Finds the maximum width for one label. */
{
    short               max_width;
    short               curr;
    char                charbuf[ 2 ];

    if( dim ) {
        max_width = _CharHeight;
    } else {
        max_width = 0;
        charbuf[ 1 ] = '\0';
        while( *label != '\0' ) {
            charbuf[ 0 ] = *label;
            curr = _getgtextextent( charbuf );
            max_width = max( max_width, curr );
            ++label;
        }
    }
    return( max_width );
}


short _StringPrint( char *str, short format, short ndigits, float value )
//=======================================================================

/*  Convert from float to string. Return the length in pixels.
        _PG_DECFORMAT: decimal  format
        _PG_EXPFORMAT: exponent format  */

{
    char                *num;
    char                *orig;
    int                 dec;
    int                 sign;
    short               exp;
    char                buffer[ 80 ];

    orig = str;
    num = buffer;
    if( format == _PG_EXPFORMAT ) {
        _GR_ecvt( &value, ndigits + 1, &dec, &sign, buffer );
        if( sign != 0 ) {
            *str++ = '-';
        }
        *str++ = *num++;
        *str++ = '.';
        strcpy( str, num );
        str += strlen( num );
        *str++ = 'E';
        if( value == 0 ) {
            exp = 0;
        } else {
            exp = dec - 1;
        }
        if( exp >= 0 ) {
            *str++ = '+';
        } else {
            *str++ = '-';
            exp = -exp;
        }
        *str++ = '0' + exp / 100;
        *str++ = '0' + ( exp % 100 ) / 10;
        *str++ = '0' + exp % 10;
        *str = '\0';
    } else {
        _GR_fcvt( &value, ndigits, &dec, &sign, buffer );
        if( sign != 0 ) {
            *str++ = '-';
        }
        if( dec < 0 ) {
            *str++ = '0';
            *str++ = '.';
            memset( str, '0', -dec );
            str += -dec;
        } else {
            if( dec == 0 && ndigits != 0 ) {
                *str++ = '0';       // always have leading 0
            } else {
                memcpy( str, num, dec );
                num += dec;
                str += dec;
            }
            if( ndigits > 0 ) {
                *str++ = '.';
             }
        }
        strcpy( str, num );
    }
    return( _sLabelLength( orig, _XVECTOR ) );
}


float _pg_scale( axistype _WCI86FAR *axis, float value )
//=================================================

/*  Modifies a value based on the axis information. */

{
    float               tmp;

    if( ( axis->rangetype == _PG_LOGAXIS ) && ( value > 0 ) ) {
//      value = log( value ) / log( axis->logbase );
        tmp = axis->logbase;
        _GR_log( &tmp );    // tmp = log( tmp );
        _GR_log( &value );  // value = log( value );
        value = value / tmp;
    }
    value /= axis->scalefactor;
    return( value );
}
