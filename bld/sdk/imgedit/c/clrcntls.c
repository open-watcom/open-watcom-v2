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
#include "imgedit.h"
#include "ieclrpal.h"

static HWND             hColoursWnd;
static HWND             hScreenWnd;
static int              coloursHeight;
static int              screenHeight;
static palette_box      paletteBox[PALETTE_SIZE];
static palette_box      screenColour;
static palette_box      inverseColour;
static BOOL             fShowScreenClr = FALSE;
static HWND             hScreenTxt;
static HWND             hInverseTxt;
static short            numberOfColours;
static HBITMAP          hColourBitmap;
static HBITMAP          hMonoBitmap;
static int              prevRestoreState = FALSE;

/*
 * paintColours - paint the available colours on the colour palette.
 */
static void paintColours( HWND hwnd )
{
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HDC         hdc;
    PAINTSTRUCT ps;
    HBITMAP     bitmap;
    HBITMAP     oldbitmap;

    pres = _wpi_beginpaint( hwnd, NULL, &ps );
#ifdef __OS2_PM__
   WinFillRect( pres, &ps, CLR_PALEGRAY );
#endif

    _wpi_torgbmode( pres );
    if (numberOfColours == 2) {
        bitmap = hMonoBitmap;
    } else {
        bitmap = hColourBitmap;
    }

    mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
    _wpi_torgbmode( mempres );
    oldbitmap = _wpi_selectbitmap( mempres, bitmap );

    _wpi_bitblt( pres, 0, 0, CUR_BMP_WIDTH, CUR_BMP_HEIGHT, mempres,
                                                            0, 0, SRCCOPY );
    _wpi_getoldbitmap( mempres, oldbitmap );
    _wpi_deletecompatiblepres( mempres, hdc );

    _wpi_endpaint( hwnd, pres, &ps );
} /* paintColours */

/*
 * paintScreen - paints the screen and inverse window
  */
void paintScreen( HWND hwnd )
{
    WPI_PRES    pres;
    PAINTSTRUCT ps;

    pres = _wpi_beginpaint( hwnd, NULL, &ps );
#ifdef __OS2_PM__
   WinFillRect( pres, &ps, CLR_PALEGRAY );
#endif
    _wpi_torgbmode( pres );
    if (fShowScreenClr) {
        DisplayColourBox(pres, &(screenColour));
        DisplayColourBox(pres, &(inverseColour));
    }
    _wpi_endpaint( hwnd, pres, &ps );

} /* paintScreen */

#ifdef __OS2_PM__
/*
 * convertToPMCoords - converts coordinates to PM (ie. origin at bottom left)
 */
static void convertToPMCoords( void )
{
    int         i;
    long        temp;

    for (i=0; i < PALETTE_SIZE; i+=2) {
        temp = paletteBox[i].box.top;
        paletteBox[i].box.top = paletteBox[i].box.bottom;
        paletteBox[i].box.bottom = temp;

        temp = paletteBox[i+1].box.top;
        paletteBox[i+1].box.top = paletteBox[i+1].box.bottom;
        paletteBox[i+1].box.bottom = temp;
    }
} /* convertToPMCoords */
#endif

/*
 * initPaletteBoxes - Assigns the values of the colours in the palette and
 *               initializes the palette boxes.
 */
static void initPaletteBoxes( BOOL firsttime )
{
    SetBoxColours(&screenColour, &inverseColour, numberOfColours, paletteBox );
    if (firsttime) {
        SetColour( LMOUSEBUTTON, BLACK, BLACK, NORMAL_CLR );
        SetColour( RMOUSEBUTTON, WHITE, WHITE, NORMAL_CLR );
        /*
         * screenColour has been set by profile information
         */
        inverseColour.colour = GetInverseColour( screenColour.colour );
        inverseColour.solid_colour = inverseColour.colour;
    } else {
        SetCurrentColours(fShowScreenClr);
    }
#ifdef __OS2_PM__
    convertToPMCoords();
#endif

} /* initPaletteBoxes */

/*
 * editCurrentColour - edits the colour under the cursor.
  */
static void editCurrentColour( WPI_POINT *pt )
{
    int         i;
    WPI_RECT    wrect;
    int         top;
    int         bottom;

    for (i=0; i < PALETTE_SIZE; ++i) {
        top = _wpi_cvth_y( paletteBox[i].box.top, coloursHeight );
        bottom = _wpi_cvth_y( paletteBox[i].box.bottom, coloursHeight );

        _wpi_setintwrectvalues( &wrect, paletteBox[i].box.left, top,
                                            paletteBox[i].box.right, bottom );
        if ( _wpi_ptinrect(&wrect, *pt) ) {
            if (numberOfColours == 2) {
                return;
            } else {
#ifndef __OS2_PM__
                EditColours();
#endif
                return;
            }
        }
    }
} /* editCurrentColour */

/*
 * selectColour - Select the colour under the cursor
 */
static void selectColour( WPI_POINT *pt, int mousebutton )
{
    short       i;
    WPI_RECT    wrect;
    int         top;
    int         bottom;

    for (i=0; i < PALETTE_SIZE; ++i) {
        top = _wpi_cvth_y( paletteBox[i].box.top, coloursHeight );
        bottom = _wpi_cvth_y( paletteBox[i].box.bottom, coloursHeight );

        _wpi_setintrectvalues( &wrect, paletteBox[i].box.left, top,
                                            paletteBox[i].box.right, bottom );
        if ( _wpi_ptinrect(&wrect, *pt) ) {
            SetColour( mousebutton, paletteBox[i].colour,
                                    paletteBox[i].solid_colour, NORMAL_CLR );
            return;
        }
    }
} /* selectColour */

/*
 * selectScreen - selects the screen colour.
 */
void selectScreen( WPI_POINT *pt, int mousebutton )
{
    WPI_RECT    wrect;
    int         top;
    int         bottom;

    if (!fShowScreenClr) {
        return;
    }
    top = _wpi_cvth_y( screenColour.box.top, screenHeight );
    bottom = _wpi_cvth_y( screenColour.box.bottom, screenHeight );

    _wpi_setintwrectvalues( &wrect, screenColour.box.left, top,
                                        screenColour.box.right, bottom );

    if ( _wpi_ptinrect(&wrect, *pt) ) {
        SetColour( mousebutton, screenColour.colour, screenColour.colour,
                                                                SCREEN_CLR );
        return;
    }

    top = _wpi_cvth_y( inverseColour.box.top, screenHeight );
    bottom = _wpi_cvth_y( inverseColour.box.bottom, screenHeight );

    _wpi_setintwrectvalues( &wrect, inverseColour.box.left, top,
                                        inverseColour.box.right, bottom );
    if ( _wpi_ptinrect(&wrect, *pt) ) {
        SetColour( mousebutton, inverseColour.colour, inverseColour.colour,
                                                                INVERSE_CLR );
        return;
    }
} /* selectScreen */

/*
 * ColoursWndProc - handle messages for the available colour selection window.
 */
MRESULT CALLBACK ColoursWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                                                          WPI_PARAM2 lparam )
{
    static WPI_POINT            pt;

    switch ( msg ) {
    case WM_CREATE:
        numberOfColours = 16;
        initPaletteBoxes(TRUE);
        InitPaletteBitmaps( hwnd, &hColourBitmap, &hMonoBitmap );
        break;

    case WM_PAINT:
        paintColours( hwnd );
        break;

    case WM_LBUTTONDOWN:
        IMGED_MAKEPOINT( wparam, lparam, pt );
        selectColour(&pt, LMOUSEBUTTON);
        break;

    case WM_LBUTTONDBLCLK:
        IMGED_MAKEPOINT( wparam, lparam, pt );
        editCurrentColour( &pt );
        break;

    case WM_RBUTTONDOWN:
        IMGED_MAKEPOINT( wparam, lparam, pt );
        selectColour(&pt, RMOUSEBUTTON);
        break;

    case WM_DESTROY:
        _wpi_deletebitmap( hColourBitmap );
        _wpi_deletebitmap( hMonoBitmap );
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );

} /* CurrentWndProc */

/*
 * ScreenWndProc - handle messages for the screen and inverse windows
 */
MRESULT CALLBACK ScreenWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                                                         WPI_PARAM2 lparam )
{
    WPI_POINT   pt;
    WPI_RECT    wrect1;
    WPI_RECT    wrect2;
    int         top;
    int         bottom;

    switch ( msg ) {
    case WM_PAINT:
        paintScreen( hwnd );
        break;

    case WM_LBUTTONDOWN:
        IMGED_MAKEPOINT( wparam, lparam, pt );
        selectScreen(&pt, LMOUSEBUTTON);
        break;

    case WM_LBUTTONDBLCLK:
        if (!fShowScreenClr) {
            break;
        }

        IMGED_MAKEPOINT( wparam, lparam, pt );

        top = _wpi_cvth_y( screenColour.box.top, screenHeight );
        bottom = _wpi_cvth_y( screenColour.box.bottom, screenHeight );
        _wpi_setintwrectvalues( &wrect1, screenColour.box.left, top,
                                            screenColour.box.right, bottom );

        top = _wpi_cvth_y( inverseColour.box.top, screenHeight );
        bottom = _wpi_cvth_y( inverseColour.box.bottom, screenHeight );
        _wpi_setintwrectvalues( &wrect2, inverseColour.box.left, top,
                                            inverseColour.box.right, bottom );
        if ( _wpi_ptinrect(&wrect1, pt) ||
                                _wpi_ptinrect(&wrect2, pt) ) {
            ChooseBkColour();
        }
        break;

    case WM_RBUTTONDOWN:
        IMGED_MAKEPOINT( wparam, lparam, pt );
        selectScreen(&pt, RMOUSEBUTTON);
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );
} /* ScreenWndProc */

/*
 * CreateColourControls - Creates controls in the colour palette window.
 */
void CreateColourControls( HWND hparent )
{
    WPI_RECT    rect;
#ifdef __OS2_PM__
    PM_CreateColourCtrls( hparent, &hColoursWnd, &hScreenWnd, &hScreenTxt,
                                        &hInverseTxt );
#else
    Win_CreateColourCtrls( hparent, &hColoursWnd, &hScreenWnd, &hScreenTxt,
                                        &hInverseTxt );
#endif
    _wpi_getclientrect( hColoursWnd, &rect );
    coloursHeight = _wpi_getheightrect( rect );
    _wpi_getclientrect( hScreenWnd, &rect );
    screenHeight =_wpi_getheightrect( rect );
} /* CreateColourControls */

/*
 * DisplayScreenClrs - Depending on the parameter, display the screen colour
 *                     and the inverse colour (for icons and cursors but
 *                     not for bitmaps).
 */
void DisplayScreenClrs( BOOL fdisplay )
{
    HWND        frame;
    char        *text;

    frame = _wpi_getframe( hScreenWnd );

    if (!fdisplay) {
        if (fShowScreenClr) {
            ChangeCurrentColour();
        }
        SetWindowText( hScreenTxt, "" );
        SetWindowText( hInverseTxt, "");
        fShowScreenClr = FALSE;
        ShowWindow( frame, SW_HIDE );
    } else {
        text = IEAllocRCString( WIE_SCREENTEXT );
        if( text ) {
            SetWindowText( hScreenTxt, text );
            IEFreeRCString( text );
        } else {
            SetWindowText( hScreenTxt, "Screen:" );
        }
        text = IEAllocRCString( WIE_INVERSETEXT );
        if( text ) {
            SetWindowText( hInverseTxt, text );
            IEFreeRCString( text );
        } else {
            SetWindowText( hInverseTxt, "Inverse:" );
        }
        fShowScreenClr = TRUE;
        ShowWindow( frame, SW_SHOWNORMAL );
#ifdef __OS2_PM__
        InvalidateRect( hScreenWnd, NULL, TRUE );
#endif
    }
} /* DisplayScreenClrs */

/*
 * SetNumColours - sets the number of colours in the image and repaints the
 *                      window.
 */
void SetNumColours( int number_of_colours )
{
    HMENU       hmenu;
    HWND        frame;

    if (numberOfColours == number_of_colours) {
        return;
    }
    SetCurrentNumColours( number_of_colours );
    numberOfColours = number_of_colours;
    initPaletteBoxes(FALSE);
    _wpi_invalidaterect( hColoursWnd, NULL, FALSE );

    if (numberOfColours == 2) {
        if (HMainWindow) {
            frame = _wpi_getframe( HMainWindow );
            hmenu = GetMenu( frame );
            if ( _wpi_isitemenabled(hmenu, IMGED_RCOLOUR) ) {
                prevRestoreState = TRUE;
            } else {
                prevRestoreState = FALSE;
            }
            _wpi_enablemenuitem( hmenu, IMGED_SCOLOUR, FALSE, FALSE );
            _wpi_enablemenuitem( hmenu, IMGED_RCOLOUR, FALSE, FALSE );
            _wpi_enablemenuitem( hmenu, IMGED_LCOLOUR, FALSE, FALSE );
        }
    } else {
        if (HMainWindow) {
            frame = _wpi_getframe( HMainWindow );
            hmenu = GetMenu( frame );
            _wpi_enablemenuitem( hmenu, IMGED_SCOLOUR, TRUE, FALSE );
            _wpi_enablemenuitem(hmenu, IMGED_RCOLOUR, prevRestoreState, FALSE);
            _wpi_enablemenuitem( hmenu, IMGED_LCOLOUR, TRUE, FALSE );
        }
    }
} /* SetNumColours */

/*
 * SetScreenClr - sets the colour to represent the screen.
 */
void SetScreenClr( COLORREF screen_colour )
{
    WPI_PRES    pres;

    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );
    screenColour.colour = _wpi_getnearestcolor(pres, screen_colour);
    screenColour.solid_colour = screenColour.colour;
    _wpi_releasepres( HWND_DESKTOP, pres );

    inverseColour.colour = GetInverseColour( screenColour.colour );
    inverseColour.solid_colour = inverseColour.colour;

    VerifyCurrentClr( screenColour.colour, inverseColour.colour );
    _wpi_invalidaterect( hScreenWnd, NULL, FALSE );
} /* SetScreenClr */

/*
 * ShowNewColour - replaces the colour of the colour box and re-displays the
 *                      boxes.
 */
void ShowNewColour( int index, COLORREF newcolour, BOOL repaint )
{
    WPI_POINT   topleft;
    WPI_POINT   bottomright;
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HDC         hdc;
    HBITMAP     oldbitmap;
    HBRUSH      brush;
    HBRUSH      oldbrush;
    HPEN        blackpen;
    HPEN        oldpen;

    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );
    paletteBox[index].colour = newcolour;
    paletteBox[index].solid_colour = _wpi_getnearestcolor( pres, newcolour );
    _wpi_releasepres( HWND_DESKTOP, pres );

    if (numberOfColours != 2) {
        topleft.x = (index / 2) * SQR_SIZE;
        bottomright.x = topleft.x + SQR_SIZE + 1;
        if ( (index/2) == (index+1)/2 ) {
            topleft.y = 0;
            bottomright.y = SQR_SIZE + 1;
        } else {
            topleft.y = SQR_SIZE;
            bottomright.y = 2*SQR_SIZE + 1;
        }
        pres = _wpi_getpres( HWND_DESKTOP );
        mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
        _wpi_releasepres( HWND_DESKTOP, pres );
        _wpi_torgbmode( mempres );

        brush = _wpi_createsolidbrush( newcolour );
        oldbrush = _wpi_selectobject( mempres, brush );
        blackpen = _wpi_createpen( PS_SOLID, 0, BLACK );
        oldpen = _wpi_selectobject( mempres, blackpen );
        oldbitmap = _wpi_selectbitmap( mempres, hColourBitmap );

        _wpi_cvth_pt( &topleft, coloursHeight );
        _wpi_cvth_pt( &bottomright, coloursHeight );

        _wpi_rectangle( mempres, topleft.x, topleft.y, bottomright.x,
                                                            bottomright.y );
        _wpi_selectobject( mempres, oldpen );
        _wpi_selectobject( mempres, oldbrush );
        _wpi_deleteobject( blackpen );
        _wpi_deleteobject( brush );
        _wpi_getoldbitmap( mempres, oldbitmap );
        _wpi_deletecompatiblepres( mempres, hdc );
    }

    if (repaint) {
        _wpi_invalidaterect( hColoursWnd, NULL, FALSE );
    }
} /* ShowNewColour */

/*
 * SetInitScreenColour - sets the initial screen and inverse colours from
 *                       the profile information.
 */
void SetInitScreenColour( COLORREF colour )
{
    WPI_PRES    pres;

    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );
    screenColour.colour = _wpi_getnearestcolor(pres, colour);
    screenColour.solid_colour = screenColour.colour;
    _wpi_releasepres( HWND_DESKTOP, pres );

    SetBkColour( screenColour.colour );
} /* SetInitScreenColour */

