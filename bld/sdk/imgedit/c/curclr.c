/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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

static selected_colour  lButton;
static selected_colour  rButton;
static HWND             hCurrentWnd;
static int              currentHeight;
static int              numberOfColors;
static bool             firstTime = true;

/*
 * paintCurrent - paint the current window (process WM_PAINT)
 */
static void paintCurrent( HWND hwnd )
{
    WPI_PRES            pres;
    WPI_PRES            mempres;
    HDC                 hdc;
    PAINTSTRUCT         ps;
    WPI_RECT            rect;
    WPI_HBITMAP         old_hbitmap;
    int                 top;
    int                 bottom;

    pres = _wpi_beginpaint( hwnd, NULL, &ps );
#ifdef __OS2_PM__
    WinFillRect( pres, &ps, CLR_PALEGRAY );
#endif

    mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
    _wpi_torgbmode( pres );
    _wpi_torgbmode( mempres );
    old_hbitmap = _wpi_selectbitmap( mempres, lButton.hbitmap );

    _wpi_bitblt( pres, 0, 0, CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1, mempres, 0, 0, SRCCOPY );
    _wpi_getoldbitmap( mempres, old_hbitmap );
    old_hbitmap = _wpi_selectbitmap( mempres, rButton.hbitmap );

    _wpi_bitblt( pres, CUR_RCOL_X - 1, 0, CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1, mempres, 0, 0, SRCCOPY );

    _wpi_getoldbitmap( mempres, old_hbitmap );
    _wpi_deletecompatiblepres( mempres, hdc );

    _wpi_setbackmode( pres, TRANSPARENT );
    _wpi_settextcolor( pres, GetInverseColor( lButton.solid ) );

    top = _wpi_cvth_y( CUR_COL_Y, currentHeight );
    bottom = _wpi_cvth_y( CUR_COL_Y + CUR_SQR_SIZE, currentHeight );
    _wpi_setintwrectvalues( &rect, CUR_LCOL_X, top, CUR_LCOL_X + CUR_SQR_SIZE, bottom );
    _wpi_drawtext( pres, "L", 1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
    _wpi_settextcolor( pres, GetInverseColor( rButton.solid ) );
    _wpi_setintwrectvalues( &rect, CUR_RCOL_X, top, CUR_RCOL_X + CUR_SQR_SIZE, bottom );
    _wpi_drawtext( pres, "R", 1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER );
    _wpi_endpaint( hwnd, pres, &ps );

} /* paintCurrent */

/*
 * CurrentWndProc - handle messages for the current color selection window
 */
WPI_MRESULT CALLBACK CurrentWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    switch ( msg ) {
    case WM_CREATE:
        SetCurrentNumColors( 16 );
        break;

    case WM_PAINT:
        paintCurrent( hwnd );
        break;

    case WM_DESTROY:
        if( lButton.hbitmap != NULL ) {
            _wpi_deletebitmap( lButton.hbitmap );
        }
        if( rButton.hbitmap != NULL ) {
            _wpi_deletebitmap( rButton.hbitmap );
        }
        break;

    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return 0;

} /* CurrentWndProc */

/*
 * CreateCurrentWnd - create the window with the current selection in it
 */
void CreateCurrentWnd( HWND hparent )
{
    WPI_RECT    rect;

    lButton.color = BLACK;
    lButton.solid = BLACK;
    lButton.type = NORMAL_CLR;
    rButton.color = WHITE;
    rButton.solid = WHITE;
    rButton.type = NORMAL_CLR;

#ifdef __OS2_PM__
    hCurrentWnd = PM_CreateCurrentDisp( hparent );
#else
    hCurrentWnd = Win_CreateCurrentDisp( hparent );
#endif

    _wpi_getclientrect( hCurrentWnd, &rect );
    currentHeight = _wpi_getheightrect( rect );

} /* CreateCurrentWnd */

/*
 * SetColor - set the current colors
 */
void SetColor( int mousebutton, COLORREF color, COLORREF solid, wie_clrtype type )
{
    HDC         hdc;
    WPI_PRES    pres;
    WPI_PRES    mempres;
    WPI_HBITMAP old_hbitmap;
    HPEN        blackpen;
    HPEN        oldpen;
    HBRUSH      brush;
    HBRUSH      oldbrush;
    int         top;
    int         bottom;

    blackpen = _wpi_createpen( PS_SOLID, 0, BLACK );
    if( mousebutton == LMOUSEBUTTON ) {
        lButton.color = color;
        lButton.solid = solid;
        lButton.type = type;
        if( lButton.hbitmap != NULL ) {
            _wpi_deletebitmap( lButton.hbitmap );
            pres = _wpi_getpres( HWND_DESKTOP );
            if( numberOfColors == 2 && type == NORMAL_CLR ) {
                lButton.hbitmap = _wpi_createbitmap( CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1, 1, 1, NULL );
            } else {
                lButton.hbitmap = _wpi_createcompatiblebitmap( pres, CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1 );
            }
            mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
            _wpi_torgbmode( mempres );
            old_hbitmap = _wpi_selectbitmap( mempres, lButton.hbitmap );
            _wpi_releasepres( HWND_DESKTOP, pres );

            oldpen = _wpi_selectpen( mempres, blackpen );
            brush = _wpi_createsolidbrush( solid );
            oldbrush = _wpi_selectbrush( mempres, brush );

            top = _wpi_cvth_y( 0, currentHeight );
            bottom = _wpi_cvth_y( CUR_SQR_SIZE + 1, currentHeight );

            _wpi_rectangle( mempres, 0, top, CUR_SQR_SIZE + 1, bottom );
            _wpi_getoldbrush( mempres, oldbrush );
            _wpi_deletebrush( brush );

            brush = _wpi_createsolidbrush( color );
            oldbrush = _wpi_selectbrush( mempres, brush );

            top = _wpi_cvth_y( CUR_SQR_SIZE, currentHeight );
            bottom = _wpi_cvth_y( 2 * CUR_SQR_SIZE + 1, currentHeight );

            _wpi_rectangle( mempres, 0, top, CUR_SQR_SIZE + 1, bottom );
            _wpi_getoldbrush( mempres, oldbrush );
            _wpi_deletebrush( brush );
            _wpi_getoldbitmap( mempres, old_hbitmap );
            _wpi_getoldpen( mempres, oldpen );
            _wpi_deletecompatiblepres( mempres, hdc );
        }
    } else {
        rButton.color = color;
        rButton.solid = solid;
        rButton.type = type;
        if( rButton.hbitmap != NULL ) {
            _wpi_deletebitmap( rButton.hbitmap );
            pres = _wpi_getpres( HWND_DESKTOP );
            if( numberOfColors == 2 && type == NORMAL_CLR ) {
                rButton.hbitmap = _wpi_createbitmap( CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1, 1, 1, NULL );
            } else {
                rButton.hbitmap = _wpi_createcompatiblebitmap( pres, CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1 );
            }
            mempres = _wpi_createcompatiblepres( pres, Instance, &hdc );
            _wpi_torgbmode( mempres );

            old_hbitmap = _wpi_selectbitmap( mempres, rButton.hbitmap );
            _wpi_releasepres( HWND_DESKTOP, pres );
            oldpen = _wpi_selectpen( mempres, blackpen );
            brush = _wpi_createsolidbrush( solid );
            oldbrush = _wpi_selectbrush( mempres, brush );

            top = _wpi_cvth_y( 0, currentHeight );
            bottom = _wpi_cvth_y( CUR_SQR_SIZE + 1, currentHeight );
            _wpi_rectangle( mempres, 0, top, CUR_SQR_SIZE + 1, bottom );
            _wpi_getoldbrush( mempres, oldbrush );
            _wpi_deletebrush( brush );

            brush = _wpi_createsolidbrush( color );
            oldbrush = _wpi_selectbrush( mempres, brush );

            top = _wpi_cvth_y( CUR_SQR_SIZE, currentHeight );
            bottom = _wpi_cvth_y( 2 * CUR_SQR_SIZE + 1, currentHeight );

            _wpi_rectangle( mempres, 0, top, CUR_SQR_SIZE + 1, bottom );
            _wpi_getoldbrush( mempres, oldbrush );
            _wpi_deletebrush( brush );
            _wpi_getoldbitmap( mempres, old_hbitmap );
            _wpi_getoldpen( mempres, oldpen );
            _wpi_deletecompatiblepres( mempres, hdc );
        }
    }
    _wpi_deletepen( blackpen );
    InvalidateRect( hCurrentWnd, NULL, TRUE );

} /* SetColor */

/*
 * GetSelectedColor - return the value of the selected fill color (i.e. dithered)
 *                  - the parameter 'solid' will have the value of the solid color
 *                    if it is not NULL
 */
COLORREF GetSelectedColor( int mousebutton, COLORREF *solid, wie_clrtype *type )
{
    if( mousebutton == LMOUSEBUTTON ) {
        *type = lButton.type;
        if( solid != NULL ) {
            *solid = lButton.solid;
        }
        return( lButton.color );
    } else {
        *type = rButton.type;
        if( solid != NULL ) {
            *solid = rButton.solid;
        }
        return( rButton.color );
    }

} /* GetSelectedColor */

/*
 * VerifyCurrentClr - if either of the left or right buttons are screen
 *                    colors, change them
 */
void VerifyCurrentClr( COLORREF screen_color, COLORREF inverse_color )
{
    if( lButton.type != NORMAL_CLR ) {
        if( lButton.type == SCREEN_CLR ) {
            lButton.color = screen_color;
            lButton.solid = screen_color;
            if( lButton.hbitmap != NULL ) {
                SetColor( LMOUSEBUTTON, screen_color, screen_color, SCREEN_CLR );
            }
        } else if( lButton.type == INVERSE_CLR ) {
            lButton.color = inverse_color;
            lButton.solid = inverse_color;
            if( lButton.hbitmap != NULL ) {
                SetColor( LMOUSEBUTTON, inverse_color, inverse_color, INVERSE_CLR );
            }
        }
    }

    if( rButton.type != NORMAL_CLR ) {
        if( rButton.type == SCREEN_CLR ) {
            rButton.color = screen_color;
            rButton.solid = screen_color;
            if( rButton.hbitmap != NULL ) {
                SetColor( RMOUSEBUTTON, screen_color, screen_color, SCREEN_CLR );
            }
        } else if( rButton.type == INVERSE_CLR ) {
            rButton.color = inverse_color;
            rButton.solid = inverse_color;
            if( rButton.hbitmap != NULL ) {
                SetColor( RMOUSEBUTTON, inverse_color, inverse_color, INVERSE_CLR );
            }
        }
    }

} /* VerifyCurrentClr */

/*
 * SetCurrentNumColors - set the number of colors for this module
 */
void SetCurrentNumColors( int color_count )
{
    WPI_PRES    pres;
    WPI_PRES    mempres;
    HDC         memdc;
    WPI_HBITMAP old_hbitmap;

    numberOfColors = color_count;
    pres = _wpi_getpres( HWND_DESKTOP );
    _wpi_torgbmode( pres );

    if( lButton.hbitmap != NULL ) {
        _wpi_deletebitmap( lButton.hbitmap );
    }

    if( rButton.hbitmap != NULL ) {
        _wpi_deletebitmap( rButton.hbitmap );
    }

    if( color_count == 2 ) {
        lButton.hbitmap = _wpi_createbitmap( CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1, 1, 1, NULL );
        rButton.hbitmap = _wpi_createbitmap( CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1, 1, 1, NULL );
    //} else if( color_count == 16 ) {
    } else {
        lButton.hbitmap = _wpi_createcompatiblebitmap( pres, CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1 );
        rButton.hbitmap = _wpi_createcompatiblebitmap( pres, CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1 );

        if( firstTime ) {
            mempres = _wpi_createcompatiblepres( pres, Instance, &memdc );
            _wpi_torgbmode( mempres );

            old_hbitmap = _wpi_selectbitmap( mempres, lButton.hbitmap );
            _wpi_patblt( mempres, 0, 0, CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1, BLACKNESS );
            _wpi_getoldbitmap( mempres, old_hbitmap );

            old_hbitmap = _wpi_selectbitmap( mempres, rButton.hbitmap );
            _wpi_patblt( mempres, 0, 0, CUR_SQR_SIZE + 1, 2 * CUR_SQR_SIZE + 1, WHITENESS );
            _wpi_getoldbitmap( mempres, old_hbitmap );
            _wpi_deletecompatiblepres( mempres, memdc );
            firstTime = false;
        }
    }
    _wpi_releasepres( HWND_DESKTOP, pres );

} /* SetCurrentNumColors */

/*
 * ChangeCurrentColor - this routine is called when the color palette
 *                      displayed is no longer showing the screen colors
 *                    - it verifies that the current color is not a screen
 *                      color
 */
void ChangeCurrentColor( void )
{
    if( lButton.type != NORMAL_CLR ) {
        if( numberOfColors > 2 ) {
            lButton.type = NORMAL_CLR;
        } else {
            SetColor( LMOUSEBUTTON, BLACK, BLACK, NORMAL_CLR );
        }
    }
    if( rButton.type != NORMAL_CLR ) {
        if( numberOfColors > 2 ) {
            rButton.type = NORMAL_CLR;
        } else {
            SetColor( LMOUSEBUTTON, WHITE, WHITE, NORMAL_CLR );
        }
    }

} /* ChangeCurrentColor */
