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
#include "colours.h"

static RGBQUAD          Palette[NUM_PALETTES][PALETTE_SIZE];
static RGBQUAD          restorePalette[PALETTE_SIZE];
static RGBQUAD          Solids[ 16 ];   // maybe vary this later
static int              currentPalIndex = 0;
static int              leftColourIndex;
static int              rightColourIndex;

/*
 * chooseThePalette - checks the number of colours and decides which palette
 *                      we need.
 */
void chooseThePalette( int num_of_colours )
{
    switch (num_of_colours)
    {
    case 2:
        currentPalIndex = COLOUR_2;
        break;
    case 16:
        currentPalIndex = COLOUR_16;
        break;
    case 256:
        currentPalIndex = COLOUR_256;
        break;
    }
} /* chooseThePalette */

/*
 * getColourIndex - given a colour, this routine returns the index of that
 *                  colour in the current palette.  It returns -1 if the
 *                  colour is not found in the palette.
 */
int getColourIndex( COLORREF colour )
{
    int         i;

    for (i=0; i < PALETTE_SIZE; ++i) {
        if ( colour == RGB( Palette[currentPalIndex][i].rgbRed,
                            Palette[currentPalIndex][i].rgbGreen,
                            Palette[currentPalIndex][i].rgbBlue ) ) {
            return( i );
        }
    }
    return ( -1 );

} /* getColourIndex */

/*
 * InitPalette - Initializes the colour palette.
 *                      Palette[0] is the 16 colour palette
 *                      Palette[1] is the 2 colour palette
 *                      Palette[2] is the 256 colour palette
 */
void InitPalette( void )
{
    short               i;
    RGBQUAD             quads[PALETTE_SIZE];

    for (i=0; i < PALETTE_SIZE; ++i) {
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

    for (i=0; i < PALETTE_SIZE; ++i) {
        Palette[COLOUR_16][i] = quads[i];
        restorePalette[i] = quads[i];
    }

    /*
     * initialize the 256 colour palette the same as the 16 colour
     */
    for (i=0; i < PALETTE_SIZE; ++i) {
        Palette[COLOUR_256][i] = quads[i];
        restorePalette[i] = quads[i];
    }

    /*
     * Brain dead method ... but the most convenient right now!
     */
    Solids[0] = quads[0];
    Solids[1] = quads[1];
    Solids[2] = quads[2];
    Solids[3] = quads[3];
    Solids[4] = quads[4];
    Solids[5] = quads[11];
    Solids[6] = quads[6];
    Solids[7] = quads[13];
    Solids[8] = quads[8];
    Solids[9] = quads[15];
    Solids[10] = quads[10];
    Solids[11] = quads[5];
    Solids[12] = quads[12];
    Solids[13] = quads[7];
    Solids[14] = quads[14];
    Solids[15] = quads[9];

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

    for (i=0; i < PALETTE_SIZE; i += 1) {
        Palette[COLOUR_2][i].rgbRed = quads[i].rgbRed;
        Palette[COLOUR_2][i].rgbBlue = quads[i].rgbRed;
        Palette[COLOUR_2][i].rgbGreen = quads[i].rgbRed;
    }

} /* InitPalette */

/*
 * SetBoxColours - Sets the values of the colour boxes.  NOTE that all
 *                 coordinates are using the windows convention of the
 *                 top left as the origin.
 */
void SetBoxColours( palette_box *screen, palette_box *inverse,
                int num_colours, palette_box *avail_colours )
{
    short               i;
    wie_clrtype         type;
    WPI_PRES            pres;
    COLORREF            rgbcolour;

    leftColourIndex = getColourIndex(GetSelectedColour(LMOUSEBUTTON, NULL, &type));
    rightColourIndex = getColourIndex(GetSelectedColour(RMOUSEBUTTON, NULL, &type));
    chooseThePalette( num_colours );

    /*
     * First initialize the screen colour boxes.
     */
    screen->box.left = SCRN_COL_X;
    screen->box.right = SCRN_COL_X + SQR_SIZE + 1;
    screen->box.top = SCRN_COL_Y;
    screen->box.bottom = SCRN_COL_Y + SQR_SIZE + 1;

    inverse->box.left = SCRN_COL_X;
    inverse->box.right = SCRN_COL_X + SQR_SIZE + 1;
    inverse->box.top = SCRN_COL_Y + SQR_SIZE;
    inverse->box.bottom = SCRN_COL_Y + 2*SQR_SIZE + 1;

    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );

    for (i=0; i < PALETTE_SIZE; i += 2) {
        rgbcolour = RGB( Palette[currentPalIndex][i].rgbRed,
                                       Palette[currentPalIndex][i].rgbGreen,
                                       Palette[currentPalIndex][i].rgbBlue );
        avail_colours[i].colour = _wpi_getcolourindex( pres, rgbcolour );
        if (num_colours == 2) {
            avail_colours[i].solid_colour = WHITE;
        } else {
            avail_colours[i].solid_colour = _wpi_getnearestcolor( pres, rgbcolour );
        }
        avail_colours[i].box.left = (i/2)*SQR_SIZE;
        avail_colours[i].box.top = 0;
        avail_colours[i].box.right = SQR_SIZE+(i/2)*SQR_SIZE+1;
        avail_colours[i].box.bottom = SQR_SIZE+1;

        rgbcolour = RGB( Palette[currentPalIndex][i+1].rgbRed,
                                         Palette[currentPalIndex][i+1].rgbGreen,
                                         Palette[currentPalIndex][i+1].rgbBlue );
        avail_colours[i+1].colour = _wpi_getcolourindex( pres, rgbcolour );
        if (num_colours == 2) {
            avail_colours[i+1].solid_colour = BLACK;
        } else {
            avail_colours[i+1].solid_colour = _wpi_getnearestcolor(pres, rgbcolour);
        }
        avail_colours[i+1].box.left = avail_colours[i].box.left;
        avail_colours[i+1].box.top = SQR_SIZE;
        avail_colours[i+1].box.right = avail_colours[i].box.right;
        avail_colours[i+1].box.bottom = SQR_SIZE*2 + 1;

    }
    _wpi_releasepres( HWND_DESKTOP, pres );
} /* SetBoxColours */

/*
 * DisplayColourBox - display the colours on the given DC.
 *                    PM Note: we assume the presentation space is already
 *                    in rgb mode
 */
void DisplayColourBox( WPI_PRES pres, palette_box *box )
{
    int                 top;
    int                 bottom;
    HBRUSH              hcolourbrush;
    HBRUSH              holdbrush;
    HPEN                holdpen;
    HPEN                blackpen;
    int                 height;

    blackpen = _wpi_createpen( PS_SOLID, 0, BLACK );
    holdpen = _wpi_selectobject( pres, blackpen );
    hcolourbrush = _wpi_createsolidbrush( box->colour );
    holdbrush = _wpi_selectobject( pres, hcolourbrush );

    height = 2* SQR_SIZE + 1;
    top = _wpi_cvth_y( box->box.top, height );
    bottom = _wpi_cvth_y( box->box.bottom, height );
    _wpi_rectangle( pres, box->box.left, top, box->box.right, bottom );

    _wpi_selectobject( pres, holdbrush );
    _wpi_deleteobject( hcolourbrush );
    _wpi_selectobject( pres, holdpen );
    _wpi_deleteobject( blackpen );
} /* DisplayColourBox */

/*
 * GetInverseColour - Returns the inverse colour.  Simply performing a bitwise
 *                    NOT doesn't do the trick so this method is employed.
 */
COLORREF GetInverseColour( COLORREF colour )
{
    int         i;
    int         num_colours;
    COLORREF    solidcolour;
    COLORREF    newcolour;
    WPI_PRES    pres;

    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );
    /*
     * change this so that we check the number of colours the machine has
     */
    num_colours = 16;
    solidcolour = _wpi_getnearestcolor( pres, colour );
    _wpi_releasepres( HWND_DESKTOP, pres );

    for (i=0; i < num_colours; ++i) {
        if ( solidcolour == RGB( Solids[i].rgbRed, Solids[i].rgbBlue,
                                        Solids[i].rgbGreen ) ) {
            break;
        }
    }

    switch (i) {
    case 0:
    case 2:
    case 4:
    case 6:
    case 8:
    case 10:
    case 12:
    case 14:
        newcolour = RGB(Solids[i+1].rgbRed, Solids[i+1].rgbBlue, Solids[i+1].rgbGreen);
        return( RGB(Solids[i+1].rgbRed, Solids[i+1].rgbBlue, Solids[i+1].rgbGreen) );

    case 1:
    case 3:
    case 5:
    case 7:
    case 9:
    case 11:
    case 13:
    case 15:
        newcolour = RGB(Solids[i-1].rgbRed, Solids[i-1].rgbBlue, Solids[i-1].rgbGreen);
        return( RGB(Solids[i-1].rgbRed, Solids[i-1].rgbBlue, Solids[i-1].rgbGreen) );
    default:
        return (WHITE);
    }

} /* GetInverseColour */

/*
 * ReplacePaletteEntry - replace the current entry with the given RGB value
 */
void ReplacePaletteEntry( COLORREF newcolour )
{
    COLORREF            cur_colour;
    COLORREF            nearestcolour;
    COLORREF            screencolour;
    COLORREF            rightcolour;
    wie_clrtype         lefttype;
    wie_clrtype         righttype;
    int                 index;
    int                 rightindex;
    WPI_PRES            pres;

    if (currentPalIndex != COLOUR_16 && currentPalIndex != COLOUR_256) {
        return;
    }
    cur_colour = GetSelectedColour( LMOUSEBUTTON, NULL, &lefttype );

    if (lefttype == NORMAL_CLR) {
        index = getColourIndex( cur_colour );

        rightcolour = GetSelectedColour( RMOUSEBUTTON, NULL, &righttype );
        rightindex = getColourIndex( rightcolour );

        Palette[currentPalIndex][index].rgbRed = GetRValue( newcolour );
        Palette[currentPalIndex][index].rgbGreen = GetGValue( newcolour );
        Palette[currentPalIndex][index].rgbBlue = GetBValue( newcolour );
        Palette[currentPalIndex][index].rgbReserved = 0;

        ShowNewColour( index, newcolour, TRUE );
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        SetColour( LMOUSEBUTTON, newcolour, _wpi_getnearestcolor(pres, newcolour), NORMAL_CLR );
        if (index == rightindex) {
            SetColour( RMOUSEBUTTON, newcolour,
                        _wpi_getnearestcolor(pres, newcolour), NORMAL_CLR );
        }
        _wpi_releasepres( HWND_DESKTOP, pres );
    } else if (lefttype == SCREEN_CLR) {
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        nearestcolour = _wpi_getnearestcolor(pres, newcolour);
        _wpi_releasepres( HWND_DESKTOP, pres );
        SetScreenClr( nearestcolour );
    } else {
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        nearestcolour = _wpi_getnearestcolor(pres, newcolour);
        _wpi_releasepres( HWND_DESKTOP, pres );

        screencolour = GetInverseColour( nearestcolour );
        SetScreenClr( screencolour );
    }

} /* ReplacePaletteEntry */

/*
 * ResetColourPalette - Resets the colour palette to it's original form (ie
 *                      the way it is when the program is first executed or
 *                      the way it was most recently loaded).  IE, in response
 *                      to hitting the Restore button.
 */
void ResetColourPalette( void )
{
    int         i;
    COLORREF    leftcolour;
    COLORREF    rightcolour;
    COLORREF    colour;
    WPI_PRES    pres;
    int         leftindex;
    int         rightindex;
    wie_clrtype lefttype;
    wie_clrtype righttype;

    if (currentPalIndex != COLOUR_16 && currentPalIndex != COLOUR_256) {
        return;
    }
    leftcolour = GetSelectedColour( LMOUSEBUTTON, NULL, &lefttype );
    rightcolour = GetSelectedColour( RMOUSEBUTTON, NULL, &righttype );

    leftindex = getColourIndex( leftcolour );
    rightindex = getColourIndex( rightcolour );

    for (i=0; i < PALETTE_SIZE-1; ++i) {
        Palette[currentPalIndex][i] = restorePalette[i];
        ShowNewColour( i, RGB(Palette[currentPalIndex][i].rgbRed,
                                Palette[currentPalIndex][i].rgbGreen,
                                Palette[currentPalIndex][i].rgbBlue), FALSE );
    }
    /*
     * We do the last one separately to invalidate the window.
     */
    Palette[currentPalIndex][i] = restorePalette[i];
    ShowNewColour( i, RGB(Palette[currentPalIndex][i].rgbRed,
                        Palette[currentPalIndex][i].rgbGreen,
                        Palette[currentPalIndex][i].rgbBlue), TRUE );

    if (lefttype == NORMAL_CLR) {
        colour = RGB(Palette[currentPalIndex][leftindex].rgbRed,
                     Palette[currentPalIndex][leftindex].rgbGreen,
                     Palette[currentPalIndex][leftindex].rgbBlue);
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        SetColour(LMOUSEBUTTON, colour, _wpi_getnearestcolor(pres, colour),
                                                                NORMAL_CLR);
        _wpi_releasepres( HWND_DESKTOP, pres );
    }
    if (righttype == NORMAL_CLR) {
        colour = RGB(Palette[currentPalIndex][rightindex].rgbRed,
                     Palette[currentPalIndex][rightindex].rgbGreen,
                     Palette[currentPalIndex][rightindex].rgbBlue);
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        SetColour(RMOUSEBUTTON, colour, _wpi_getnearestcolor(pres, colour),
                                                                NORMAL_CLR);
        _wpi_releasepres( HWND_DESKTOP, pres );
    }
} /* ResetColourPalette */

/*
 * SetCurrentColours - sets the current colour selection (we need to be
 *                     careful that the image supports the current palette).
 */
void SetCurrentColours( BOOL fshowscreenclrs )
{
    COLORREF    leftcolour;
    COLORREF    rightcolour;
    COLORREF    solid;
    wie_clrtype type;
    WPI_PRES    pres;

    leftcolour = GetSelectedColour(LMOUSEBUTTON, NULL, &type);
    if (type == NORMAL_CLR) {
        leftcolour = GetPaletteColour( leftColourIndex );
        if (currentPalIndex == COLOUR_16 || currentPalIndex == COLOUR_256) {
            pres = _wpi_getpres( HWND_DESKTOP );
            _wpi_torgbmode( pres );
            solid = _wpi_getnearestcolor(pres, leftcolour);
            _wpi_releasepres( HWND_DESKTOP, pres );
        } else if ( (currentPalIndex == COLOUR_2) &&
                ((leftColourIndex/2) == (leftColourIndex+1)/2) ) {
            solid = WHITE;
        } else {
            solid = BLACK;
        }
        SetColour( LMOUSEBUTTON, leftcolour, solid, NORMAL_CLR );
    } else if (!fshowscreenclrs) {
        leftcolour = GetPaletteColour( 1 );
        if (currentPalIndex == COLOUR_16 || currentPalIndex == COLOUR_256) {
            pres = _wpi_getpres( HWND_DESKTOP );
            _wpi_torgbmode( pres );
            solid = _wpi_getnearestcolor(pres, leftcolour);
            _wpi_releasepres( HWND_DESKTOP, pres );
        } else if (currentPalIndex == COLOUR_2) {
            solid = BLACK;
        }
        SetColour( LMOUSEBUTTON, leftcolour, solid, NORMAL_CLR );
    } else {
        SetColour( LMOUSEBUTTON, leftcolour, leftcolour, type );
    }

    rightcolour = GetSelectedColour(RMOUSEBUTTON, NULL, &type);
    if (type == NORMAL_CLR) {
        rightcolour = GetPaletteColour( rightColourIndex );
        if (currentPalIndex == COLOUR_16 || currentPalIndex == COLOUR_256) {
            pres = _wpi_getpres( HWND_DESKTOP );
            _wpi_torgbmode( pres );
            solid = _wpi_getnearestcolor(pres, rightcolour);
            _wpi_releasepres( HWND_DESKTOP, pres );
        } else if ( (currentPalIndex == COLOUR_2) &&
                ((rightColourIndex/2) == (rightColourIndex+1)/2) ) {
            solid = WHITE;
        } else {
            solid = BLACK;
        }
        SetColour( RMOUSEBUTTON, rightcolour, solid, NORMAL_CLR );
    } else if (!fshowscreenclrs) {
        rightcolour = GetPaletteColour( 0 );
        if (currentPalIndex == COLOUR_16 || currentPalIndex == COLOUR_256) {
            pres = _wpi_getpres( HWND_DESKTOP );
            _wpi_torgbmode( pres );
            solid = _wpi_getnearestcolor(pres, rightcolour);
            _wpi_releasepres( HWND_DESKTOP, pres );
        } else if (currentPalIndex == COLOUR_2) {
            solid = WHITE;
        }
        SetColour( RMOUSEBUTTON, rightcolour, solid, NORMAL_CLR );
    } else {
        SetColour( RMOUSEBUTTON, rightcolour, rightcolour, type );
    }
} /* SetCurrentColours */

/*
 * GetPaletteColour - Returns the COLORREF associated with the given index.
 */
COLORREF GetPaletteColour( int index )
{
    COLORREF    colour;

    colour = RGB( Palette[currentPalIndex][index].rgbRed,
                  Palette[currentPalIndex][index].rgbGreen,
                  Palette[currentPalIndex][index].rgbBlue );
    return( colour );
} /* GetPaletteColour */

/*
 * InitFromColourPalette - Sets the values of the colour boxes in the dialog
 *                              box.
 */
void InitFromColourPalette( palette_box *screen, palette_box *inverse,
                                palette_box *avail_colours )
{
    short               i;
    short               left;

    /*
     * For now we assume they can choose from only 16 colours!
     * First initialize the screen colour boxes.
     */
    left = 76;
    screen->box.left = 3;
    screen->box.right = SQR_SIZE + 4;
    screen->box.top = 3;
    screen->box.bottom = SQR_SIZE + 4;

    inverse->box.left = 3;
    inverse->box.right = SQR_SIZE + 4;
    inverse->box.top = SQR_SIZE + 3;
    inverse->box.bottom = 2*SQR_SIZE + 4;

    for (i=0; i < 16; i += 2) {
        avail_colours[i].colour = RGB( Solids[i].rgbRed,
                                       Solids[i].rgbGreen,
                                       Solids[i].rgbBlue );
        avail_colours[i].box.left = left + (i/2)*SQR_SIZE;
        avail_colours[i].box.top = screen->box.top;
        avail_colours[i].box.right = left + SQR_SIZE+(i/2)*SQR_SIZE+1;
        avail_colours[i].box.bottom = screen->box.bottom;

        avail_colours[i+1].colour = RGB( Solids[i+1].rgbRed,
                                         Solids[i+1].rgbGreen,
                                         Solids[i+1].rgbBlue );
        avail_colours[i+1].box.left = avail_colours[i].box.left;
        avail_colours[i+1].box.top = avail_colours[i].box.bottom-1;
        avail_colours[i+1].box.right = avail_colours[i].box.right;
        avail_colours[i+1].box.bottom = avail_colours[i].box.bottom+SQR_SIZE+1;
    }
}

/*
 * GetPaletteFile - This routine sets up the palette file.  Since it isn't
 *                  documented (?!), I got this from a hex dump of a file
 *                  created by other image editors (MS).
 */
BOOL GetPaletteFile( a_pal_file *pal_file )
{
    int                 i;

    if (currentPalIndex != COLOUR_16 && currentPalIndex != COLOUR_256) {
        return(FALSE);
    }

    pal_file->file_id = PALETTE_FILE;
    for (i=0; i < 12; ++i) {
        pal_file->reserved[i] = 0;
    }

    for (i=0; i < PALETTE_SIZE; ++i) {
        /*
         * I don't know why they do this but it seems to work ?!
         */
        pal_file->rgbqs[i].rgbBlue = Palette[currentPalIndex][i].rgbRed;
        pal_file->rgbqs[i].rgbRed = Palette[currentPalIndex][i].rgbBlue;
        pal_file->rgbqs[i].rgbGreen = Palette[currentPalIndex][i].rgbGreen;
        pal_file->rgbqs[i].rgbReserved = 0;
    }

    return(TRUE);
} /* GetPaletteFile */

/*
 * SetNewPalette - Sets a new palette from a loaded file.
 */
void SetNewPalette( a_pal_file *pal_file )
{
    int         i;
    wie_clrtype type;
    HMENU       hmenu;

    if (currentPalIndex != COLOUR_16 && currentPalIndex != COLOUR_256) {
        return;
    }

    leftColourIndex = getColourIndex(GetSelectedColour(LMOUSEBUTTON, NULL, &type));
    rightColourIndex = getColourIndex(GetSelectedColour(RMOUSEBUTTON, NULL, &type));

    for (i=0; i < PALETTE_SIZE-1; ++i) {
        Palette[currentPalIndex][i].rgbRed = pal_file->rgbqs[i].rgbBlue;
        Palette[currentPalIndex][i].rgbGreen = pal_file->rgbqs[i].rgbGreen;
        Palette[currentPalIndex][i].rgbBlue = pal_file->rgbqs[i].rgbRed;
        ShowNewColour( i, RGB(Palette[currentPalIndex][i].rgbRed,
                                Palette[currentPalIndex][i].rgbGreen,
                                Palette[currentPalIndex][i].rgbBlue), FALSE );
        restorePalette[i] = Palette[currentPalIndex][i];
    }
    Palette[currentPalIndex][i].rgbRed = pal_file->rgbqs[i].rgbBlue;
    Palette[currentPalIndex][i].rgbGreen = pal_file->rgbqs[i].rgbGreen;
    Palette[currentPalIndex][i].rgbBlue = pal_file->rgbqs[i].rgbRed;
    restorePalette[i] = Palette[currentPalIndex][i];

    ShowNewColour( i, RGB(Palette[currentPalIndex][i].rgbRed,
                        Palette[currentPalIndex][i].rgbGreen,
                        Palette[currentPalIndex][i].rgbBlue), TRUE );
    SetCurrentColours( TRUE );

    if (HMainWindow) {
        hmenu = GetMenu( _wpi_getframe(HMainWindow) );
        _wpi_enablemenuitem( hmenu, IMGED_RCOLOUR, TRUE, FALSE );
    }
} /* SetNewPalette */

/*
 * RestoreColourPalette - Restores the colour palette to the hard coded
 *                        colours in the init routine.
 */
void RestoreColourPalette( void )
{
    int         i;
    COLORREF    leftcolour;
    COLORREF    rightcolour;
    COLORREF    colour;
    WPI_PRES    pres;
    int         leftindex;
    int         rightindex;
    wie_clrtype lefttype;
    wie_clrtype righttype;
    HMENU       hmenu;
    HWND        frame;

    if (currentPalIndex != COLOUR_16 && currentPalIndex != COLOUR_256) {
        return;
    }
    leftcolour = GetSelectedColour( LMOUSEBUTTON, NULL, &lefttype );
    rightcolour = GetSelectedColour( RMOUSEBUTTON, NULL, &righttype );

    leftindex = getColourIndex( leftcolour );
    rightindex = getColourIndex( rightcolour );

    InitPalette();
    for (i=0; i < PALETTE_SIZE-1; ++i) {
        ShowNewColour( i, RGB(Palette[currentPalIndex][i].rgbRed,
                                Palette[currentPalIndex][i].rgbGreen,
                                Palette[currentPalIndex][i].rgbBlue), FALSE );
    }
    /*
     * We do the last one separately to invalidate the window.
     */
    ShowNewColour( i, RGB(Palette[currentPalIndex][i].rgbRed,
                        Palette[currentPalIndex][i].rgbGreen,
                        Palette[currentPalIndex][i].rgbBlue), TRUE );

    if (lefttype == NORMAL_CLR) {
        colour = RGB(Palette[currentPalIndex][leftindex].rgbRed,
                     Palette[currentPalIndex][leftindex].rgbGreen,
                     Palette[currentPalIndex][leftindex].rgbBlue);
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        SetColour(LMOUSEBUTTON, colour, _wpi_getnearestcolor(pres, colour),
                                                                NORMAL_CLR);
        _wpi_releasepres( HWND_DESKTOP, pres );
    }
    if (righttype == NORMAL_CLR) {
        colour = RGB(Palette[currentPalIndex][rightindex].rgbRed,
                     Palette[currentPalIndex][rightindex].rgbGreen,
                     Palette[currentPalIndex][rightindex].rgbBlue);
        pres = _wpi_getpres( HWND_DESKTOP );
        _wpi_torgbmode( pres );
        SetColour(RMOUSEBUTTON, colour, _wpi_getnearestcolor(pres, colour),
                                                                NORMAL_CLR);
        _wpi_releasepres( HWND_DESKTOP, pres );
    }

    PrintHintTextByID( WIE_PALETTERESTORED, NULL );

    if (HMainWindow) {
        frame = _wpi_getframe( _wpi_getframe(HMainWindow) );
        hmenu = GetMenu( frame );
        _wpi_enablemenuitem( hmenu, IMGED_RCOLOUR, FALSE, FALSE );
    }

} /* RestoreColourPalette */

/*
 * InitPaletteBitmaps - Initializes the available colour bitmaps
 */
void InitPaletteBitmaps( HWND hwnd, HBITMAP *colourbitmap,
                                                        HBITMAP *monobitmap )
{
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HDC         hdc;
    HBITMAP     oldbitmap;
    COLORREF    colour;
    int         i;
    int         left_sqr;
    HBRUSH      colourbrush;
    HBRUSH      oldbrush;
    HPEN        blackpen;
    HPEN        oldpen;
    int         top;
    int         bottom;
    int         height;

    pres = _wpi_getpres( hwnd );

    *colourbitmap = _wpi_createcompatiblebitmap( pres, CUR_BMP_WIDTH, CUR_BMP_HEIGHT );
    *monobitmap = _wpi_createbitmap( CUR_BMP_WIDTH, CUR_BMP_HEIGHT, 1, 1, NULL );
    mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );

    _wpi_releasepres( hwnd, pres );
    _wpi_torgbmode( mempres );

    blackpen = _wpi_createpen( PS_SOLID, 0, BLACK );
    oldpen = _wpi_selectobject( mempres, blackpen );
    oldbitmap = _wpi_selectobject( mempres, *colourbitmap );

    /*
     * PM NOTE:  all box coordinates are relative to the windows origin
     * of top left.  so we convert the height for PM.
     */
    left_sqr = 0;
    height = 2 * SQR_SIZE + 1;
    for (i = 0; i < PALETTE_SIZE; i+=2) {
        colour = RGB(Palette[COLOUR_16][i].rgbRed,
                     Palette[COLOUR_16][i].rgbGreen,
                     Palette[COLOUR_16][i].rgbBlue);
        colourbrush = _wpi_createsolidbrush( colour );
        oldbrush = _wpi_selectobject( mempres, colourbrush );

        top = _wpi_cvth_y( 0, height );
        bottom = _wpi_cvth_y( SQR_SIZE + 1, height );

        _wpi_rectangle( mempres, left_sqr, top, left_sqr+SQR_SIZE+1, bottom );
        _wpi_selectobject( mempres, oldbrush );
        _wpi_deleteobject( colourbrush );
        colour = RGB(Palette[COLOUR_16][i+1].rgbRed,
                     Palette[COLOUR_16][i+1].rgbGreen,
                     Palette[COLOUR_16][i+1].rgbBlue);
        colourbrush = _wpi_createsolidbrush( colour );
        oldbrush = _wpi_selectobject( mempres, colourbrush );

        top = _wpi_cvth_y( SQR_SIZE, height );
        bottom = _wpi_cvth_y( 2*SQR_SIZE + 1, height );

        _wpi_rectangle( mempres, left_sqr, top, left_sqr+SQR_SIZE+1, bottom );
        _wpi_selectobject( mempres, oldbrush );
        _wpi_deleteobject( colourbrush );

        left_sqr += SQR_SIZE;
    }

    _wpi_selectobject( mempres, oldbitmap );
    oldbitmap = _wpi_selectobject( mempres, *monobitmap );

    left_sqr = 0;
    for (i = 0; i < PALETTE_SIZE; i+=2) {
        colour = RGB(Palette[COLOUR_2][i].rgbRed,
                     Palette[COLOUR_2][i].rgbGreen,
                     Palette[COLOUR_2][i].rgbBlue);
        colourbrush = _wpi_createsolidbrush( colour );
        oldbrush = _wpi_selectobject( mempres, colourbrush );

        top = _wpi_cvth_y( 0, height );
        bottom = _wpi_cvth_y( SQR_SIZE + 1, height );

        _wpi_rectangle( mempres, left_sqr, top, left_sqr+SQR_SIZE+1, bottom );
        _wpi_selectobject( mempres, oldbrush );
        _wpi_deleteobject( colourbrush );

        colour = RGB(Palette[COLOUR_2][i+1].rgbRed,
                     Palette[COLOUR_2][i+1].rgbGreen,
                     Palette[COLOUR_2][i+1].rgbBlue);
        colourbrush = _wpi_createsolidbrush( colour );
        oldbrush = _wpi_selectobject( mempres, colourbrush );

        top = _wpi_cvth_y( SQR_SIZE, height );
        bottom = _wpi_cvth_y( 2*SQR_SIZE + 1, height );

        _wpi_rectangle( mempres, left_sqr, top, left_sqr+SQR_SIZE+1, bottom );
        _wpi_selectobject( mempres, oldbrush );
        _wpi_deleteobject( colourbrush );

        left_sqr += SQR_SIZE;
    }
    _wpi_selectobject( mempres, oldbitmap );
    _wpi_selectobject( mempres, oldpen );
    _wpi_deletecompatiblepres( mempres, hdc );
    _wpi_deleteobject( blackpen );

} /* InitPaletteBitmaps */

