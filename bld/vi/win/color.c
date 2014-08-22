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
* Description:  Color support.
*
****************************************************************************/


#include "vi.h"

typedef struct color {
    long        rgb;
    HBRUSH      brush;
    HPEN        pen;
} color;

static color colorData[MAX_COLORS];

long ColorRGB( vi_color color )
{
    return( colorData[color].rgb );
}

HBRUSH ColorBrush( vi_color color )
{
    return( colorData[color].brush );
}

HPEN ColorPen( vi_color color )
{
    return( colorData[color].pen );
}

static void NewColor( vi_color index, long rgb )
{
    color       *c;
    LOGBRUSH    brush;
    long        nearest;
    HDC         hdc;

    hdc = GetDC( (HWND)NULLHANDLE );
    c = &colorData[index];
    if( c->pen ) {
        DeleteObject( c->pen );
    }
    if( c->brush ) {
        DeleteObject( c->brush );
    }
    c->rgb = rgb;
    nearest = GetNearestColor( hdc, rgb );
    ReleaseDC( (HWND)NULLHANDLE, hdc );
    c->pen = CreatePen( PS_SOLID, 1, nearest );
    brush.lbStyle = BS_SOLID;
    brush.lbColor = nearest;
    brush.lbHatch = 0;
    c->brush = CreateBrushIndirect( &brush );
}

void InitColors( void )
{
    vi_color        i;
    PALETTEENTRY    palette[MAX_COLORS], *p;
    HDC             hdc;

    hdc = GetDC( (HWND)NULLHANDLE );
    GetSystemPaletteEntries( hdc, 0, MAX_COLORS, &palette[0] );
    ReleaseDC( (HWND)NULLHANDLE, hdc );
    p = &palette[0];
    memset( &colorData[0], 0, sizeof( color ) * MAX_COLORS );
    for( i = 0; i < MAX_COLORS; i++, p++ ) {
        NewColor( i, RGB( p->peRed, p->peGreen, p->peBlue ) );
    }
    EditFlags.Color = true;
}

int GetNumColors( void )
{
    return( MAX_COLORS );
}

bool GetColorSetting( vi_color index, rgb *value )
{
    color       *c;

    if( index < MAX_COLORS && index >= 0 ) {
        c = &colorData[index];
        value->red = GetRValue( c->rgb );
        value->blue = GetBValue( c->rgb );
        value->green = GetGValue( c->rgb );
        return( true );
    }
    return( false );
}

COLORREF GetRGB( vi_color index )
{
    color       *c;

    if( index < MAX_COLORS && index >= 0 ) {
        c = &colorData[index];
        return( RGB( GetRValue( c->rgb ),
                     GetGValue( c->rgb ),
                     GetBValue( c->rgb ) ) );
    }
    return( RGB( 0, 0, 0 ) );
}

static bool chooseColor( vi_color index, COLORREF *rgb, HWND parent )
{
    CHOOSECOLOR cc;
    COLORREF    color_table[MAX_COLORS];
    vi_color    i;

    for( i = 0; i < MAX_COLORS; i++ ) {
        color_table[i] = colorData[i].rgb;
    }
    memset( &cc, 0, sizeof( CHOOSECOLOR ) );
    cc.lStructSize = sizeof( CHOOSECOLOR );
    cc.hwndOwner = parent;
    cc.rgbResult = colorData[index].rgb;
    cc.lpCustColors = color_table;
    cc.Flags = CC_PREVENTFULLOPEN | CC_RGBINIT;

    if( ChooseColor( &cc ) ) {
        for( i = 0; i < MAX_COLORS; i++ ) {
            colorData[i].rgb = color_table[i];
        }
        *rgb = cc.rgbResult;
        return( true );
    }
    return( false );
}


/*
 * setUpColor - set up a selected color
 */
static void setUpColor( vi_color index, COLORREF *rgb )
{
    NewColor( index, *rgb );
    if( EditFlags.WindowsStarted ) {
        ReDisplayScreen();
    }

} /* setUpColor */

/*
 * SetAColor - set a new color
 */
vi_rc SetAColor( char *data )
{
    char        token[MAX_STR];
    int         index;
    int         red, blue, green;
    COLORREF    rgb;

    if( NextWord1( data, token ) <= 0 ) {
        return( ERR_INVALID_SETCOLOR );
    }
    index = atoi( token );
    if( NextWord1( data, token ) <= 0 ) {
        if( !chooseColor( index, &rgb, Root ) ) {
            return( ERR_NO_ERR );
        }
    } else {
        red = atoi( token );
        if( NextWord1( data, token ) <= 0 ) {
            return( ERR_INVALID_SETCOLOR );
        }
        green = atoi( token );
        if( NextWord1( data, token ) <= 0 ) {
            return( ERR_INVALID_SETCOLOR );
        }
        blue = atoi( token );
        if( index >= MAX_COLORS || index < 0 ) {
            return( ERR_INVALID_SETCOLOR );
        }
        rgb = RGB( red, green, blue );
    }
    setUpColor( index, &rgb );
    return( ERR_NO_ERR );

} /* SetAColor */

/*
 * FiniColors - done with all pens and brushes used for coloring text
 */
void FiniColors( void )
{
    int         i;
    color       *c;

    c = &colorData[0];
    for( i = 0; i < MAX_COLORS; i++, c++ ) {
        DeleteObject( c->brush );
        DeleteObject( c->pen );
    }

} /* FiniColors */
