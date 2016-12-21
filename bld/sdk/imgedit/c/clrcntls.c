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

static HWND             hColorsWnd;
static HWND             hScreenWnd;
static int              colorsHeight;
static int              screenHeight;
static palette_box      paletteBox[PALETTE_SIZE];
static palette_box      screenColor;
static palette_box      inverseColor;
static BOOL             fShowScreenClr = FALSE;
static HWND             hScreenTxt;
static HWND             hInverseTxt;
static short            numberOfColors;
static HBITMAP          hColorBitmap;
static HBITMAP          hMonoBitmap;
static int              prevRestoreState = FALSE;

/*
 * paintColors - paint the available colors on the color palette
 */
static void paintColors( HWND hwnd )
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
    if( numberOfColors == 2 ) {
        bitmap = hMonoBitmap;
    } else {
        bitmap = hColorBitmap;
    }

    mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
    _wpi_torgbmode( mempres );
    oldbitmap = _wpi_selectbitmap( mempres, bitmap );

    _wpi_bitblt( pres, 0, 0, CUR_BMP_WIDTH, CUR_BMP_HEIGHT, mempres, 0, 0, SRCCOPY );
    _wpi_getoldbitmap( mempres, oldbitmap );
    _wpi_deletecompatiblepres( mempres, hdc );

    _wpi_endpaint( hwnd, pres, &ps );

} /* paintColors */

/*
 * paintScreen - paint the screen and inverse window
  */
static void paintScreen( HWND hwnd )
{
    WPI_PRES    pres;
    PAINTSTRUCT ps;

    pres = _wpi_beginpaint( hwnd, NULL, &ps );
#ifdef __OS2_PM__
   WinFillRect( pres, &ps, CLR_PALEGRAY );
#endif
    _wpi_torgbmode( pres );
    if( fShowScreenClr ) {
        DisplayColorBox( pres, &screenColor );
        DisplayColorBox( pres, &inverseColor );
    }
    _wpi_endpaint( hwnd, pres, &ps );

} /* paintScreen */

#ifdef __OS2_PM__

/*
 * convertToPMCoords - converts coordinates to PM (i.e. origin at bottom left)
 */
static void convertToPMCoords( void )
{
    int         i;
    long        temp;

    for( i = 0; i < PALETTE_SIZE; i += 2 ) {
        temp = paletteBox[i].box.top;
        paletteBox[i].box.top = paletteBox[i].box.bottom;
        paletteBox[i].box.bottom = temp;

        temp = paletteBox[i + 1].box.top;
        paletteBox[i + 1].box.top = paletteBox[i + 1].box.bottom;
        paletteBox[i + 1].box.bottom = temp;
    }

} /* convertToPMCoords */

#endif

/*
 * initPaletteBoxes - assign the values of the colors in the palette and
 *                    initialize the palette boxes
 */
static void initPaletteBoxes( bool firsttime )
{
    SetBoxColors( &screenColor, &inverseColor, numberOfColors, paletteBox );
    if( firsttime ) {
        SetColor( LMOUSEBUTTON, BLACK, BLACK, NORMAL_CLR );
        SetColor( RMOUSEBUTTON, WHITE, WHITE, NORMAL_CLR );
        /*
         * screenColor has been set by profile information
         */
        inverseColor.color = GetInverseColor( screenColor.color );
        inverseColor.solid_color = inverseColor.color;
    } else {
        SetCurrentColors( fShowScreenClr );
    }
#ifdef __OS2_PM__
    convertToPMCoords();
#endif

} /* initPaletteBoxes */

/*
 * editCurrentColor - edit the color under the cursor
  */
static void editCurrentColor( WPI_POINT *pt )
{
    int         i;
    WPI_RECT    wrect;
    int         top;
    int         bottom;

    for( i = 0; i < PALETTE_SIZE; i++ ) {
        top = _wpi_cvth_y( paletteBox[i].box.top, colorsHeight );
        bottom = _wpi_cvth_y( paletteBox[i].box.bottom, colorsHeight );

        _wpi_setintwrectvalues( &wrect, paletteBox[i].box.left, top,
                                        paletteBox[i].box.right, bottom );
        if( _wpi_ptinrect( &wrect, *pt ) ) {
            if( numberOfColors == 2 ) {
                return;
            } else {
#ifndef __OS2_PM__
                EditColors();
#endif
                return;
            }
        }
    }

} /* editCurrentColor */

/*
 * selectColor - select the color under the cursor
 */
static void selectColor( WPI_POINT *pt, int mousebutton )
{
    short       i;
    WPI_RECT    wrect;
    int         top;
    int         bottom;

    for( i = 0; i < PALETTE_SIZE; i++ ) {
        top = _wpi_cvth_y( paletteBox[i].box.top, colorsHeight );
        bottom = _wpi_cvth_y( paletteBox[i].box.bottom, colorsHeight );

        _wpi_setintrectvalues( &wrect, paletteBox[i].box.left, top,
                                       paletteBox[i].box.right, bottom );
        if( _wpi_ptinrect( &wrect, *pt ) ) {
            SetColor( mousebutton, paletteBox[i].color,
                                   paletteBox[i].solid_color, NORMAL_CLR );
            return;
        }
    }

} /* selectColor */

/*
 * selectScreen - selects the screen color
 */
static void selectScreen( WPI_POINT *pt, int mousebutton )
{
    WPI_RECT    wrect;
    int         top;
    int         bottom;

    if( !fShowScreenClr ) {
        return;
    }
    top = _wpi_cvth_y( screenColor.box.top, screenHeight );
    bottom = _wpi_cvth_y( screenColor.box.bottom, screenHeight );

    _wpi_setintwrectvalues( &wrect, screenColor.box.left, top,
                                    screenColor.box.right, bottom );

    if( _wpi_ptinrect( &wrect, *pt ) ) {
        SetColor( mousebutton, screenColor.color, screenColor.color, SCREEN_CLR );
        return;
    }

    top = _wpi_cvth_y( inverseColor.box.top, screenHeight );
    bottom = _wpi_cvth_y( inverseColor.box.bottom, screenHeight );

    _wpi_setintwrectvalues( &wrect, inverseColor.box.left, top,
                                    inverseColor.box.right, bottom );
    if( _wpi_ptinrect( &wrect, *pt ) ) {
        SetColor( mousebutton, inverseColor.color, inverseColor.color, INVERSE_CLR );
        return;
    }

} /* selectScreen */

/*
 * ColorsWndProc - handle messages for the available color selection window
 */
WPI_MRESULT CALLBACK ColorsWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    static WPI_POINT            pt;

    switch( msg ) {
    case WM_CREATE:
        numberOfColors = 16;
        initPaletteBoxes( TRUE );
        InitPaletteBitmaps( hwnd, &hColorBitmap, &hMonoBitmap );
        break;

    case WM_PAINT:
        paintColors( hwnd );
        break;

    case WM_LBUTTONDOWN:
        IMGED_MAKEPOINT( wparam, lparam, pt );
        selectColor( &pt, LMOUSEBUTTON );
        break;

    case WM_LBUTTONDBLCLK:
        IMGED_MAKEPOINT( wparam, lparam, pt );
        editCurrentColor( &pt );
        break;

    case WM_RBUTTONDOWN:
        IMGED_MAKEPOINT( wparam, lparam, pt );
        selectColor( &pt, RMOUSEBUTTON );
        break;

    case WM_DESTROY:
        _wpi_deletebitmap( hColorBitmap );
        _wpi_deletebitmap( hMonoBitmap );
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );

} /* ColorsWndProc */

/*
 * ScreenWndProc - handle messages for the screen and inverse windows
 */
WPI_MRESULT CALLBACK ScreenWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    WPI_POINT   pt;
    WPI_RECT    wrect1;
    WPI_RECT    wrect2;
    int         top;
    int         bottom;

    switch( msg ) {
    case WM_PAINT:
        paintScreen( hwnd );
        break;

    case WM_LBUTTONDOWN:
        IMGED_MAKEPOINT( wparam, lparam, pt );
        selectScreen( &pt, LMOUSEBUTTON );
        break;

    case WM_LBUTTONDBLCLK:
        if( !fShowScreenClr ) {
            break;
        }

        IMGED_MAKEPOINT( wparam, lparam, pt );

        top = _wpi_cvth_y( screenColor.box.top, screenHeight );
        bottom = _wpi_cvth_y( screenColor.box.bottom, screenHeight );
        _wpi_setintwrectvalues( &wrect1, screenColor.box.left, top,
                                         screenColor.box.right, bottom );

        top = _wpi_cvth_y( inverseColor.box.top, screenHeight );
        bottom = _wpi_cvth_y( inverseColor.box.bottom, screenHeight );
        _wpi_setintwrectvalues( &wrect2, inverseColor.box.left, top,
                                         inverseColor.box.right, bottom );
        if( _wpi_ptinrect( &wrect1, pt ) || _wpi_ptinrect( &wrect2, pt ) ) {
            ChooseBkColor();
        }
        break;

    case WM_RBUTTONDOWN:
        IMGED_MAKEPOINT( wparam, lparam, pt );
        selectScreen( &pt, RMOUSEBUTTON );
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0 );

} /* ScreenWndProc */

/*
 * CreateColorControls - create controls in the color palette window
 */
void CreateColorControls( HWND hparent )
{
    WPI_RECT    rect;
#ifdef __OS2_PM__
    PM_CreateColorCtrls( hparent, &hColorsWnd, &hScreenWnd, &hScreenTxt, &hInverseTxt );
#else
    Win_CreateColorCtrls( hparent, &hColorsWnd, &hScreenWnd, &hScreenTxt, &hInverseTxt );
#endif
    _wpi_getclientrect( hColorsWnd, &rect );
    colorsHeight = _wpi_getheightrect( rect );
    _wpi_getclientrect( hScreenWnd, &rect );
    screenHeight =_wpi_getheightrect( rect );

} /* CreateColorControls */

/*
 * DisplayScreenClrs - depending on the parameter, display the screen color
 *                     and the inverse color (for icons and cursors but
 *                     not for bitmaps)
 */
void DisplayScreenClrs( BOOL fdisplay )
{
    HWND        frame;
    char        *text;

    frame = _wpi_getframe( hScreenWnd );

    if( !fdisplay ) {
        if( fShowScreenClr ) {
            ChangeCurrentColor();
        }
        SetWindowText( hScreenTxt, "" );
        SetWindowText( hInverseTxt, "" );
        fShowScreenClr = FALSE;
        ShowWindow( frame, SW_HIDE );
    } else {
        text = IEAllocRCString( WIE_SCREENTEXT );
        if( text != NULL ) {
            SetWindowText( hScreenTxt, text );
            IEFreeRCString( text );
        } else {
            SetWindowText( hScreenTxt, "Screen:" );
        }
        text = IEAllocRCString( WIE_INVERSETEXT );
        if( text != NULL ) {
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
 * SetNumColors - set the number of colors in the image and repaint the window
 */
void SetNumColors( int number_of_colors )
{
    HMENU       hmenu;
    HWND        frame;

    if( numberOfColors == number_of_colors ) {
        return;
    }
    SetCurrentNumColors( number_of_colors );
    numberOfColors = number_of_colors;
    initPaletteBoxes( FALSE );
    _wpi_invalidaterect( hColorsWnd, NULL, FALSE );

    if( numberOfColors == 2 ) {
        if( HMainWindow != NULL ) {
            frame = _wpi_getframe( HMainWindow );
            hmenu = GetMenu( frame );
            if( _wpi_isitemenabled( hmenu, IMGED_RCOLOR ) ) {
                prevRestoreState = TRUE;
            } else {
                prevRestoreState = FALSE;
            }
            _wpi_enablemenuitem( hmenu, IMGED_SCOLOR, FALSE, FALSE );
            _wpi_enablemenuitem( hmenu, IMGED_RCOLOR, FALSE, FALSE );
            _wpi_enablemenuitem( hmenu, IMGED_LCOLOR, FALSE, FALSE );
        }
    } else {
        if( HMainWindow != NULL ) {
            frame = _wpi_getframe( HMainWindow );
            hmenu = GetMenu( frame );
            _wpi_enablemenuitem( hmenu, IMGED_SCOLOR, TRUE, FALSE );
            _wpi_enablemenuitem( hmenu, IMGED_RCOLOR, prevRestoreState, FALSE );
            _wpi_enablemenuitem( hmenu, IMGED_LCOLOR, TRUE, FALSE );
        }
    }

} /* SetNumColors */

/*
 * SetScreenClr - set the color to represent the screen
 */
void SetScreenClr( COLORREF screen_color )
{
    WPI_PRES    pres;

    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );
    screenColor.color = _wpi_getnearestcolor( pres, screen_color );
    screenColor.solid_color = screenColor.color;
    _wpi_releasepres( HWND_DESKTOP, pres );

    inverseColor.color = GetInverseColor( screenColor.color );
    inverseColor.solid_color = inverseColor.color;

    VerifyCurrentClr( screenColor.color, inverseColor.color );
    _wpi_invalidaterect( hScreenWnd, NULL, FALSE );

} /* SetScreenClr */

/*
 * ShowNewColor - replace the color of the color box and redisplay the boxes
 */
void ShowNewColor( int index, COLORREF newcolor, BOOL repaint )
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
    paletteBox[index].color = newcolor;
    paletteBox[index].solid_color = _wpi_getnearestcolor( pres, newcolor );
    _wpi_releasepres( HWND_DESKTOP, pres );

    if( numberOfColors != 2 ) {
        topleft.x = (index / 2) * SQR_SIZE;
        bottomright.x = topleft.x + SQR_SIZE + 1;
        if( index / 2 == (index + 1) / 2 ) {
            topleft.y = 0;
            bottomright.y = SQR_SIZE + 1;
        } else {
            topleft.y = SQR_SIZE;
            bottomright.y = 2 * SQR_SIZE + 1;
        }
        pres = _wpi_getpres( HWND_DESKTOP );
        mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
        _wpi_releasepres( HWND_DESKTOP, pres );
        _wpi_torgbmode( mempres );

        brush = _wpi_createsolidbrush( newcolor );
        oldbrush = _wpi_selectobject( mempres, brush );
        blackpen = _wpi_createpen( PS_SOLID, 0, BLACK );
        oldpen = _wpi_selectobject( mempres, blackpen );
        oldbitmap = _wpi_selectbitmap( mempres, hColorBitmap );

        _wpi_cvth_pt( &topleft, colorsHeight );
        _wpi_cvth_pt( &bottomright, colorsHeight );

        _wpi_rectangle( mempres, topleft.x, topleft.y, bottomright.x, bottomright.y );
        _wpi_selectobject( mempres, oldpen );
        _wpi_selectobject( mempres, oldbrush );
        _wpi_deleteobject( blackpen );
        _wpi_deleteobject( brush );
        _wpi_getoldbitmap( mempres, oldbitmap );
        _wpi_deletecompatiblepres( mempres, hdc );
    }

    if( repaint ) {
        _wpi_invalidaterect( hColorsWnd, NULL, FALSE );
    }

} /* ShowNewColor */

/*
 * SetInitScreenColor - set the initial screen and inverse colors from
 *                      the profile information
 */
void SetInitScreenColor( COLORREF color )
{
    WPI_PRES    pres;

    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );
    screenColor.color = _wpi_getnearestcolor( pres, color );
    screenColor.solid_color = screenColor.color;
    _wpi_releasepres( HWND_DESKTOP, pres );

    SetViewBkColor( screenColor.color );

} /* SetInitScreenColor */
