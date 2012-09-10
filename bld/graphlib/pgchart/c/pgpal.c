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
#include "pgvars.h"


static unsigned char    _DefaultFill[ 16 ][ 8 ] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa,
    0x33, 0xcc, 0x33, 0xcc, 0x33, 0xcc, 0x33, 0xcc,
    0x00, 0xe7, 0x00, 0xe7, 0x00, 0xe7, 0x00, 0xe7,
    0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe,
    0x7e, 0xbd, 0xdb, 0xe7, 0xe7, 0xdb, 0xbd, 0x7e,
    0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f,
    0x33, 0xcc, 0xcc, 0x33, 0x33, 0xcc, 0xcc, 0x33,
    0x00, 0xfd, 0x00, 0xf7, 0x00, 0xdf, 0x00, 0x7f,
    0x0f, 0x87, 0xc3, 0xe1, 0xf0, 0x78, 0x3c, 0x1e,
    0xa8, 0x51, 0xa2, 0x45, 0x8a, 0x15, 0x2a, 0x55,
    0xaa, 0x55, 0x00, 0x00, 0xaa, 0x55, 0x00, 0x00,
    0x2a, 0x15, 0x8a, 0x45, 0xa2, 0x51, 0xa8, 0x54,
    0x88, 0x00, 0x22, 0x00, 0x88, 0x00, 0x22, 0x00,
    0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00
};

static char             _DefaultPlotchar[ 16 ] = {
    ' ', '*', 'o', 'x',
    '=', '+', '/', ':',
    '&', '#', '@', '%',
    '!', '[', '$', '^'
};


_WCRTLINK short _WCI86FAR _CGRAPH _pg_resetpalette( void )
//=========================================

/*  Initializes the global palettetype variable to default values.  */

{
    struct videoconfig  vc;
    short               counter;
    short               style;
    short               s_counter;
    short               colour;

    _getvideoconfig( &vc );
    if( vc.numxpixels == 0 ) {
        return( _PG_BADSCREENMODE );
    }
    colour = 0;
    style = 1;
    s_counter = 1;
    for( counter = 1; counter <= 15; counter++ ) {
        _PGPalette[ counter ].color = colour;
        _PGPalette[ counter ].style = _Style[ style ];
        memcpy( &_PGPalette[ counter ].fill, &_DefaultFill[ style ], 8 );
        _PGPalette[ counter ].plotchar = _DefaultPlotchar[ counter ];
        ++s_counter;
        if( s_counter == vc.numcolors ) {
            s_counter = 1;
            ++style;
        }
        ++colour;
        if( colour == vc.numcolors ) {
            colour = 1;
        }
    }
    _PGPalette[ 0 ].color = 0;
    _PGPalette[ 1 ].color = 15;
    _PGPalette[ 0 ].style = _Style[ 0 ];
    memcpy( _PGPalette[ 0 ].fill, _DefaultFill[ 0 ], 8 );
    _PGPalette[ 0 ].plotchar = _DefaultPlotchar[ 0 ];
    return( 0 );
}

Entry( _PG_RESETPALETTE, _pg_resetpalette ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _pg_getpalette( paletteentry _WCI86FAR *palette )
//=============================================================

/*  Gets the contents of the global palettetype variable "_PGPalette"
    and returns these in the parameter palette. */

{
    MemoryCopy( palette, _PGPalette, sizeof( palettetype ) );
    return( 0 );
}

Entry( _PG_GETPALETTE, _pg_getpalette ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _pg_setpalette( paletteentry _WCI86FAR *palette )
//=============================================================

/*  Sets the contents of the global palettetype variable "_PGPalette"
    to those found in the parameter palette. */

{
    MemoryCopy( _PGPalette, palette, sizeof( palettetype ) );
    return( 0 );
}

Entry( _PG_SETPALETTE, _pg_setpalette ) // alternate entry-point
