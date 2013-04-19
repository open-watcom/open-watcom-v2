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


#include "imgedit.h"
#include "ieclrpal.h"
#include "colors.h"

static RGBQUAD          palette[NUM_PALETTES][PALETTE_SIZE];
static RGBQUAD          restorePalette[PALETTE_SIZE];
static RGBQUAD          solids[16];   // maybe vary this later
static int              currentPalIndex = 0;
static int              leftColorIndex;
static int              rightColorIndex;

/*
 * chooseThePalette - check the number of colors and decide which palette we need
 */
static void chooseThePalette( int num_of_colors )
{
    switch( num_of_colors ) {
    case 2:
        currentPalIndex = COLOR_2;
        break;
    case 16:
        currentPalIndex = COLOR_16;
        break;
    case 256:
        currentPalIndex = COLOR_256;
        break;
    }

} /* chooseThePalette */

/*
 * getColorIndex - given a color, return the index of that color in the current palette
 *               - return -1 if the color is not found in the palette
 */
static int getColorIndex( COLORREF color )
{
    int         i;

    for( i = 0; i < PALETTE_SIZE; i++ ) {
        if( color == RGB( palette[currentPalIndex][i].rgbRed,
                          palette[currentPalIndex][i].rgbGreen,
                          palette[currentPalIndex][i].rgbBlue ) ) {
            return( i );
        }
    }
    return( -1 );

} /* getColorIndex */

/*
 * InitPalette - initializes the color palette
 *             - palette[0] is the 16 color palette
 *             - palette[1] is the 2 color palette
 *             - palette[2] is the 256 color palette
 */
void InitPalette( void )
{
    short               i;
    RGBQUAD             quads[PALETTE_SIZE];

    for( i = 0; i < PALETTE_SIZE; i++ ) {
        quads[i].rgbReserved = (BYTE)0;
    }

    // white
    quads[0].rgbRed = 0xFF;
    quads[0].rgbGreen = 0xFF;
    quads[0].rgbBlue = 0xFF;

    // black
    quads[1].rgbRed = 0x00;
    quads[1].rgbGreen = 0x00;
    quads[1].rgbBlue = 0x00;

#ifdef __OS2_PM__
    quads[2].rgbRed = 0xCC;
    quads[2].rgbGreen = 0xCC;
    quads[2].rgbBlue = 0xCC;
#else
    quads[2].rgbRed = 0xC0;
    quads[2].rgbGreen = 0xC0;
    quads[2].rgbBlue = 0xC0;
#endif

    quads[3].rgbRed = 0x80;
    quads[3].rgbGreen = 0x80;
    quads[3].rgbBlue = 0x80;

    // red
    quads[4].rgbRed = 0xFF;
    quads[4].rgbGreen = 0x00;
    quads[4].rgbBlue = 0x00;

    quads[5].rgbRed = 0x80;
    quads[5].rgbGreen = 0x00;
    quads[5].rgbBlue = 0x00;

    quads[6].rgbRed = 0xFF;
    quads[6].rgbGreen = 0xFF;
    quads[6].rgbBlue = 0x00;

    quads[7].rgbRed = 0x80;
    quads[7].rgbGreen = 0x80;
    quads[7].rgbBlue = 0x00;

    quads[8].rgbRed = 0x00;
    quads[8].rgbGreen = 0xFF;
    quads[8].rgbBlue = 0x00;

    quads[9].rgbRed = 0x00;
    quads[9].rgbGreen = 0x80;
    quads[9].rgbBlue = 0x00;

    quads[10].rgbRed = 0x00;
    quads[10].rgbGreen = 0xFF;
    quads[10].rgbBlue = 0xFF;

    quads[11].rgbRed = 0x00;
    quads[11].rgbGreen = 0x80;
    quads[11].rgbBlue = 0x80;

    quads[12].rgbRed = 0x00;
    quads[12].rgbGreen = 0x00;
    quads[12].rgbBlue = 0xFF;

    quads[13].rgbRed = 0x00;
    quads[13].rgbGreen = 0x00;
    quads[13].rgbBlue = 0x80;

    quads[14].rgbRed = 0xFF;
    quads[14].rgbGreen = 0x00;
    quads[14].rgbBlue = 0xFF;

    quads[15].rgbRed = 0x80;
    quads[15].rgbGreen = 0x00;
    quads[15].rgbBlue = 0x80;

    quads[16].rgbRed = 0xFF;
    quads[16].rgbGreen = 0xFF;
    quads[16].rgbBlue = 0x80;

    quads[17].rgbRed = 0x80;
    quads[17].rgbGreen = 0x80;
    quads[17].rgbBlue = 0x40;

    quads[18].rgbRed = 0x80;
    quads[18].rgbGreen = 0xFF;
    quads[18].rgbBlue = 0x80;

    quads[19].rgbRed = 0x00;
    quads[19].rgbGreen = 0x40;
    quads[19].rgbBlue = 0x40;

    quads[20].rgbRed = 0x80;
    quads[20].rgbGreen = 0xFF;
    quads[20].rgbBlue = 0xFF;

    quads[21].rgbRed = 0x00;
    quads[21].rgbGreen = 0x80;
    quads[21].rgbBlue = 0xFF;

    quads[22].rgbRed = 0x80;
    quads[22].rgbGreen = 0x80;
    quads[22].rgbBlue = 0xFF;

    quads[23].rgbRed = 0x00;
    quads[23].rgbGreen = 0x40;
    quads[23].rgbBlue = 0x80;

    quads[24].rgbRed = 0xFF;
    quads[24].rgbGreen = 0x00;
    quads[24].rgbBlue = 0x80;

    quads[25].rgbRed = 0x40;
    quads[25].rgbGreen = 0x00;
    quads[25].rgbBlue = 0x80;

    quads[26].rgbRed = 0xFF;
    quads[26].rgbGreen = 0x80;
    quads[26].rgbBlue = 0x40;

    quads[27].rgbRed = 0x80;
    quads[27].rgbGreen = 0x40;
    quads[27].rgbBlue = 0x00;

    for( i = 0; i < PALETTE_SIZE; i++ ) {
        palette[COLOR_16][i] = quads[i];
        restorePalette[i] = quads[i];
    }

    /*
     * initialize the 256 color palette the same as the 16 color
     */
    for( i = 0; i < PALETTE_SIZE; i++ ) {
        palette[COLOR_256][i] = quads[i];
        restorePalette[i] = quads[i];
    }

    /*
     * Brain dead method ... but the most convenient right now!
     */
    solids[0] = quads[0];
    solids[1] = quads[1];
    solids[2] = quads[2];
    solids[3] = quads[3];
    solids[4] = quads[4];
    solids[5] = quads[11];
    solids[6] = quads[6];
    solids[7] = quads[13];
    solids[8] = quads[8];
    solids[9] = quads[15];
    solids[10] = quads[10];
    solids[11] = quads[5];
    solids[12] = quads[12];
    solids[13] = quads[7];
    solids[14] = quads[14];
    solids[15] = quads[9];

    quads[0].rgbRed = 255;
    quads[1].rgbRed = 0;
    quads[2].rgbRed = 250;
    quads[3].rgbRed = 9;
    quads[4].rgbRed = 242;
    quads[5].rgbRed = 18;
    quads[6].rgbRed = 226;
    quads[7].rgbRed = 33;
    quads[8].rgbRed = 208;
    quads[9].rgbRed = 50;
    quads[10].rgbRed = 194;
    quads[11].rgbRed = 64;
    quads[12].rgbRed = 176;
    quads[13].rgbRed = 82;
    quads[14].rgbRed = 159;
    quads[15].rgbRed = 97;
    quads[16].rgbRed = 130;
    quads[17].rgbRed = 72;
    quads[18].rgbRed = 174;
    quads[19].rgbRed = 81;
    quads[20].rgbRed = 165;
    quads[21].rgbRed = 90;
    quads[22].rgbRed = 156;
    quads[23].rgbRed = 99;
    quads[24].rgbRed = 147;
    quads[25].rgbRed = 108;
    quads[26].rgbRed = 138;
    quads[27].rgbRed = 117;

    for( i = 0; i < PALETTE_SIZE; i++ ) {
        palette[COLOR_2][i].rgbRed = quads[i].rgbRed;
        palette[COLOR_2][i].rgbBlue = quads[i].rgbRed;
        palette[COLOR_2][i].rgbGreen = quads[i].rgbRed;
    }

} /* InitPalette */

/*
 * SetBoxColors - set the values of the color boxes
 *              - all coordinates are using the Windows convention of the
 *                top left as the origin
 */
void SetBoxColors( palette_box *screen, palette_box *inverse,
                   int num_colors, palette_box *avail_colors )
{
    short               i;
    wie_clrtype         type;
    WPI_PRES            pres;
    COLORREF            rgbcolor;

    leftColorIndex = getColorIndex( GetSelectedColor( LMOUSEBUTTON, NULL, &type ) );
    rightColorIndex = getColorIndex( GetSelectedColor( RMOUSEBUTTON, NULL, &type ) );
    chooseThePalette( num_colors );

    /*
     * First initialize the screen color boxes.
     */
    screen->box.left = SCRN_COL_X;
    screen->box.right = SCRN_COL_X + SQR_SIZE + 1;
    screen->box.top = SCRN_COL_Y;
    screen->box.bottom = SCRN_COL_Y + SQR_SIZE + 1;

    inverse->box.left = SCRN_COL_X;
    inverse->box.right = SCRN_COL_X + SQR_SIZE + 1;
    inverse->box.top = SCRN_COL_Y + SQR_SIZE;
    inverse->box.bottom = SCRN_COL_Y + 2 * SQR_SIZE + 1;

    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );

    for( i = 0; i < PALETTE_SIZE; i += 2 ) {
        rgbcolor = RGB( palette[currentPalIndex][i].rgbRed,
                        palette[currentPalIndex][i].rgbGreen,
                        palette[currentPalIndex][i].rgbBlue );
        avail_colors[i].color = _wpi_getcolorindex( pres, rgbcolor );
        if( num_colors == 2 ) {
            avail_colors[i].solid_color = WHITE;
        } else {
            avail_colors[i].solid_color = _wpi_getnearestcolor( pres, rgbcolor );
        }
        avail_colors[i].box.left = (i / 2) * SQR_SIZE;
        avail_colors[i].box.top = 0;
        avail_colors[i].box.right = SQR_SIZE + (i / 2) * SQR_SIZE + 1;
        avail_colors[i].box.bottom = SQR_SIZE + 1;

        rgbcolor = RGB( palette[currentPalIndex][i + 1].rgbRed,
                        palette[currentPalIndex][i + 1].rgbGreen,
                        palette[currentPalIndex][i + 1].rgbBlue );
        avail_colors[i + 1].color = _wpi_getcolorindex( pres, rgbcolor );
        if( num_colors == 2 ) {
            avail_colors[i + 1].solid_color = BLACK;
        } else {
            avail_colors[i + 1].solid_color = _wpi_getnearestcolor( pres, rgbcolor );
        }
        avail_colors[i + 1].box.left = avail_colors[i].box.left;
        avail_colors[i + 1].box.top = SQR_SIZE;
        avail_colors[i + 1].box.right = avail_colors[i].box.right;
        avail_colors[i + 1].box.bottom = SQR_SIZE * 2 + 1;
    }
    _wpi_releasepres( HWND_DESKTOP, pres );

} /* SetBoxColors */

/*
 * DisplayColorBox - display the colors on the given device context
 *                 - PM Note: we assume the presentation space is already in RGB mode
 */
void DisplayColorBox( WPI_PRES pres, palette_box *box )
{
    int                 top;
    int                 bottom;
    HBRUSH              hcolorbrush;
    HBRUSH              holdbrush;
    HPEN                holdpen;
    HPEN                blackpen;
    int                 height;

    blackpen = _wpi_createpen( PS_SOLID, 0, BLACK );
    holdpen = _wpi_selectobject( pres, blackpen );
    hcolorbrush = _wpi_createsolidbrush( box->color );
    holdbrush = _wpi_selectobject( pres, hcolorbrush );

    height = 2 * SQR_SIZE + 1;
    top = _wpi_cvth_y( box->box.top, height );
    bottom = _wpi_cvth_y( box->box.bottom, height );
    _wpi_rectangle( pres, box->box.left, top, box->box.right, bottom );

    _wpi_selectobject( pres, holdbrush );
    _wpi_deleteobject( hcolorbrush );
    _wpi_selectobject( pres, holdpen );
    _wpi_deleteobject( blackpen );

} /* DisplayColorBox */

/*
 * GetInverseColor - return the inverse color
 *                 - simply performing a bitwise NOT doesn't do the trick
 *                   so this method is employed
 */
COLORREF GetInverseColor( COLORREF color )
{
    int         i;
    int         num_colors;
    COLORREF    solidcolor;
    COLORREF    newcolor;
    WPI_PRES    pres;

    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );
    /*
     * change this so that we check the number of colors the machine has
     */
    num_colors = 16;
    solidcolor = _wpi_getnearestcolor( pres, color );
    _wpi_releasepres( HWND_DESKTOP, pres );

    for( i = 0; i < num_colors; i++ ) {
        if( solidcolor == RGB( solids[i].rgbRed, solids[i].rgbBlue, solids[i].rgbGreen ) ) {
            break;
        }
    }

    switch( i ) {
    case 0:
    case 2:
    case 4:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
        newcolor = RGB( solids[i + 1].rgbRed, solids[i + 1].rgbBlue,
                        solids[i + 1].rgbGreen );
        return( RGB( solids[i + 1].rgbRed, solids[i + 1].rgbBlue, solids[i + 1].rgbGreen ) );

    case 1:
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15:
        newcolor = RGB( solids[i - 1].rgbRed, solids[i - 1].rgbBlue,
                        solids[i - 1].rgbGreen );
        return( RGB( solids[i - 1].rgbRed, solids[i - 1].rgbBlue, solids[i - 1].rgbGreen ) );
    default:
        return( WHITE );
    }

} /* GetInverseColor */

/*
 * ReplacePaletteEntry - replace the current entry with the given RGB value
 */
void ReplacePaletteEntry( COLORREF newcolor )
{
    COLORREF            cur_color;
    COLORREF            nearestcolor;
    COLORREF            screencolor;
    COLORREF            rightcolor;
    wie_clrtype         lefttype;
    wie_clrtype         righttype;
    int                 index;
    int                 rightindex;
    WPI_PRES            pres;

    if( currentPalIndex != COLOR_16 && currentPalIndex != COLOR_256 ) {
        return;
    }
    cur_color = GetSelectedColor( LMOUSEBUTTON, NULL, &lefttype );

    if( lefttype == NORMAL_CLR ) {
        index = getColorIndex( cur_color );

        rightcolor = GetSelectedColor( RMOUSEBUTTON, NULL, &righttype );
        rightindex = getColorIndex( rightcolor );

        palette[currentPalIndex][index].rgbRed = GetRValue( newcolor );
        palette[currentPalIndex][index].rgbGreen = GetGValue( newcolor );
        palette[currentPalIndex][index].rgbBlue = GetBValue( newcolor );
        palette[currentPalIndex][index].rgbReserved = 0;

        ShowNewColor( index, newcolor, TRUE );
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        SetColor( LMOUSEBUTTON, newcolor, _wpi_getnearestcolor( pres, newcolor ), NORMAL_CLR );
        if( index == rightindex ) {
            SetColor( RMOUSEBUTTON, newcolor, _wpi_getnearestcolor( pres, newcolor ),
                      NORMAL_CLR );
        }
        _wpi_releasepres( HWND_DESKTOP, pres );
    } else if( lefttype == SCREEN_CLR ) {
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        nearestcolor = _wpi_getnearestcolor( pres, newcolor );
        _wpi_releasepres( HWND_DESKTOP, pres );
        SetScreenClr( nearestcolor );
    } else {
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        nearestcolor = _wpi_getnearestcolor( pres, newcolor );
        _wpi_releasepres( HWND_DESKTOP, pres );

        screencolor = GetInverseColor( nearestcolor );
        SetScreenClr( screencolor );
    }

} /* ReplacePaletteEntry */

/*
 * ResetColorPalette - reset the color palette to its original form (the way it
 *                     is when the program is first executed or the way it was
 *                     most recently loaded) in response to hitting the Restore button
 */
void ResetColorPalette( void )
{
    int         i;
    COLORREF    leftcolor;
    COLORREF    rightcolor;
    COLORREF    color;
    WPI_PRES    pres;
    int         leftindex;
    int         rightindex;
    wie_clrtype lefttype;
    wie_clrtype righttype;

    if( currentPalIndex != COLOR_16 && currentPalIndex != COLOR_256 ) {
        return;
    }
    leftcolor = GetSelectedColor( LMOUSEBUTTON, NULL, &lefttype );
    rightcolor = GetSelectedColor( RMOUSEBUTTON, NULL, &righttype );

    leftindex = getColorIndex( leftcolor );
    rightindex = getColorIndex( rightcolor );

    for( i = 0; i < PALETTE_SIZE - 1; i++ ) {
        palette[currentPalIndex][i] = restorePalette[i];
        ShowNewColor( i, RGB( palette[currentPalIndex][i].rgbRed,
                              palette[currentPalIndex][i].rgbGreen,
                              palette[currentPalIndex][i].rgbBlue ), FALSE );
    }
    /*
     * We do the last one separately to invalidate the window.
     */
    palette[currentPalIndex][i] = restorePalette[i];
    ShowNewColor( i, RGB( palette[currentPalIndex][i].rgbRed,
                          palette[currentPalIndex][i].rgbGreen,
                          palette[currentPalIndex][i].rgbBlue ), TRUE );

    if( lefttype == NORMAL_CLR ) {
        color = RGB( palette[currentPalIndex][leftindex].rgbRed,
                     palette[currentPalIndex][leftindex].rgbGreen,
                     palette[currentPalIndex][leftindex].rgbBlue );
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        SetColor( LMOUSEBUTTON, color, _wpi_getnearestcolor( pres, color ), NORMAL_CLR );
        _wpi_releasepres( HWND_DESKTOP, pres );
    }
    if( righttype == NORMAL_CLR ) {
        color = RGB( palette[currentPalIndex][rightindex].rgbRed,
                     palette[currentPalIndex][rightindex].rgbGreen,
                     palette[currentPalIndex][rightindex].rgbBlue );
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        SetColor( RMOUSEBUTTON, color, _wpi_getnearestcolor( pres, color ), NORMAL_CLR );
        _wpi_releasepres( HWND_DESKTOP, pres );
    }

} /* ResetColorPalette */

/*
 * SetCurrentColors - set the current color selection (we need to be
 *                    careful that the image supports the current palette)
 */
void SetCurrentColors( BOOL fshowscreenclrs )
{
    COLORREF    leftcolor;
    COLORREF    rightcolor;
    COLORREF    solid;
    wie_clrtype type;
    WPI_PRES    pres;

    leftcolor = GetSelectedColor( LMOUSEBUTTON, NULL, &type );
    if( type == NORMAL_CLR ) {
        leftcolor = GetPaletteColor( leftColorIndex );
        if( currentPalIndex == COLOR_16 || currentPalIndex == COLOR_256 ) {
            pres = _wpi_getpres( HWND_DESKTOP );
            _wpi_torgbmode( pres );
            solid = _wpi_getnearestcolor( pres, leftcolor );
            _wpi_releasepres( HWND_DESKTOP, pres );
        } else if( currentPalIndex == COLOR_2 &&
                   leftColorIndex / 2 == (leftColorIndex + 1) / 2 ) {
            solid = WHITE;
        } else {
            solid = BLACK;
        }
        SetColor( LMOUSEBUTTON, leftcolor, solid, NORMAL_CLR );
    } else if( !fshowscreenclrs ) {
        leftcolor = GetPaletteColor( 1 );
        if( currentPalIndex == COLOR_16 || currentPalIndex == COLOR_256 ) {
            pres = _wpi_getpres( HWND_DESKTOP );
            _wpi_torgbmode( pres );
            solid = _wpi_getnearestcolor( pres, leftcolor );
            _wpi_releasepres( HWND_DESKTOP, pres );
        } else if( currentPalIndex == COLOR_2 ) {
            solid = BLACK;
        }
        SetColor( LMOUSEBUTTON, leftcolor, solid, NORMAL_CLR );
    } else {
        SetColor( LMOUSEBUTTON, leftcolor, leftcolor, type );
    }

    rightcolor = GetSelectedColor( RMOUSEBUTTON, NULL, &type );
    if( type == NORMAL_CLR ) {
        rightcolor = GetPaletteColor( rightColorIndex );
        if( currentPalIndex == COLOR_16 || currentPalIndex == COLOR_256 ) {
            pres = _wpi_getpres( HWND_DESKTOP );
            _wpi_torgbmode( pres );
            solid = _wpi_getnearestcolor( pres, rightcolor );
            _wpi_releasepres( HWND_DESKTOP, pres );
        } else if( currentPalIndex == COLOR_2 &&
                   rightColorIndex / 2 == (rightColorIndex + 1) / 2 ) {
            solid = WHITE;
        } else {
            solid = BLACK;
        }
        SetColor( RMOUSEBUTTON, rightcolor, solid, NORMAL_CLR );
    } else if( !fshowscreenclrs ) {
        rightcolor = GetPaletteColor( 0 );
        if( currentPalIndex == COLOR_16 || currentPalIndex == COLOR_256 ) {
            pres = _wpi_getpres( HWND_DESKTOP );
            _wpi_torgbmode( pres );
            solid = _wpi_getnearestcolor( pres, rightcolor );
            _wpi_releasepres( HWND_DESKTOP, pres );
        } else if( currentPalIndex == COLOR_2 ) {
            solid = WHITE;
        }
        SetColor( RMOUSEBUTTON, rightcolor, solid, NORMAL_CLR );
    } else {
        SetColor( RMOUSEBUTTON, rightcolor, rightcolor, type );
    }

} /* SetCurrentColors */

/*
 * GetPaletteColor - return the COLORREF associated with the given index
 */
COLORREF GetPaletteColor( int index )
{
    COLORREF    color;

    color = RGB( palette[currentPalIndex][index].rgbRed,
                 palette[currentPalIndex][index].rgbGreen,
                 palette[currentPalIndex][index].rgbBlue );
    return( color );

} /* GetPaletteColor */

/*
 * InitFromColorPalette - set the values of the color boxes in the dialog box
 */
void InitFromColorPalette( palette_box *screen, palette_box *inverse,
                           palette_box *avail_colors )
{
    short               i;
    short               left;

    /*
     * For now we assume they can choose from only 16 colors!
     * First initialize the screen color boxes.
     */
    left = 76;
    screen->box.left = 3;
    screen->box.right = SQR_SIZE + 4;
    screen->box.top = 3;
    screen->box.bottom = SQR_SIZE + 4;

    inverse->box.left = 3;
    inverse->box.right = SQR_SIZE + 4;
    inverse->box.top = SQR_SIZE + 3;
    inverse->box.bottom = 2 * SQR_SIZE + 4;

    for( i = 0; i < 16; i += 2 ) {
        avail_colors[i].color = RGB( solids[i].rgbRed, solids[i].rgbGreen,
                                     solids[i].rgbBlue );
        avail_colors[i].box.left = left + (i / 2) * SQR_SIZE;
        avail_colors[i].box.top = screen->box.top;
        avail_colors[i].box.right = left + SQR_SIZE + (i / 2) * SQR_SIZE + 1;
        avail_colors[i].box.bottom = screen->box.bottom;

        avail_colors[i + 1].color = RGB( solids[i + 1].rgbRed, solids[i + 1].rgbGreen,
                                          solids[i + 1].rgbBlue );
        avail_colors[i + 1].box.left = avail_colors[i].box.left;
        avail_colors[i + 1].box.top = avail_colors[i].box.bottom - 1;
        avail_colors[i + 1].box.right = avail_colors[i].box.right;
        avail_colors[i + 1].box.bottom = avail_colors[i].box.bottom + SQR_SIZE + 1;
    }
}

/*
 * GetPaletteFile - set up the palette file
 *                - since it isn't documented (?!), I got this from a hex dump of a file
 *                  created by other image editors (MS)
 */
BOOL GetPaletteFile( a_pal_file *pal_file )
{
    int                 i;

    if( currentPalIndex != COLOR_16 && currentPalIndex != COLOR_256 ) {
        return( FALSE );
    }

    pal_file->file_id = PALETTE_FILE;
    for( i = 0; i < 12; i++ ) {
        pal_file->reserved[i] = 0;
    }

    for( i = 0; i < PALETTE_SIZE; i++ ) {
        /*
         * I don't know why they do this but it seems to work ?!
         */
        pal_file->rgbqs[i].rgbBlue = palette[currentPalIndex][i].rgbRed;
        pal_file->rgbqs[i].rgbRed = palette[currentPalIndex][i].rgbBlue;
        pal_file->rgbqs[i].rgbGreen = palette[currentPalIndex][i].rgbGreen;
        pal_file->rgbqs[i].rgbReserved = 0;
    }

    return( TRUE );

} /* GetPaletteFile */

/*
 * SetNewPalette - set a new palette from a loaded file
 */
void SetNewPalette( a_pal_file *pal_file )
{
    int         i;
    wie_clrtype type;
    HMENU       hmenu;

    if( currentPalIndex != COLOR_16 && currentPalIndex != COLOR_256 ) {
        return;
    }

    leftColorIndex = getColorIndex( GetSelectedColor( LMOUSEBUTTON, NULL, &type ) );
    rightColorIndex = getColorIndex( GetSelectedColor( RMOUSEBUTTON, NULL, &type ) );

    for( i = 0; i < PALETTE_SIZE - 1; i++ ) {
        palette[currentPalIndex][i].rgbRed = pal_file->rgbqs[i].rgbBlue;
        palette[currentPalIndex][i].rgbGreen = pal_file->rgbqs[i].rgbGreen;
        palette[currentPalIndex][i].rgbBlue = pal_file->rgbqs[i].rgbRed;
        ShowNewColor( i, RGB( palette[currentPalIndex][i].rgbRed,
                              palette[currentPalIndex][i].rgbGreen,
                              palette[currentPalIndex][i].rgbBlue ), FALSE );
        restorePalette[i] = palette[currentPalIndex][i];
    }
    palette[currentPalIndex][i].rgbRed = pal_file->rgbqs[i].rgbBlue;
    palette[currentPalIndex][i].rgbGreen = pal_file->rgbqs[i].rgbGreen;
    palette[currentPalIndex][i].rgbBlue = pal_file->rgbqs[i].rgbRed;
    restorePalette[i] = palette[currentPalIndex][i];

    ShowNewColor( i, RGB( palette[currentPalIndex][i].rgbRed,
                          palette[currentPalIndex][i].rgbGreen,
                          palette[currentPalIndex][i].rgbBlue ), TRUE );
    SetCurrentColors( TRUE );

    if( HMainWindow != NULL ) {
        hmenu = GetMenu( _wpi_getframe( HMainWindow ) );
        _wpi_enablemenuitem( hmenu, IMGED_RCOLOR, TRUE, FALSE );
    }

} /* SetNewPalette */

/*
 * RestoreColorPalette - restore the color palette to the hard-coded colors
 *                       in the initialization routine
 */
void RestoreColorPalette( void )
{
    int         i;
    COLORREF    leftcolor;
    COLORREF    rightcolor;
    COLORREF    color;
    WPI_PRES    pres;
    int         leftindex;
    int         rightindex;
    wie_clrtype lefttype;
    wie_clrtype righttype;
    HMENU       hmenu;
    HWND        frame;

    if( currentPalIndex != COLOR_16 && currentPalIndex != COLOR_256 ) {
        return;
    }
    leftcolor = GetSelectedColor( LMOUSEBUTTON, NULL, &lefttype );
    rightcolor = GetSelectedColor( RMOUSEBUTTON, NULL, &righttype );

    leftindex = getColorIndex( leftcolor );
    rightindex = getColorIndex( rightcolor );

    InitPalette();
    for( i = 0; i < PALETTE_SIZE - 1; i++ ) {
        ShowNewColor( i, RGB( palette[currentPalIndex][i].rgbRed,
                              palette[currentPalIndex][i].rgbGreen,
                              palette[currentPalIndex][i].rgbBlue ), FALSE );
    }
    /*
     * We do the last one separately to invalidate the window.
     */
    ShowNewColor( i, RGB( palette[currentPalIndex][i].rgbRed,
                          palette[currentPalIndex][i].rgbGreen,
                          palette[currentPalIndex][i].rgbBlue ), TRUE );

    if( lefttype == NORMAL_CLR ) {
        color = RGB( palette[currentPalIndex][leftindex].rgbRed,
                     palette[currentPalIndex][leftindex].rgbGreen,
                     palette[currentPalIndex][leftindex].rgbBlue );
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        SetColor( LMOUSEBUTTON, color, _wpi_getnearestcolor( pres, color ), NORMAL_CLR );
        _wpi_releasepres( HWND_DESKTOP, pres );
    }
    if( righttype == NORMAL_CLR ) {
        color = RGB( palette[currentPalIndex][rightindex].rgbRed,
                     palette[currentPalIndex][rightindex].rgbGreen,
                     palette[currentPalIndex][rightindex].rgbBlue );
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        SetColor( RMOUSEBUTTON, color, _wpi_getnearestcolor( pres, color ), NORMAL_CLR );
        _wpi_releasepres( HWND_DESKTOP, pres );
    }

    PrintHintTextByID( WIE_PALETTERESTORED, NULL );

    if( HMainWindow != NULL ) {
        frame = _wpi_getframe( _wpi_getframe( HMainWindow ) );
        hmenu = GetMenu( frame );
        _wpi_enablemenuitem( hmenu, IMGED_RCOLOR, FALSE, FALSE );
    }

} /* RestoreColorPalette */

/*
 * InitPaletteBitmaps - initialize the available color bitmaps
 */
void InitPaletteBitmaps( HWND hwnd, HBITMAP *colorbitmap, HBITMAP *monobitmap )
{
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HDC         hdc;
    HBITMAP     oldbitmap;
    COLORREF    color;
    int         i;
    int         left_sqr;
    HBRUSH      colorbrush;
    HBRUSH      oldbrush;
    HPEN        blackpen;
    HPEN        oldpen;
    int         top;
    int         bottom;
    int         height;

    pres = _wpi_getpres( hwnd );

    *colorbitmap = _wpi_createcompatiblebitmap( pres, CUR_BMP_WIDTH, CUR_BMP_HEIGHT );
    *monobitmap = _wpi_createbitmap( CUR_BMP_WIDTH, CUR_BMP_HEIGHT, 1, 1, NULL );
    mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );

    _wpi_releasepres( hwnd, pres );
    _wpi_torgbmode( mempres );

    blackpen = _wpi_createpen( PS_SOLID, 0, BLACK );
    oldpen = _wpi_selectobject( mempres, blackpen );
    oldbitmap = _wpi_selectobject( mempres, *colorbitmap );

    /*
     * PM NOTE: All box coordinates are relative to the window's origin
     * of top left.  So we convert the height for PM.
     */
    left_sqr = 0;
    height = 2 * SQR_SIZE + 1;
    for( i = 0; i < PALETTE_SIZE; i += 2 ) {
        color = RGB( palette[COLOR_16][i].rgbRed, palette[COLOR_16][i].rgbGreen,
                     palette[COLOR_16][i].rgbBlue );
        colorbrush = _wpi_createsolidbrush( color );
        oldbrush = _wpi_selectobject( mempres, colorbrush );

        top = _wpi_cvth_y( 0, height );
        bottom = _wpi_cvth_y( SQR_SIZE + 1, height );

        _wpi_rectangle( mempres, left_sqr, top, left_sqr + SQR_SIZE + 1, bottom );
        _wpi_selectobject( mempres, oldbrush );
        _wpi_deleteobject( colorbrush );
        color = RGB( palette[COLOR_16][i + 1].rgbRed, palette[COLOR_16][i + 1].rgbGreen,
                     palette[COLOR_16][i + 1].rgbBlue );
        colorbrush = _wpi_createsolidbrush( color );
        oldbrush = _wpi_selectobject( mempres, colorbrush );

        top = _wpi_cvth_y( SQR_SIZE, height );
        bottom = _wpi_cvth_y( 2 * SQR_SIZE + 1, height );

        _wpi_rectangle( mempres, left_sqr, top, left_sqr + SQR_SIZE + 1, bottom );
        _wpi_selectobject( mempres, oldbrush );
        _wpi_deleteobject( colorbrush );

        left_sqr += SQR_SIZE;
    }

    _wpi_selectobject( mempres, oldbitmap );
    oldbitmap = _wpi_selectobject( mempres, *monobitmap );

    left_sqr = 0;
    for( i = 0; i < PALETTE_SIZE; i += 2 ) {
        color = RGB( palette[COLOR_2][i].rgbRed, palette[COLOR_2][i].rgbGreen,
                     palette[COLOR_2][i].rgbBlue );
        colorbrush = _wpi_createsolidbrush( color );
        oldbrush = _wpi_selectobject( mempres, colorbrush );

        top = _wpi_cvth_y( 0, height );
        bottom = _wpi_cvth_y( SQR_SIZE + 1, height );

        _wpi_rectangle( mempres, left_sqr, top, left_sqr + SQR_SIZE + 1, bottom );
        _wpi_selectobject( mempres, oldbrush );
        _wpi_deleteobject( colorbrush );

        color = RGB( palette[COLOR_2][i + 1].rgbRed, palette[COLOR_2][i + 1].rgbGreen,
                     palette[COLOR_2][i + 1].rgbBlue );
        colorbrush = _wpi_createsolidbrush( color );
        oldbrush = _wpi_selectobject( mempres, colorbrush );

        top = _wpi_cvth_y( SQR_SIZE, height );
        bottom = _wpi_cvth_y( 2 * SQR_SIZE + 1, height );

        _wpi_rectangle( mempres, left_sqr, top, left_sqr + SQR_SIZE + 1, bottom );
        _wpi_selectobject( mempres, oldbrush );
        _wpi_deleteobject( colorbrush );

        left_sqr += SQR_SIZE;
    }
    _wpi_selectobject( mempres, oldbitmap );
    _wpi_selectobject( mempres, oldpen );
    _wpi_deletecompatiblepres( mempres, hdc );
    _wpi_deleteobject( blackpen );

} /* InitPaletteBitmaps */
